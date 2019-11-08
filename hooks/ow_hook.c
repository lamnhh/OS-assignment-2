#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
#include <asm/current.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lam Nguyen - 1712932");
MODULE_DESCRIPTION("Hooks to SYS_open and SYS_write");

static unsigned long **syscall_table_addr;
static void find_sys_call_table(void)
{
        syscall_table_addr = (unsigned long **)kallsyms_lookup_name("sys_call_table");
}

static asmlinkage long (*original_open)(const char __user *, int, int);
static asmlinkage long (*original_write)(unsigned int, const char __user *, size_t);

static asmlinkage long hook_open(const char __user *filename,
                                 int flags, int mode)
{
        static char filename_kernel[256];
        int pid = task_pid_nr(current);
        int len = strnlen_user(filename, 256);
        copy_from_user(filename_kernel, filename, len);
        printk(KERN_INFO "[OWHook]: (Open) %s (%d) opens %s\n", current->comm, pid, filename_kernel);
        return original_open(filename, flags, mode);
}

static asmlinkage long hook_write(unsigned int fd, const char __user *buf, size_t count)
{
        int pid = task_pid_nr(current);
        if (fd != 1)
        {
                printk(KERN_INFO "[OWHook]: (Write) %s (%d) write %ld bytes", current->comm, pid, count);
        }
        return original_write(fd, buf, count);
}

static int make_rw(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);
        if (pte->pte & ~_PAGE_RW)
        {
                pte->pte |= _PAGE_RW;
        }
        return 0;
}

static int make_ro(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);
        pte->pte = pte->pte & ~_PAGE_RW;
        return 0;
}

static int __init entry_point(void)
{
        printk(KERN_INFO "[OWHook]: Loading...\n");

        printk(KERN_INFO "[OWHook]: Searching for syscall table...\n");
        find_sys_call_table();
        printk(KERN_INFO "[OWHook]: Syscall table found at %p\n", syscall_table_addr);

        printk(KERN_INFO "[OWHook]: Hooking...\n");
        original_open = (void *)syscall_table_addr[__NR_open];
        original_write = (void *)syscall_table_addr[__NR_write];

        make_rw((unsigned long)syscall_table_addr);
        syscall_table_addr[__NR_open] = (unsigned long *)hook_open;
        syscall_table_addr[__NR_write] = (unsigned long *)hook_write;
        make_ro((unsigned long)syscall_table_addr);

        printk(KERN_INFO "[OWHook]: Hooked successfully\n");
        printk(KERN_INFO "[OWHook]: Module successfully loaded\n");
        return 0;
}

static void __exit exit_point(void)
{
        make_rw((unsigned long)syscall_table_addr);
        syscall_table_addr[__NR_open] = (unsigned long *)original_open;
        syscall_table_addr[__NR_write] = (unsigned long *)original_write;
        make_ro((unsigned long)syscall_table_addr);

        printk(KERN_INFO "[OWHook]: Module successfully unloaded\n");
}

module_init(entry_point);
module_exit(exit_point);