#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char file_name[] = "test.txt";

    // Create a readable/writable file.
    syscall(SYS_open, file_name, O_CREAT, S_IRUSR | S_IWUSR);

    // Open as write-only
    int fd = syscall(SYS_open, file_name, O_WRONLY);

    // Write something
    syscall(SYS_write, fd, "hello", 5);

    // Close the file
    syscall(SYS_close, fd);

    return 0;
}
