#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char fileName[] = "test.txt";

    //create a file that the owner can read/write. flags can be found easily by google
    syscall(SYS_open, fileName, O_CREAT, S_IRUSR | S_IWUSR);
    //open it as write only
    int fd = syscall(SYS_open, fileName, O_WRONLY);
    //write somehing
    syscall(SYS_write, fd, "hello", 5);
    //close the file
    syscall(SYS_close, fd);
    //change permission to executable
    syscall(SYS_fchmod, fd, S_IXUSR);
}
