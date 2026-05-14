/*
 * GF3208 fingerprint sensor kernel driver - header
 * Ported from nubia msm8953 3.18 + tissot 4.9 API adaptations
 * Compatible: goodix,fingerprint platform device (USE_PLATFORM_BUS)
 * Target: nx549j (MSM8953/SD625), RST=tlmm47, INT=tlmm48, PWR=tlmm35
 */
#ifndef __GF_SPI_H
#define __GF_SPI_H

#include <linux/types.h>
#include <linux/notifier.h>

enum FP_MODE {
	GF_IMAGE_MODE = 0,
	GF_KEY_MODE,
	GF_SLEEP_MODE,
	GF_FF_MODE,
	GF_DEBUG_MODE = 0x56
};

struct gf_key {
	unsigned int key;
	int value;
};

struct gf_key_map {
	char *name;
	unsigned short val;
};

/* IOC set — from nubia 3.18 GF3208 (matches Nubia userspace HAL) */
#define  GF_IOC_MAGIC         'G'
#define  GF_IOC_DISABLE_IRQ   _IO(GF_IOC_MAGIC, 0)
#define  GF_IOC_ENABLE_IRQ    _IO(GF_IOC_MAGIC, 1)
#define  GF_IOC_SETSPEED      _IOW(GF_IOC_MAGIC, 2, unsigned int)
#define  GF_IOC_RESET         _IO(GF_IOC_MAGIC, 3)
#define  GF_IOC_COOLBOOT      _IO(GF_IOC_MAGIC, 4)
#define  GF_IOC_SENDKEY       _IOW(GF_IOC_MAGIC, 5, struct gf_key)
#define  GF_IOC_CLK_READY     _IO(GF_IOC_MAGIC, 6)
#define  GF_IOC_CLK_UNREADY   _IO(GF_IOC_MAGIC, 7)
#define  GF_IOC_PM_FBCABCK    _IO(GF_IOC_MAGIC, 8)
#define  GF_IOC_POWER_ON      _IO(GF_IOC_MAGIC, 9)
#define  GF_IOC_POWER_OFF     _IO(GF_IOC_MAGIC, 10)
#define  GF_IOC_ENABLE_GPIO   _IO(GF_IOC_MAGIC, 11)
#define  GF_IOC_RELEASE_GPIO  _IO(GF_IOC_MAGIC, 12)
#define  GF_IOC_MAXNR         13

/* Bus selection: platform device (not SPI master) */
#define  USE_PLATFORM_BUS     1

/* Netlink event IDs */
#define GF_NETLINK_ENABLE      1
#define GF_NET_EVENT_IRQ       0
#define GF_NET_EVENT_FB_BLACK  1
#define GF_NET_EVENT_FB_UNBLACK 2
#define NETLINK_TEST           25
#define MAX_MSGSIZE            32

/* Device/driver names */
#define GF_SPIDEV_NAME      "goodix,fingerprint"
#define GF_DEV_NAME         "goodix_fp"
#define GF_INPUT_NAME       "gf3208"
#define CHRD_DRIVER_NAME    "goodix_fp_spi"
#define CLASS_NAME          "goodix_fp"
#define SPIDEV_MAJOR        225
#define N_SPI_MINORS        32

/* pinctrl state names (optional — guarded by ENABLE_PINCTRL) */
#ifdef ENABLE_PINCTRL
static const char * const pctl_names[] = {
	"goodixfp_reset_reset",
	"goodixfp_reset_active",
	"goodixfp_irq_active",
};
#endif

struct gf_dev {
	dev_t devt;
	struct list_head device_entry;
#if defined(USE_SPI_BUS)
	struct spi_device *spi;
#elif defined(USE_PLATFORM_BUS)
	struct platform_device *spi;
#endif
	struct clk *core_clk;
	struct clk *iface_clk;

#ifdef ENABLE_PINCTRL
	struct pinctrl *fingerprint_pinctrl;
	struct pinctrl_state *pinctrl_state[ARRAY_SIZE(pctl_names)];
#endif

	struct input_dev *input;
	unsigned users;
	signed irq_gpio;
	signed reset_gpio;
	signed pwr_gpio;
	int irq;
	int irq_enabled;
	int clk_enabled;
#ifdef GF_FASYNC
	struct fasync_struct *async;
#endif
	struct notifier_block notifier;
	char device_available;
	char fb_black;
};

int gf_parse_dts(struct gf_dev *gf_dev);
void gf_cleanup(struct gf_dev *gf_dev);

int gf_power_on(struct gf_dev *gf_dev);
int gf_power_off(struct gf_dev *gf_dev);

int gf_hw_reset(struct gf_dev *gf_dev, unsigned int delay_ms);
int gf_irq_num(struct gf_dev *gf_dev);

void gf_disable_irq(struct gf_dev *gf_dev);
void gf_free_irq(struct gf_dev *gf_dev);

void sendnlmsg(char *message);
int netlink_init(void);
void netlink_exit(void);

#endif /* __GF_SPI_H */
