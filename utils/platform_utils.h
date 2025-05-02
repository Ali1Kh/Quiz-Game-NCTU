#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdbool.h>

#define SLEEP_MS(ms) platform_sleep(ms)
#define HAS_INPUT() platform_has_input()

#include <windows.h>
#include <conio.h>
void platform_sleep(unsigned int ms)
{
    Sleep(ms);
}
bool platform_has_input()
{
    return _kbhit();
}

#endif // PLATFORM_UTILS_H