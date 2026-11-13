#include "app.hpp"
#include <fcntl.h>
#include <io.h>

static long long counter()
{
    LARGE_INTEGER n;
    if (!QueryPerformanceCounter(&n))
        checkWin(ERROR_NOT_SUPPORTED, L"QueryPerformanceCounter", L"search timer");
    return n.QuadPart;
}

int wmain(int argc, wchar_t **argv)
{
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stderr), _O_U8TEXT);
    try
    {
        auto o = parse(argc, argv);
        setQuiet(o.quiet);

        if (o.mode == L"info")
            logEvent(L"CONFIG", L"HKEY_CURRENT_USER\\" + sandbox(o), L"status=OK");
        else if (o.mode == L"prepare")
            prepare(o);
        else if (o.mode == L"search")
        {
            logEvent(L"REG_SEARCH", o.root, L"status=START");
            LARGE_INTEGER frequency;

            if (!QueryPerformanceFrequency(&frequency))
                checkWin(ERROR_NOT_SUPPORTED, L"QueryPerformanceFrequency", L"search timer");

            auto begin = counter();

            auto r = search(o);
            double ms = 1000.0 * (counter() - begin) / frequency.QuadPart;
            logEvent(L"REG_SEARCH", o.root, L"status=OK");
            writeMeasurement(o, r, ms);

            if (!o.quiet)
            {
                for (const auto &hit : r.hits)
                    std::wcout << hit.path << L'\n';
            }

            std::wcout << L"visited=" << r.visited << L" found=" << r.hits.size()
                       << L" elapsed_ms=" << ms << L'\n';
        }
        else
            throw std::invalid_argument("unknown mode");
        return 0;
    }
    catch (const std::exception &e)
    {
        logEvent(L"RUN", L"reg", std::wstring(L"status=ERROR detail=") + std::wstring(e.what(), e.what() + std::char_traits<char>::length(e.what())));
        return 1;
    }
}