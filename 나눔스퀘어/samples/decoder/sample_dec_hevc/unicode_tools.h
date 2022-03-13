#ifndef UUID_A6622920_B655_402A_AF05_8AFE93C7086D
#define UUID_A6622920_B655_402A_AF05_8AFE93C7086D

#if defined(_WIN32)
#include <map>
#define NOMINMAX
#include <windows.h>
#endif
#include <vector>
#include <string>

#include "sample_common_misc.h"

class UnicodeTools
{
public:
    static std::vector<const char*> initialize(int argc, char* argv[])
    {
        UnicodeTools& unicode_tools = UnicodeTools::instance();
        std::vector<const char*> ansi_arguments;
#if defined(_WIN32)
        LPWSTR* wide_arguments;
        char ansi_argument[4096];

        wide_arguments = CommandLineToArgvW(GetCommandLineW(), &argc);

        unicode_tools.m_ansi_arguments.reserve(argc);
        ansi_arguments.reserve(argc);

        for (int i = 0; i < argc; ++i) {
            WideCharToMultiByte(CP_ACP, 0, wide_arguments[i], -1, ansi_argument, 4096, NULL, NULL);
            unicode_tools.m_ansi_arguments.push_back(ansi_argument);
            ansi_arguments.push_back(unicode_tools.m_ansi_arguments[i].c_str());
            unicode_tools.m_unicode_arguments[ansi_arguments[i]] = wide_arguments[i];
        }

        LocalFree(wide_arguments);
#else
        ansi_arguments.reserve(argc);
        for (int i = 0; i < argc; ++i)
            ansi_arguments.push_back(argv[i]);
#endif
        return ansi_arguments;
    }

#if defined(_WIN32)
    static const std::wstring unicodeArgument(const char* ansi_argument)
    {
        UnicodeTools& unicode_tools = UnicodeTools::instance();

        if (unicode_tools.m_unicode_arguments.count(ansi_argument)) {
            return unicode_tools.m_unicode_arguments.find(ansi_argument)->second;
        }
        else {
            wchar_t wide_argument[4096];
            MultiByteToWideChar(CP_UTF8, 0, ansi_argument, -1, wide_argument, 4096);
            return std::wstring(wide_argument);
        }
    }
#else
    static const std::string unicodeArgument(const char* ansi_argument) { return ansi_argument; }
#endif
    static void printf(FILE* file, const char* const format, ...);
    static FILE* openFile(char const* const filename, const bool input);

private:
    UnicodeTools()
    {
#if defined(_WIN32)
        CRITICALSECTION_INIT(m_print_cs);
#endif
    }

    ~UnicodeTools()
    {
#if defined(_WIN32)
        CRITICALSECTION_DESTROY(m_print_cs);
#endif
    }

    static UnicodeTools& instance()
    {
        static UnicodeTools unicode_tools;
        return unicode_tools;
    }

#if defined(_WIN32)
    CRITICALSECTION_DEFINE(m_print_cs);
    std::vector<std::string> m_ansi_arguments;
    std::map<const char*, std::wstring> m_unicode_arguments;
#endif
};

#endif
