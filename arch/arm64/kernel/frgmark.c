// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/export.h>
#include <linux/frgmark.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>

#define FRG_IMEM_PA   0x08600040UL
#define FRG_IMEM_LEN  0xC0
#define FRG_MAGIC_HI  0x46524700U
#define FRG_SIG_INIT  0x46524747U
#define FRG_SLOT_SIG   0x00
#define FRG_SLOT_LAST  0x04
#define FRG_SLOT_BASE  0x08
#define FRG_FORCE_PANIC_ARTIFACT "nx549j-frgmark-force-panic"

#ifndef FRGMARK_FORCE_PANIC_STAGE
#define FRGMARK_FORCE_PANIC_STAGE 0
#endif

static void __iomem *frg_imem;
static struct delayed_work frg_heartbeat_work;
static unsigned int frg_heartbeat_count;
static u8 frg_force_panic_stage = FRGMARK_FORCE_PANIC_STAGE;
static bool frg_force_panic_armed = FRGMARK_FORCE_PANIC_STAGE != 0;
static bool frg_force_panic_done;

static const char *frgmark_stage_name(u8 stage)
{
	switch (stage) {
	case FRGMARK_STAGE_IMEM_READY:
		return "imem_ready";
	case FRGMARK_STAGE_SETUP_ARCH_DONE:
		return "setup_arch_done";
	case FRGMARK_STAGE_TRAP_INIT_DONE:
		return "trap_init_done";
	case FRGMARK_STAGE_MM_INIT_DONE:
		return "mm_init_done";
	case FRGMARK_STAGE_SCHED_INIT_DONE:
		return "sched_init_done";
	case FRGMARK_STAGE_SOFTIRQ_INIT_DONE:
		return "softirq_init_done";
	case FRGMARK_STAGE_TIMEKEEPING_DONE:
		return "timekeeping_done";
	case FRGMARK_STAGE_TIME_INIT_DONE:
		return "time_init_done";
	case FRGMARK_STAGE_DRIVER_INIT_BEGIN:
		return "driver_init_begin";
	case FRGMARK_STAGE_DRIVER_INIT_DONE:
		return "driver_init_done";
	case FRGMARK_STAGE_KERNEL_INIT_BEGIN:
		return "kernel_init_begin";
	case FRGMARK_STAGE_KERNEL_INIT_FREEABLE_DONE:
		return "kernel_init_freeable_done";
	case FRGMARK_STAGE_INITCALLS_BEGIN:
		return "initcalls_begin";
	case FRGMARK_STAGE_INITCALLS_DONE:
		return "initcalls_done";
	case FRGMARK_STAGE_KERNEL_FREEABLE_DONE:
		return "kernel_freeable_done";
	case FRGMARK_STAGE_EXEC_RAMDISK_INIT:
		return "exec_ramdisk_init";
	case FRGMARK_STAGE_EXEC_RAMDISK_INIT_FAILED:
		return "exec_ramdisk_init_failed";
	case FRGMARK_STAGE_EXEC_CMDLINE_INIT:
		return "exec_cmdline_init";
	case FRGMARK_STAGE_EXEC_FALLBACK_INIT:
		return "exec_fallback_init";
	case FRGMARK_STAGE_CONSOLE_INIT_DONE:
		return "console_init_done";
	case FRGMARK_STAGE_MSM_DRM_REGISTER_BEGIN:
		return "msm_drm_register_begin";
	case FRGMARK_STAGE_MSM_DRM_REGISTER_DONE:
		return "msm_drm_register_done";
	case FRGMARK_STAGE_MSM_DRM_PROBE_BEGIN:
		return "msm_drm_probe_begin";
	case FRGMARK_STAGE_MSM_DRM_COMPONENTS_DONE:
		return "msm_drm_components_done";
	case FRGMARK_STAGE_MSM_DRM_BIND_BEGIN:
		return "msm_drm_bind_begin";
	case FRGMARK_STAGE_MSM_DRM_DEV_REGISTER_DONE:
		return "msm_drm_dev_register_done";
	case FRGMARK_STAGE_MSM_DRM_SPLASH_DONE:
		return "msm_drm_splash_done";
	case FRGMARK_STAGE_MSM_DRM_FBDEV_DONE:
		return "msm_drm_fbdev_done";
	case FRGMARK_STAGE_MSM_DRM_REGISTER_FAILED:
		return "msm_drm_register_failed";
	case FRGMARK_STAGE_DSI_REGISTER_BEGIN:
		return "dsi_register_begin";
	case FRGMARK_STAGE_DSI_REGISTER_DONE:
		return "dsi_register_done";
	case FRGMARK_STAGE_DSI_DISPLAY_PROBE_BEGIN:
		return "dsi_display_probe_begin";
	case FRGMARK_STAGE_DSI_DISPLAY_ACTIVE_INIT_BEGIN:
		return "dsi_display_active_init_begin";
	case FRGMARK_STAGE_DSI_DISPLAY_ACTIVE_INIT_DONE:
		return "dsi_display_active_init_done";
	case FRGMARK_STAGE_DSI_DISPLAY_COMPONENT_DONE:
		return "dsi_display_component_done";
	case FRGMARK_STAGE_DSI_REGISTER_FAILED:
		return "dsi_register_failed";
	default:
		if (stage >= FRGMARK_STAGE_HEARTBEAT_BASE &&
		    stage <= FRGMARK_STAGE_HEARTBEAT_LAST)
			return "heartbeat";
		return "unknown";
	}
}

static int __init frgmark_force_panic_setup(char *str)
{
	unsigned long value;

	if (!str || !*str)
		return 0;

	value = simple_strtoul(str, NULL, 0);
	if (value > 0xff) {
		pr_emerg("FRGmark: invalid forced panic stage '%s'\n", str);
		return 0;
	}

	frg_force_panic_stage = (u8)value;
	frg_force_panic_armed = frg_force_panic_stage != 0;
	pr_emerg("FRGmark: forced panic %s stage=%02x name=%s artifact=%s\n",
		 frg_force_panic_armed ? "armed" : "disabled",
		 frg_force_panic_stage, frgmark_stage_name(frg_force_panic_stage),
		 FRG_FORCE_PANIC_ARTIFACT);
	return 0;
}
early_param("frgmark.force_panic_stage", frgmark_force_panic_setup);

static void frgmark_maybe_force_panic(u8 stage)
{
	if (!frg_force_panic_armed || frg_force_panic_done ||
	    stage != frg_force_panic_stage)
		return;

	frg_force_panic_done = true;
	pr_emerg("FRGmark: forced panic firing stage=%02x name=%s artifact=%s\n",
		 stage, frgmark_stage_name(stage), FRG_FORCE_PANIC_ARTIFACT);
	panic("FRGmark forced panic stage=%02x name=%s artifact=%s",
	      stage, frgmark_stage_name(stage), FRG_FORCE_PANIC_ARTIFACT);
}

void __init frgmark_init_iomap(void)
{
	if (frg_imem)
		return;
	frg_imem = ioremap(FRG_IMEM_PA, FRG_IMEM_LEN);
	if (!frg_imem) {
		pr_emerg("FRGmark: ioremap(0x%lx) failed\n", FRG_IMEM_PA);
		return;
	}
	__raw_writel(FRG_SIG_INIT, frg_imem + FRG_SLOT_SIG);
	__raw_writel(FRG_MAGIC_HI | 0x01, frg_imem + FRG_SLOT_LAST);
	pr_emerg("FRGmark: IMEM @0x%lx OK; sig=FRGG stage=01 name=%s\n",
		 FRG_IMEM_PA, frgmark_stage_name(FRGMARK_STAGE_IMEM_READY));
}

void frgmark(u8 stage)
{
	u32 v;
	if (!frg_imem)
		return;
	v = FRG_MAGIC_HI | stage;
	__raw_writel(v, frg_imem + FRG_SLOT_LAST);
	__raw_writel(v, frg_imem + FRG_SLOT_BASE + ((stage & 0x1F) << 2));
	pr_emerg("FRGmark stage=%02x name=%s\n", stage,
		 frgmark_stage_name(stage));
	frgmark_maybe_force_panic(stage);
}
EXPORT_SYMBOL(frgmark);

static void frgmark_heartbeat(struct work_struct *work)
{
	u8 stage = FRGMARK_STAGE_HEARTBEAT_BASE + frg_heartbeat_count;

	frgmark(stage);
	if (frg_heartbeat_count < FRGMARK_STAGE_HEARTBEAT_LAST -
				  FRGMARK_STAGE_HEARTBEAT_BASE)
		frg_heartbeat_count++;

	schedule_delayed_work(&frg_heartbeat_work, 10 * HZ);
}

static int __init frgmark_late_init(void)
{
	if (!frg_imem)
		return 0;

	INIT_DELAYED_WORK(&frg_heartbeat_work, frgmark_heartbeat);
	schedule_delayed_work(&frg_heartbeat_work, 10 * HZ);
	return 0;
}
late_initcall(frgmark_late_init);
