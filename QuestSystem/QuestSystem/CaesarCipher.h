#include <string>
#include <fstream>

class CaesarCipher
{
public:
    static std::wstring Encrypt(const std::wstring& text)
    {
        std::wstring result;
        wchar_t* work = nullptr;
        std::size_t len = text.length();
        work = new wchar_t[len + 1];
        wcscpy_s(work, len + 1, text.c_str());
        for (std::size_t i = 0; i < len + 1; ++i)
        {
            work[i] = work[i] + 10;
        }
        // work[i]を+10したときに0になってしまうことがある。
        // そのためworkの中には0x00が含まれることを考慮する
        result = std::wstring(&work[0], &work[len + 1]);
        delete[] work;
        return result;
    }

    static void EncryptToFile(const std::wstring& text, const std::wstring& filename)
    {
        std::wstring buff = Encrypt(text);

        std::wofstream ofs(filename);

        ofs.write(buff.data(), buff.size()); // ignore NULL
    }

    static std::wstring Decrypt(const std::wstring& text)
    {
        std::wstring result;
        wchar_t* work = nullptr;
        std::size_t len = text.length();
        work = new wchar_t[len]; // +1しない。暗号化時にヌル文字を+10した記号が末尾にあるから。

        // 途中にnull文字があっても指定バイト数までコピーする
        memcpy(work, text.c_str(), len);
        for (std::size_t i = 0; i < len; ++i)
        {
            work[i] = work[i] - 10;
        }
        result = std::wstring(work);
        delete[] work;
        return result;
    }

    static std::wstring DecryptFromFile(const std::wstring& text)
    {
        std::wifstream ifs(text);
        if (!ifs)
        {
            return std::wstring();
        }
        std::istreambuf_iterator<wchar_t> itBegin(ifs);
        std::istreambuf_iterator<wchar_t> itEnd;
        std::wstring work(itBegin, itEnd);

        work = Decrypt(work);
        return work;
    }
};