#include "app.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

static std::string field(const std::wstring &text)
{
    if (text.empty())
        return "\"\"";
    int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1,
                                nullptr, 0, nullptr, nullptr);
    if (!n)
        checkWin(GetLastError(), L"WideCharToMultiByte", L"CSV field");
    std::string bytes(n, '\0');
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1,
                             bytes.data(), n, nullptr, nullptr))
        checkWin(GetLastError(), L"WideCharToMultiByte", L"CSV field");
    bytes.pop_back();
    std::string result = "\"";
    for (char c : bytes)
    {
        if (c == '"')
            result += '"';
        result += c;
    }
    return result + '"';
}

static std::ofstream output(const fs::path &path, std::ios::openmode mode)
{
    fs::create_directories(path.parent_path());
    std::ofstream file;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    file.open(path, mode);
    return file;
}

void writeMeasurement(const Options &o, const Results &r, double ms)
{
    auto path = fs::path("data/results.csv");
    logEvent(L"MEASUREMENT", path.wstring(), L"status=START");
    auto file = output(path, std::ios::app);

  file << "student_id,run,seed,root,pattern,depth,target_pct,"
                "visited,found,elapsed_ms\n";

    file << o.student << ',' << o.run << ',' << 1000 + o.student * 97
         << ',' << field(o.root) << ',' << field(o.pattern) << ',' << o.depth
         << ',' << o.targetPct << ',' << r.visited << ',' << r.hits.size() << ',' << ms << '\n';
    file.close();
    logEvent(L"MEASUREMENT", path.wstring(), L"status=OK");
}