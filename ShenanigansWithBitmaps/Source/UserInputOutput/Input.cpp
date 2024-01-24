#include "Pch.h"

#include "Input.hpp"

// -----------------------------------------------------------------------------
std::wstring SWInputOutput::InputString(const std::wstring& message, const bool& suppressSetMessage)
{
    std::wstring in = L"";

    std::wcout << message << L" ";
    std::wcin >> in;

    if (!suppressSetMessage)
        std::wcout << L"Value has been set to " << in << std::endl;

    return in;
}
