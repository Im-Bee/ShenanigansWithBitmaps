#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#ifdef _WIN32
    #include <Windows.h>
    #include "../resource2.h"

    #define	HInstance() GetModuleHandle(NULL)
#endif // _WIN64