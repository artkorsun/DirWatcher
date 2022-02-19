#include "Utils.h"

#include <array>

std::wstring getLastError()
{
    std::array<wchar_t, 256> buf;
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        &buf[0], (sizeof(buf) / sizeof(wchar_t)), NULL);

    return std::wstring(std::begin(buf), std::end(buf));
}