#include "app.hpp"
#include <cwchar>

static bool quietMode = false;

void setQuiet(bool enabled)
{
    quietMode = enabled;
}

void logEvent(const std::wstring &op, const std::wstring &object, const std::wstring &result)
{
    if (quietMode && result.rfind(L"status=ERROR", 0) != 0)
        return;

    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t stamp[32];
    swprintf(stamp, 32, L"[%02u:%02u:%02u.%03u]", unsigned(time.wHour),
             unsigned(time.wMinute), unsigned(time.wSecond), unsigned(time.wMilliseconds));
    std::wcout << stamp << L" " << op << L" object=\"" << object << L"\" " << result << L'\n';
}

static std::wstring winErrorText(DWORD code)
{
    wchar_t message[1024]{};
    DWORD length = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, code, 0, message, 1024, nullptr);

    while (length > 0 &&
           (message[length - 1] == L'\r' || message[length - 1] == L'\n'))
    {
        --length;
    }

    if (length == 0)
        return L"Error description unavailable";

    return std::wstring(message, length);
}

void checkWin(LSTATUS code, const wchar_t *api, const std::wstring &object)
{
    if (code == ERROR_SUCCESS)
        return;

    logEvent(api, object, L"status=ERROR code=" + std::to_wstring(code) + L" text=" + winErrorText(code));
    throw std::runtime_error("Windows API error");
}