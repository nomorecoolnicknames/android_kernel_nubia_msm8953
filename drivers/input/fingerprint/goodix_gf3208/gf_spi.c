/*
 * GF3208 fingerprint sensor kernel driver — main file
 *
 * Donor: nubia android_kernel_nubia_msm8953 3.18
 *        (drivers/hwmon/nubia_sensors/gf3208/gf_spi.c)
 * 4.9 API adaptations applied:
 *   1. wake_lock / wake_lock_init / wake_lock_destroy →
 *      wakeup_source / wakeup_source_init / wakeup_source_trash
 *      (wake_lock removed from 4.4+; tissot tree uses wakeup_source_init)
 *   2. class_create(THIS_MODULE, ...) — unchanged, correct in both 3.18 and 4.9
 *   3. register_chrdev(SPIDEV_MAJOR, ...) — static major kept (matches nubia HAL expectation)
 *   4. netlink_init/exit moved to netlink.c; sendnlmsg declared extern via header
 *   5. USE_PLATFORM_BUS kept — nx549j uses SoC platform device (not SPI master)
 *   6. gf_free_irq: devm_free_irq call kept as-is (4.9 compatible)
 *   7. access_ok(VERIFY_WRITE/READ, ...) — 2-arg form used in kernel 4.9 (not yet removed)
 *   8. Removed mback_disable extern (m1721-specific, not applicable here)
 *   9. Removed saved_command_line charger check (m1721-specific)
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/of_gpio.h>
#include <linux/timer.h>
#include <linux/notifier.h>
#include <linux/fb.h>
#include <linux/pm_qos.h>
#include <linux/cpufreq.h>
#include <linux/pm_wakeup.h>

#include "gf_spi.h"

#if defined(USE_SPI_BUS)
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#elif defined(USE_PLATFORM_BUS)
#include <linux/platform_device.h>
#endif

#define WAKELOCK_HOLD_TIME 2000 /* ms */

struct gf_key_map key_map[] = {
	{ "POWER", KEY_POWER },
	{ "HOME",  KEY_HOME  },
	{ "MENU",  KEY_MENU  },
	{ "BACK",  KEY_BACK  },
	{ "UP",    KEY_UP    },
	{ "DOWN",  KEY_DOWN  },
	{ "LEFT",  KEY_LEFT  },
	{ "RIGHT", KEY_RIGHT },
	{ "FORCE", KEY_F9    },
	{ "CLICK", KEY_F3    },
};

#define GF_DEBUG
#ifdef GF_DEBUG
#define gf_dbg(fmt, args...) pr_warn("goodix:" fmt, ##args)
#define FUNC_ENTRY()  pr_warn("goodix:%s, entry\n", __func__)
#define FUNC_EXIT()   pr_warn("goodix:%s, exit\n", __func__)
#else
#define gf_dbg(fmt, args...)
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

static DECLARE_BITMAP(minors, N_SPI_MINORS);
static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);
static struct gf_dev gf;
static struct class *gf_class;
/* 4.9 adaptation: wakeup_source replaces wake_lock */
static struct wakeup_source gf_ttw_wl;

void gf_free_irq(struct gf_dev *gf_dev)
{
	if (!gf_dev->irq_requested)
		return;

	devm_free_irq(&gf_dev->spi->dev, gf_dev->irq, gf_dev);
	gf_dev->irq_enabled = 1;
	gf_dev->irq_requested = 0;
}

static void gf_enable_irq(struct gf_dev *gf_dev)
{
	if (!gf_dev->irq_enabled) {
		enable_irq(gf_dev->irq);
		gf_dev->irq_enabled = 1;
	}
}

void gf_disable_irq(struct gf_dev *gf_dev)
{
	if (gf_dev->irq_enabled) {
		gf_dev->irq_enabled = 0;
		disable_irq(gf_dev->irq);
	}
}

static long gf_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct gf_dev *gf_dev = &gf;
	struct gf_key gf_key = { 0 };
	int retval = 0;
	int i;

	if (_IOC_TYPE(cmd) != GF_IOC_MAGIC)
		return -ENODEV;

	if (_IOC_DIR(cmd) & _IOC_READ)
		retval = !access_ok(VERIFY_WRITE, (void __user *)arg,
				    _IOC_SIZE(cmd));
	if ((retval == 0) && (_IOC_DIR(cmd) & _IOC_WRITE))
		retval = !access_ok(VERIFY_READ, (void __user *)arg,
				    _IOC_SIZE(cmd));
	if (retval)
		return -EFAULT;

	if (gf_dev->device_available == 0) {
		if ((cmd == GF_IOC_POWER_ON) || (cmd == GF_IOC_POWER_OFF) ||
		    (cmd == GF_IOC_ENABLE_GPIO)) {
			pr_info("gf3208: power cmd\n");
		} else {
			pr_info("gf3208: sensor is powered off\n");
			return -ENODEV;
		}
	}

	switch (cmd) {
	case GF_IOC_ENABLE_GPIO:
		gf_dbg("ioctl cmd = GF_IOC_ENABLE_GPIO\n");
		break;
	case GF_IOC_RELEASE_GPIO:
		gf_cleanup(gf_dev);
		gf_dbg("ioctl cmd = GF_IOC_RELEASE_GPIO\n");
		break;
	case GF_IOC_DISABLE_IRQ:
		gf_disable_irq(gf_dev);
		gf_dbg("ioctl cmd = GF_IOC_DISABLE_IRQ\n");
		break;
	case GF_IOC_ENABLE_IRQ:
		gf_enable_irq(gf_dev);
		gf_dbg("ioctl cmd = GF_IOC_ENABLE_IRQ\n");
		break;
	case GF_IOC_SETSPEED:
		/* SPI clock control not needed for platform bus */
		break;
	case GF_IOC_RESET:
		gf_hw_reset(gf_dev, 3);
		gf_dbg("ioctl cmd = GF_IOC_RESET\n");
		break;
	case GF_IOC_COOLBOOT:
		gf_dbg("ioctl cmd = GF_IOC_COOLBOOT\n");
		gf_power_off(gf_dev);
		mdelay(5);
		gf_power_on(gf_dev);
		break;
	case GF_IOC_SENDKEY:
		gf_dbg("ioctl cmd = GF_IOC_SENDKEY\n");
		if (copy_from_user(&gf_key, (struct gf_key *)arg,
				   sizeof(struct gf_key))) {
			retval = -EFAULT;
			break;
		}
		for (i = 0; i < ARRAY_SIZE(key_map); i++) {
			if (key_map[i].val == gf_key.key) {
				input_report_key(gf_dev->input, gf_key.key,
						 gf_key.value);
				input_sync(gf_dev->input);
				break;
			}
		}
		if (i == ARRAY_SIZE(key_map))
			retval = -EFAULT;
		break;
	case GF_IOC_CLK_READY:
		/* no AP clock control on platform bus */
		break;
	case GF_IOC_CLK_UNREADY:
		break;
	case GF_IOC_PM_FBCABCK:
		gf_dbg("ioctl cmd = GF_IOC_PM_FBCABCK\n");
		__put_user(gf_dev->fb_black, (u8 __user *)arg);
		break;
	case GF_IOC_POWER_ON:
		gf_dbg("ioctl cmd = GF_IOC_POWER_ON\n");
		if (gf_dev->device_available == 1)
			pr_info("gf3208: already powered on\n");
		else
			gf_power_on(gf_dev);
		gf_dev->device_available = 1;
		break;
	case GF_IOC_POWER_OFF:
		gf_dbg("ioctl cmd = GF_IOC_POWER_OFF\n");
		if (gf_dev->device_available == 0)
			pr_info("gf3208: already powered off\n");
		else
			gf_power_off(gf_dev);
		gf_dev->device_available = 0;
		break;
	default:
		gf_dbg("unsupported cmd: 0x%x\n", cmd);
		break;
	}
	return retval;
}

#ifdef CONFIG_COMPAT
static long gf_compat_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	return gf_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
}
#endif

static irqreturn_t gf_irq(int irq, void *handle)
{
#if defined(GF_NETLINK_ENABLE)
	char temp = GF_NET_EVENT_IRQ;
	/* 4.9 adaptation: __pm_wakeup_event replaces wake_lock_timeout */
	__pm_wakeup_event(&gf_ttw_wl, msecs_to_jiffies(WAKELOCK_HOLD_TIME));
	sendnlmsg(&temp);
	pr_warn("gf3208: irq fired\n");
#elif defined(GF_FASYNC)
	struct gf_dev *gf_dev = &gf;

	if (gf_dev->async)
		kill_fasync(&gf_dev->async, SIGIO, POLL_IN);
#endif
	return IRQ_HANDLED;
}

static int driver_init_partial(struct gf_dev *gf_dev)
{
	int ret = 0;

	FUNC_ENTRY();
	gf_dev->device_available = 1;
	if (gf_parse_dts(gf_dev))
		goto error;

	gf_dev->irq = gf_irq_num(gf_dev);
	ret = devm_request_threaded_irq(&gf_dev->spi->dev,
					gf_dev->irq,
					NULL,
					gf_irq,
					IRQF_TRIGGER_RISING | IRQF_ONESHOT,
					"gf", gf_dev);
	if (ret) {
		pr_err("gf3208: could not request irq %d\n",
		       gpio_to_irq(gf_dev->irq_gpio));
		goto error;
	}
	gf_dev->irq_requested = 1;
	gf_dev->irq_enabled = 1;
	FUNC_EXIT();
	return 0;

error:
	gf_cleanup(gf_dev);
	gf_dev->device_available = 0;
	return -1;
}

static int gf_open(struct inode *inode, struct file *filp)
{
	struct gf_dev *gf_dev;
	int status = -ENXIO;

	FUNC_ENTRY();
	mutex_lock(&device_list_lock);

	list_for_each_entry(gf_dev, &device_list, device_entry) {
		if (gf_dev->devt == inode->i_rdev) {
			gf_dbg("Found\n");
			status = 0;
			break;
		}
	}

	if (status == 0) {
		driver_init_partial(gf_dev);
		gf_dev->users++;
		filp->private_data = gf_dev;
		nonseekable_open(inode, filp);
		gf_dbg("opened device, irq = %d\n", gf_dev->irq);
		if (gf_dev->users == 1)
			gf_enable_irq(gf_dev);
		gf_power_on(gf_dev);
		gf_hw_reset(gf_dev, 360);
	} else {
		gf_dbg("no device for minor %d\n", iminor(inode));
	}
	mutex_unlock(&device_list_lock);
	FUNC_EXIT();
	return status;
}

#ifdef GF_FASYNC
static int gf_fasync(int fd, struct file *filp, int mode)
{
	struct gf_dev *gf_dev = filp->private_data;

	return fasync_helper(fd, filp, mode, &gf_dev->async);
}
#endif

static int gf_release(struct inode *inode, struct file *filp)
{
	struct gf_dev *gf_dev;
	int status = 0;

	FUNC_ENTRY();
	mutex_lock(&device_list_lock);
	gf_dev = filp->private_data;
	filp->private_data = NULL;

	gf_dev->users--;
	if (!gf_dev->users) {
		gf_dev->device_available = 0;
		gf_power_off(gf_dev);
	}
	gf_cleanup(gf_dev);
	mutex_unlock(&device_list_lock);
	FUNC_EXIT();
	return status;
}

static const struct file_operations gf_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = gf_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = gf_compat_ioctl,
#endif
	.open           = gf_open,
	.release        = gf_release,
#ifdef GF_FASYNC
	.fasync         = gf_fasync,
#endif
};

static int goodix_fb_state_chg_callback(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct gf_dev *gf_dev;
	struct fb_event *evdata = data;
	unsigned int blank;
#if defined(GF_NETLINK_ENABLE)
	char temp = 0;
#endif

	if (val != FB_EARLY_EVENT_BLANK)
		return 0;

	gf_dev = container_of(nb, struct gf_dev, notifier);
	if (evdata && evdata->data && gf_dev) {
		blank = *(int *)(evdata->data);
		switch (blank) {
		case FB_BLANK_POWERDOWN:
			if (gf_dev->device_available == 1) {
				gf_dev->fb_black = 1;
#if defined(GF_NETLINK_ENABLE)
				temp = GF_NET_EVENT_FB_BLACK;
				sendnlmsg(&temp);
#elif defined(GF_FASYNC)
				if (gf_dev->async)
					kill_fasync(&gf_dev->async, SIGIO,
						    POLL_IN);
#endif
			}
			break;
		case FB_BLANK_UNBLANK:
			if (gf_dev->device_available == 1) {
				gf_dev->fb_black = 0;
#if defined(GF_NETLINK_ENABLE)
				temp = GF_NET_EVENT_FB_UNBLACK;
				sendnlmsg(&temp);
#elif defined(GF_FASYNC)
				if (gf_dev->async)
					kill_fasync(&gf_dev->async, SIGIO,
						    POLL_IN);
#endif
			}
			break;
		default:
			break;
		}
	}
	return NOTIFY_OK;
}

static struct notifier_block goodix_noti_block = {
	.notifier_call = goodix_fb_state_chg_callback,
};

static void gf_reg_key_kernel(struct gf_dev *gf_dev)
{
	int i;

	set_bit(EV_KEY, gf_dev->input->evbit);
	for (i = 0; i < ARRAY_SIZE(key_map); i++)
		set_bit(key_map[i].val, gf_dev->input->keybit);

	gf_dev->input->name = GF_INPUT_NAME;
	if (input_register_device(gf_dev->input))
		pr_warn("gf3208: failed to register input device\n");
}

#if defined(USE_SPI_BUS)
static int gf_probe(struct spi_device *spi)
#elif defined(USE_PLATFORM_BUS)
static int gf_probe(struct platform_device *pdev)
#endif
{
	struct gf_dev *gf_dev = &gf;
	int status = -EINVAL;
	unsigned long minor;

	FUNC_ENTRY();
	INIT_LIST_HEAD(&gf_dev->device_entry);

#if defined(USE_SPI_BUS)
	gf_dev->spi = spi;
#elif defined(USE_PLATFORM_BUS)
	gf_dev->spi = pdev;
#endif
	gf_dev->irq_gpio         = -EINVAL;
	gf_dev->reset_gpio       = -EINVAL;
	gf_dev->pwr_gpio         = -EINVAL;
	gf_dev->device_available = 0;
	gf_dev->fb_black         = 0;
	gf_dev->irq_enabled      = 1;
	gf_dev->irq_requested    = 0;

	mutex_lock(&device_list_lock);
	minor = find_first_zero_bit(minors, N_SPI_MINORS);
	if (minor < N_SPI_MINORS) {
		struct device *dev;

		gf_dev->devt = MKDEV(SPIDEV_MAJOR, minor);
		dev = device_create(gf_class, &gf_dev->spi->dev,
				    gf_dev->devt, gf_dev, GF_DEV_NAME);
		status = IS_ERR(dev) ? PTR_ERR(dev) : 0;
	} else {
		dev_dbg(&gf_dev->spi->dev, "gf3208: no minor number available\n");
		status = -ENODEV;
	}

	if (status == 0) {
		set_bit(minor, minors);
		list_add(&gf_dev->device_entry, &device_list);
	} else {
		gf_dev->devt = 0;
	}
	mutex_unlock(&device_list_lock);

	if (status == 0) {
		gf_dev->input = input_allocate_device();
		if (gf_dev->input == NULL) {
			pr_err("gf3208: failed to allocate input device\n");
			status = -ENOMEM;
			goto error;
		}

		gf_dev->notifier = goodix_noti_block;
		fb_register_client(&gf_dev->notifier);
		gf_reg_key_kernel(gf_dev);

		/* 4.9 adaptation: wakeup_source_init replaces wake_lock_init */
		wakeup_source_init(&gf_ttw_wl, "gf3208_ttw_wl");
	}

	pr_info("gf3208: probe OK\n");
	FUNC_EXIT();
	return status;

error:
	gf_dev->device_available = 0;
	if (gf_dev->devt != 0) {
		pr_info("gf3208: probe error status = %d\n", status);
		mutex_lock(&device_list_lock);
		list_del(&gf_dev->device_entry);
		device_destroy(gf_class, gf_dev->devt);
		clear_bit(MINOR(gf_dev->devt), minors);
		mutex_unlock(&device_list_lock);
		if (gf_dev->input != NULL)
			input_unregister_device(gf_dev->input);
	}
	FUNC_EXIT();
	return status;
}

#if defined(USE_SPI_BUS)
static int gf_remove(struct spi_device *spi)
#elif defined(USE_PLATFORM_BUS)
static int gf_remove(struct platform_device *pdev)
#endif
{
	struct gf_dev *gf_dev = &gf;

	FUNC_ENTRY();
	if (gf_dev->irq)
		free_irq(gf_dev->irq, gf_dev);

	if (gf_dev->input != NULL)
		input_unregister_device(gf_dev->input);
	input_free_device(gf_dev->input);

	mutex_lock(&device_list_lock);
	list_del(&gf_dev->device_entry);
	device_destroy(gf_class, gf_dev->devt);
	clear_bit(MINOR(gf_dev->devt), minors);
	if (gf_dev->users == 0)
		kfree(gf_dev);
	mutex_unlock(&device_list_lock);

	fb_unregister_client(&gf_dev->notifier);

	/* 4.9 adaptation: wakeup_source_trash replaces wake_lock_destroy */
	wakeup_source_trash(&gf_ttw_wl);

	FUNC_EXIT();
	return 0;
}

#if defined(USE_SPI_BUS)
static int gf_suspend(struct spi_device *spi, pm_message_t mesg)
#elif defined(USE_PLATFORM_BUS)
static int gf_suspend(struct platform_device *pdev, pm_message_t state)
#endif
{
	struct gf_dev *gf_dev = &gf;

	enable_irq_wake(gf_dev->irq);
	pr_info("gf3208: suspend\n");
	return 0;
}

#if defined(USE_SPI_BUS)
static int gf_resume(struct spi_device *spi)
#elif defined(USE_PLATFORM_BUS)
static int gf_resume(struct platform_device *pdev)
#endif
{
	struct gf_dev *gf_dev = &gf;

	disable_irq_wake(gf_dev->irq);
	pr_info("gf3208: resume\n");
	return 0;
}

static struct of_device_id gx_match_table[] = {
	{ .compatible = GF_SPIDEV_NAME },
	{ },
};

#if defined(USE_SPI_BUS)
static struct spi_driver gf_driver = {
#elif defined(USE_PLATFORM_BUS)
static struct platform_driver gf_driver = {
#endif
	.driver = {
		.name           = GF_DEV_NAME,
		.owner          = THIS_MODULE,
		.of_match_table = gx_match_table,
	},
	.probe   = gf_probe,
	.remove  = gf_remove,
	.suspend = gf_suspend,
	.resume  = gf_resume,
};

static int __init gf_init(void)
{
	int status;

	FUNC_ENTRY();
	pr_warn("gf3208: gf_init start\n");

	BUILD_BUG_ON(N_SPI_MINORS > 256);
	status = register_chrdev(SPIDEV_MAJOR, CHRD_DRIVER_NAME, &gf_fops);
	if (status < 0) {
		pr_warn("gf3208: failed to register char device\n");
		FUNC_EXIT();
		return status;
	}

	gf_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(gf_class)) {
		unregister_chrdev(SPIDEV_MAJOR, gf_driver.driver.name);
		pr_warn("gf3208: failed to create class\n");
		FUNC_EXIT();
		return PTR_ERR(gf_class);
	}

#if defined(USE_PLATFORM_BUS)
	status = platform_driver_register(&gf_driver);
#elif defined(USE_SPI_BUS)
	status = spi_register_driver(&gf_driver);
#endif
	if (status < 0) {
		class_destroy(gf_class);
		unregister_chrdev(SPIDEV_MAJOR, gf_driver.driver.name);
		pr_warn("gf3208: failed to register driver\n");
	}

#ifdef GF_NETLINK_ENABLE
	netlink_init();
#endif

	pr_info("gf3208: status = 0x%x\n", status);
	FUNC_EXIT();
	pr_warn("gf3208: gf_init OK\n");
	return 0;
}
module_init(gf_init);

static void __exit gf_exit(void)
{
	FUNC_ENTRY();
#ifdef GF_NETLINK_ENABLE
	netlink_exit();
#endif
#if defined(USE_PLATFORM_BUS)
	platform_driver_unregister(&gf_driver);
#elif defined(USE_SPI_BUS)
	spi_unregister_driver(&gf_driver);
#endif
	class_destroy(gf_class);
	unregister_chrdev(SPIDEV_MAJOR, gf_driver.driver.name);
	FUNC_EXIT();
}
module_exit(gf_exit);

MODULE_AUTHOR("Jiangtao Yi, <yijiangtao@goodix.com>");
MODULE_DESCRIPTION("Goodix GF3208 fingerprint sensor driver (nx549j port)");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:gf3208");
