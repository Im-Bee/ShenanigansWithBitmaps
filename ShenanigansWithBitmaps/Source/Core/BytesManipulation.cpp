#include "Pch.h"

#include "BytesManipulation.hpp"

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::Start()
{
    StartUserControls();
}

void SWBytesManipulation::ManipulationSession::Stop()
{
    StopUserControls();
}

// Setters ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void SWBytesManipulation::ManipulationSession::SetBuffer(char* target, uint64_t targetSize)
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
        ClearScreen();
        PrintBufferRow();

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
}
