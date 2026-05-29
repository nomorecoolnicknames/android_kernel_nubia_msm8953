// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/nx549j_splashprobe.h>
#include <linux/printk.h>
#include <linux/sizes.h>
#include <linux/types.h>

#include <asm/early_ioremap.h>

#define NX549J_SPLASHPROBE_ENABLED	1
#define NX549J_SPLASHPROBE_PA		0x90001000ULL
#define NX549J_SPLASHPROBE_BYTES	SZ_2M
#define NX549J_SPLASHPROBE_CHUNK	SZ_256K

static u32 __init nx549j_splashprobe_word(enum nx549j_splashprobe_stage stage,
					  unsigned long word)
{
	switch (stage) {
	case NX549J_SPLASH_STAGE_EARLY_IOREMAP_READY:
		return 0x00ff0000;
	case NX549J_SPLASH_STAGE_FDT_DONE:
		return 0x0000ff00;
	case NX549J_SPLASH_STAGE_EARLY_PARAM_DONE:
		return 0x000000ff;
	case NX549J_SPLASH_STAGE_MEMBLOCK_DONE:
		return 0x00ffff00;
	case NX549J_SPLASH_STAGE_PAGING_DONE:
		return (word & BIT(8)) ? 0x00ffffff : 0x00000000;
	case NX549J_SPLASH_STAGE_BEFORE_IOREMAP_RESET:
		return 0x00ff00ff;
	case NX549J_SPLASH_STAGE_PSCI_DONE:
		return 0x0000ffff;
	case NX549J_SPLASH_STAGE_SETUP_ARCH_DONE:
		return (word & BIT(9)) ? 0x00ffffff : 0x000000ff;
	case NX549J_SPLASH_STAGE_START_KERNEL_AFTER_SETUP_ARCH:
		return ((word >> 7) ^ (word >> 13)) & 1 ?
			0x00ffffff : 0x00000000;
	default:
		return 0x00ffffff;
	}
}

void __init nx549j_splashprobe(enum nx549j_splashprobe_stage stage)
{
	phys_addr_t splash_pa = NX549J_SPLASHPROBE_PA;
	unsigned long offset;

	if (!NX549J_SPLASHPROBE_ENABLED)
		return;

	pr_emerg("NX549J splashprobe: stage=%u pa=%pa bytes=0x%x\n",
		 stage, &splash_pa, NX549J_SPLASHPROBE_BYTES);

	for (offset = 0; offset < NX549J_SPLASHPROBE_BYTES;
	     offset += NX549J_SPLASHPROBE_CHUNK) {
		unsigned long bytes = min_t(unsigned long,
					    NX549J_SPLASHPROBE_CHUNK,
					    NX549J_SPLASHPROBE_BYTES - offset);
		u32 __iomem *pixels;
		unsigned long words;
		unsigned long i;

		pixels = early_ioremap(NX549J_SPLASHPROBE_PA + offset, bytes);
		if (!pixels) {
			pr_emerg("NX549J splashprobe: map failed stage=%u offset=0x%lx bytes=0x%lx\n",
				 stage, offset, bytes);
			return;
		}

		words = bytes / sizeof(u32);
		for (i = 0; i < words; i++)
			writel_relaxed(nx549j_splashprobe_word(stage,
							       (offset / sizeof(u32)) + i),
				       &pixels[i]);

		wmb();
		early_iounmap(pixels, bytes);
	}
}

static u32 nx549j_splashprobe_frg_color(u8 stage)
{
	switch (stage) {
	case 0x50:
		return 0x00ff0000;
	case 0x51:
		return 0x0000ff00;
	case 0x52:
		return 0x000000ff;
	case 0x53:
		return 0x00ffff00;
	case 0x54:
		return 0x0000ffff;
	case 0x55:
		return 0x00ff00ff;
	case 0x56:
		return 0x00ffffff;
	case 0x57:
		return 0x00ff8000;
	default:
		return 0;
	}
}

void nx549j_splashprobe_frg_stage(u8 stage)
{
	phys_addr_t splash_pa = NX549J_SPLASHPROBE_PA;
	u32 color = nx549j_splashprobe_frg_color(stage);
	u32 __iomem *pixels;
	unsigned long words;
	unsigned long i;

	if (!NX549J_SPLASHPROBE_ENABLED || !color)
		return;

	pr_emerg("NX549J splashprobe: frg_stage=%02x color=0x%08x pa=%pa bytes=0x%x\n",
		 stage, color, &splash_pa, NX549J_SPLASHPROBE_BYTES);

	pixels = ioremap(NX549J_SPLASHPROBE_PA, NX549J_SPLASHPROBE_BYTES);
	if (!pixels) {
		pr_emerg("NX549J splashprobe: late map failed frg_stage=%02x bytes=0x%x\n",
			 stage, NX549J_SPLASHPROBE_BYTES);
		return;
	}

	words = NX549J_SPLASHPROBE_BYTES / sizeof(u32);
	for (i = 0; i < words; i++)
		writel_relaxed(color, &pixels[i]);

	wmb();
	iounmap(pixels);
}
