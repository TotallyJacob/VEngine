#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace util
{

// Data
constexpr static const char*  writePath = "C:\\Users\\jacob\\source\\repos\\VSpace\\VEngine\\renderer\\";
constexpr static const char*  readPath = "C:\\Users\\jacob\\source\\repos\\VSpace\\VSpace";
constexpr static unsigned int maxGeometrySize = 2000;
constexpr static unsigned int maxVertexDataSize = 10000000;

// Util
inline static void newLine(std::stringstream& ss) noexcept
{
    ss << "\n";
}

inline static constexpr const char quotes() noexcept
{
    return '\"';
}

// wchar and char util
inline static const std::wstring widen(std::string string)
{
    using namespace std;

    std::wostringstream   wideStringStream;
    const ctype<wchar_t>& ctfacet = use_facet<ctype<wchar_t>>(wideStringStream.getloc());
    for (size_t t = 0; t < string.size(); t++)
    {
        wideStringStream << ctfacet.widen(string[t]);
    }

    return wideStringStream.str();
}
inline static const std::string narrow(std::wstring string)
{
    using namespace std;

    ostringstream stringStream;

    const ctype<wchar_t>& ctfacet = use_facet<ctype<wchar_t>>(stringStream.getloc());
    for (size_t t = 0; t < string.size(); t++)
        stringStream << ctfacet.narrow(string[t], 0);

    return stringStream.str();
}

// Misc
inline static bool hasIdentifier(std::wstring& path, std::wstring identifier)
{
    return (path.find(identifier) != std::string::npos);
}
inline static void getFiles(const char* path, void func(std::wstring& path))
{

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {

        std::wstring path = entry.path();

        func(path);
    }
}
inline static void getFiles(std::string path, std::vector<std::filesystem::path>& paths, std::string identifier)
{
    std::wstring wIdentifier = widen(identifier);
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {

        std::wstring path = entry.path();

        if (hasIdentifier(path, wIdentifier))
            paths.push_back(narrow(path));
    }
}
inline static void getFiles(std::string& path, std::vector<std::string>& paths, std::string identifier)
{

    std::wstring wIdentifier = widen(identifier);
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {

        std::wstring path = entry.path();

        if (hasIdentifier(path, wIdentifier))
            paths.push_back(narrow(path));
    }
}

// Spliting
inline static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos)
    {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}
[[nodiscard]] inline std::vector<std::string> split(const std::string& txt, char ch)
{
    std::vector<std::string> splitted(0);
    split(txt, splitted, ch);
    return splitted;
}
inline static void toSplitWhitespace(std::vector<char>& currentData, std::vector<std::vector<std::string>>& newData)
{
    std::string line = "";

    using SplitLine = std::vector<std::string>;

    for (auto c : currentData)
    {
        if (c == '\n')
        {

            SplitLine splitByWhiteSpace(0);
            split(line, splitByWhiteSpace, ' ');

            newData.push_back(std::move(splitByWhiteSpace));

            line.empty();
            line.clear();

            continue;
        }

        line.push_back(c);
    }
}
inline static void splitByWhitespace(std::vector<char>& currentData, std::vector<std::vector<std::string>>& newData)
{
    std::string line = "";

    using SplitLine = std::vector<std::string>;

    for (auto c : currentData)
    {
        if (c == '\n')
        {

            SplitLine splitByWhiteSpace(0);
            split(line, splitByWhiteSpace, ' ');

            newData.push_back(std::move(splitByWhiteSpace));

            line.empty();
            line.clear();

            continue;
        }

        line.push_back(c);
    }
}

// Conversion
inline static float toFloat(std::string& str)
{
    return atof(str.c_str());
}

// Reading and writing util
template <typename T>
[[nodiscard]] inline static char* readData(T filePath, unsigned int& length)
{
    std::ifstream   file(filePath, std::ios::binary);
    std::streambuf* raw_buffer = file.rdbuf();

    file.seekg(0, std::ios::end);
    length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* data = new char[length + 1];

    raw_buffer->sgetn(data, length);

    file.close();

    data[length] = '\0'; // To stop reading garbage at end of file.

    return data;
}

template <typename T>
[[nodiscard]] inline std::tuple<unsigned int, std::ifstream> getFileData(T filePath, std::ifstream& file)
{
    std::ifstream file(filePath, std::ios::binary);
    file.seekg(0, std::ios::end);
    unsigned int length = file.tellg();
    file.seekg(0, std::ios::beg);

    return std::make_tuple(length, file);
}


template <typename T>
inline static void writeData(T filePath, char* data, unsigned int length)
{
    auto myfile = std::fstream(filePath, std::ios::out | std::ios::binary);
    myfile.write(&data[0], sizeof(char) * length);
    myfile.close();
}

template <typename T>
inline static void createWriteData(T filePath, char* data, unsigned int length)
{
    auto myfile = std::ofstream(filePath, std::ios::out | std::ios::binary);
    myfile.write(&data[0], sizeof(char) * length);
    myfile.close();
}

}; // namespace util
