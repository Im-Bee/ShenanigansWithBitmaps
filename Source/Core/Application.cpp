#include "Pch.h"
#include <linux/limits.h>

#include "Application.hpp"

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif // __linux__

// Application ----------------------------------------------------------------
// Public  --------------------------------------------------------------------
void LOL::Application::Initialize()
{
    std::wcout << L"Initializing app\n";
    
    FindCurrentDir();
}

// Private --------------------------------------------------------------------
void LOL::Application::FindCurrentDir()
{
#ifdef  _WIN32
#elif __linux__
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        std::wcout << L"WARRNING: Couldn't find current path!\n";
        return;
    }

    std::string tmp(cwd);
    m_wPathCurrent = std::wstring(tmp.begin(), tmp.end()) + L'/';
    std::wcout << L"Setting current path to" << m_wPathCurrent << std::endl;

    return;
#endif // _WIN32
}


