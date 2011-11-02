
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/w1-gpio.h>

#include <mach/gpio.h>


/* w1 registration process */

static struct w1_gpio_platform_data foo_w1_gpio_pdata = {
	.pin		= S3C64XX_GPQ(0),	//GPQ0
	.is_open_drain	= 0,			//The PIN is not Open Drain
};

static struct platform_device foo_w1_device = {
	.name			= "w1-gpio",
	.id			= -1,
	.dev.platform_data	= &foo_w1_gpio_pdata,
};

static int __init s3c64xx_w1_gpio_init(void)
{
	int ret = platform_device_register(&foo_w1_device);

	printk("s3c64xx: Initializing w1(1-wire) on gpio with result %d\n", ret);

	return ret;
}

static void __exit s3c64xx_w1_gpio_exit(void)
{
	platform_device_unregister(&foo_w1_device);

	printk(KERN_ALERT "s3c64xx: Finalizing w1(1-wire) on gpio\n");
}

module_init(s3c64xx_w1_gpio_init);
module_exit(s3c64xx_w1_gpio_exit);

MODULE_LICENSE("Dual BSD/GPL");  //should always exist or you’ll get a warning
MODULE_AUTHOR("Deven Fan"); //optional
MODULE_DESCRIPTION("s3c6410 1-wire gpio module"); //optional


