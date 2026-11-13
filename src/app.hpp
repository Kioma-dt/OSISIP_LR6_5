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

struct Options
{
    int student = 1, run = 1;
    std::wstring mode = L"info";
    std::wstring root;
    std::wstring pattern = L"target";
    int depth = 6;
    bool quiet = false;
};

struct Hit
{
    int depth;
    std::wstring path;
};

struct Results
{
    int visited = 0;
    std::vector<Hit> hits;
};

Options parse(int argc, wchar_t **argv);
std::wstring sandbox(const Options &o);
void setQuiet(bool enabled);
void logEvent(const std::wstring &op, const std::wstring &object, const std::wstring &result);
void prepare(const Options &o);
Results search(const Options &o);
void checkWin(LSTATUS code, const wchar_t *api, const std::wstring &object);