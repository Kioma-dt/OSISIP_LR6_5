#include "app.hpp"
#include <cwchar>

void logEvent(const Text &op, const Text &object, const Text &result)
{
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t stamp[32];
    swprintf(stamp, 32, L"[%02u:%02u:%02u.%03u]", unsigned(time.wHour),
             unsigned(time.wMinute), unsigned(time.wSecond), unsigned(time.wMilliseconds));
    std::wcout << stamp << L" " << op << L" object=\"" << object << L"\" " << result << L'\n';
}