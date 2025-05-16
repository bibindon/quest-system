#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <exception>
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib" ) 

class csv
{
public:

    static std::vector<std::vector<std::wstring> > Read(const std::wstring& filepath)
    {
        std::vector<std::vector<std::wstring> > csvData;
        int result = PathFileExists(filepath.c_str());
        if (result == 0)
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string result(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, &result[0], size, nullptr, nullptr);

            std::string work = "cannot open " + result;
            throw std::exception(work.c_str());
        }

        // 「"」記号で囲まれているとセル内改行ができることに注意
        // 「"」記号で囲まれているとセル内で「,」が使用できることに注意
        std::wifstream ifs(filepath);
        if (ifs.is_open() == false)
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string result(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, &result[0], size, nullptr, nullptr);

            std::string work = "cannot open " + result;
            throw std::exception(work.c_str());
        }

        std::wstring buffComma;
        bool doubleQuoteMode = false;
        std::vector<std::wstring> work;
        std::istreambuf_iterator<wchar_t> itBegin(ifs);
        std::istreambuf_iterator<wchar_t> itEnd;

        for (; itBegin != itEnd; itBegin++)
        {
            if (*itBegin != ',' && *itBegin != '\n')
            {
                buffComma += *itBegin;
                if (*itBegin == '"')
                {
                    if (!doubleQuoteMode)
                    {
                        doubleQuoteMode = true;
                    }
                    else
                    {
                        doubleQuoteMode = false;
                    }
                }
            }
            else if (*itBegin == ',')
            {
                if (!doubleQuoteMode)
                {
                    work.push_back(buffComma);
                    buffComma.clear();
                }
                else
                {
                    buffComma += *itBegin;
                }
            }
            else if (*itBegin == '\n')
            {
                if (!doubleQuoteMode)
                {
                    work.push_back(buffComma);
                    buffComma.clear();
                    csvData.push_back(work);
                    work.clear();
                }
                else
                {
                    buffComma += *itBegin;
                }
            }
        }

        return csvData;
    }

    static std::vector<std::vector<std::wstring> > ReadFromString(const std::wstring& text)
    {
        std::vector<std::vector<std::wstring> > csvData;

        // 「"」記号で囲まれているとセル内改行ができることに注意
        std::wstring buffComma;
        bool doubleQuoteMode = false;
        std::vector<std::wstring> work;
        std::wstring::const_iterator itBegin(text.cbegin());
        std::wstring::const_iterator itEnd(text.cend());;

        for (; itBegin != itEnd; itBegin++)
        {
            if (*itBegin != ',' && *itBegin != '\n')
            {
                buffComma += *itBegin;
                if (*itBegin == '"')
                {
                    if (!doubleQuoteMode)
                    {
                        doubleQuoteMode = true;
                    }
                    else
                    {
                        doubleQuoteMode = false;
                    }
                }
            }
            else if (*itBegin == ',')
            {
                if (!doubleQuoteMode)
                {
                    work.push_back(buffComma);
                    buffComma.clear();
                }
                else
                {
                    buffComma += *itBegin;
                }
            }
            else if (*itBegin == '\n')
            {
                if (!doubleQuoteMode)
                {
                    work.push_back(buffComma);
                    buffComma.clear();
                    csvData.push_back(work);
                    work.clear();
                }
                else
                {
                    buffComma += *itBegin;
                }
            }
        }

        return csvData;
    }

    static void Write(const std::wstring& filepath, const std::vector<std::vector<std::wstring> >& csvData)
    {
        std::wofstream ofs(filepath);
        if (ofs.is_open() == false)
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string result(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), -1, &result[0], size, nullptr, nullptr);

            std::string work = "Cannot open " + result;
            throw std::exception(work.c_str());
        }

        for (std::size_t i = 0; i < csvData.size(); ++i)
        {
            for (std::size_t j = 0; j < csvData.at(i).size(); ++j)
            {
                ofs << csvData.at(i).at(j);
                if (j != csvData.at(i).size() - 1)
                {
                    ofs << ",";
                }
            }
            ofs << "\n";
        }
    }

private:
    csv();

    static void ltrim(std::wstring& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](wchar_t ch)
            {
                return !std::isspace(ch);
            }
        ));
    }

    static void rtrim(std::wstring& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](wchar_t ch)
                             {
                                 return !std::isspace(ch);
                             }
        ).base(), s.end());
    }

    static void trim(std::wstring& s)
    {
        rtrim(s);
        ltrim(s);
    }
};

