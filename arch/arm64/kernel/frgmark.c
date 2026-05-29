// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/kmsg_dump.h>
#include <linux/mm.h>
#include <linux/sizes.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/export.h>
#include <linux/frgmark.h>
#include <linux/input/qpnp-power-on.h>
#include <linux/jiffies.h>
#include <linux/notifier.h>
#include <linux/nx549j_splashprobe.h>
#include <linux/reboot.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <soc/qcom/watchdog.h>
#include <asm/early_ioremap.h>

#define FRG_IMEM_PA   0x08600040UL
#define FRG_IMEM_LEN  0xC0
#define FRG_RAMOOPS_PA 0x9ff00000UL
#define FRG_RAMOOPS_MARKER_LEN SZ_4K
#define FRG_PERSISTENT_RAM_SIG 0x43474244U
#define FRG_RAMOOPS_OLD_SIZE 0x200U
#define FRG_RAMOOPS_SLOT_LATEST 0x4c
#define FRG_RAMOOPS_SLOT_BASE 0x8c
#define FRG_RESTART_REASON_PA 0x0860065cUL
#define FRG_RESTART_REASON_LEN 0x4
#define FRG_RESTART_REASON_RECOVERY 0x77665502U
#define FRG_RESTART_REASON_CLEAR 0x0U
#define FRG_WDT_PA 0x0b017000UL
#define FRG_WDT_LEN 0x1000
#define FRG_WDT_HZ 32765UL
#define FRG_WDT_RST 0x04
#define FRG_WDT_EN 0x08
#define FRG_WDT_BARK_TIME 0x10
#define FRG_WDT_BITE_TIME 0x14
#define FRG_PSHOLD_PA 0x004ab000UL
#define FRG_PSHOLD_LEN 0x4
#define FRG_BCB_SIZE 2048
#define FRG_BCB_DIAG_OFF 1024
#define FRG_BCB_DIAG_LEN 256
#define FRG_BCB_MISC_MAJOR 179U
#define FRG_BCB_MISC_MINOR 28U
#define FRG_MAGIC_HI  0x46524700U
#define FRG_SIG_INIT  0x46524747U
#define FRG_SLOT_SIG   0x00
#define FRG_SLOT_LAST  0x04
#define FRG_SLOT_BASE  0x08
#define FRG_FORCE_PANIC_ARTIFACT "nx549j-frgmark-force-panic"
#define FRG_FORCE_RESET_ARTIFACT "nx549j-frgmark-kernel-restart-recovery"
#define FRG_RECOVERY_TIMEOUT_ARTIFACT "nx549j-frgmark-recovery-timeout"
#define FRG_RECOVERY_SELECTOR_REFRESH_SEC 5U
#define FRG_RECOVERY_BCB_RETRY_SEC 5U
#define FRG_RECOVERY_NO_BCB_GRACE_SEC 60U
#define FRG_PANIC_REBOOT_SEC 5

#ifndef FRGMARK_FORCE_PANIC_STAGE
#define FRGMARK_FORCE_PANIC_STAGE 0
#endif

static void __iomem *frg_imem;
static void __iomem *frg_ramoops;
static void __iomem *frg_restart_reason;
static void __iomem *frg_pshold;
static struct delayed_work frg_heartbeat_work;
static struct delayed_work frg_recovery_bcb_work;
static struct delayed_work frg_recovery_timeout_work;
static struct delayed_work frg_recovery_selector_work;
static struct timer_list frg_recovery_timeout_timer;
static unsigned int frg_heartbeat_count;
static unsigned int frg_recovery_timeout_sec;
static u8 frg_force_panic_stage = FRGMARK_FORCE_PANIC_STAGE;
static u8 frg_force_reset_stage;
static bool frg_force_panic_armed = FRGMARK_FORCE_PANIC_STAGE != 0;
static bool frg_force_reset_armed;
static bool frg_recovery_timeout_armed;
static bool frg_recovery_bcb_work_armed;
static bool frg_recovery_timeout_work_armed;
static bool frg_recovery_userspace_done;
static bool frg_force_panic_done;
static bool frg_force_reset_done;
static bool frg_recovery_timeout_done;
static bool frg_panic_notifier_registered;
static bool frg_recovery_timeout_timer_armed;
static bool frg_recovery_bcb_checkpoint_requested;
static bool frg_early_wdt_programmed;
static bool frg_raw_wdt_enabled;
static bool frg_recovery_bcb_written;
static bool frg_recovery_bcb_cleared;
static bool frg_recovery_bcb_disabled;
static bool frg_panic_reboot_enabled;
static unsigned long frg_recovery_no_bcb_reset_jiffies;
static u8 frg_last_stage;
static unsigned int frg_bcb_misc_major = FRG_BCB_MISC_MAJOR;
static unsigned int frg_bcb_misc_minor = FRG_BCB_MISC_MINOR;

static void frgmark_prime_recovery_selectors(const char *reason);
static void frgmark_arm_early_recovery_guard(const char *reason);
static int frgmark_write_recovery_bcb(const char *reason);
static int frgmark_clear_recovery_bcb(const char *reason);

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
	case FRGMARK_STAGE_HEAD_ENTRY:
		return "head_entry";
	case FRGMARK_STAGE_HEAD_ARGS_PRESERVED:
		return "head_args_preserved";
	case FRGMARK_STAGE_HEAD_EL2_DONE:
		return "head_el2_done";
	case FRGMARK_STAGE_HEAD_PAGETABLES_DONE:
		return "head_pagetables_done";
	case FRGMARK_STAGE_HEAD_CPU_SETUP_DONE:
		return "head_cpu_setup_done";
	case FRGMARK_STAGE_HEAD_PRIMARY_SWITCH:
		return "head_primary_switch";
	case FRGMARK_STAGE_SETUP_EARLY_IOREMAP_READY:
		return "setup_early_ioremap_ready";
	case FRGMARK_STAGE_SETUP_FDT_DONE:
		return "setup_fdt_done";
	case FRGMARK_STAGE_SETUP_EARLY_PARAM_DONE:
		return "setup_early_param_done";
	case FRGMARK_STAGE_SETUP_MEMBLOCK_DONE:
		return "setup_memblock_done";
	case FRGMARK_STAGE_SETUP_PAGING_DONE:
		return "setup_paging_done";
	case FRGMARK_STAGE_SETUP_BEFORE_IOREMAP_RESET:
		return "setup_before_ioremap_reset";
	case FRGMARK_STAGE_INITCALL_EARLY_DONE:
		return "initcall_early_done";
	case FRGMARK_STAGE_INITCALL_CORE_DONE:
		return "initcall_core_done";
	case FRGMARK_STAGE_INITCALL_POSTCORE_DONE:
		return "initcall_postcore_done";
	case FRGMARK_STAGE_INITCALL_ARCH_DONE:
		return "initcall_arch_done";
	case FRGMARK_STAGE_INITCALL_SUBSYS_DONE:
		return "initcall_subsys_done";
	case FRGMARK_STAGE_INITCALL_FS_DONE:
		return "initcall_fs_done";
	case FRGMARK_STAGE_INITCALL_DEVICE_DONE:
		return "initcall_device_done";
	case FRGMARK_STAGE_INITCALL_LATE_DONE:
		return "initcall_late_done";
	case FRGMARK_STAGE_RECOVERY_NO_BCB_GRACE:
		return "recovery_no_bcb_grace";
	case FRGMARK_STAGE_RECOVERY_NO_BCB_RESET:
		return "recovery_no_bcb_reset";
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

static int __init frgmark_force_reset_setup(char *str)
{
	unsigned long value;

	if (!str || !*str)
		return 0;

	value = simple_strtoul(str, NULL, 0);
	if (value > 0xff) {
		pr_emerg("FRGmark: invalid forced reset stage '%s'\n", str);
		return 0;
	}

	frg_force_reset_stage = (u8)value;
	frg_force_reset_armed = frg_force_reset_stage != 0;
	pr_emerg("FRGmark: forced reset %s stage=%02x name=%s artifact=%s\n",
		 frg_force_reset_armed ? "armed" : "disabled",
		 frg_force_reset_stage, frgmark_stage_name(frg_force_reset_stage),
		 FRG_FORCE_RESET_ARTIFACT);
	return 0;
}
early_param("frgmark.force_reset_stage", frgmark_force_reset_setup);

static int __init frgmark_recovery_timeout_setup(char *str)
{
	unsigned long value;

	if (!str || !*str)
		return 0;

	value = simple_strtoul(str, NULL, 0);
	if (!value || value > 3600) {
		pr_emerg("FRGmark: invalid recovery timeout '%s'\n", str);
		return 0;
	}

	frg_recovery_timeout_sec = (unsigned int)value;
	frg_recovery_timeout_armed = true;
	pr_emerg("FRGmark: recovery timeout armed seconds=%u panic_reboot_waits_for_bcb=1 artifact=%s\n",
		 frg_recovery_timeout_sec, FRG_RECOVERY_TIMEOUT_ARTIFACT);
	frgmark_arm_early_recovery_guard("early-param");
	return 0;
}
early_param("frgmark.recovery_timeout_sec", frgmark_recovery_timeout_setup);

static int __init frgmark_raw_wdt_setup(char *str)
{
	unsigned long value;

	if (!str || !*str)
		return 0;

	value = simple_strtoul(str, NULL, 0);
	frg_raw_wdt_enabled = value != 0;
	pr_emerg("FRGmark: raw WDT %s artifact=%s\n",
		 frg_raw_wdt_enabled ? "enabled" : "disabled",
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	return 0;
}
early_param("frgmark.raw_wdt", frgmark_raw_wdt_setup);

static int __init frgmark_bcb_misc_devt_setup(char *str)
{
	unsigned int major, minor;
	char tail;

	if (!str || !*str)
		return 0;

	if (sscanf(str, "%u:%u%c", &major, &minor, &tail) != 2) {
		pr_emerg("FRGmark: invalid BCB misc devt '%s'\n", str);
		return 0;
	}

	if (!major && !minor) {
		frg_recovery_bcb_disabled = true;
		pr_emerg("FRGmark: BCB write disabled artifact=%s\n",
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		return 0;
	}

	frg_bcb_misc_major = major;
	frg_bcb_misc_minor = minor;
	pr_emerg("FRGmark: BCB misc devt set to %u:%u artifact=%s\n",
		 frg_bcb_misc_major, frg_bcb_misc_minor,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	return 0;
}
early_param("frgmark.bcb_misc_devt", frgmark_bcb_misc_devt_setup);

static void frgmark_write_imem_record(void __iomem *imem, u8 stage)
{
	u32 v = FRG_MAGIC_HI | stage;

	if (!imem)
		return;

	__raw_writel(FRG_SIG_INIT, imem + FRG_SLOT_SIG);
	__raw_writel(v, imem + FRG_SLOT_LAST);
	__raw_writel(v, imem + FRG_SLOT_BASE + ((stage & 0x1F) << 2));
	mb();
}

static void frgmark_write_ramoops_record(void __iomem *ramoops, u8 stage)
{
	u32 v = FRG_MAGIC_HI | stage;
	unsigned int slot;

	if (!ramoops)
		return;

	__raw_writel(FRG_PERSISTENT_RAM_SIG, ramoops);
	__raw_writel(0, ramoops + 4);
	__raw_writel(FRG_RAMOOPS_OLD_SIZE, ramoops + 8);
	__raw_writel(0x3d3d3d3d, ramoops + 0x0c); /* "====" */
	__raw_writel(0x2d302e30, ramoops + 0x10); /* "0.0-" */
	__raw_writel(0x52460a44, ramoops + 0x14); /* "D\nFR" */
	__raw_writel(0x4e0a4547, ramoops + 0x18); /* "GE\nN" */
	__raw_writel(0x39343558, ramoops + 0x1c); /* "X549" */
	__raw_writel(0x6568204a, ramoops + 0x20); /* "J he" */
	__raw_writel(0x6d206461, ramoops + 0x24); /* "ad m" */
	__raw_writel(0x656b7261, ramoops + 0x28); /* "arke" */
	__raw_writel(0x00000a72, ramoops + 0x2c); /* "r\n" */
	__raw_writel(v, ramoops + FRG_RAMOOPS_SLOT_LATEST);

	if (stage >= FRGMARK_STAGE_HEAD_ENTRY &&
	    stage <= FRGMARK_STAGE_SETUP_BEFORE_IOREMAP_RESET)
		slot = FRG_RAMOOPS_SLOT_BASE +
		       ((stage - FRGMARK_STAGE_HEAD_ENTRY) << 2);
	else
		slot = FRG_RAMOOPS_SLOT_BASE + ((stage & 0x1f) << 2);

	if (slot + sizeof(u32) <= FRG_RAMOOPS_OLD_SIZE)
		__raw_writel(v, ramoops + slot);
	mb();
}

void __init frgmark_early(u8 stage)
{
	void __iomem *imem;
	void __iomem *ramoops;

	imem = early_ioremap(FRG_IMEM_PA, FRG_IMEM_LEN);
	if (imem) {
		frgmark_write_imem_record(imem, stage);
		early_iounmap(imem, FRG_IMEM_LEN);
	}

	ramoops = early_ioremap(FRG_RAMOOPS_PA, FRG_RAMOOPS_MARKER_LEN);
	if (ramoops) {
		frgmark_write_ramoops_record(ramoops, stage);
		early_iounmap(ramoops, FRG_RAMOOPS_MARKER_LEN);
	}

	pr_emerg("FRGmark early stage=%02x name=%s imem=%d ramoops=%d\n",
		 stage, frgmark_stage_name(stage), !!imem, !!ramoops);
}

static void frgmark_program_early_wdt(unsigned int seconds)
{
	void __iomem *wdt;
	unsigned long bark_ticks;
	unsigned long bite_ticks;

	wdt = ioremap(FRG_WDT_PA, FRG_WDT_LEN);
	if (!wdt) {
		pr_emerg("FRGmark: early WDT ioremap(0x%lx) failed\n",
			 FRG_WDT_PA);
		return;
	}

	bark_ticks = FRG_WDT_HZ * seconds;
	bite_ticks = FRG_WDT_HZ * (seconds + 3);
	__raw_writel(0, wdt + FRG_WDT_EN);
	__raw_writel(1, wdt + FRG_WDT_RST);
	__raw_writel(bark_ticks, wdt + FRG_WDT_BARK_TIME);
	__raw_writel(bite_ticks, wdt + FRG_WDT_BITE_TIME);
	__raw_writel(1, wdt + FRG_WDT_EN);
	__raw_writel(1, wdt + FRG_WDT_RST);
	mb();
	iounmap(wdt);
	pr_emerg("FRGmark: early raw WDT armed seconds=%u bark=0x%lx bite=0x%lx artifact=%s\n",
		 seconds, bark_ticks, bite_ticks, FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static void frgmark_enable_recovery_panic_reboot(const char *reason)
{
	if (frg_panic_reboot_enabled)
		return;

	if (panic_timeout == 0 || panic_timeout > FRG_PANIC_REBOOT_SEC)
		panic_timeout = FRG_PANIC_REBOOT_SEC;
	panic_on_oops = 1;
	frg_panic_reboot_enabled = true;
	pr_emerg("FRGmark: panic recovery reboot enabled reason=%s panic_timeout=%d panic_on_oops=%d artifact=%s\n",
		 reason ? reason : "unknown", panic_timeout, panic_on_oops,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static int frgmark_panic_notify(struct notifier_block *this,
				unsigned long event, void *ptr)
{
	frgmark_prime_recovery_selectors("panic");
	return NOTIFY_DONE;
}

static struct notifier_block frgmark_panic_nb = {
	.notifier_call = frgmark_panic_notify,
	.priority = INT_MAX,
};

static void frgmark_register_panic_notifier(void)
{
	if (frg_panic_notifier_registered)
		return;
	atomic_notifier_chain_register(&panic_notifier_list,
				       &frgmark_panic_nb);
	frg_panic_notifier_registered = true;
}

static void frgmark_arm_early_recovery_guard(const char *reason)
{
	if (!frg_recovery_timeout_armed)
		return;

	frgmark_register_panic_notifier();
	frgmark_prime_recovery_selectors(reason);
	if (!frg_raw_wdt_enabled)
		return;
	if (frg_early_wdt_programmed)
		return;

	frgmark_program_early_wdt(frg_recovery_timeout_sec);
	frg_early_wdt_programmed = true;
}

static void frgmark_fill_recovery_bcb(void *buf, const char *reason)
{
	u8 stage = frg_last_stage;

	memset(buf, 0, FRG_BCB_SIZE);
	memcpy(buf, "boot-recovery", sizeof("boot-recovery") - 1);
	memcpy(buf + 64, "recovery\n--reason=nx549j-frgmark-timeout\n",
	       sizeof("recovery\n--reason=nx549j-frgmark-timeout\n") - 1);
	snprintf(buf + FRG_BCB_DIAG_OFF, FRG_BCB_DIAG_LEN,
		 "FRGMARK-BCB-v1\nartifact=%s\nreason=%s\nlast_stage=0x%02x\nlast_name=%s\njiffies=%lu\n",
		 FRG_RECOVERY_TIMEOUT_ARTIFACT,
		 reason ? reason : "unknown",
		 stage, frgmark_stage_name(stage), jiffies);
}

static void frgmark_fill_empty_bcb(void *buf)
{
	memset(buf, 0, FRG_BCB_SIZE);
}

static int frgmark_write_bcb_bio(struct block_device *bdev, void *buf)
{
	struct bio *bio;
	unsigned int added;
	int ret;

	bio = bio_alloc(GFP_KERNEL, 1);
	if (!bio)
		return -ENOMEM;

	bio->bi_bdev = bdev;
	bio->bi_iter.bi_sector = 0;
	bio_set_op_attrs(bio, REQ_OP_WRITE, WRITE_SYNC);
	added = bio_add_page(bio, virt_to_page(buf), FRG_BCB_SIZE,
			     offset_in_page(buf));
	if (added < FRG_BCB_SIZE) {
		bio_put(bio);
		return -EIO;
	}

	ret = submit_bio_wait(bio);
	bio_put(bio);
	return ret;
}

static int frgmark_write_bcb_page(const char *reason, bool recovery)
{
	struct block_device *bdev;
	void *buf;
	dev_t dev;
	int ret;
	int flush_ret;

	if (recovery && frg_recovery_bcb_written)
		return 0;
	if (!recovery && frg_recovery_bcb_cleared)
		return 0;
	if (frg_recovery_bcb_disabled)
		return -EOPNOTSUPP;

	buf = (void *)__get_free_page(GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (recovery)
		frgmark_fill_recovery_bcb(buf, reason);
	else
		frgmark_fill_empty_bcb(buf);
	dev = MKDEV(frg_bcb_misc_major, frg_bcb_misc_minor);
	bdev = blkdev_get_by_dev(dev, FMODE_WRITE, frgmark_write_recovery_bcb);
	if (IS_ERR(bdev)) {
		ret = PTR_ERR(bdev);
		free_page((unsigned long)buf);
		pr_emerg("FRGmark: BCB %s open failed reason=%s devt=%u:%u ret=%d artifact=%s\n",
			 recovery ? "recovery" : "clear",
			 reason ? reason : "unknown", frg_bcb_misc_major,
			 frg_bcb_misc_minor, ret, FRG_RECOVERY_TIMEOUT_ARTIFACT);
		return ret;
	}

	ret = frgmark_write_bcb_bio(bdev, buf);
	if (!ret) {
		flush_ret = blkdev_issue_flush(bdev, GFP_KERNEL, NULL);
		if (flush_ret)
			ret = flush_ret;
	}
	blkdev_put(bdev, FMODE_WRITE);
	free_page((unsigned long)buf);

	if (ret) {
		pr_emerg("FRGmark: BCB %s write failed reason=%s devt=%u:%u ret=%d artifact=%s\n",
			 recovery ? "recovery" : "clear",
			 reason ? reason : "unknown", frg_bcb_misc_major,
			 frg_bcb_misc_minor, ret, FRG_RECOVERY_TIMEOUT_ARTIFACT);
		return ret;
	}

	if (recovery) {
		frg_recovery_bcb_written = true;
		pr_emerg("FRGmark: BCB recovery command written reason=%s devt=%u:%u artifact=%s\n",
			 reason ? reason : "unknown", frg_bcb_misc_major,
			 frg_bcb_misc_minor, FRG_RECOVERY_TIMEOUT_ARTIFACT);
		frgmark_enable_recovery_panic_reboot(reason);
	} else {
		frg_recovery_bcb_cleared = true;
		pr_emerg("FRGmark: BCB command cleared reason=%s devt=%u:%u artifact=%s\n",
			 reason ? reason : "unknown", frg_bcb_misc_major,
			 frg_bcb_misc_minor, FRG_RECOVERY_TIMEOUT_ARTIFACT);
	}
	return 0;
}

static int frgmark_write_recovery_bcb(const char *reason)
{
	return frgmark_write_bcb_page(reason, true);
}

static int frgmark_clear_recovery_bcb(const char *reason)
{
	return frgmark_write_bcb_page(reason, false);
}

static bool frgmark_drop_mapped_pshold(const char *reason)
{
	if (!frg_pshold) {
		pr_emerg("FRGmark: mapped PS_HOLD unavailable reason=%s artifact=%s\n",
			 reason ? reason : "unknown",
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		return false;
	}

	pr_emerg("FRGmark: dropping PS_HOLD reason=%s artifact=%s\n",
		 reason ? reason : "unknown", FRG_RECOVERY_TIMEOUT_ARTIFACT);
	__raw_writel(0, frg_pshold);
	mb();
	return true;
}

static void frgmark_direct_pshold_reset(const char *reason)
{
	void __iomem *pshold;

	if (frgmark_drop_mapped_pshold(reason)) {
		mdelay(1000);
		return;
	}

	pshold = ioremap(FRG_PSHOLD_PA, FRG_PSHOLD_LEN);
	if (!pshold) {
		pr_emerg("FRGmark: PS_HOLD ioremap failed reason=%s artifact=%s\n",
			 reason ? reason : "unknown",
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		return;
	}

	pr_emerg("FRGmark: dropping temporary PS_HOLD reason=%s artifact=%s\n",
		 reason ? reason : "unknown", FRG_RECOVERY_TIMEOUT_ARTIFACT);
	__raw_writel(0, pshold);
	mb();
	mdelay(1000);
	iounmap(pshold);
}

static void frgmark_prime_recovery_selectors(const char *reason)
{
	int pon_rc;

	if (frg_restart_reason) {
		__raw_writel(FRG_RESTART_REASON_RECOVERY, frg_restart_reason);
		mb();
	}

	pon_rc = qpnp_pon_set_restart_reason(PON_RESTART_REASON_RECOVERY);
	pr_emerg("FRGmark: recovery selector reason=%s imem=%p pon_rc=%d artifact=%s\n",
		 reason ? reason : "unknown", frg_restart_reason, pon_rc,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static void frgmark_prime_recovery_imem(const char *reason)
{
	if (frg_restart_reason) {
		__raw_writel(FRG_RESTART_REASON_RECOVERY, frg_restart_reason);
		mb();
	}

	pr_emerg("FRGmark: recovery imem selector reason=%s imem=%p artifact=%s\n",
		 reason ? reason : "unknown", frg_restart_reason,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static void frgmark_timeout_marker(u8 stage)
{
	frg_last_stage = stage;
	if (frg_imem)
		frgmark_write_imem_record(frg_imem, stage);
	if (frg_ramoops)
		frgmark_write_ramoops_record(frg_ramoops, stage);
}

static void frgmark_clear_recovery_selectors(const char *reason)
{
	int pon_rc;

	if (frg_restart_reason) {
		__raw_writel(FRG_RESTART_REASON_CLEAR, frg_restart_reason);
		mb();
	}

	pon_rc = qpnp_pon_set_restart_reason(PON_RESTART_REASON_UNKNOWN);
	pr_emerg("FRGmark: recovery selector clear reason=%s imem=%p pon_rc=%d artifact=%s\n",
		 reason ? reason : "unknown", frg_restart_reason, pon_rc,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static bool frgmark_no_bcb_grace_expired(void)
{
	return time_after_eq(jiffies, frg_recovery_no_bcb_reset_jiffies);
}

static void frgmark_no_bcb_fallback_reset(const char *reason, bool can_sleep)
{
	frg_recovery_timeout_done = true;
	frgmark_timeout_marker(FRGMARK_STAGE_RECOVERY_NO_BCB_RESET);
	if (can_sleep)
		frgmark_prime_recovery_selectors(reason);
	else
		frgmark_prime_recovery_imem(reason);
	pr_emerg("FRGmark: no-BCB fallback reset reason=%s timeout=%u grace=%u artifact=%s\n",
		 reason ? reason : "unknown", frg_recovery_timeout_sec,
		 FRG_RECOVERY_NO_BCB_GRACE_SEC, FRG_RECOVERY_TIMEOUT_ARTIFACT);
	kmsg_dump(KMSG_DUMP_PANIC);

	if (can_sleep) {
		kernel_restart("recovery");
		pr_emerg("FRGmark: no-BCB kernel_restart returned, forcing raw reset artifact=%s\n",
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		frgmark_direct_pshold_reset("no-bcb-after-kernel-restart");
		msm_trigger_wdog_bite();
		return;
	}

	if (!frgmark_drop_mapped_pshold("no-bcb-timer-timeout"))
		msm_trigger_wdog_bite();
}

static void frgmark_recovery_selector_refresh(struct work_struct *work)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done ||
	    frg_recovery_timeout_done)
		return;

	frgmark_prime_recovery_selectors("selector-refresh");
	schedule_delayed_work(&frg_recovery_selector_work,
			      FRG_RECOVERY_SELECTOR_REFRESH_SEC * HZ);
}

static void frgmark_recovery_bcb_retry(struct work_struct *work)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done ||
	    frg_recovery_timeout_done)
		return;

	if (!frg_recovery_bcb_written) {
		frgmark_write_recovery_bcb("bcb-retry");
		if (frg_recovery_bcb_written)
			frgmark_prime_recovery_selectors("bcb-retry-written");
	}

	if (!frg_recovery_bcb_written)
		schedule_delayed_work(&frg_recovery_bcb_work,
				      FRG_RECOVERY_BCB_RETRY_SEC * HZ);
}

static void frgmark_recovery_timeout_fire(struct work_struct *work)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done ||
	    frg_recovery_timeout_done)
		return;

	frgmark_write_recovery_bcb("timeout");
	if (!frg_recovery_bcb_written) {
		frgmark_timeout_marker(FRGMARK_STAGE_RECOVERY_NO_BCB_GRACE);
		if (frgmark_no_bcb_grace_expired()) {
			frgmark_no_bcb_fallback_reset("timeout-no-bcb", true);
			return;
		}
		pr_emerg("FRGmark: recovery timeout defers reset; no BCB recovery selector written grace=%u artifact=%s\n",
			 FRG_RECOVERY_NO_BCB_GRACE_SEC,
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		schedule_delayed_work(&frg_recovery_timeout_work,
				      FRG_RECOVERY_BCB_RETRY_SEC * HZ);
		return;
	}

	frg_recovery_timeout_done = true;
	frgmark_prime_recovery_selectors("timeout");
	pr_emerg("FRGmark: recovery timeout firing seconds=%u artifact=%s\n",
		 frg_recovery_timeout_sec, FRG_RECOVERY_TIMEOUT_ARTIFACT);
	kmsg_dump(KMSG_DUMP_PANIC);
	kernel_restart("recovery");
	pr_emerg("FRGmark: kernel_restart(\"recovery\") returned, forcing watchdog bite artifact=%s\n",
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	if (frg_recovery_bcb_written) {
		frgmark_direct_pshold_reset("timeout-after-kernel-restart");
		msm_trigger_wdog_bite();
		return;
	}

	pr_emerg("FRGmark: refusing watchdog bite after timeout; no BCB recovery selector written artifact=%s\n",
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

static void frgmark_recovery_timeout_timer_fire(unsigned long data)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done ||
	    frg_recovery_timeout_done)
		return;

	frgmark_prime_recovery_imem("timer-timeout");
	pr_emerg("FRGmark: recovery timer firing seconds=%u bcb_written=%d artifact=%s\n",
		 frg_recovery_timeout_sec, frg_recovery_bcb_written,
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	if (!frg_recovery_bcb_written) {
		frgmark_timeout_marker(FRGMARK_STAGE_RECOVERY_NO_BCB_GRACE);
		if (frgmark_no_bcb_grace_expired()) {
			frgmark_no_bcb_fallback_reset("timer-no-bcb", false);
			return;
		}
		pr_emerg("FRGmark: recovery timer defers reset; no BCB recovery selector written grace=%u artifact=%s\n",
			 FRG_RECOVERY_NO_BCB_GRACE_SEC,
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
		mod_timer(&frg_recovery_timeout_timer,
			  jiffies + FRG_RECOVERY_BCB_RETRY_SEC * HZ);
		return;
	}

	frg_recovery_timeout_done = true;
	if (!frgmark_drop_mapped_pshold("timer-timeout"))
		msm_trigger_wdog_bite();
}

static void frgmark_maybe_checkpoint_bcb(u8 stage)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done ||
	    frg_recovery_timeout_done || frg_recovery_bcb_written)
		return;

	if (stage < FRGMARK_STAGE_INITCALL_EARLY_DONE ||
	    stage > FRGMARK_STAGE_INITCALL_LATE_DONE)
		return;

	if (!frg_recovery_bcb_work_armed)
		return;

	mod_delayed_work(system_wq, &frg_recovery_bcb_work, 0);
	if (!frg_recovery_bcb_checkpoint_requested) {
		frg_recovery_bcb_checkpoint_requested = true;
		pr_emerg("FRGmark: BCB checkpoint write queued stage=%02x name=%s artifact=%s\n",
			 stage, frgmark_stage_name(stage),
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	}
}

void __init frgmark_recovery_timeout_arm(void)
{
	if (!frg_recovery_timeout_armed || frg_recovery_timeout_work_armed)
		return;

	frgmark_register_panic_notifier();
	frgmark_prime_recovery_selectors("arm");
	frg_recovery_no_bcb_reset_jiffies =
		jiffies + (frg_recovery_timeout_sec +
			   FRG_RECOVERY_NO_BCB_GRACE_SEC) * HZ;
	INIT_DELAYED_WORK(&frg_recovery_bcb_work, frgmark_recovery_bcb_retry);
	INIT_DELAYED_WORK(&frg_recovery_selector_work,
			  frgmark_recovery_selector_refresh);
	INIT_DELAYED_WORK(&frg_recovery_timeout_work,
			  frgmark_recovery_timeout_fire);
	if (!frg_recovery_timeout_timer_armed) {
		setup_timer(&frg_recovery_timeout_timer,
			    frgmark_recovery_timeout_timer_fire, 0);
		mod_timer(&frg_recovery_timeout_timer,
			  jiffies + frg_recovery_timeout_sec * HZ);
		frg_recovery_timeout_timer_armed = true;
		pr_emerg("FRGmark: recovery timer armed seconds=%u artifact=%s\n",
			 frg_recovery_timeout_sec,
			 FRG_RECOVERY_TIMEOUT_ARTIFACT);
	}
	schedule_delayed_work(&frg_recovery_selector_work,
			      FRG_RECOVERY_SELECTOR_REFRESH_SEC * HZ);
	schedule_delayed_work(&frg_recovery_bcb_work, HZ);
	frg_recovery_bcb_work_armed = true;
	schedule_delayed_work(&frg_recovery_timeout_work,
			      frg_recovery_timeout_sec * HZ);
	frg_recovery_timeout_work_armed = true;
	pr_emerg("FRGmark: recovery timeout work armed seconds=%u artifact=%s\n",
		 frg_recovery_timeout_sec, FRG_RECOVERY_TIMEOUT_ARTIFACT);
}

void frgmark_userspace_reached(void)
{
	if (!frg_recovery_timeout_armed || frg_recovery_userspace_done)
		return;

	frg_recovery_userspace_done = true;
	if (frg_recovery_timeout_timer_armed)
		del_timer_sync(&frg_recovery_timeout_timer);
	cancel_delayed_work_sync(&frg_recovery_selector_work);
	if (frg_recovery_bcb_work_armed)
		cancel_delayed_work_sync(&frg_recovery_bcb_work);
	cancel_delayed_work_sync(&frg_recovery_timeout_work);
	frgmark_clear_recovery_bcb("userspace-reached");
	frgmark_clear_recovery_selectors("userspace-reached");
	pr_emerg("FRGmark: userspace reached, recovery timeout disarmed artifact=%s\n",
		 FRG_RECOVERY_TIMEOUT_ARTIFACT);
}
EXPORT_SYMBOL(frgmark_userspace_reached);

static void frgmark_maybe_force_reset(u8 stage)
{
	if (!frg_force_reset_armed || frg_force_reset_done ||
	    stage != frg_force_reset_stage)
		return;

	frg_force_reset_done = true;
	pr_emerg("FRGmark: forced reset firing stage=%02x name=%s artifact=%s\n",
		 stage, frgmark_stage_name(stage), FRG_FORCE_RESET_ARTIFACT);

	if (stage < FRGMARK_STAGE_INITCALL_POSTCORE_DONE) {
		pr_emerg("FRGmark: refusing kernel_restart before postcore initcalls stage=%02x\n",
			 stage);
		return;
	}
	if (frg_recovery_timeout_armed && !frg_recovery_bcb_written) {
		pr_emerg("FRGmark: refusing forced recovery reset stage=%02x; no BCB recovery selector written artifact=%s\n",
			 stage, FRG_FORCE_RESET_ARTIFACT);
		return;
	}

	pr_emerg("FRGmark: calling kernel_restart(\"recovery\") stage=%02x\n",
		 stage);
	kernel_restart("recovery");
	pr_emerg("FRGmark: kernel_restart(\"recovery\") returned unexpectedly\n");
}

static void frgmark_maybe_force_panic(u8 stage)
{
	if (!frg_force_panic_armed || frg_force_panic_done ||
	    stage != frg_force_panic_stage)
		return;

	frg_force_panic_done = true;
	if (frg_recovery_timeout_armed && !frg_recovery_bcb_written) {
		pr_emerg("FRGmark: refusing forced panic stage=%02x name=%s; no BCB recovery selector written artifact=%s\n",
			 stage, frgmark_stage_name(stage),
			 FRG_FORCE_PANIC_ARTIFACT);
		return;
	}

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
	frg_restart_reason = ioremap(FRG_RESTART_REASON_PA,
				     FRG_RESTART_REASON_LEN);
	if (!frg_restart_reason)
		pr_emerg("FRGmark: restart_reason ioremap(0x%lx) failed\n",
			 FRG_RESTART_REASON_PA);
	frg_pshold = ioremap(FRG_PSHOLD_PA, FRG_PSHOLD_LEN);
	if (!frg_pshold)
		pr_emerg("FRGmark: pshold ioremap(0x%lx) failed\n",
			 FRG_PSHOLD_PA);
	frg_ramoops = ioremap(FRG_RAMOOPS_PA, FRG_RAMOOPS_MARKER_LEN);
	if (!frg_ramoops)
		pr_emerg("FRGmark: ramoops marker ioremap(0x%lx) failed\n",
			 FRG_RAMOOPS_PA);
	frg_last_stage = FRGMARK_STAGE_IMEM_READY;
	frgmark_write_imem_record(frg_imem, FRGMARK_STAGE_IMEM_READY);
	frgmark_write_ramoops_record(frg_ramoops, FRGMARK_STAGE_IMEM_READY);
	pr_emerg("FRGmark: IMEM @0x%lx OK; ramoops @0x%lx stage=01 name=%s\n",
		 FRG_IMEM_PA, FRG_RAMOOPS_PA,
		 frgmark_stage_name(FRGMARK_STAGE_IMEM_READY));
	frgmark_arm_early_recovery_guard("early-imem");
}

void frgmark(u8 stage)
{
	frg_last_stage = stage;
	if (!frg_imem) {
		frgmark_write_ramoops_record(frg_ramoops, stage);
		frgmark_maybe_force_reset(stage);
		frgmark_maybe_force_panic(stage);
		return;
	}
	frgmark_write_imem_record(frg_imem, stage);
	frgmark_write_ramoops_record(frg_ramoops, stage);
	pr_emerg("FRGmark stage=%02x name=%s\n", stage,
		 frgmark_stage_name(stage));
	nx549j_splashprobe_frg_stage(stage);
	frgmark_maybe_checkpoint_bcb(stage);
	frgmark_maybe_force_reset(stage);
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

	if (frg_recovery_timeout_armed && frg_recovery_bcb_work_armed)
		mod_delayed_work(system_wq, &frg_recovery_bcb_work, 0);
	INIT_DELAYED_WORK(&frg_heartbeat_work, frgmark_heartbeat);
	schedule_delayed_work(&frg_heartbeat_work, 10 * HZ);
	return 0;
}
late_initcall(frgmark_late_init);
