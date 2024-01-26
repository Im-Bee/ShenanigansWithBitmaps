#pragma once

#define SWBytesManipulation_FOOTER \
"W - up; S - down; A - left; D - right; j - decrease value; k - increase value; o - change between Hex and Dec mode"

namespace SWBytesManipulation
{
    enum DisplayMode
    {
        Hex,
        Dec
    };

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
        
        void UpdateSession();

        void Stop();

    public:

        // Getters -------------------------------------------------------------

        bool IsSessionAlive() { return m_UserControlThreadSwitch.load(); }

    public:

        // Setters -------------------------------------------------------------
        
        void SetBuffer(char* target, const uint64_t& targetSize);

    private:

        void StartUserControls();

        void StopUserControls();

        void UserControlLoop();

        void ClearScreen();

        std::string PrintBufferRow(const uint64_t& i);

        void DrawOutput();

        void IncreaseHeight();

        void DecreaseHeight();

        void GoRight();

        void GoLeft();

        void IncreaseValue();

        void DecreaseValue();

    private:

        std::atomic_bool m_UserControlThreadSwitch = false;
        std::thread m_UserControlThread;

        uint64_t m_TargetBufferSize = 0;
        char* m_TargetBuffer = nullptr;

        uint64_t m_WidthIndx = 0;
        const uint64_t m_RowWidth = 16;
        uint64_t m_HeightIndx = 0;

        DisplayMode m_DisplayMode = Hex;

    };
}