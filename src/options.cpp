#include "app.hpp"
#include <set>

static int number(const Text &value, int low, int high)
{
    if (value.empty() || value.find_first_not_of(L"0123456789") != Text::npos)
        throw std::invalid_argument("expected unsigned decimal integer");
    auto n = std::stoll(value);
    if (n < low || n > high)
        throw std::out_of_range("number outside allowed range");
    return static_cast<int>(n);
}

Options parse(int argc, wchar_t **argv)
{
    Options o;
    std::set<Text> seen;
    for (int i = 1; i < argc; i += 2)
    {
        Text k = argv[i];
        if (i + 1 == argc || !seen.insert(k).second)
            throw std::invalid_argument("missing value or repeated option");
        Text v = argv[i + 1];
        if (k == L"--student_id")
            o.student = number(v, 1, 32);
        else if (k == L"--run")
            o.run = number(v, 1, 2147483647);
        else if (k == L"--mode")
            o.mode = v;
        else
            throw std::invalid_argument("unknown option");
    }
    return o;
}

Text sandbox(const Options &o)
{
    return L"Software\\OSiSP\\Lab06\\Student_" + std::to_wstring(o.student) + L"\\Run_" + std::to_wstring(o.run);
}