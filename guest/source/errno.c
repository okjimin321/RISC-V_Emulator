
#include "errno.h"
// TODO (seperate)
#include <sys/stat.h>

int errno;

// TODO (seperate)
int mkdir(const char* path, mode_t mode) {
    (void)path;
    (void)mode;
    return -1;
}