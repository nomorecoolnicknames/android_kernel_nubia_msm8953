/*
 * GF3208 platform/GPIO support
 * Donor: nubia android_kernel_nubia_msm8953 3.18 (drivers/hwmon/nubia_sensors/gf3208/platform.c)
 * Adaptations for 4.9:
 *   - Removed extern gf_disable_irq / gf_free_irq (defined in gf_spi.c, declared in header)
 *   - devm_pinctrl_get / pinctrl_lookup_state / pinctrl_select_state: unchanged (identical 3.18/4.9)
 *   - devm_gpio_request / devm_gpio_free: unchanged
 *   - gpio_direction_output / gpio_set_value / gpio_to_irq: unchanged
 *   - USE_PLATFORM_BUS kept (nx549j uses SoC platform device, not SPI master)
 * GPIO DTS names: goodix,gpio_rst  goodix,gpio_int  goodix,gpio_pwr
 *   matching msm8953-mtp-nx549j.dts (RST=tlmm47, INT=tlmm48, PWR=tlmm35)
 */
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/pinctrl/consumer.h>

#include "gf_spi.h"

#if defined(USE_SPI_BUS)
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#elif defined(USE_PLATFORM_BUS)
#include <linux/platform_device.h>
#endif

#define gf_dbg(fmt, args...) pr_warn("gf3208:" fmt, ##args)

static int gf3208_request_named_gpio(struct gf_dev *gf_dev, const char *label,
				     int *gpio)
{
	struct device *dev = &gf_dev->spi->dev;
	struct device_node *np = dev->of_node;
	int rc = of_get_named_gpio(np, label, 0);

	if (rc < 0) {
		gf_dbg("failed to get '%s'\n", label);
		return rc;
	}
	*gpio = rc;
	rc = devm_gpio_request(dev, *gpio, label);
	if (rc) {
		gf_dbg("failed to request gpio %d\n", *gpio);
		return rc;
	}
	return 0;
}

#ifdef ENABLE_PINCTRL
static int select_pin_ctl(struct gf_dev *gf_dev, const char *name)
{
	size_t i;
	int rc;

	for (i = 0; i < ARRAY_SIZE(gf_dev->pinctrl_state); i++) {
		const char *n = pctl_names[i];

		if (!strncmp(n, name, strlen(n))) {
			rc = pinctrl_select_state(gf_dev->fingerprint_pinctrl,
						  gf_dev->pinctrl_state[i]);
			if (rc)
				gf_dbg("cannot select '%s'\n", name);
			else
				gf_dbg("selected '%s'\n", name);
			goto exit;
		}
	}
	rc = -EINVAL;
exit:
	return rc;
}
#endif

int gf_parse_dts(struct gf_dev *gf_dev)
{
	int rc = 0;
#ifdef ENABLE_PINCTRL
	int i = 0;
#endif

	/* RST GPIO — DTS: goodix,gpio_rst */
	rc = gf3208_request_named_gpio(gf_dev, "goodix,gpio_rst",
				       &gf_dev->reset_gpio);
	if (rc) {
		gf_dbg("Failed to request gpio_rst, rc = %d\n", rc);
		return -1;
	}

	/* IRQ GPIO — DTS: goodix,gpio_int */
	rc = gf3208_request_named_gpio(gf_dev, "goodix,gpio_int",
				       &gf_dev->irq_gpio);
	if (rc) {
		gf_dbg("Failed to request gpio_int, rc = %d\n", rc);
		return -1;
	}

	/* PWR GPIO — DTS: goodix,gpio_pwr (optional, log but continue) */
	rc = gf3208_request_named_gpio(gf_dev, "goodix,gpio_pwr",
				       &gf_dev->pwr_gpio);
	if (rc) {
		gf_dbg("Failed to request gpio_pwr, rc = %d (optional)\n", rc);
		rc = 0;
	}

	gpio_direction_input(gf_dev->irq_gpio);

#ifdef ENABLE_PINCTRL
	gf_dev->fingerprint_pinctrl = devm_pinctrl_get(&gf_dev->spi->dev);
	if (IS_ERR(gf_dev->fingerprint_pinctrl)) {
		gf_dbg("devm_pinctrl_get failed\n");
		gf_dev->fingerprint_pinctrl = NULL;
		goto skip_pinctrl;
	}

	for (i = 0; i < ARRAY_SIZE(gf_dev->pinctrl_state); i++) {
		const char *n = pctl_names[i];
		struct pinctrl_state *state =
			pinctrl_lookup_state(gf_dev->fingerprint_pinctrl, n);

		if (IS_ERR(state)) {
			gf_dbg("cannot find pinctrl state '%s'\n", n);
			rc = -EINVAL;
		}
		gf_dev->pinctrl_state[i] = state;
	}

	rc = select_pin_ctl(gf_dev, "goodixfp_reset_active");
	if (rc)
		goto skip_pinctrl;
	rc = select_pin_ctl(gf_dev, "goodixfp_irq_active");
	if (rc)
		goto skip_pinctrl;

skip_pinctrl:
	rc = 0; /* pinctrl is optional */
#endif

	pr_info("gf3208: gf_parse_dts OK (IRQ=%d RST=%d PWR=%d)\n",
		gf_dev->irq_gpio, gf_dev->reset_gpio, gf_dev->pwr_gpio);
	return rc;
}

void gf_cleanup(struct gf_dev *gf_dev)
{
	gf_dbg("enter %s\n", __func__);

	if (gpio_is_valid(gf_dev->irq_gpio)) {
		if (gf_dev->irq > 0)
			gf_free_irq(gf_dev);
		devm_gpio_free(&gf_dev->spi->dev, gf_dev->irq_gpio);
		gf_dev->irq_gpio = -EINVAL;
		gf_dev->irq = 0;
	}
	if (gpio_is_valid(gf_dev->reset_gpio)) {
		devm_gpio_free(&gf_dev->spi->dev, gf_dev->reset_gpio);
		gf_dev->reset_gpio = -EINVAL;
	}
	if (gpio_is_valid(gf_dev->pwr_gpio)) {
		devm_gpio_free(&gf_dev->spi->dev, gf_dev->pwr_gpio);
		gf_dev->pwr_gpio = -EINVAL;
	}

#ifdef ENABLE_PINCTRL
	if (gf_dev->fingerprint_pinctrl != NULL) {
		devm_pinctrl_put(gf_dev->fingerprint_pinctrl);
		gf_dev->fingerprint_pinctrl = NULL;
	}
#endif
	gf_dbg("gf_cleanup success\n");
}

int gf_power_on(struct gf_dev *gf_dev)
{
	if (gpio_is_valid(gf_dev->pwr_gpio)) {
		gpio_direction_output(gf_dev->pwr_gpio, 0);
		gpio_set_value(gf_dev->pwr_gpio, 0);
	}
	if (gpio_is_valid(gf_dev->reset_gpio)) {
		gpio_direction_output(gf_dev->reset_gpio, 0);
		gpio_set_value(gf_dev->reset_gpio, 0);
	}
	if (gpio_is_valid(gf_dev->pwr_gpio))
		gpio_set_value(gf_dev->pwr_gpio, 1);
	mdelay(10);
	if (gpio_is_valid(gf_dev->reset_gpio))
		gpio_set_value(gf_dev->reset_gpio, 1);
	pr_info("gf3208: power on ok\n");
	return 0;
}

int gf_power_off(struct gf_dev *gf_dev)
{
	if (gpio_is_valid(gf_dev->pwr_gpio))
		gpio_set_value(gf_dev->pwr_gpio, 0);
	pr_info("gf3208: power off\n");
	return 0;
}

int gf_hw_reset(struct gf_dev *gf_dev, unsigned int delay_ms)
{
	if (gf_dev == NULL) {
		gf_dbg("input is NULL\n");
		return -1;
	}
	if (gpio_is_valid(gf_dev->reset_gpio)) {
		gpio_set_value(gf_dev->reset_gpio, 1);
		mdelay(1);
		gpio_set_value(gf_dev->reset_gpio, 0);
		mdelay(3);
		gpio_set_value(gf_dev->reset_gpio, 1);
	}
	mdelay(delay_ms);
	return 0;
}

int gf_irq_num(struct gf_dev *gf_dev)
{
	if (gf_dev == NULL) {
		gf_dbg("input is NULL\n");
		return -1;
	}
	return gpio_to_irq(gf_dev->irq_gpio);
}
