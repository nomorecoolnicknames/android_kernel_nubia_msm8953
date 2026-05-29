/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_NX549J_SPLASHPROBE_H
#define _LINUX_NX549J_SPLASHPROBE_H

#include <linux/init.h>
#include <linux/types.h>

enum nx549j_splashprobe_stage {
	NX549J_SPLASH_STAGE_EARLY_IOREMAP_READY = 1,
	NX549J_SPLASH_STAGE_FDT_DONE,
	NX549J_SPLASH_STAGE_EARLY_PARAM_DONE,
	NX549J_SPLASH_STAGE_MEMBLOCK_DONE,
	NX549J_SPLASH_STAGE_PAGING_DONE,
	NX549J_SPLASH_STAGE_BEFORE_IOREMAP_RESET,
	NX549J_SPLASH_STAGE_PSCI_DONE,
	NX549J_SPLASH_STAGE_SETUP_ARCH_DONE,
	NX549J_SPLASH_STAGE_START_KERNEL_AFTER_SETUP_ARCH,
};

#ifdef CONFIG_MACH_NUBIA_NX549J
void __init nx549j_splashprobe(enum nx549j_splashprobe_stage stage);
void nx549j_splashprobe_frg_stage(u8 stage);
#else
static inline void __init
nx549j_splashprobe(enum nx549j_splashprobe_stage stage) {}
static inline void nx549j_splashprobe_frg_stage(u8 stage) {}
#endif

#endif /* _LINUX_NX549J_SPLASHPROBE_H */
