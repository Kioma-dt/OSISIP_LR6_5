#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <windows.h>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using Text = std::wstring;
struct Options
{
    int student = 1, run = 1;
    Text mode = L"info";
};
Options parse(int argc, wchar_t **argv);
Text sandbox(const Options &o);
void logEvent(const Text &op, const Text &object, const Text &result);