#include "Pch.h"

#include "HexEditor.hpp"
#include "Bitmap.hpp"

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::Start()
{
    StartUserControls();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::UpdateSession()
{
    DrawOutput();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::Stop()
{
    StopUserControls();
}

// Setters ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::SetBuffer(IN std::shared_ptr<SWBitmaps::Bitmap> target)
{
    if (m_UserControlThreadSwitch.load())
        return;
    
    m_pTargetBitmap = target;

    m_pTargetBuffer = target->GetRawPtr();
    m_uTargetBufferSize = target->GetRawSize();
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::StartUserControls()
{
    if (m_UserControlThreadSwitch.load())
        StopUserControls();

    m_UserControlThreadSwitch.store(true);
    m_UserControlThread = std::thread(&SWBytesManipulation::Session::UserControlLoop, 
        this);
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::StopUserControls()
{
    m_UserControlThreadSwitch.store(false);
    
    if (m_UserControlThread.joinable())
        m_UserControlThread.join();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::UserControlLoop()
{
    DWORD numberOfEvents;
    INPUT_RECORD iRec;
    HANDLE console = GetStdHandle(STD_INPUT_HANDLE);

    // Console not found
    if (console == NULL)
        throw;

    while (m_UserControlThreadSwitch.load())
    {
        ReadConsoleInput(console, &iRec, 1, &numberOfEvents);

        if (iRec.EventType != KEY_EVENT
            || !((KEY_EVENT_RECORD&)iRec.Event).bKeyDown)
        {
            continue;
        }

        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'q')
        {
            m_UserControlThreadSwitch.store(false);
            return;
        }

        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'w')
        {
            IncreaseHeight();
        }        
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 's')
        {
            DecreaseHeight();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'd')
        {
            GoRight();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'a')
        {
            GoLeft();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'k')
        {
            IncreaseValue();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'j')
        {
            DecreaseValue();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'o')
        {
            if (m_DisplayMode == Hex)
                m_DisplayMode = Dec;
            else
                m_DisplayMode = Hex;
        }
    }
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::ClearScreen()
{
#ifdef _WIN32
    system("cls");

    return;
#endif // _WIN32

    throw;
}

// -----------------------------------------------------------------------------
std::string SWBytesManipulation::Session::PrintBufferRow(IN const uint64_t& i)
{
    const uint64_t startingIndex = (i * m_uRowWidth);

    // Outside of buffer scope
    if (i < 0 || startingIndex >= m_uTargetBufferSize)
        return "";

    bool isSelected;
    if (i == m_uHeightIndx)
        isSelected = true;
    else
        isSelected = false;
    
    std::string result;
    result += "Index ";
    result += std::format("{:6d}", i);
    result += " | ";
    if (isSelected)
        result += "---  ";
    else
        result += "    ";

    std::string tmp;
    for (uint64_t i = startingIndex; i < startingIndex + m_uRowWidth; i++)
    {
        if (isSelected &&
            i == startingIndex + m_uWidthIndx)
            tmp = " >";
        else
            tmp = " ";

        if (m_DisplayMode == Hex)
            tmp += std::format("{:2x}", (uint8_t)m_pTargetBuffer[i]);
        else if (m_DisplayMode == Dec)
            tmp += std::format("{:3d}", (uint8_t)m_pTargetBuffer[i]);

        if (isSelected && 
            i == startingIndex + m_uWidthIndx)
            tmp += "< ";
        else
            tmp += " ";

        result += tmp;
    }

    std::for_each(result.begin(), result.end(), [](char& c) {
        c = std::toupper(c);
        });

    if (isSelected)
        result += "---";
    else
    {
        result += "    ";
        result += "  ";
    }

    result += " | ";
    result += "\n";

    return result;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::DrawOutput()
{
    using namespace std::chrono_literals;

    std::string output = {};
    const uint8_t offsetUpAndDown = 14;
    uint64_t upIndex;
    uint64_t downIndex = (m_uHeightIndx + offsetUpAndDown);

    // Hacky way around
    if (m_uHeightIndx < offsetUpAndDown)
    {
        upIndex = 0;
        downIndex += offsetUpAndDown - m_uHeightIndx;
    }
    else 
    {
        upIndex = m_uHeightIndx - offsetUpAndDown;
    }

    for (uint64_t& i = upIndex;
        i < downIndex;
        i++)
    { 
        output += PrintBufferRow(i);
    }
    
    std::cout << output;
    std::cout << std::endl << SWBytesManipulation_FOOTER;
    std::this_thread::sleep_for(11ms);
    ClearScreen();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::IncreaseHeight()
{
    if (m_uHeightIndx == 0)
    {
        m_uHeightIndx = m_uTargetBufferSize / m_uRowWidth;
        return;
    }

    m_uHeightIndx--;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::DecreaseHeight()
{
    if ((m_uHeightIndx + 1) * m_uRowWidth >= m_uTargetBufferSize)
    {
        m_uHeightIndx = 0;
        return;
    }

    m_uHeightIndx++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::GoRight()
{
    if ((m_uWidthIndx + 1) >= m_uRowWidth)
    {
        m_uWidthIndx = 0;
        return;
    }

    m_uWidthIndx++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::GoLeft()
{
    if (m_uWidthIndx == 0)
    {
        m_uWidthIndx = m_uRowWidth - 1;
        return;
    }

    m_uWidthIndx--;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::IncreaseValue()
{
    m_pTargetBuffer[(m_uHeightIndx * m_uRowWidth) + m_uWidthIndx]++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::Session::DecreaseValue()
{
    m_pTargetBuffer[(m_uHeightIndx * m_uRowWidth) + m_uWidthIndx]--;
}
