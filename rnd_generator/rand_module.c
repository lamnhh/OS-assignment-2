#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>

#define DRIVER_AUTHOR "Lam Nguyen"
#define DRIVER_DESC "A Linux kernel for generating random numbers"
#define DEVICE_NAME "GRN" // Stands for Generating Random Numbers
#define CLASS_NAME "gen_rand"

// Results will be in [0, 1024)
#define RND_MIN 0
#define RND_MAX 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");

// Global variable
static int major_number;
static struct class *gen_rand_class = NULL;
static struct device *gen_rand_device = NULL;

// Prototype for functions of the character driver
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

static int __init _module_constructor(void)
{
        printk(KERN_INFO "[%s]: Initialize\n", DEVICE_NAME);

        // Dynamically allocate major number
        major_number = register_chrdev(0, DEVICE_NAME, &fops);
        if (major_number < 0)
        {
                printk(KERN_ALERT "[%s]: Cannot register major number\n", DEVICE_NAME);
                return major_number;
        }

        printk(KERN_INFO "[%s]: Success register with major number %d\n", DEVICE_NAME, major_number);

        // Register device class
        gen_rand_class = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(gen_rand_class))
        {
                unregister_chrdev(major_number, DEVICE_NAME);
                printk(KERN_ALERT "[%s]: Failed to register device class\n", DEVICE_NAME);
                return PTR_ERR(gen_rand_class);
        }

        printk(KERN_INFO "[%s]: Success register device class\n", DEVICE_NAME);

        // Register the device driver
        gen_rand_device = device_create(gen_rand_class, NULL,
                                        MKDEV(major_number, 0), NULL, DEVICE_NAME);
        if (IS_ERR(gen_rand_device))
        {
                class_destroy(gen_rand_class);
                unregister_chrdev(major_number, DEVICE_NAME);
                printk(KERN_ALERT "[%s]: Failed to create the device\n", DEVICE_NAME);
                return PTR_ERR(gen_rand_device);
        }
        printk(KERN_INFO "[%s]: Device init sucess\n", DEVICE_NAME);
        return 0;
}

static void __exit _module_exit(void)
{
        device_destroy(gen_rand_class, MKDEV(major_number, 0));
        class_unregister(gen_rand_class);
        class_destroy(gen_rand_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_INFO "[%s]: Exit \n", DEVICE_NAME);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
        printk(KERN_INFO "[%s]: Open\n", DEVICE_NAME);
        return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
        unsigned int random_integer = 0;
        int diff = RND_MAX - RND_MIN;
        int error_count;

        get_random_bytes(&random_integer, sizeof(random_integer));
        random_integer = random_integer % diff;
        random_integer += RND_MIN;

        error_count = copy_to_user(buffer, &random_integer, sizeof(random_integer));
        return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
        printk(KERN_INFO "[%s]: Device successfully closed", DEVICE_NAME);
        return 0;
}

module_init(_module_constructor);
module_exit(_module_exit);