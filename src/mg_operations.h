#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "filesystem_data.h"

static int mg_chmod(const char* path, mode_t new_mode)
{
     printf("calling mg_chmod");
     return 0;
}
