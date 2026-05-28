# NX549J 4.9 Bring-up State

Last updated: 2026-05-28

## Objective

Bring the Nubia NX549J / Z11 mini S LineageOS 18.1 4.9 kernel to a bootable,
diagnosable state. The current priority is to make the verified attempt55
diagnostic boot produce a 4.9 stage marker that survives into recovery via
pstore/ramoops or another recovery-readable persistence path.

## Source and Artifact Anchors

- ROM tree: `/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot`
- Kernel source: `kernel/nubia/msm8953`
- Kernel code baseline: `60961b559 nx549j: 4.9.227 kernel (xiaomi/tissot fork) + wave-3 drivers (synaptics_dsx, gf3208, panel-detect, pinctrl-nx549j, ramoops region)`
- Latest checkpoint: use `git log -1` in the kernel tree for the current
  local checkpoint. Attempt54 state checkpoints include
  `94c60a586` and `3d6057d4b`.
- Device config: `device/nubia/nx549j/BoardConfig.mk`
  - `TARGET_KERNEL_CONFIG := lineageos_nx549j_defconfig`
- Common config: `device/nubia/msm8953-common/BoardConfigCommon.mk`
  - `TARGET_KERNEL_SOURCE := kernel/nubia/msm8953`
- Previous out boot before the 2026-05-27 rebuild:
  - `out/target/product/nx549j/boot.img`
  - SHA-256: `bb59a8cc25b30cef38f05dfe725b5e4d229bfe6c42990c7e9b0e384f2009c1ab`
- Previous out kernel before the 2026-05-27 rebuild:
  - `out/target/product/nx549j/kernel`
  - SHA-256: `7b9122e33d503b8ad1c49758fcaf8292321891c1f5a1a298ff9aa533622eb353`
- Preserved 4.9 artifacts:
  - `/srv/forge/work/nx549j-preserve/Image.gz-dtb-nx549j-4.9`
  - `/srv/forge/work/nx549j-preserve/nx549j-boot-test-01.img`

## Current Build Result

2026-05-28 forced-panic diagnostic gate build completed through direct ninja
target `out/target/product/nx549j/boot.img`. This build keeps the FRGmark
forced-panic gate disabled by default in the normal boot image and preserves a
separate stage-14 cmdline repack for the pstore falsification test.

Preserved diagnostic artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt56-20260528-forcepanic`

Diagnostic artifact identity:

- `boot-normal-gate.img`
  - SHA-256: `925ce8c46486cca1c9a15db3b73fb1e29cc0a21a751a64bdb987c6c7521f9523`
  - size: `12929024`
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`
- `boot-forcepanic-consoleinit-stage14.img`
  - SHA-256: `030cde5bd1bd1888b299e4c96cf6d852574e708b338d9392f3ee9628383f315d`
  - size: `12929024`
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug frgmark.force_panic_stage=0x14`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `ccd319e4c9a536db050e1f5006f1fab0a9170ee7f63b0f1597ac0cc264a71fe5`
  - size: `12101441`
- `System.map`
  - SHA-256: `e0bd05fafd4be431717c025ad6576e2a7c3ae7675f3fdf6b994f19263e49c77c`
  - size: `5353104`
- `vmlinux`
  - SHA-256: `2a36a78da127e0c1c2508386e92f3901dc21531ce8cfb9bf32c7ffaf65577e09`
  - size: `299242608`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
  - size: `276277`
- `nx549j-frgmark-decode-spm.sh`
  - SHA-256: `5091ff56ce810fd43dc01b978ab23540cc3c6297fee7f424f7561780624fc8a3`

Diagnostic boot image header:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- boot header DT size field: `0`
- kernel size: `12101441`
- ramdisk size: `824801`

Expected capture for `boot-forcepanic-consoleinit-stage14.img`:

- Captured boot prefix SHA-256 should be
  `030cde5bd1bd1888b299e4c96cf6d852574e708b338d9392f3ee9628383f315d`.
- The refreshed NX549J collector should show
  `collector_version=2026-05-28-nx549j-freshness-v1`.
- Success marker: recovery pstore contains
  `FRGmark forced panic firing stage=14 name=console_init_done` or the panic
  reason
  `FRGmark forced panic stage=14 name=console_init_done`.
- If recovery pstore remains empty after verified flash of this exact image,
  treat pstore/ramoops readback as the blocker and move to Task 7 alternate
  marker persistence.

2026-05-27 diagnostic FRGmark build completed through direct ninja target
`out/target/product/nx549j/boot.img`. The full `mka bootimage -j4` path was
stopped after `ckati` spent 35 minutes in disk-read wait; the direct ninja path
used the existing generated graph and rebuilt the kernel config, `Image.gz-dtb`,
DTB, copied `kernel`, and repacked `boot.img`.

Preserved diagnostic artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark`

Diagnostic artifact identity:

- `boot.img`
  - SHA-256: `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`
  - size: `12929024`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `cef5b003b0c6941d9008d466f6d155b3db23042d1e482e9c2298f076ccbd9379`
  - size: `12101267`
- `System.map`
  - SHA-256: `b51da9bfab71b81271e06a3c420d982363d91713f0ca1fda47964b194135f258`
  - size: `5352885`
- `vmlinux`
  - SHA-256: `6143b77a7e6aac30333fdca465f7d743d31b0ac7e4415192db8535db5a5afb84`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
  - size: `276277`

Diagnostic kernel identity:

- `Linux version 4.9.227-perf+ (n8n@n8nagent) (Android ... clang version 11.0.2 ...) #40 SMP PREEMPT Wed May 27 19:59:06 CDT 2026`

Diagnostic boot image header:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- cmdline:
  `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`

Diagnostic marker decoder:

- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
- SHA-256: `5091ff56ce810fd43dc01b978ab23540cc3c6297fee7f424f7561780624fc8a3`
- preserved copy:
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/nx549j-frgmark-decode-spm.sh`

Previous baseline:

2026-05-27 official `mka bootimage -j4` completed successfully after the NXP
Soong namespace build-unblock and the 4.9-compatible ramoops DTS restore.

Preserved artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt54-20260527-ramoops-fix`

Artifact identity:

- `boot.img`
  - SHA-256: `979bf25077112c3ded79db59261cd79a9049a318fbb38048b4472dc78c8c6a2a`
  - size: `12929024`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `ddd3debcba2b708e4fe69f2dab88a50404787f549ece1fd6d258baf8259bbd83`
  - size: `12099611`
- `System.map`
  - SHA-256: `b03b1a61ff51ca116a5876067368de048870aebe33ed2707a5490b10fc52d29c`
  - size: `5352833`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
  - size: `276277`

Kernel identity:

- `Linux version 4.9.227-perf+ (nobody@android-build) (Android ... clang version 11.0.2 ...) #39 SMP PREEMPT Wed May 27 18:32:21 CDT 2026`

Boot image header:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- cmdline:
  `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`

DTB sanity:

- `reserved-memory/ramoops@9ff00000` is present in the built DTB with:
  - `compatible = "ramoops"`
  - `reg = <0x00 0x9ff00000 0x00 0x100000>`
  - `record-size = <0x10000>`
  - `console-size = <0x40000>`
  - `ftrace-size = <0x10000>`
  - `pmsg-size = <0x40000>`
  - `no-map`

Next action:

- Flash
  `/srv/forge/work/nx549j-preserve/release-attempt56-20260528-forcepanic/boot-forcepanic-consoleinit-stage14.img`
  for the next diagnostic run. The goal is to force a panic after
  `FRGmark stage=14 name=console_init_done` and prove whether recovery can read
  the resulting pstore record.
- Validate the refreshed debug-page/local-agent collector freshness and header
  capture before trusting another `boot_candidate_warning.txt`.
- Decode any crash address against
  `/srv/forge/work/nx549j-preserve/release-attempt56-20260528-forcepanic/System.map`.
- Decode marker text or raw IMEM marker words with
  `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`.
- Expected diagnostic improvement: recovery should expose a fresh
  forced-panic pstore record. If it does not, Task 7 should stop relying on
  recovery pstore/devmem and implement another recovery-readable marker path.

## Latest Verified Capture

### 2026-05-28 - `f89d96f3` attempt55 identity verification

FACT:

- Test: `f89d96f3-8e05-4531-9ee5-1f2961af9802`
- Capture:
  `/home/n8n/forge-work/debug/7636c990-f363-48b2-8b9d-c8f7d1a4792f/47f3099f-80f9-4e35-b1ae-f020fc4b1de2/browser-debug-evidence-1779938995226.tar`
- Triage:
  `/home/n8n/build-station/docs/run_reports/2026-05-28_f89d96f3_nx549j_frgmark_triage.md`
- Captured boot partition:
  `evidence/adb/failed_boot/mtk/partitions/boot-full-16MB.img`
- Local expected artifact:
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/boot.img`
- Verification command compared the first `12929024` bytes of captured boot
  partition against local `boot.img`.
- Captured prefix SHA-256:
  `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`
- Local `boot.img` SHA-256:
  `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`

FACT:

- Recovery live kernel in the same capture is TWRP `Linux version 3.18.31`,
  not target 4.9.
- `/sys/fs/pstore` was present but empty.
- `pstore_dump.txt` was recovery `/cache/recovery/last_log` and `log`, not
  fresh target 4.9 pstore output.
- `frgmark_decoded.txt` reported `no nx549j FRGmark markers decoded`.
- Recovery `devmem` and `/dev/mem` did not yield readable FRGmark IMEM words.

INFERENCE:

- For attempt55, the "wrong boot image flashed" hypothesis is rejected.
- The current blocker is diagnostic persistence/freshness: either the 4.9
  kernel does not reach the existing markers, or recovery cannot read the
  marker/pstore path after the manual reboot.

### 2026-05-28 - Capture freshness gate for the next NX549J tar

FACT:

- Browser recovery collector now writes `collector_version` into
  `collection-report.json` and `collection-log.txt`.
- Expected version for the next capture:
  `2026-05-28-nx549j-freshness-v1`
- Browser NX549J failed-boot captures should include:
  - `evidence/adb/failed_boot/mtk/partitions/boot-header-hex.txt`
  - `evidence/adb/failed_boot/mtk/partitions/recovery-header-hex.txt`
- Local-agent failed-boot text now writes the same collector version and
  captures boot/recovery 2048-byte partition headers in
  `mtk_partition_headers`.

INFERENCE:

- If the next tar lacks `collector_version=2026-05-28-nx549j-freshness-v1` or
  the NX549J header files, treat it as stale/incomplete before drawing a new
  boot-magic conclusion.
- The old `f89d96f3` `boot_candidate_warning.txt` is not trusted as current
  parser behavior because direct extraction showed `ANDROID!` at offset 0.

## Baseline Inventory

### 2026-05-28 - stocktruth availability and selected comparison baseline

FACT:

- No explicit stocktruth files were found under `/srv/forge/work/nx549j-preserve`
  at max depth 3 using these names:
  - `stock_boot.img`
  - `stock_recovery.img`
  - `stock_cmdline.txt`
  - `stock_dmesg.txt`
  - `stock_proc_iomem.txt`
  - `stock_proc_config.gz`
- Because explicit stock boot truth is missing, do not call any preserved
  recovery image "stock". Recovery artifacts are valid only for recovery
  header geometry, partition layout, and TWRP/runtime identity checks.

FACT:

- Selected nearest-known-good recovery/3.18 baseline:
  `/srv/forge/work/nx549j-preserve/release-attempt53`
- `release-attempt53/boot.img`
  - SHA-256: `8accf60609a8217d5fbd636529bcf906f62565ab94d250358e2dbed3823cf9c3`
  - header starts with `ANDROID!`
- `release-attempt53/recovery.img`
  - SHA-256: `3a3e2fc4240e623820e9d9f50e9c54cd4b1210e8e8cb1b6569822539844c88a1`
  - header starts with `ANDROID!`
- `release-attempt53/20260516-014442/cmdline.txt`
  - SHA-256: `5d937f6ba779e5698384b9ea13b20ddda39873582557795852853c5b42908711`
  - contains TWRP/recovery 3.18 cmdline with
    `androidboot.hardware=qcom`, `androidboot.bootdevice=7824900.sdhci`,
    `earlycon=msm_hsl_uart,0x78af000`, and panel selection
    `qcom,mdss_dsi_jdi_s6d1fa4x01_1080p_5p2_cmd`
- `release-attempt53/20260516-014442/twrp-dmesg.txt`
  - SHA-256: `ef256f77808cde55522ddadeb31d28f67fa43caffcaea4da290cd2f7ca0bbe3d`
  - first kernel banner is TWRP `Linux version 3.18.31-lineageos...`

INFERENCE:

- Use attempt53 recovery artifacts as the nearest available reference for
  legacy Android boot header geometry and recovery-side runtime behavior.
- Do not use attempt53 as hardware-complete stock truth for 4.9 DTB memory,
  reserved-memory, PMIC, clock, or regulator decisions. Those fields still need
  stock source/dump evidence or must be treated as hypotheses.

## Boot Image And DTB Audit

### 2026-05-28 - attempt55 header and compiled DTB audit

FACT:

- Compared images:
  - `/srv/forge/work/nx549j-preserve/release-attempt53/boot.img`
    SHA-256:
    `8accf60609a8217d5fbd636529bcf906f62565ab94d250358e2dbed3823cf9c3`
  - `/srv/forge/work/nx549j-preserve/release-attempt53/recovery.img`
    SHA-256:
    `3a3e2fc4240e623820e9d9f50e9c54cd4b1210e8e8cb1b6569822539844c88a1`
  - `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/boot.img`
    SHA-256:
    `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`
- Attempt53 boot, attempt53 recovery, and attempt55 boot all use Android boot
  header v0 geometry:
  - kernel load address: `0x80008000`
  - ramdisk load address: `0x81000000`
  - tags load address: `0x80000100`
  - page size: `2048`
  - second size/address: `0`
  - boot header DT size field: `0`
- Attempt55 header kernel size is `12101267` bytes, matching its preserved
  `kernel` / `Image.gz-dtb` artifact. Attempt53 header kernel size is
  `12099570` bytes, so attempt55 grew by `1697` bytes.
- Attempt55 cmdline matches the selected attempt53 baseline cmdline:
  `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`

FACT:

- Decompiled exact compiled DTB:
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/msm8953-mtp-nx549j.dtb`
- DTB SHA-256:
  `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
- `dtc` completed decompilation. Warnings are inherited structural DT warnings
  such as `unit_address_vs_reg`, `simple_bus_reg`, `/memory` unit-address, and
  reserved-memory unit-name formatting; no fatal DTB parse error was seen.
- Top-level identity:
  - `model = "Nubia Z11 mini S (nx549j) MSM8953 MTP"`
  - `compatible = "qcom,msm8953-mtp", "qcom,msm8953", "qcom,mtp"`
  - `qcom,msm-id = <0x125 0x00>`
  - `qcom,board-id = <0x08 0x00>`
  - `qcom,pmic-id = <0x10016 0x10011 0x00 0x00>`
- `/chosen` contains `bootargs = "core_ctl_disable_cpumask=0-7 kpti=0"`.
- `/memory` is present but reports `reg = <0x00 0x00 0x00 0x00>`, so LK is
  still expected to provide the usable RAM layout at boot.
- Key early hardware nodes in the compiled DTB:
  - `qcom,smem@86300000`, compatible `qcom,smem`
  - `qcom,rpm-smd`, compatible `qcom,rpm-smd`
  - `qcom,spmi@200f000`, compatible `qcom,spmi-pmic-arb`
  - `sdhci@7824900`, compatible `qcom,sdhci-msm`, status `ok`
  - `serial@78af000`, compatible `qcom,msm-uartdm-v1.4`, matching cmdline
    `earlycon=msm_hsl_uart,0x78af000`
  - `qcom,msm-imem@8600000`, compatible `qcom,msm-imem`
  - `tz-log@08600720`, compatible `qcom,tz-log`
- No compiled node with `compatible = "qcom,scm"` was found in the attempt55
  MSM8953 DTB; SCM references appear only as SCM proxy clock names on TZ/PIL
  related nodes in this tree.
- Ramoops is present in the compiled DTB:
  - `ramoops@9ff00000`
  - `compatible = "ramoops"`
  - `reg = <0x00 0x9ff00000 0x00 0x100000>`
  - `record-size = <0x10000>`
  - `console-size = <0x40000>`
  - `ftrace-size = <0x10000>`
  - `pmsg-size = <0x40000>`
  - `no-map`

INFERENCE:

- Attempt55 does not show an early packaging/header mismatch against the
  nearest available 3.18 recovery baseline.
- The compiled DTB is internally parseable and includes the expected MSM8953
  boot-critical nodes for SMEM/RPM/SPMI/SDHCI/early UART plus the intended
  ramoops region.
- Because explicit stock boot DTB/iomem truth is still missing and `/memory` is
  zero-filled in the DTB, this audit cannot prove the RAM/reserved-memory map is
  stock-correct. That remains a limitation for later hardware-truth work, not a
  proven attempt55 no-entry cause.

## Pstore And Ramoops Compatibility Audit

### 2026-05-28 - recovery-readable ramoops gate

FACT:

- Attempt55 4.9 defconfig enables:
  - `CONFIG_PSTORE=y`
  - `CONFIG_PSTORE_ZLIB_COMPRESS=y`
  - `CONFIG_PSTORE_CONSOLE=y`
  - `CONFIG_PSTORE_PMSG=y`
  - `CONFIG_PSTORE_RAM=y`
  - `CONFIG_PANIC_TIMEOUT=5`
  - `CONFIG_QCOM_FORCE_WDOG_BITE_ON_PANIC=y`
  - `CONFIG_DEVMEM=y`
  - `# CONFIG_STRICT_DEVMEM is not set`
- Attempt55 compiled DTB has the intended ramoops region:
  - physical base `0x9ff00000`
  - size `0x100000`
  - record size `0x10000`
  - console size `0x40000`
  - ftrace size `0x10000`
  - pmsg size `0x40000`
  - no `ecc-size`, so 4.9 expects `ecc: 0/0`
- The 4.9 ramoops driver in `fs/pstore/ram.c` reads those exact DT properties:
  `record-size`, `console-size`, `ftrace-size`, `pmsg-size`, and `ecc-size`.

FACT:

- Recovery-side reference dmesg:
  `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-014442/twrp-dmesg.txt`
- The reference TWRP 3.18 kernel registered ramoops successfully:
  - `console [pstore-1] enabled`
  - `pstore: Registered ramoops as persistent store backend`
  - `ramoops: attached 0x100000@0x9ff00000, ecc: 0/0`
- The same log also contains an overlapping duplicate reserved-memory warning
  and a second ramoops probe failure:
  - `ramoops_region@0 ... overlaps with pstore_reserve_mem_region@0`
  - `ramoops: probe of 9ff0000000100000.ramoops failed with error -22`
- A later attempt53 capture,
  `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-023046/twrp-dmesg.txt`,
  did not register useful ramoops and reports:
  - `ramoops: The memory size and the record/console size must be non-zero`
  - `ramoops: probe of 9ff00000.ramoops failed with error -22`
- Both attempt53 recovery captures show `/sys/fs/pstore` mounted but empty, and
  direct `/dev/mem` readback was unavailable:
  - `dd: can't open '/dev/mem': No such file or directory`

FACT:

- Current verified `f89d96f3` capture has:
  - flashed boot prefix matching attempt55
  - recovery live kernel `3.18.31`, not target 4.9
  - empty `/sys/fs/pstore`
  - `pstore_dump.txt` sourced from `/cache/recovery/last_log`, not a target
    4.9 pstore record
  - no recovery `devmem` readback path for FRGmark IMEM words
- Current `f89d96f3` recovery partition hash is
  `15156f7e8a11a6cdbfa01191bd3e5b1d34c9568461769b0c4b4b45f54b29804a`.
  This does not match the preserved attempt53 `recovery.img`
  (`3a3e2fc4240e623820e9d9f50e9c54cd4b1210e8e8cb1b6569822539844c88a1`) or
  the two named attempt53 TWRP ramoops test images
  (`26e5e3d4e07a43dc6ca1908565c4c8139173ea622b61b7b7297ab1b94327951f`,
  `ff469b1be810389b61f95e16efbf5b6dca52e034fd541d41c586f10b80ae3e48`).
- The recovery source tree mounts pstore in
  `bootable/recovery/etc/init.rc` with:
  `mount pstore pstore /sys/fs/pstore`.
  Recovery userspace expects `pmsg-ramoops-0`, `console-ramoops-0`, or
  `console-ramoops` paths in `bootable/recovery/recovery-persist.cpp`.

INFERENCE:

- Empty pstore in `f89d96f3` is not enough to conclude that 4.9 never entered:
  it can also mean the current recovery image did not bind a compatible ramoops
  backend, the reset path did not preserve the region, or the target kernel
  did not reach/panic after ramoops registration.
- The 4.9 side is configured plausibly for pstore/ramoops, and at least one
  TWRP 3.18 reference proved that `0x9ff00000/0x100000/ecc 0` can register as
  a pstore backend. The current recovery hash mismatch means recovery
  compatibility remains unproven for the exact capture.

HYPOTHESIS:

- A forced panic after a known FRGmark stage is the next falsifying test. If
  recovery shows a fresh `FRGmark` panic/console record, ramoops is usable and
  normal diagnostics can rely on pstore. If recovery pstore remains empty after
  a verified forced-panic boot, the blocker is the persistence/readback path,
  not the stage table itself, and Task 7 should pivot to an alternate
  recovery-readable marker path.

## Evidence Rules

- Treat `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-*`
  TWRP captures as 3.18 recovery evidence only unless a log proves a 4.9
  kernel banner from the exact flashed image.
- Before using any runtime marker from a capture, verify flashed `boot.img`
  hash and decode against the matching `System.map`.
- The current 4.9 tree has no fresh runtime dmesg proving the next crash point.
  The immediate diagnostic blocker is to flash attempt56 forced-panic stage 14
  and recover either fresh pstore `FRGmark` lines or documented empty pstore.
- `browser-debug-evidence-1779926521776.tar` was extracted to
  `/srv/forge/work/nx549j-preserve/browser-debug-evidence-1779926521776`.
  It is recovery-only evidence: identity reports TWRP `Linux version 3.18.31`,
  `/sys/fs/pstore` is empty, and `pstore_dump.txt` contains
  `/cache/recovery/last_log`, not a fresh 4.9 crash log.
- `f89d96f3-8e05-4531-9ee5-1f2961af9802` verifies that the flashed boot
  partition prefix matches attempt55. Use this capture as identity proof, but
  not as proof of the target 4.9 runtime stage because it still lacks fresh 4.9
  pstore or decoded FRGmark evidence.

## Patch History

### 2026-05-28 - Add FRGmark forced-panic diagnostic gate

Category: DIAGNOSTIC

Hypothesis: attempt55 has no fresh target 4.9 pstore or IMEM readback, so the
next capture needs a falsifying test that intentionally panics after a named
FRGmark stage. If recovery can read the resulting ramoops console/dmesg record,
pstore is usable and the next patch can classify the earliest stage. If it
cannot, the bring-up must switch to an alternate recovery-readable persistence
path before chasing display or initcall symptoms.

Evidence:

- Verified attempt55 boot prefix in `f89d96f3` matches
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/boot.img`
  SHA-256
  `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`.
- Attempt55 DTB contains `ramoops@9ff00000` with
  `0x100000` size, `0x10000` record, `0x40000` console, `0x10000` ftrace,
  `0x40000` pmsg, and no ECC.
- Attempt55 defconfig enables pstore RAM, console, pmsg, zlib compression, and
  panic reboot timeout.
- `f89d96f3` recovery `/sys/fs/pstore` is empty and recovery `devmem` is not
  usable, so the current capture cannot decide whether 4.9 reached pstore.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - adds a disabled-by-default forced-panic gate tied to the existing FRGmark
    stage names and `FRGmark` emergency logs.

Why each file changed:

- `frgmark.c` is the narrowest existing diagnostic point: every instrumented
  stage already passes through `frgmark(stage)`, so a single guarded panic can
  test pstore persistence without changing the stage table, marker decoder,
  DTS, or unrelated init code.

Expected next marker:

- A forced diagnostic boot built with
  `FRGMARK_FORCE_PANIC_STAGE=0x14` or booted with
  `frgmark.force_panic_stage=0x14` should panic after
  `FRGmark stage=14 name=console_init_done`.
- Recovery should expose a fresh `FRGmark forced panic firing stage=14` or
  `FRGmark forced panic stage=14` line in `/sys/fs/pstore/console-ramoops*` or
  `dmesg-ramoops*`.

Rollback condition:

- Revert this diagnostic gate after the pstore/readback path is proven or if a
  verified forced-panic boot still yields empty pstore and Task 7 implements an
  alternate marker path.

Verification commands:

- Build a diagnostic boot image with an explicit stage, preserve `boot.img`,
  `System.map`, `vmlinux`, DTB, and decoder hashes, flash it, boot once, enter
  recovery, then run the refreshed NX549J collector. The next tar must include
  `collector_version=2026-05-28-nx549j-freshness-v1`, boot/recovery header
  files, a boot prefix hash matching the forced-panic artifact, and either
  fresh pstore `FRGmark` lines or documented empty pstore.

### 2026-05-27 - Add FRGmark IMEM and printk boot-stage diagnostics

Category: DIAGNOSTIC

Hypothesis: the bootlogo hang is past the point where the available recovery
ADB capture can identify the 4.9 failure. The supplied evidence tar only
proves a later TWRP recovery boot (`Linux version 3.18.31`), an empty
`/sys/fs/pstore`, and a recovery `last_log`, so the next boot image needs
persistent stage breadcrumbs that can survive a warm reset. IMEM markers plus
`pr_emerg` lines should split early kernel init, initcalls, `/init` handoff,
DRM/DSI bring-up, and alive-but-stuck heartbeat cases.

Evidence:

- User capture:
  `/home/n8n/forge-work/debug/7636c990-f363-48b2-8b9d-c8f7d1a4792f/e4c821a4-a779-458c-a7de-51fe622f5061/browser-debug-evidence-1779926521776.tar`
- Extracted copy:
  `/srv/forge/work/nx549j-preserve/browser-debug-evidence-1779926521776`
- Capture facts:
  - `evidence/adb/failed_boot/identity.txt` reports TWRP
    `Linux version 3.18.31-lineageos...`, not the 4.9 diagnostic kernel.
  - `evidence/adb/failed_boot/pstore_index.txt` shows no pstore files.
  - `evidence/adb/failed_boot/pstore_dump.txt` is `/cache/recovery/last_log`,
    not a 4.9 crash log.
- The built attempt55 DTB still contains `qcom,msm-imem@8600000` and
  `reserved-memory/ramoops@9ff00000`.
- IMEM marker range is physical `0x08600040..0x086000c4`, ending before the
  observed `diag_dload@c8` region in the IMEM node.
- Diagnostic artifact:
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark`
  with `boot.img` SHA-256
  `cbb8280a7def7242a67a6d71932b8963229beaf9713a7231d4f53076d6f28b9d`
  and `System.map` SHA-256
  `b51da9bfab71b81271e06a3c420d982363d91713f0ca1fda47964b194135f258`.

Files changed:

- `include/linux/frgmark.h`
  - adds the canonical NX549J FRGmark stage table and prototypes.
- `arch/arm64/kernel/frgmark.c`
  - writes stage words to IMEM, prints named `FRGmark` emergency lines, and
    starts a delayed heartbeat after late init.
- `arch/arm64/kernel/Makefile`
  - links `frgmark.o` into the arm64 kernel image.
- `init/main.c`
  - marks early init, console init, driver init, initcalls, freeable init, and
    `/init` execution boundaries.
- `drivers/gpu/drm/msm/msm_drv.c`
  - marks MSM DRM register/probe/component/bind/register/splash/fbdev stages.
- `drivers/gpu/drm/msm/dsi-staging/dsi_display.c`
  - marks DSI register/probe/active-init/component stages.
- `arch/arm64/Kconfig.platforms`,
  `arch/arm64/boot/dts/qcom/Makefile`,
  `arch/arm64/configs/lineageos_nx549j_defconfig`
  - keep NX549J DTB selection explicit for the marker artifact.
- `arch/arm64/boot/dts/qcom/msm8953-mdss-panels-nx549j.dtsi`,
  `arch/arm64/boot/dts/qcom/msm8953-pinctrl-nx549j.dtsi`
  - clarify source comments without changing DT behavior.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes `FRGmark stage=xx` printk lines and raw `0x465247xx` IMEM words.

Why each file changed:

- The marker header and decoder must remain in sync so a capture can be
  interpreted without guessing stage numbers.
- The arm64 marker writer is intentionally tiny and always built for this
  diagnostic kernel so markers are available before drivers or userspace.
- `init/main.c` instrumentation answers whether the kernel reaches initcalls,
  finishes initcalls, and attempts Android `/init`.
- MSM DRM and DSI instrumentation answers whether the bootlogo hang is display
  driver bring-up, splash/fbdev handoff, or a later userspace stall.
- DTB selection changes are required because the artifact must carry
  `msm8953-mtp-nx549j.dtb`, not an accidental Xiaomi/Tissot DTB.

Expected next marker:

- Recovery pstore or recovery logs contain named lines such as
  `FRGmark stage=0e name=initcalls_done`, or IMEM reads return raw words
  beginning with `0x465247`.
- A heartbeat marker `0x40..0x4f` means the kernel remains alive after late
  init and the bootlogo hang is probably userspace/display handoff rather than
  an early kernel stop.
- A last marker before `0x0e` points to an initcall or earlier kernel blocker;
  a last marker in `0x20..0x35` narrows the next patch to MSM DRM/DSI.

Rollback condition:

- Revert this diagnostic if attempt55 regresses earlier than attempt54, if IMEM
  writes affect recovery/download behavior, or if a fresh capture proves a
  specific blocker and the markers are no longer needed.
- Keep the marker patch as diagnostic-only until a fresh capture proves the
  actual hang point. This is not a boot fix by itself.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
prebuilts/build-tools/linux-x86/bin/ninja -f out/combined-lineage_nx549j.ninja out/target/product/nx549j/boot.img -j4
sha256sum out/target/product/nx549j/boot.img out/target/product/nx549j/kernel out/target/product/nx549j/obj/KERNEL_OBJ/System.map
gzip -dc out/target/product/nx549j/kernel | strings | grep -E 'Linux version|FRGmark|msm_drm_|dsi_display|heartbeat'
/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh --adb
```

### 2026-05-27 - Restore 4.9-compatible ramoops DT binding

Category: DIAGNOSTIC

Hypothesis: the 4.9 boot failure cannot be diagnosed reliably if pstore is
not registered. The 4.9 `fs/pstore/ram.c` path expects a normal platform MEM
resource from DT `reg`; the `memory-region = <&ramoops_mem>` shape used during
bring-up is not consumed by this kernel path. Restore the 4.9-compatible
`reserved-memory/ramoops@9ff00000` node so the next flash can preserve the
earliest kernel panic or console output.

Evidence:

- `kernel/nubia/msm8953/fs/pstore/ram.c` uses
  `platform_get_resource(pdev, IORESOURCE_MEM, 0)` in `ramoops_parse_dt`.
- Earlier TWRP recovery evidence showed both working and broken ramoops forms:
  - Working-style capture:
    `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-014442/twrp-dmesg.txt`
    included pstore/ramoops registration at `0x100000@0x9ff00000`.
  - Broken-style capture:
    `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-023046/twrp-dmesg.txt`
    included `ramoops: The memory size and the record/console size must be non-zero`.
- These recovery logs are not 4.9 boot proof; they are only binding and capture
  infrastructure evidence.

Files changed:

- `arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dts`
  - restored `reserved-memory/ramoops@9ff00000` with `compatible = "ramoops"`
    and a direct `reg = <0x0 0x9ff00000 0x0 0x100000>`;
  - restored `qcom,pmic-id = <0x010016 0x010011 0x0 0x0>`.

Why each file changed:

- The DTS must expose a MEM resource directly to the 4.9 ramoops platform
  driver. Without that resource, the next kernel crash can be lost and later
  userspace/recovery symptoms are not trustworthy as the first failure.

Expected next marker:

- Rebuilt DTB decompiles to `reserved-memory/ramoops@9ff00000` with `reg`.
- After flashing the rebuilt 4.9 boot image, recovery capture should expose a
  4.9 `dmesg-ramoops-*`, `console-ramoops-*`, or fresh pstore registration.

Rollback condition:

- If the rebuilt DTB still lacks the direct `ramoops@9ff00000` `reg`, fix the
  build inclusion path before changing kernel logic.
- If a fresh 4.9 capture proves the same pstore resource-size failure, revert
  this diagnostic and instrument `ramoops_probe` / DT population.
- If a fresh 4.9 capture proves a later unrelated crash, keep this diagnostic
  unless it conflicts with memory layout.

Verification commands:

```sh
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
sha256sum out/target/product/nx549j/boot.img out/target/product/nx549j/kernel out/target/product/nx549j/obj/KERNEL_OBJ/System.map
dtc -I dtb -O dts out/target/product/nx549j/obj/KERNEL_OBJ/arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dtb | grep -n -A12 -B4 ramoops
gzip -dc out/target/product/nx549j/kernel | strings | grep -m5 -E 'Linux version|4\.9\.227|nx549j|ramoops'
```

### 2026-05-27 - Unblock LineageOS 18.1 Soong graph for NXP NFC modules

Category: BOOT-UNBLOCK

Hypothesis: the 4.9 boot image cannot be rebuilt because inherited NXP NFC
Android.bp files define isolated Soong namespaces but omit imports for the HAL
modules used by sibling HIDL service modules. This is a build graph wiring
failure, not a kernel behavior change.

Evidence:

- Build log:
  `/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/nx549j-18.1-bootimage-ramoops-fix.log`
- Soong errors reported missing `libese_client_headers`,
  `nfc_nci.nqx.default.hw`, and `se_nq_extn_client` while also listing the
  exact namespaces where those modules exist.

Files changed:

- `vendor/nxp/opensource/sn100x/halimpl/SN100x/Android.bp`
  - imports `vendor/nxp/opensource/sn100x/halimpl` for
    `libese_client_headers`.
- `vendor/nxp/opensource/sn100x/hidlimpl/1.2/default/Android.bp`
  - imports `vendor/nxp/opensource/sn100x/halimpl/SN100x`.
- `vendor/nxp/opensource/sn100x/hidlimpl/2.0/default/Android.bp`
  - imports `vendor/nxp/opensource/sn100x/halimpl` and
    `vendor/nxp/opensource/sn100x/halimpl/SN100x`.
- `vendor/nxp/opensource/pn5xx/hidlimpl/1.2/default/Android.bp`
  - imports `vendor/nxp/opensource/pn5xx/halimpl`.
- `vendor/nxp/opensource/pn5xx/hidlimpl/2.0/default/Android.bp`
  - imports `vendor/nxp/opensource/pn5xx/halimpl`.

Why each file changed:

- Each service namespace is allowed to read only itself and imported
  namespaces. Imports are targeted to the matching NXP family to avoid
  ambiguous duplicate HAL module names between `pn5xx` and `sn100x`.

Expected next marker:

- `soong_build out/soong/build.ninja` completes and the build advances into
  `ckati`/ninja/kernel work.

Rollback condition:

- If product selection later proves NX549J should include only one NXP family,
  replace this graph wiring with product-level pruning instead of compiling
  both families.

Verification commands:

```sh
grep -n "error:" nx549j-18.1-bootimage-ramoops-fix.log
tail -n 200 nx549j-18.1-bootimage-ramoops-fix.log
```
