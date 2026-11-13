#include "app.hpp"
#include <random>

namespace
{
    struct Key
    {
        HKEY h = nullptr;
        std::wstring path;
        explicit Key(std::wstring p) : path(std::move(p)) {}
        Key(const Key &) = delete;
        Key &operator=(const Key &) = delete;
        ~Key()
        {
            if (h)
            {
                auto code = RegCloseKey(h);
                if (code)
                {
                    try
                    {
                        checkWin(code, L"RegCloseKey", path);
                    }
                    catch (...)
                    {
                    }
                }
            }
        }
    };

    void grow(HKEY parent, const std::wstring &path, int level, std::mt19937 &rng)
    {
        if (level > 6)
            return;
        for (int side = 0; side < 2; ++side)
        {
            std::wstring name = level % 2 == 0 && side == 0 ? L"TaRgEt_" : L"Node_";
            name += std::to_wstring(side) + L"_" + std::to_wstring(rng() % 10000);
            Key child(path + L"\\" + name);
            checkWin(RegCreateKeyExW(parent, name.c_str(), 0, nullptr, 0, KEY_CREATE_SUB_KEY,
                                     nullptr, &child.h, nullptr),
                     L"RegCreateKeyExW", child.path);
            grow(child.h, child.path, level + 1, rng);
        }
    }

    void walk(HKEY key, const std::wstring &path, int level, const Options &o, Results &r)
    {
        if (level > o.depth)
            return;
        for (DWORD index = 0;; ++index)
        {
            wchar_t name[256];
            DWORD size = 256;
            auto code = RegEnumKeyExW(key, index, name, &size, nullptr, nullptr, nullptr, nullptr);
            if (code == ERROR_NO_MORE_ITEMS)
                break;
            checkWin(code, L"RegEnumKeyExW", path);
            std::wstring full = path + L"\\" + std::wstring(name, size);
            ++r.visited;
            SetLastError(ERROR_SUCCESS);
            int pos = FindStringOrdinal(FIND_FROMSTART, name, size, o.pattern.c_str(), -1, TRUE);
            if (pos >= 0)
                r.hits.push_back({level, full});
            else
                checkWin(GetLastError(), L"FindStringOrdinal", full);
            if (level < o.depth)
            {
                Key child(full);
                checkWin(RegOpenKeyExW(key, name, 0, KEY_ENUMERATE_SUB_KEYS, &child.h),
                         L"RegOpenKeyExW", full);
                walk(child.h, full, level + 1, o, r);
            }
        }
    }
}

void prepare(const Options &o)
{
    std::wstring sub = sandbox(o);
    Key root(L"HKEY_CURRENT_USER\\" + sub);
    DWORD disposition;
    checkWin(RegCreateKeyExW(HKEY_CURRENT_USER, sub.c_str(), 0, nullptr, 0, KEY_CREATE_SUB_KEY,
                             nullptr, &root.h, &disposition),
             L"RegCreateKeyExW", root.path);
            
    if (disposition != REG_CREATED_NEW_KEY)
        throw std::runtime_error("run key exists; reuse it for search or choose a new run");

    std::mt19937 rng(1000 + o.student * 97);
    grow(root.h, root.path, 1, rng);
    logEvent(L"REG_CREATE_TREE", root.path, L"status=OK created=126");
}

Results search(const Options &o)
{
    struct Hive
    {
        const wchar_t *shortName;
        const wchar_t *name;
        HKEY h;
    };

    const Hive hives[] = {
        {L"HKCU", L"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
        {L"HKLM", L"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
        {L"HKCR", L"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
        {L"HKU", L"HKEY_USERS", HKEY_USERS},
        {L"HKCC", L"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG}};

    auto split = o.root.find(L'\\');
    std::wstring prefix = o.root.substr(0, split);
    std::wstring sub = split == std::wstring::npos ? L"" : o.root.substr(split + 1);
    
    while (!sub.empty() && sub.back() == L'\\')
        sub.pop_back();

    for (const auto &hive : hives)
    {
        if (_wcsicmp(prefix.c_str(), hive.shortName) && _wcsicmp(prefix.c_str(), hive.name))
            continue;
        Key root(std::wstring(hive.name) + (sub.empty() ? L"" : L"\\" + sub));
        checkWin(RegOpenKeyExW(hive.h, sub.c_str(), 0, KEY_ENUMERATE_SUB_KEYS, &root.h),
             L"RegOpenKeyExW", root.path);
        Results r;
        walk(root.h, root.path, 1, o, r);
        return r;
    }
    throw std::invalid_argument("unknown registry hive");
}