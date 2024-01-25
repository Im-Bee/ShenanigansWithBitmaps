#include "Pch.h"

#include "Application.hpp"
#include "UserInputOutput/Output.hpp"
#include "UserInputOutput/Input.hpp"
#include "BytesManipulation.hpp"

// -----------------------------------------------------------------------------
void Application::Initialize()
{
    SWInputOutput::OutputPrompt(IDS_STRING_PROMPT_WELCOME);
    std::wcout << L"Availble commands\n\
        - [q] for quit\n\
        - [load] to load a file from path\n\
        - [save] to save file in output dir\n\
        - [colorhalf] to color half\n\
        - [color] to color whole\n\
        - [lookat] to view file as dec \n\
        - [negative] to make file negative\n";

    FindPathToItself();
    CreateSaveDir();
}

// -----------------------------------------------------------------------------
void Application::Update()
{
    std::wstring r = SWInputOutput::InputString(L"... : ", true);

    // Normalize
    std::for_each(r.begin(), r.end(), [](wchar_t c) {
        return std::tolower(c);
        });

    if (r == L"q")
    {
        m_bQuit = true;
        return;
    }
    if (r == L"load")
    {
        LoadFile();
        return;
    }
    if (r == L"save")
    {
        SaveFile();
        return;
    }
    if (r == L"lookat")
    {
        LookAtFile();
        return;
    }
    if (r == L"colorhalf")
    {
        ColorHalfOfFile();
        return;
    }
    if (r == L"color")
    {
        ColorWholeFile();
        return;
    }
    if (r == L"negative")
    {
        MakeFileNegative();
        return;
    }

    std::wcout << L"Invalid command" << std::endl;
}

// -----------------------------------------------------------------------------
void Application::Destroy()
{
    if (m_LoadedBitmap.get())
        m_LoadedBitmap->Destroy();
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void Application::LoadFile()
{
    if (m_LoadedBitmap.get())
        m_LoadedBitmap->Destroy();
    else
        m_LoadedBitmap = std::make_shared<SWBitmaps::Bitmap>();

    SWInputOutput::OutputPrompt(IDS_STRING_PROMPT_DOING_BITMAP);
    std::wstring p = SWInputOutput::InputString(L"Set path to target bitmap:");

    // Remove all of '"'
    while (p.find('"') != std::string::npos)
    {
        auto at = p.find_first_of('"');
        p.erase(at, at + 1);
    }

    m_LoadedBitmap->Initialize(p);
}   

// -----------------------------------------------------------------------------
void Application::SaveFile()
{
    if (!m_LoadedBitmap.get())
    {
        std::cout << "There is no file to save" << std::endl;
        return;
    }

    m_LoadedBitmap->SaveToFile(SAVE_DIR 
        + L"Output" 
        + std::to_wstring(m_BitmapCounter++) 
        + L".bmp");
}

// -----------------------------------------------------------------------------
void Application::LookAtFile()
{
    auto session = SWBytesManipulation::ManipulationSession();

    session.Start();

    while (session.IsSessionAlive())
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }

    std::wcout << L"Availble commands\n\
        - [q] for quit\n\
        - [load] to load a file from path\n\
        - [save] to save file in output dir\n\
        - [colorhalf] to color half\n\
        - [color] to color whole\n\
        - [lookat] to view file as dec \n\
        - [negative] to make file negative\n";
}

// -----------------------------------------------------------------------------
void Application::ColorHalfOfFile()
{
    if (!m_LoadedBitmap.get())
    {
        std::cout << "There is no file" << std::endl;
        return;
    }

    m_LoadedBitmap->ColorHalf({250, 170, 15});
}

// -----------------------------------------------------------------------------
void Application::ColorWholeFile()
{
    if (!m_LoadedBitmap.get())
    {
        std::cout << "There is no file" << std::endl;
        return;
    }

    m_LoadedBitmap->ColorWhole({ 250, 170, 15 });
}

// -----------------------------------------------------------------------------
void Application::MakeFileNegative()
{
    if (!m_LoadedBitmap.get())
    {
        std::cout << "There is no file" << std::endl;
        return;
    }

    m_LoadedBitmap->MakeItNegative();
}

// -----------------------------------------------------------------------------
void Application::FindPathToItself()
{
#ifdef _WIN32
    wchar_t* tmpFileName = (wchar_t*) malloc(sizeof(wchar_t) * MAX_PATH);
    if (!tmpFileName)
        throw std::bad_alloc();

    GetModuleFileName(NULL, tmpFileName, MAX_PATH);

    wchar_t* lastSlash = wcsrchr(tmpFileName, L'\\');
    *(lastSlash + 1) = L'\0';

    m_PathToItself = tmpFileName;
    free(tmpFileName);

    return;
#endif // DEBUG
    throw;
}

// -----------------------------------------------------------------------------
void Application::CreateSaveDir()
{
#ifdef _WIN64
    CreateDirectory(SAVE_DIR.c_str(), NULL);

    return;
#endif // _WIN64

    throw;
}
