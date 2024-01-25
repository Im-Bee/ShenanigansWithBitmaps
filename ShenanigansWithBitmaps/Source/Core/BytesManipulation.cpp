#include "Pch.h"

#include "BytesManipulation.hpp"

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::Start()
{
    StartUserControls();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::UpdateSession()
{
    DrawOutput();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::Stop()
{
    StopUserControls();
}

// Setters ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::SetBuffer(char* target, const uint64_t& targetSize)
{
    if (m_UserControlThreadSwitch.load())
        return;

    m_TargetBuffer = target;
    m_TargetBufferSize = targetSize;
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::StartUserControls()
{
    if (m_UserControlThreadSwitch.load())
        StopUserControls();

    m_UserControlThreadSwitch.store(true);
    m_UserControlThread = std::thread(&SWBytesManipulation::ManipulationSession::UserControlLoop, 
        this);
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::StopUserControls()
{
    m_UserControlThreadSwitch.store(false);
    
    if (m_UserControlThread.joinable())
        m_UserControlThread.join();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::UserControlLoop()
{
    DWORD numberOfEvents;
    INPUT_RECORD iRec;
    HANDLE console = GetStdHandle(STD_INPUT_HANDLE);

    // Console not found
    if (console == NULL)
        throw;

    while (m_UserControlThreadSwitch.load())
    {
        using namespace std::chrono_literals;

        std::this_thread::sleep_for(11ms);

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
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'j')
        {
            IncreaseValue();
        }
        if (((KEY_EVENT_RECORD&)iRec.Event).uChar.AsciiChar == 'k')
        {
            DecreaseValue();
        }
    }
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::ClearScreen()
{
#ifdef _WIN32
    system("cls");

    return;
#endif // _WIN32

    throw;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::PrintBufferRow()
{
    std::string image;
    image += "\n\n\n\n\n\n\n\n\n\n\n\n\n";
    image += "Index " + std::to_string(m_HeightIndx);
    image += " | ";

    const uint64_t startingIndex = (m_HeightIndx * m_RowWidth);
    std::string tmp;
    for (uint64_t i = startingIndex; i < startingIndex + m_RowWidth; i++)
    {
        if (i == startingIndex + m_WidthIndx)
            tmp = " >";
        else
            tmp = " ";

        tmp += std::format("{:2x}", (uint8_t)m_TargetBuffer[i]);
        
        if (i == startingIndex + m_WidthIndx)
            tmp += "< ";
        else
            tmp += " ";

        image += tmp;
    }

    std::for_each(image.begin(), image.end(), [](char& c) {
        c = std::toupper(c);
        });

    std::cout << image;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::DrawOutput()
{
    using namespace std::chrono_literals;

    std::this_thread::sleep_for(11ms);
    ClearScreen();
    PrintBufferRow();
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::IncreaseHeight()
{
    if (m_HeightIndx == 0)
    {
        m_HeightIndx = m_TargetBufferSize / m_RowWidth;
        return;
    }

    m_HeightIndx--;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::DecreaseHeight()
{
    if ((m_HeightIndx + 1) * m_RowWidth >= m_TargetBufferSize)
    {
        m_HeightIndx = 0;
        return;
    }

    m_HeightIndx++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::GoRight()
{
    if ((m_WidthIndx + 1) >= m_RowWidth)
    {
        m_WidthIndx = 0;
        return;
    }

    m_WidthIndx++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::GoLeft()
{
    if (m_WidthIndx == 0)
    {
        m_WidthIndx = m_RowWidth;
        return;
    }

    m_WidthIndx--;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::IncreaseValue()
{
    m_TargetBuffer[(m_HeightIndx * m_RowWidth) + m_WidthIndx]++;
}

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::DecreaseValue()
{
    m_TargetBuffer[(m_HeightIndx * m_RowWidth) + m_WidthIndx]--;
}
