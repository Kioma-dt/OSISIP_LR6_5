#include "app.hpp"
#include <set>

static int number(const std::wstring &value, int low, int high)
{
    if (value.empty() || value.find_first_not_of(L"0123456789") != std::wstring::npos)
        throw std::invalid_argument("expected unsigned decimal integer");

    auto n = std::stoll(value);

    if (n < low || n > high)
        throw std::out_of_range("number outside allowed range");

    return static_cast<int>(n);
}

Options parse(int argc, wchar_t **argv)
{
    Options o;
    std::set<std::wstring> seen;
    for (int i = 1; i < argc;)
    {
        std::wstring k = argv[i++];

        if (!seen.insert(k).second)
            throw std::invalid_argument("repeated option");

        if (k == L"--quiet")
        {
            o.quiet = true;
            continue;
        }

        if (i == argc || std::wstring(argv[i]).rfind(L"--", 0) == 0)
            throw std::invalid_argument("missing option value");

        std::wstring v = argv[i++];

        if (k == L"--student_id")
            o.student = number(v, 1, 32);
        else if (k == L"--run")
            o.run = number(v, 1, 2147483647);
        else if (k == L"--mode")
            o.mode = v;
        else if (k == L"--root")
            o.root = v;
        else if (k == L"--pattern")
            o.pattern = v;
        else if (k == L"--depth")
            o.depth = number(v, 0, 512);
        else if (k == L"--target_pct")
            o.targetPct = number(v, 0, 100);
        else
            throw std::invalid_argument("unknown option");
    }

    if (o.pattern.empty())
        throw std::invalid_argument("empty search pattern");

    if (o.mode == L"prepare" && seen.count(L"--root"))
        throw std::invalid_argument("prepare uses the student/run key only");

    if (seen.count(L"--root") && o.root.empty())
        throw std::invalid_argument("empty root");

    if (o.root.empty())
        o.root = L"HKEY_CURRENT_USER\\" + sandbox(o);

    return o;
}

std::wstring sandbox(const Options &o)
{
    return L"Software\\OSiSP\\Lab06\\Student_" + std::to_wstring(o.student) + L"\\Run_" + std::to_wstring(o.run);
}