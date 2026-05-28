# NX549J 4.9 Bring-up State

Last updated: 2026-05-27

## Objective

Bring the Nubia NX549J / Z11 mini S LineageOS 18.1 4.9 kernel to a bootable,
diagnosable state. The current priority is to produce a fresh 4.9 `boot.img`
from the living source tree, flash it, and capture the earliest boot failure
with pstore/ramoops or serial markers.

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

- Flash `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/boot.img`.
- Capture fresh post-flash evidence and verify it is this exact artifact before
  reading any marker or pstore data.
- Decode any crash address against
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/System.map`.
- Decode marker text or raw IMEM marker words with
  `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`.
- Expected diagnostic improvement: recovery should expose either fresh 4.9
  `FRGmark stage=..` lines in pstore/last_log, or raw IMEM words beginning
  with `0x465247..` from physical `0x08600040`.

## Evidence Rules

- Treat `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-*`
  TWRP captures as 3.18 recovery evidence only unless a log proves a 4.9
  kernel banner from the exact flashed image.
- Before using any runtime marker from a capture, verify flashed `boot.img`
  hash and decode against the matching `System.map`.
- The current 4.9 tree has no fresh runtime dmesg proving the next crash point.
  The immediate diagnostic blocker is to flash attempt55 and recover either
  FRGmark pstore lines or IMEM marker words.
- `browser-debug-evidence-1779926521776.tar` was extracted to
  `/srv/forge/work/nx549j-preserve/browser-debug-evidence-1779926521776`.
  It is recovery-only evidence: identity reports TWRP `Linux version 3.18.31`,
  `/sys/fs/pstore` is empty, and `pstore_dump.txt` contains
  `/cache/recovery/last_log`, not a fresh 4.9 crash log.

## Patch History

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
