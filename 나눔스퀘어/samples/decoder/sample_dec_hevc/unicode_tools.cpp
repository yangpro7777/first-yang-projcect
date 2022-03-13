#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#endif

#include "unicode_tools.h"

void UnicodeTools::printf(FILE* file, const char* const format, ...)
{
#if defined(_WIN32)
    UnicodeTools& unicode_tools = UnicodeTools::instance();
    va_list args;
    wchar_t wide_format[4096];

    MultiByteToWideChar(CP_ACP, 0, format, -1, wide_format, 4096);

    CRITICALSECTION_ENTER(unicode_tools.m_print_cs);
    _setmode(_fileno(file), _O_U8TEXT);
    va_start(args, format);
    vfwprintf(file, wide_format, args);
    va_end(args);
    _setmode(_fileno(file), _O_TEXT);
    CRITICALSECTION_LEAVE(unicode_tools.m_print_cs);
#else
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
#endif
}

FILE* UnicodeTools::openFile(char const* const filename, const bool input)
{
#if defined(_WIN32)
    return _wfopen(unicodeArgument(filename).c_str(), input ? L"rb" : L"wb");
#else
    return fopen(unicodeArgument(filename).c_str(), input ? "rb" : "wb");
#endif
}
