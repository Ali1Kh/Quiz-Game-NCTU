// platform_utils.h

#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdbool.h>

// تعريف الدوال والماكروز المشتركة
#define SLEEP_MS(ms) platform_sleep(ms)
#define HAS_INPUT() platform_has_input()

// تعريف الدوال حسب النظام
#ifdef _WIN32
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
#else
#include <unistd.h>
#include <sys/select.h>
void platform_sleep(unsigned int ms)
{
    usleep(ms * 1000);
}
bool platform_has_input()
{
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}
#endif

#endif // PLATFORM_UTILS_H