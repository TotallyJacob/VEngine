#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>


namespace vengine
{

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

// File system stuff
inline static bool hasIdentifier(std::wstring& path, std::wstring identifier)
{
    return (path.find(identifier) != std::string::npos);
}
inline static void getFiles(std::string& path, std::vector<std::filesystem::path>& paths, std::string identifier)
{

    std::wstring wIdentifier = widen(identifier);
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {

        std::wstring path = entry.path();

        if (hasIdentifier(path, wIdentifier))
            paths.push_back(narrow(path));
    }
}

// Reading and writing util
template <typename T>
[[nodiscard]] inline static void readData(T filePath, unsigned int& length, std::string& data)
{
    std::ifstream   file(filePath, std::ios::binary);
    std::streambuf* raw_buffer = file.rdbuf();

    file.seekg(0, std::ios::end);
    length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::unique_ptr<char> data_ptr(new char[length + 1]);
    raw_buffer->sgetn(data_ptr.get(), length);

    file.close();

    data_ptr.get()[length] = '\0'; // To stop reading garbage at end of file.

    data.assign(data_ptr.get(), length + 1);

    data_ptr.reset();
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



} // namespace vengine
