#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <memory>
#include <format>
#include <cstring>
#include <cmath>

#ifdef _WIN32
    #include <Windows.h>

    #define	HInstance() GetModuleHandle(NULL)
#endif // _WIN64
