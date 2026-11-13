#include "app.hpp"
#include <fcntl.h>
#include <io.h>

//g++ -std=c++17 -O2 -Wall -Wextra -municode src/*.cpp -o build/reg.exe -ladvapi32
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
            auto r = search(o);

            if (!o.quiet)
            {
                for (const auto &hit : r.hits)
                    std::wcout << hit.path << L'\n';
            }

            std::wcout << L"visited=" << r.visited << L" found=" << r.hits.size() << L'\n';
        }
        else
            throw std::invalid_argument("unknown mode");
        return 0;
    }
    catch (const std::exception &e)
    {
        logEvent(L"RUN", L"lab06", std::wstring(L"status=ERROR detail=") + std::wstring(e.what(), e.what() + std::char_traits<char>::length(e.what())));
        return 1;
    }
}