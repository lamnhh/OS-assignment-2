#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
static char buffer[BUFFER_LENGTH];

int main()
{
        int ret, fd;

        fd = open("/dev/GRN", O_RDONLY); // Open the device with read/write access
        if (fd < 0)
        {
                perror("Failed to open the device...");
                return errno;
        }

        ret = read(fd, buffer, BUFFER_LENGTH); // Read the response from the LKM
        if (ret != 0)
        {
                perror("Failed to generate random number...\n");
                return errno;
        }

        int x;
        memcpy(&x, buffer, sizeof(int));
        printf("Random number generated %d\n", x);
        return 0;
}