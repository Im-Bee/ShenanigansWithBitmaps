#pragma once

namespace LOL
{
    class Application
    {
        private:

            std::wstring m_wPathCurrent;

        public:

            Application() noexcept = default;
            
            ~Application() noexcept = default;

        public:

            void Initialize();


        private:

            void FindCurrentDir();

    };
}
