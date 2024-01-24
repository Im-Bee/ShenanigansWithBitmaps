#include "Pch.h"

#include "Output.hpp"

// -----------------------------------------------------------------------------
void SWInputOutput::OutputPrompt(const int& id)
{
#ifdef _WIN32
    wchar_t str[PROMPT_MAX_SIZE] = { 0 };
    LoadString(HInstance(), id, str, PROMPT_MAX_SIZE);

    std::wcout << str << std::endl;

    return;
#endif // _WIN32
    throw;
}
