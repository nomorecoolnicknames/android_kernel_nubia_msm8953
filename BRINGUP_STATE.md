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
- Kernel HEAD: `60961b559 nx549j: 4.9.227 kernel (xiaomi/tissot fork) + wave-3 drivers (synaptics_dsx, gf3208, panel-detect, pinctrl-nx549j, ramoops region)`
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

- Flash `/srv/forge/work/nx549j-preserve/release-attempt54-20260527-ramoops-fix/boot.img`.
- Capture fresh post-flash evidence and verify it is this exact artifact before
  reading any marker or pstore data.
- Decode any crash address against
  `/srv/forge/work/nx549j-preserve/release-attempt54-20260527-ramoops-fix/System.map`.
- Expected diagnostic improvement: recovery should expose a fresh 4.9
  `console-ramoops-*`, `dmesg-ramoops-*`, or clear pstore/ramoops registration
  evidence instead of the previous resource-size failure.

## Evidence Rules

- Treat `/srv/forge/work/nx549j-preserve/release-attempt53/20260516-*`
  TWRP captures as 3.18 recovery evidence only unless a log proves a 4.9
  kernel banner from the exact flashed image.
- Before using any runtime marker from a capture, verify flashed `boot.img`
  hash and decode against the matching `System.map`.
- The current 4.9 tree has no fresh runtime dmesg proving the next crash point.
  The immediate diagnostic blocker is to rebuild and flash a 4.9 image that
  preserves ramoops/pstore.

## Patch History

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
