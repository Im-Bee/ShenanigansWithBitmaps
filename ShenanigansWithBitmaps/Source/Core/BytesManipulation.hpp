#pragma once

namespace SWBytesManipulation
{
    class ManipulationSession
    {
    public:

        ManipulationSession() = default;

        ~ManipulationSession()
        {
            StopUserControls();
        }

    public:

        void Start();

        void Stop();

    public:

        // Getters -------------------------------------------------------------

        const bool& IsSessionAlive() { return m_UserControlThreadSwitch.load(); }

    public:

        // Setters -------------------------------------------------------------
        
        void SetBuffer(char* target, uint64_t targetSize);

    private:

        void StartUserControls();

        void StopUserControls();

        void UserControlLoop();

        void ClearScreen();

        void PrintBufferRow();

    private:

        std::atomic_bool m_UserControlThreadSwitch = false;
        std::thread m_UserControlThread;

        uint64_t m_TargetBufferSize = 0;
        char* m_TargetBuffer = nullptr;

    };
}