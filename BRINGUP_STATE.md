# NX549J 4.9 Bring-up State

Last updated: 2026-06-07T14:01:44-05:00

## Objective

Bring the Nubia NX549J / Z11 mini S LineageOS 18.1 4.9 kernel to a bootable,
diagnosable state. The current priority is to recover an automatic reboot/reset
signal from the target 4.9 kernel, then use that signal to bracket how far
early boot gets before returning to pstore/ramoops or another recovery-readable
persistence path.

## 2026-06-07 attempt165 flash result / old TWRP recovery rescue

Patch category: BOOT-UNBLOCK / RECOVERY-RESCUE.

Facts:
- attempt165 was flashed with the no-wipe fastboot runner:
  `/srv/forge/work/nx549j-preserve/release-attempt165-20260607-clean-source-full-rom-dpmd-rootfc-ztemt-battid460/run-attempt165-nowipe-and-capture.sh --write-partitions`
- Flash run directory:
  `/srv/forge/work/nx549j-preserve/flash-runs/attempt165-20260607_133853`
- Flash transcript:
  `/srv/forge/work/nx549j-preserve/flash-runs/attempt165-20260607_133853/transcript.log`
- Fastboot product guard passed as `product=MSM8953`.
- Written partitions, all `OKAY`:
  - `boot` from `boot.img`
  - `recovery` from attempt165 Lineage recovery
  - `system` from `fastboot-system.img`
  - `oem` from `fastboot-vendor.img`
- User-visible result after flash: bootloop/logo hang.
- Postflash capture:
  `/srv/forge/work/nx549j-preserve/captures/attempt165-postflash-20260607_134133`
- Postflash capture verdict:
  `INCOMPLETE_CAPTURE`; `adb wait-for-device timed out after 900s`.
- This is not runtime proof of attempt165 userspace. Treat attempt165 as
  flashed-but-not-booted until recovery/pstore/ramoops or a later ADB capture
  proves more.
- The user reported the flashed recovery is broken and requested the old TWRP
  recovery repacked for the current layout.

Old TWRP rescue image:
- Rescue directory:
  `/srv/forge/work/nx549j-preserve/recovery-rescue-twrp-attempt165-20260607_1346`
- Base old TWRP image:
  `/srv/forge/work/nx549j-preserve/release-attempt53/twrp-3.2.1-ramoops-v4-4cell-reg.img`
- Base old TWRP SHA-256:
  `ff469b1be810389b61f95e16efbf5b6dca52e034fd541d41c586f10b80ae3e48`
- Repacked output:
  `/srv/forge/work/nx549j-preserve/recovery-rescue-twrp-attempt165-20260607_1346/twrp-3.2.1-attempt165-layout-oem-vendor.img`
- Repacked output SHA-256:
  `7308ad08fe35039ac039317b76b29da2142e466ba99b0f0b5450a2ebf0aad0aa`
- Repacked image size:
  `17278976` bytes, below the recovery partition size `41943040`.
- Repack details:
  old TWRP kernel/header were kept, and both ramdisk fstab files were patched
  to the current NX549J A-only layout where physical `oem` is mounted as
  `/vendor`:
  - `fstab.qcom`
  - `etc/recovery.fstab`
- Wait-and-flash helper:
  `/srv/forge/work/nx549j-preserve/recovery-rescue-twrp-attempt165-20260607_1346/flash-repacked-twrp-recovery.sh`
- Helper SHA-256:
  `783ac361a26e4cf090280dc3c8d21f19d8d0ad8c10ed04dffc45ec113a747587`
- Helper validation:
  `bash -n flash-repacked-twrp-recovery.sh` passed.
- Rescue recovery flash log:
  `/srv/forge/work/nx549j-preserve/recovery-rescue-twrp-attempt165-20260607_1346/flash-repacked-twrp-recovery-20260607_1358.log`
- Rescue recovery flash log SHA-256:
  `43be44adeac7ec606ddfc217a31089fdbfb490bb73a8fb277f9e1939e94800ce`
- Rescue recovery flash result:
  `fastboot product=MSM8953`, `fastboot flash recovery` returned `OKAY`, and
  `fastboot reboot recovery` returned `OKAY`.
- Userdata was not wiped.
- Current blocker at this note:
  after `fastboot reboot recovery`, host USB does not see the phone as ADB or
  fastboot; `fastboot devices -l` is empty, `adb devices -l` is empty, and
  `lsusb` does not list Nubia/ZTE.

Next action:
- Confirm visually whether TWRP appears on the phone screen.
- If TWRP is visible but ADB is absent, use hardware/UI storage access or
  re-enter fastboot and retry recovery boot/capture.
- If TWRP is not visible and fastboot returns, keep recovery-only flashing to
  known-good TWRP variants; do not rewrite system/oem until recovery evidence
  is available.

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

## 2026-06-07 attempt165 full ROM release candidate / flash gate

Patch category: RELEASE-CANDIDATE / FULL-ROM / FLASH-GATED.

Facts:
- Kernel repo HEAD at this note:
  `da266e3f5 nx549j: checkpoint attempt163 hardware bringup`.
- attempt165 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt165-20260607-clean-source-full-rom-dpmd-rootfc-ztemt-battid460/`
- Build incremental:
  `nx549j_attempt165_20260607_113349`.
- Expected runtime identity:
  - `ro.lineage.version=18.1-20260607-UNOFFICIAL-nx549j`
  - `ro.system.build.version.incremental=nx549j_attempt165_20260607_113349`
  - `ro.vendor.build.version.incremental=nx549j_attempt165_20260607_113349`
- Artifact SHA-256:
  - final OTA `lineage-18.1-20260607-UNOFFICIAL-nx549j.zip`:
    `43fb2c525b047faaf2054b165ddff6f0de1cac30285c95c13bf804a379d68676`
  - `boot.img`:
    `db2024042a4805e732cb16dc8d0f9c8ac22310cba2f593854348822cf1f2baf4`
  - `fastboot-system.img`:
    `d2ddb69361387b3830a75779da95c916d4812fdc251a8bc82ed226826867a418`
  - `fastboot-vendor.img`:
    `b121087c98023f6494a743375b1f9060d8232875a758d3dc18d6e7a00ffcd5f7`
  - release `SHA256SUMS`:
    `24769633b24793fac7dad17308ee5efe9f641e74f9e7aaa28e492556d0f9381f`

Static release evidence:
- `BOOT_IMAGE_AUDIT.md` in the release directory proves the boot image embeds
  the attempt165 kernel/`Image.gz-dtb`, and its strings include
  `ztemt_lg_3000mah` and `qcom,batt-id-kohm`.
- `STATIC_RUNTIME_AUDIT.md` in the release directory records the dpmd/DPM,
  netmgr/location, and camera artifacts, init declarations, VINTF entries, and
  SELinux labels present in the target files.
- `PREFLASH_SIGNATURE_COMPARISON.md` compares the currently running old system
  against attempt165 and records that old dpmd was labeled `system_file` while
  attempt165 labels `/system/bin/dpmd` as `vendor_dpmd_exec`.
  Updated SHA-256:
  `6360f2c53c29cbdcd31d1263c406a9073c001cae6b579b0e5773cd995466f801`.
  The report now also references the fixed capture and records targeted service
  state, battery sysfs, old binary labels, and old VINTF snippets.
- `ATTEMPT165_FLASH_ROLLBACK_RUNBOOK.md` records the no-wipe flash command,
  postflash capture path, incomplete-ADB behavior, and rollback command.
  Updated SHA-256:
  `0a364e92eee8b58e400ffd1936e270b596f98ac1010e4b0d1c16e974ffbfa30a`.
- `attempt165-release.env` records machine-readable release identity,
  immutable build-code refs, expected runtime identity, no-wipe policy, oem
  vendor target, rollback dir, and key artifact SHA-256 values. SHA-256:
  `e5b8b59439968b5ecebb6bf735460f06d40da5f4839aa1db83896d45ae892cca`.
  It also records the default fastboot product allowlist regex
  `^(nx549j|NX549J|msm8953|MSM8953)$`.
- `run-attempt165-nowipe-and-capture.sh` now copies
  `attempt165-release.env` to each flash-run as `release_identity.env`, copies
  the release `SHA256SUMS` as `release_SHA256SUMS`, and writes
  `run_identity.env` with run date, serial, release dir, helper hashes, and the
  release checksum manifest hash before writing partitions. SHA-256:
  `cd9bc7ca83f2f2207179c8302838b45f4a806c37f94a3bafc8cba11cbc6d7040`.
- `summarize-attempt165-analysis.sh` converts a postflash capture analysis into
  compact run verdict fields such as `WRONG_BUILD_OR_PREFLASH_CAPTURE`,
  `INCOMPLETE_CAPTURE`, or `ATTEMPT165_RUNTIME_BLOCKERS`. SHA-256:
  `cddc1396c4defd1a3b7d26c16d896cb441973bd5c63644565cc6e17d6e84265a`.
- The run helper now writes `run_verdict.txt` into each flash-run directory and
  embeds it in `run_summary.txt` after postflash capture.
- `flash-attempt165-fastboot-nowipe.sh` now validates local image sizes against
  recorded NX549J partition sizes before the ADB-dependent checks. This keeps
  the size guard active even if the phone is already in fastboot and Android
  ADB is unavailable. SHA-256:
  `f3f945dcdc3b49710e1b583a133c05de29fedd5ac10317580cdee91757fc68cb`.
  It also refuses to write in fastboot unless `getvar product` matches
  `EXPECTED_FASTBOOT_PRODUCT_REGEX`, defaulting to
  `^(nx549j|NX549J|msm8953|MSM8953)$`.
- `SPARSE_IMAGE_FS_AUDIT.md` records host-side sparse-to-raw conversion and
  read-only filesystem checks for `fastboot-system.img` and
  `fastboot-vendor.img`. SHA-256:
  `942c23aa33676bf71e8e4f4d7c1795714cf8598758f3b80bc2a7fb90bf1099bb`.
  Audit result:
  - `fastboot-system.img` expands to `4294967296` bytes and passes
    `e2fsck -fn`
  - `fastboot-vendor.img` expands to `300384256` bytes and passes
    `e2fsck -fn`
- `RELEASE_ARTIFACT_CONSISTENCY.md` records that standalone fastboot images,
  target-files `IMAGES/*`, the OTA embedded `boot.img`, and OTA metadata all
  agree on the same attempt165 artifact set. SHA-256:
  `499fd17ff2a83a4abdd6124c55fa0879cc54ff57b66581ee71346606e51be342`.
- Release `README.md` SHA-256 after the consistency-audit reference update:
  `0f912545badf86044dc7fe417d5684dfdb19e9ed4cb242c37c503f684cd192d9`.
- Updated `capture-attempt165-postflash.sh` SHA-256:
  `99d3a78a38b0e9215b621b31682453938afec3623115a52c2a0c5364723ceac0`.
  This version captures multi-command Android shell snippets as a single
  `adb shell` command string so targeted service, battery, binary-context, and
  VINTF files are populated instead of failing with `unexpected 'do'`.

Current connected-device state before any attempt165 flash:
- `adb devices -l` sees exact target serial `30785d1a` in Android userspace.
- After the user reconnected the cable, `adb devices -l` still sees
  `30785d1a` in Android userspace and `fastboot devices -l` is empty; the
  phone has not been rebooted to bootloader by the helper.
- The phone is still running the old working build:
  - `ro.lineage.version=18.1-20260603-UNOFFICIAL-nx549j`
  - `ro.system.build.version.incremental=eng.n8n.20260603.073227`
  - `ro.vendor.build.version.incremental=eng.n8n.20260603.073227`
- The old runtime still shows:
  - `init.svc.dpmd=restarting`
  - `init.svc.vendor.netmgrd=restarting`
  - `init.svc.loc_launcher=restarting`
- Therefore attempt165 is not runtime-proven yet.
- Fresh current-runtime capture after the capture-helper fix:
  `/srv/forge/work/nx549j-preserve/captures/pre-attempt165-current-runtime-refresh-fixed/attempt165-postflash-20260607_125139`
- The fixed capture has populated targeted files and no `capture_errors.txt`.
  It confirms:
  - old build identity is still `18.1-20260603-UNOFFICIAL-nx549j` with
    system/vendor incrementals `eng.n8n.20260603.073227`
  - battery profile is already `ztemt_lg_3000mah`, resistance id `460600`
  - `targeted_service_props.txt` reports `dpmd`, `loc_launcher`, and
    `vendor.netmgrd` as `restarting`
  - `/system/bin/dpmd` is labeled `u:object_r:system_file:s0` on the old
    runtime, while the attempt165 release labels it `vendor_dpmd_exec`
  - camera provider still reports `Number of camera devices: 0`

Flash readiness:
- Latest read-only command:
  `./flash-attempt165-fastboot-nowipe.sh --preflight-only`
- Result:
  passed after release helper/runbook/capture updates and again after the
  cable reconnect at `2026-06-07T13:31:56-05:00`.
- Preflight verified all release checksums, recorded expected partition sizes,
  exact ADB identity
  `serial=30785d1a`, `/vendor` mounted from `/dev/block/mmcblk0p31`, and image
  sizes:
  - `boot.img` 12941312 fits boot 41943040
  - `recovery.img` 21594112 fits recovery 41943040
  - `fastboot-system.img` 1611813444 fits system 4294967296
  - `fastboot-vendor.img` 280854676 fits oem 300384256
- Important partition mapping:
  physical by-name `vendor` is absent; runtime `/vendor` is physical `oem`.
  Fastboot must flash the vendor image to `oem`, not `vendor`.

Gated no-wipe flash command:
```sh
cd /srv/forge/work/nx549j-preserve/release-attempt165-20260607-clean-source-full-rom-dpmd-rootfc-ztemt-battid460
./run-attempt165-nowipe-and-capture.sh --write-partitions
```

Rollback backup:
- Verified known-working backup directory:
  `/srv/forge/work/nx549j-preserve/backups/current-working-pre-attempt164-20260607_111715`
- `sha256sum -c SHA256SUMS` passed.
- `gzip -t boot.img.gz recovery.img.gz system.img.gz oem.img.gz` passed.
- Restore helper refuses to write without `--write-partitions`.
- Restore command, no userdata wipe:
```sh
cd /srv/forge/work/nx549j-preserve/backups/current-working-pre-attempt164-20260607_111715
./restore-current-working-fastboot-nowipe.sh --write-partitions
```

Next runtime proof required:
- Flash attempt165 with the gated no-wipe helper only after explicit user
  confirmation.
- Confirm the system/vendor incrementals match
  `nx549j_attempt165_20260607_113349`.
- Recheck battery profile `ztemt_lg_3000mah` and resistance id `460600`.
- Recheck `dpmd`, `vendor.netmgrd`, `loc_launcher`, and camera provider from
  the postflash capture before making the next source patch.

## 2026-06-07 attempt161 full ROM build with ZTEMT batt-id 460

Patch category: PROPER-FIX / PACKAGING-FIX.

Facts:
- Full `m bacon -j2` completed successfully in the LineageOS 18.1 ROM tree.
- Source/packaging commits published to GitHub after this build:
  - kernel repo `nomorecoolnicknames/android_kernel_nubia_msm8953`,
    branch `nx549j-port-4.9`, build-state commit
    `ecd3185db docs: record nx549j attempt161 rom build`.
  - device repo `nomorecoolnicknames/android_device_nubia_nx549j`,
    branch `lineage-18.1`, packaging commit
    `6134e3f nx549j: align recovery system-root property`.
  - common repo `nomorecoolnicknames/android_device_nubia_msm8953-common`,
    branch `lineage-18.1-xiaomi-tissot-base`, packaging commit
    `380adbc msm8953-common: expose system mount in recovery fstab`.
- Attempt160 failed only at final OTA packaging:
  `ota_from_target_files.py` raised `KeyError: '/system'`.
- Root cause:
  recovery fstab had `/system_root`, but this product sets
  `BOARD_BUILD_SYSTEM_ROOT_IMAGE := false`; LineageOS 18.1 non-A/B
  `ota_from_target_files` needs a `/system` fstab entry to resolve the block
  device for system payload generation.
- Device-tree fix:
  `device/nubia/msm8953-common/rootdir/etc/fstab_recovery.qcom` now uses
  `/system`, and `device/nubia/nx549j/rootdir/init.recovery.qcom.rc` now sets
  `ro.build.system_root_image=false`.
- Final ROM ZIP:
  `/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/out/target/product/nx549j/lineage-18.1-20260607-UNOFFICIAL-nx549j.zip`
- Preserved attempt161 release:
  `/srv/forge/work/nx549j-preserve/release-attempt161-20260607-full-rom-ztemt-battid460/`
- ROM ZIP SHA-256:
  `ff3cda4a9622f1f7fea402f556d4d33b9de0096cc2ff10fea679736c9e382df3`
- Final boot image SHA-256:
  `850419c7b2129ba59390f1d9a93d4082103fe11b2e10995ef6d3335e0b1cebc3`
- Final recovery image SHA-256:
  `fda0b9fb96ab9acb98b01136247193e4585a8feb097cfc8489931e2452eb7f5f`
- Final target-files `IMAGES/vendor.img` SHA-256:
  `61392fe8b4a4c302b7615f2ca9bc0e531f024720d2cf146d83dac5ff540f171f`
- Final target-files `IMAGES/system.img` SHA-256:
  `599e6bf6b2ae2feaf776ff95806ff611a635787eb966fe56ab88680cefd8279e`
- The `boot.img` inside the final ROM ZIP matches
  `out/target/product/nx549j/boot.img`.
- Kernel payload check:
  `out/target/product/nx549j/kernel` matches
  `out/target/product/nx549j/obj/KERNEL_OBJ/arch/arm64/boot/Image.gz-dtb`
  with SHA-256
  `b3322c9fb39083cb683e14b7c75e3d20103002f574f5861f00f4e84a1f84379d`.
- DTB verification:
  `msm8953-mtp-nx549j.dtb` decompiles with
  `qcom,batt-id-kohm = <0x1cc>` and
  `qcom,battery-type = "ztemt_lg_3000mah"`.
- OTA packaging ran `checkvintf --check-compat` and got `COMPATIBLE`.
- `sha256sum -c SHA256SUMS` passes in the preserved attempt161 release
  directory.

Current interpretation:
- attempt161 is the latest built full ROM artifact and includes the
  source-level battery profile fix verified at runtime by attempt159.
- The full ROM ZIP has not yet been runtime-flashed/boot-verified as a full
  system install. The latest runtime-confirmed boot image remains attempt159.

Build command:
- `source build/envsetup.sh`
- `lunch lineage_nx549j-userdebug`
- `m bacon -j2`

## 2026-06-07 attempt159 confirmed boot and ZTEMT batterydata

Patch category: PROPER-FIX.

Facts:
- Confirmed working boot base is attempt130:
  `/srv/forge/work/nx549j-preserve/release-attempt130-20260601-real-vendor-repartition/boot-real-vendor-repartition.img`,
  SHA `684f7734e96b8229da8a60b3b38b0adf0882351f61aa239cbf5c7e3cc6403ff9`.
- User confirmed attempt130 reached Android UI with working screen, touchscreen,
  and Wi-Fi.
- Attempt158 replaced only the appended DTB battery profile with
  `ztemt_lg_3000mah`, flashed successfully, and booted Android, but dmesg
  showed `of_batterydata_get_best_profile: No battery data found`.
- On the running attempt158 image, `/proc/device-tree/qcom,battery-data`
  contained `qcom,ztemt-batterydata`, proving the boot repack and DTB path were
  valid.
- Live measured battery ID on the target was
  `/sys/.../power_supply/bms/resistance_id=460600`; the driver matches
  `resistance_id / 1000` against `qcom,batt-id-kohm` within
  `qcom,batt-id-range-pct=<15>`.
- Attempt159 changed the ZTEMT profile's `qcom,batt-id-kohm` from `47` to
  `460` and preserved the attempt130 ramdisk, boot header, cmdline, and image
  size.
- Attempt159 flash result: `Sending 'boot' (12636 KB) OKAY`, `Writing 'boot'
  OKAY`, total `1.054s`.
- Runtime after attempt159: `sys.boot_completed=1`, `dev.bootcomplete=1`,
  `init.svc.bootanim=stopped`, `init.svc.surfaceflinger=running`, and
  `init.svc.wpa_supplicant=running`.
- Runtime battery result after attempt159:
  `/sys/class/power_supply/bms/battery_type=ztemt_lg_3000mah`,
  `battery/capacity=66`, `battery/status=Charging`, and
  `bms/resistance_id=460600`.
- Final running DTB check:
  `/proc/device-tree/qcom,battery-data/qcom,ztemt-batterydata/qcom,batt-id-kohm`
  is `00 00 01 cc`.
- Final flashable image:
  `/srv/forge/work/nx549j-preserve/release-attempt159-20260607-boot130-ztemt-battid460/boot-boot130-ztemt-battid460.img`.
- Convenience flash copy:
  `/srv/forge/android/nx549j/restored/last-successful-boot-attempt130-ztemt-battid460.img`.
- Final boot SHA:
  `f4b02a0d281da2d80be5428a3042a69b74bfe9a9b47f3523034cbf1e7ecca05a`.
- Source owner patch:
  `arch/arm64/boot/dts/qcom/nx549j/batterydata-ztemt-4v4-3000mah.dtsi`
  now carries `qcom,batt-id-kohm = <460>`.
- ADB transport was temporarily blocked by stale orphan `adb -L tcp:150xx`
  reverse-server processes holding the USB node; libusb reported
  `LIBUSB_ERROR_BUSY`. Killing `adb -L tcp:150*` restored normal ADB.

Current interpretation:
- Attempt159 is the latest confirmed booting boot image with the ZTEMT battery
  profile applied and matched.

Rollback condition:
- If UI/touch/Wi-Fi regress or charging behavior gets worse, flash
  `/srv/forge/android/nx549j/restored/last-successful-boot-attempt130.img`
  back to `boot`.

## Current Build Result

2026-05-29 attempt68 Image-header immediate recovery reset:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and captured; automatic recovery fallback failed.
- Attempt68 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt68-20260529-image-header-immediate-recovery-reset`.
- Attempt68 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt68-20260529-image-header-immediate-recovery-reset/boot-image-header-immediate-recovery-reset.img`.
- SHA-256:
  `1a81ec450ac1c82ff40d3fb9a8b5dcbf30bd4baca58b698c9becc3d2989135fe`.
- Source change:
  - `arch/arm64/kernel/head.S` jumps from the arm64 Image header branch
    directly to `nx549j_head_entry_recovery_reset` for NX549J.
  - `ENTRY(stext)` still calls the same reset helper as a fallback.
  - The helper writes marker stage `0x68` to physical IMEM `0x08600040`,
    writes recovery magic `0x77665502` to physical
    `qcom,msm-imem-restart_reason` address `0x0860065c`, then writes `0` to
    MSM PS_HOLD physical `0x004ab000`.
- Artifact identity:
  - FACT: boot partition prefix SHA-256 in recovery capture matched attempt68:
    `1a81ec450ac1c82ff40d3fb9a8b5dcbf30bd4baca58b698c9becc3d2989135fe`.
  - FACT: `System.map` contains `stext` at `ffffff8009a00000`,
    `nx549j_head_entry_recovery_reset` at `ffffff8009a00030`, and
    `nx549j_head_post_cpu_setup_recovery_probe` at `ffffff8009a00090`.
- Runtime evidence:
  - FACT: recovery capture path:
    `/srv/forge/work/nx549j-preserve/release-attempt68-20260529-image-header-immediate-recovery-reset/recovery-capture-20260529-124719`.
  - FACT: user reported the phone did not enter recovery automatically; manual
    recovery entry was required after repeated 2-3 second vibration/bootlogo
    loops.
  - FACT: recovery dmesg recorded PS_HOLD as the prior power-off reason.
  - FACT: `/sys/fs/pstore` and `/proc/last_kmsg` were empty.
  - FACT: recovery dmesg reported:
    `ramoops: The memory size and the record/console size must be non-zero`.
- INFERENCE: early IMEM `0x77665502` plus direct PS_HOLD proves a reset path
  but does not prove automatic recovery selection on this bootloader. The next
  plan must test Android BCB in `misc`, Qualcomm PMIC PON restart reason, and
  ramoops independently.
- Next plan:
  `.ai-factory/PLAN.md` now targets both blockers: automatic recovery fallback
  and recovery-readable 4.9 kernel logs.

2026-05-29 misc BCB backup/write/restore tooling:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: host-side scripts created and sanity-tested without reboot.
- Scripts:
  - `/srv/forge/android/nx549j/scripts/nx549j-bcb-lib.sh`
  - `/srv/forge/android/nx549j/scripts/nx549j-backup-misc-bcb.sh`
  - `/srv/forge/android/nx549j/scripts/nx549j-write-bcb-recovery.sh`
  - `/srv/forge/android/nx549j/scripts/nx549j-restore-misc-bcb.sh`
  - `/srv/forge/android/nx549j/scripts/nx549j-test-bcb-recovery.sh`
- Safety:
  - FACT: scripts refuse serials other than `30785d1a`.
  - FACT: target gate accepted `state=recovery`, model `Nubia Z11 mini s`,
    device `NX549J`, product `NX549J`.
  - FACT: `/dev/block/bootdevice/by-name/misc` resolved to
    `/dev/block/mmcblk0p28`, and `/proc/partitions` reports `mmcblk0p28` as
    `1024` KiB.
- Backup:
  - FACT: read-only backup path:
    `/srv/forge/work/nx549j-preserve/misc-bcb/backup-20260529-130421/misc-backup.img`.
  - FACT: backup size is `1048576` bytes.
  - FACT: remote and local backup SHA-256 matched:
    `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
  - FACT: first 8 KiB of `misc` were all zero before BCB tests.
- Write/restore sanity:
  - FACT: `aosp-offset0` write directory:
    `/srv/forge/work/nx549j-preserve/misc-bcb/write-aosp-offset0-20260529-130504`.
  - FACT: the written page contained `boot-recovery` at byte `0x0` and
    `recovery\n` at byte `0x40`.
  - FACT: restore directory:
    `/srv/forge/work/nx549j-preserve/misc-bcb/restore-20260529-130513`.
  - FACT: restore SHA-256 matched the original misc backup:
    `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
  - FACT: `lk-page1` write directory:
    `/srv/forge/work/nx549j-preserve/misc-bcb/write-lk-page1-20260529-130535`.
  - FACT: the written page contained `boot-recovery` at page 1 byte `0x0`
    and `recovery\n` at page 1 byte `0x40`.
  - FACT: restore directory:
    `/srv/forge/work/nx549j-preserve/misc-bcb/restore-20260529-130543`.
  - FACT: restore SHA-256 matched the original misc backup:
    `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
  - FACT: after restore, the first 8 KiB of live `misc` were all zero again.
- Next runtime test:
  - Task 4 must run exactly one layout with `nx549j-test-bcb-recovery.sh`.
  - If the layout is wrong, the phone can fall back into the current 4.9
    bootloop and manual recovery entry will be needed before restore.
  - Do not treat BCB as a solved automatic fallback until a normal reboot
    returns to recovery without button input.
- Reusable skill:
  - FACT: `/home/n8n/.codex/skills/android-early-boot-no-userspace/SKILL.md`
    was created for future Qualcomm/MTK no-userspace boot diagnostics.
  - FACT: validation passed with
    `/home/n8n/.codex/skills/.system/skill-creator/scripts/quick_validate.py`.

2026-05-29 BCB auto-recovery selector test:

- Patch category: DIAGNOSTIC tooling/runtime evidence.
- Runtime status: `aosp-offset0` is proven to enter recovery automatically.
- Test directory:
  `/srv/forge/work/nx549j-preserve/misc-bcb/test-aosp-offset0-20260529-131128`.
- FACT: before the test, live `misc` first 8 KiB were all zero.
- FACT: backup image:
  `/srv/forge/work/nx549j-preserve/misc-bcb/test-aosp-offset0-20260529-131128/misc-backup.img`.
- FACT: backup SHA-256:
  `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
- FACT: the test wrote a 2048-byte BCB payload at `misc` page 0:
  - byte `0x0`: `boot-recovery`
  - byte `0x40`: `recovery\n`
  - payload SHA-256:
    `ceaae1a6314a19927ab431d55b46d08a6aaf73e9bcceb578244783c06bffd00e`
- FACT: the script issued a normal `adb reboot`.
- FACT: wait log shows three missing-ADB polls followed by:
  `target-online state=recovery model=Nubia Z11 mini s device=NX549J`.
- FACT: no manual recovery button input was used for the second
  `test-aosp-offset0-20260529-131128` run.
- FACT: the script restored live `misc`, and restore verify SHA-256 matched
  the original backup:
  `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
- FACT: after restore, live `misc` first 8 KiB were all zero again.
- INFERENCE: NX549J bootloader honors the modern/AOSP BCB layout at misc
  offset 0. `lk-page1` no longer needs a risky reboot test unless later
  evidence contradicts this.
- Important limit: this proves a recovery selector, not target-kernel logging.
 A failing early kernel can only use BCB after block storage is available, so
 pre-storage failures still require working persistent RAM logs or a later
 platform restart hook.

2026-05-29 attempt72 BCB-at-timeout result and queue hypothesis:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; automatic recovery failed.
- Attempt72 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt72-20260529-bcb-timeout-recovery`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt72-20260529-bcb-timeout-recovery/boot-recovery-timeout-120s-bcb.img`.
- SHA-256:
  `73c0487479dfdf53245a0c4f785cd7e9b803203d90d9dfa464f305f2c9eedbe0`.
- FACT: boot partition prefix verification on serial `30785d1a` matched the
  local attempt72 boot image SHA before reboot.
- FACT: preboot `misc` first 2048 bytes were all zero:
  `e5a00aa9991ac8a5ee3109844d84a55583bd20572ad3ffcd42792f3c36b183ad`.
- FACT: reboot watcher polled for 72 cycles / about 6 minutes and never saw
  `30785d1a` return to recovery.
- FACT: the user manually returned the device to recovery afterward.
- INFERENCE: writing BCB inside the timeout `delayed_work` is too late or not
  reliable for this failure window.
- INFERENCE: the strongest implementation bug in attempt72 is queueing the
  emergency timeout before kworker execution is guaranteed. Local
  `init/main.c` calls `frgmark_recovery_timeout_arm()` in early `start_kernel()`
  after `time_init()`, but calls `workqueue_init()` only later inside
  `kernel_init_freeable()` after `kthreadd_done`.
- Documentation cross-check:
  - Linux workqueue documentation describes work items as executing in worker
    threads, and `schedule_delayed_work()` only puts a job into the global
    workqueue after the delay.
  - Linux lockup watchdog documentation uses hrtimer as the independent timing
    primitive for lockup detection, which matches replacing the emergency path
    with a timer instead of threaded work.
  - AOSP `bootloader_message.h` documents the misc/BCB offset 0 layout; NX549J
    has already proven the `boot-recovery` selector at that layout.

2026-05-29 attempt73 timer fallback with prewritten BCB:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed; automatic recovery returned, but this is an
  invalid target-kernel test because host-prewritten BCB made LK boot recovery
  before executing the flashed boot image.
- Attempt73 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt73-20260529-timer-bcb-prewrite`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt73-20260529-timer-bcb-prewrite/boot-timer-bcb-prewrite-120s.img`.
- SHA-256:
  `5c9a412de8442b1742bf81410d9b1ae2a4479de9087b52b12c7d27a24e78c6e3`.
- Cmdline adds:
  `frgmark.recovery_timeout_sec=120 frgmark.bcb_misc_devt=179:28 panic=5 oops=panic softlockup_panic=1 hung_task_panic=1 nmi_watchdog=panic initcall_debug`.
- Cmdline intentionally does not add `frgmark.raw_wdt=1`.
- Source change:
  - `arch/arm64/kernel/frgmark.c` now maps PS_HOLD during `frgmark_init_iomap()`.
  - The emergency timeout has a plain `timer_list` callback,
    `frgmark_recovery_timeout_timer_fire()`, which writes only the IMEM
    recovery selector and drops mapped PS_HOLD.
  - The old `delayed_work` path remains as a secondary later path that can
    still write BCB and call `kernel_restart("recovery")` if kworkers run.
  - `frgmark_late_init()` attempts an in-kernel BCB prewrite if late initcalls
    are reached.
  - `frgmark_userspace_reached()` cancels the timer/work and clears BCB, so a
    successful Android userspace handoff should not leave sticky recovery.
- Host-side test helper:
  `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`.
  In the original attempt73 invocation it wrote the proven `aosp-offset0` BCB
  before flashing/rebooting, verified the boot partition prefix SHA, waited for
  recovery, captured recovery-side files, and restored the original `misc`
  backup after recovery returned.
- Verification:
  - `mka bootimage -j4`: passed.
  - `bash -n` for the new helper and BCB scripts: passed.
  - `git diff --check`: passed.
  - Repacked boot unpack check confirmed the expected header geometry and
    cmdline.
  - Repacked kernel and ramdisk SHA-256 values match the unpacked base boot
    kernel and ramdisk.
- Runtime result:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt73-20260529-timer-bcb-prewrite/runtime`.
  - Result: `automatic-recovery-ok`.
  - FACT: `Host prewrite BCB` was effectively enabled in this run, so
    `boot-recovery` was present before rebooting.
  - FACT: a prewritten BCB is already independently proven to select recovery
    on NX549J without executing the target boot partition.
  - INFERENCE: attempt73 is only a BCB sanity check and helper-flow test. It
    does not confirm the timer fallback and must not be used as evidence that
    the 4.9 kernel can reset itself.
  - Follow-up fix: the helper now defaults to no host BCB prewrite; the
    `--prewrite-bcb` flag is only for BCB sanity checks.

2026-05-29 attempt74 initcall BCB checkpoints plus timer reset:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; automatic recovery failed.
- Attempt74 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt74-20260529-initcall-bcb-timer`.
- Runtime directory:
  `/srv/forge/work/nx549j-preserve/release-attempt74-20260529-initcall-bcb-timer/runtime`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt74-20260529-initcall-bcb-timer/boot-initcall-bcb-timer-120s.img`.
- SHA-256:
  `7fb8b8d11cb48cd0006bbee710acda83140b3b4d4a810075d7200bdf4c8ae5bb`.
- Cmdline adds:
  `frgmark.recovery_timeout_sec=120 frgmark.bcb_misc_devt=179:28 panic=5 oops=panic softlockup_panic=1 hung_task_panic=1 nmi_watchdog=panic initcall_debug`.
- Source change over attempt73:
  - `frgmark_maybe_checkpoint_bcb()` writes AOSP BCB from initcall-stage
    `frgmark()` checkpoints if block storage is already usable.
  - The timer callback remains queue-independent and avoids sleeping/block I/O:
    it only refreshes the IMEM recovery selector and drops mapped PS_HOLD, with
    watchdog bite as fallback if PS_HOLD is unavailable.
  - `scripts/nx549j-flash-boot-with-bcb-fallback.sh` defaults to
    `Host prewrite BCB: 0`, so target-kernel tests no longer bypass the boot
    partition.
- Verification:
  - `mka bootimage -j4`: passed.
  - `bash -n scripts/nx549j-flash-boot-with-bcb-fallback.sh`: passed.
  - `git diff --check`: passed before packaging.
  - Repacked boot unpack check confirmed expected kernel/ramdisk identity.
  - Boot partition prefix verification on serial `30785d1a` matched the local
    attempt74 boot image SHA before reboot.
- Runtime facts:
  - `Host prewrite BCB: 0`.
  - Preboot `misc` backup first 256 bytes were all zero.
  - Recovery watcher timed out after 180 seconds with only
    `poll state=missing`.
  - After the timeout, ADB listed only another attached device; target
    `30785d1a` was not in recovery/adb.
- INFERENCE: the queue hypothesis has now been implemented and tested negative
  for this bootlogo hang. Either the kernel never reaches `time_init()` /
  timer arming, the timer cannot fire in the failing state, mapped PS_HOLD does
  not reset from this context, or the target resets without a recovery selector
  because block/BCB checkpoints are not reached.
- HYPOTHESIS: the next useful work is not another workqueue/reset variant; it
  is early-hang evidence. Highest-value gates are ramoops/pstore registration
  and another compiled-DTB audit against the Nubia 3.18 truth, especially
  early reserved-memory/secure-memory/PIL/QUSB/PM deltas that can fail before
  userspace or ADB.

2026-05-29 attempt75 pstore compat parser plus postcore panic probe:

- Patch category: DIAGNOSTIC.
- Runtime status: built; not flashed because target serial `30785d1a` is not
  currently in recovery/adb after attempt74 timeout.
- Attempt75 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt75-20260529-pstore-compat-postcore-panic`.
- Base built boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt75-20260529-pstore-compat-postcore-panic/boot-base-built-pstore-compat.img`.
- Base built boot SHA-256:
  `c49c49a95b5a240344939132386b083853ec93d4db8adb78de1f79470621ae46`.
- Test boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt75-20260529-pstore-compat-postcore-panic/boot-pstore-compat-postcore-panic.img`.
- Test boot SHA-256:
  `4ca1af2a3352f79d9175e2b57caa8ecfaef42a2fdc0ec7e0765939494f1bdccb`.
- Cmdline adds:
  `frgmark.recovery_timeout_sec=120 frgmark.bcb_misc_devt=179:28 frgmark.force_panic_stage=0x52 panic=5 oops=panic softlockup_panic=1 hung_task_panic=1 nmi_watchdog=panic initcall_debug`.
- Source changes:
  - `fs/pstore/ram.c` now parses both generic 4.9 ramoops properties and old
    Nubia/Android `android,ramoops-*` properties.
  - `fs/pstore/ram.c` logs the parsed DT address/size fields before probe
    continues, so recovery dmesg should show whether sizes are still zero.
  - `msm8953-mtp-nx549j.dts` now carries both property families on the ramoops
    node.
- Verification:
  - `mka bootimage -j4`: passed.
  - Build log shows `CC fs/pstore/ram.o` and `DTC msm8953-mtp-nx549j.dtb`.
  - Repacked test image unpacked as header v0, page size 2048, kernel
    `0x80008000`, ramdisk `0x81000000`, tags `0x80000100`.
  - Kernel / Image.gz-dtb SHA-256:
    `80ef13f614e9727caeddbf7dbdb236463afcac4332961b539d8e897cf41f966b`.
  - DTB SHA-256:
    `e355de8644a747558ddc7891f91b7a92143e172a191ca24c488f7ee875ef71d5`.
- Test intent:
  - `0x52` is `FRGMARK_STAGE_INITCALL_POSTCORE_DONE`; `ramoops_init` is a
    `postcore_initcall`, so pstore should be registered before this forced
    panic if the probe succeeds.
  - `frgmark()` writes BCB before forcing panic at the stage, so automatic
    recovery should be possible if block/BCB is alive by postcore.

2026-05-29 attempt76 no-loop guarded timeout:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; automatic recovery still failed.
- Attempt76 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt76-20260529-no-loop-guarded-timeout`.
- Runtime directory:
  `/srv/forge/work/nx549j-preserve/release-attempt76-20260529-no-loop-guarded-timeout/runtime-postcore`.
- Flashed image:
  `/srv/forge/work/nx549j-preserve/release-attempt76-20260529-no-loop-guarded-timeout/boot-postcore-panic-no-loop.img`.
- SHA-256:
  `836185ca02bc35a98fb52a86beaa57126f7fdf5db36af06f2f00c40c88c9a776`.
- Source change over attempt75:
  - `frgmark.recovery_timeout_sec` no longer sets `panic_timeout=5` or
    `panic_on_oops=1` immediately.
  - `frgmark_enable_recovery_panic_reboot()` enables panic reboot only after a
    successful in-kernel AOSP BCB recovery write.
  - forced panic/reset and timer/watchdog fallback refuse blind reset under
    recovery-timeout mode when BCB has not been written.
- Runtime facts:
  - Boot partition prefix SHA on serial `30785d1a` matched the local image.
  - Host prewrite BCB was not used.
  - The watcher timed out waiting for recovery after 180 seconds.
  - After timeout, target `30785d1a` was not visible in ADB; non-target devices
    remained visible.
- INFERENCE: attempt76 prevents intentional frgmark reset loops before BCB, but
  it still did not produce automatic recovery. It does not prove stage `0x52`
  was not reached; it proves BCB was not successfully written by then, or the
  boot froze before/around that path.

2026-05-29 attempt77 stock Nubia MSS DT correction:

- Patch category: BOOT-UNBLOCK candidate / stock-truth DT correction.
- Runtime status: built and superseded by attempt78 before flashing.
- Attempt77 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt77-20260529-nubia-mss-dt-no-loop`.
- Key source change:
  - The 3.18 Nubia stock tree deletes `qcom,pas-id` and
    `qcom,pil-mss-memsetup` from `qcom,mss@4080000`.
  - The 4.9 NX549J common DTS now applies the same deletion under
    `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`.
- Verification:
  - `mka bootimage -j4`: passed.
  - Repacked image cmdlines had no `panic=5`.
  - Decompiled compiled DTB `qcom,mss@4080000` had no `qcom,pas-id` and no
    `qcom,pil-mss-memsetup`.
- INFERENCE: this is a real hardware-truth correction, not a random driver
  disable. Runtime root-cause status is still unproven until flashed.

2026-05-29 attempt78 panic0 no-loop plus Nubia MSS DT:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK candidate.
- Runtime status: flashed and verified; automatic recovery still failed.
- Attempt78 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt78-20260529-nubia-mss-panic0-no-loop`.
- Test image:
  `/srv/forge/work/nx549j-preserve/release-attempt78-20260529-nubia-mss-panic0-no-loop/boot-nubia-mss-postcore-panic0-no-loop.img`.
- Test image SHA-256:
  `0bb9340901fae9d27268a154051fc9f60bc8cef5c26aabc4174742b0b23e824a`.
- Timeout-only image:
  `/srv/forge/work/nx549j-preserve/release-attempt78-20260529-nubia-mss-panic0-no-loop/boot-nubia-mss-panic0-timeout-120s.img`.
- Timeout-only image SHA-256:
  `4c89f79135e2dc74e3d11cc5ef44e7f999e96527953f65cea4a7572316ee2664`.
- Identity:
  - `kernel`: `fdedbe9467d90063b8759e07cdb93fe95180fe67e2cabe8e885b6ae769fe326a`
  - `System.map`: `4054fdd342c71b7ee1cdb55651f4bb7904ae0891ed95d386efaf826a29a4a3d3`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `d09b6ee6be992c44e664c3c0aaeba813341e3e69ad39af1cc5bebc9e37fe10b6`
- Source change over attempt77:
  - `arch/arm64/configs/lineageos_nx549j_defconfig` now sets
    `CONFIG_PANIC_TIMEOUT=0`.
- Verification:
  - `mka bootimage -j4`: passed.
  - `git diff --check`: passed.
  - `bash -n` for the flash/BCB scripts passed.
  - `sha256sum -c SHA256SUMS`: passed.
  - `kernel.config` confirms `CONFIG_PANIC_TIMEOUT=0` and pstore/ramoops
    remain enabled.
  - Decompiled compiled DTB keeps non-zero ramoops sizes and the MSS node lacks
    the stock-deleted PAS properties.
  - Repacked postcore image cmdline adds `frgmark.force_panic_stage=0x52` but
    still has no `panic=5`.
- Runtime facts:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt78-20260529-nubia-mss-panic0-no-loop/runtime-postcore`.
  - Target gate accepted serial `30785d1a` in recovery before flash.
  - Host prewrite BCB was not used.
  - Preboot `misc` first 8 KiB were all zero.
  - Boot image was pushed and flashed to
    `/dev/block/bootdevice/by-name/boot`.
  - Remote pushed image SHA and boot partition prefix SHA both matched
    `0bb9340901fae9d27268a154051fc9f60bc8cef5c26aabc4174742b0b23e824a`.
  - Waiter polled for recovery until timeout and recorded only
    `poll state=missing`.
  - After timeout, `adb devices -l` showed only non-target devices; target
    `30785d1a` was not visible.
- INFERENCE: `CONFIG_PANIC_TIMEOUT=5` was still a possible blind reboot source
  even after removing `panic=5` from cmdline. attempt78 removes that default;
  frgmark will re-enable panic reboot only after a successful BCB write.
- INFERENCE: because automatic recovery still failed with `CONFIG_PANIC_TIMEOUT=0`,
  the current failure is not explained by the generic panic timeout alone. The
  next useful work remains earliest-hang cause search: stock 3.18 DT deltas,
  reserved memory/PIL/secure memory, and a fallback evidence path that works
  before postcore/BCB.

2026-05-29 attempt79 stock Nubia CPR panic DT correction:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK candidate.
- Runtime status: flashed and verified; automatic recovery still failed.
- Attempt79 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt79-20260529-nubia-cpr-panic-dt`.
- Test image:
  `/srv/forge/work/nx549j-preserve/release-attempt79-20260529-nubia-cpr-panic-dt/boot-nubia-cpr-panic-dt-postcore-no-loop.img`.
- Test image SHA-256:
  `9dc462717700edfcb5a957b88906780199e6674a2694eab67ca4a71a24b5e4a8`.
- Timeout-only image:
  `/srv/forge/work/nx549j-preserve/release-attempt79-20260529-nubia-cpr-panic-dt/boot-nubia-cpr-panic-dt-timeout-120s.img`.
- Timeout-only image SHA-256:
  `d6e204e0305d4d9eefdd04cf17282bed6fa2cf5e586dfcdd9177a7a595cca56c`.
- Identity:
  - `kernel`: `de9bff7f6adcdf6069654b85e7ed152683788dc9e3eebd8370c30000325f7997`
  - `System.map`: `4054fdd342c71b7ee1cdb55651f4bb7904ae0891ed95d386efaf826a29a4a3d3`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `beb0558df6989ac87ecd337a262eb675ee3e5c7dc0a6f3b6cf65c3945b256a6f`
- Source change over attempt78:
  - `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`
    now applies the stock 3.18 Nubia deletion of
    `qcom,cpr-panic-reg-addr-list` and
    `qcom,cpr-panic-reg-name-list` from `&apc_cpr`.
- Evidence:
  - Stock 3.18 reference:
    `highwaystar-nx549j/kernel/arch/arm/boot/dts/qcom/nubia_common/msm8953-regulator.dtsi`
    deletes the same two properties from `apc_cpr: cpr4-ctrl@b018000`.
  - attempt78 decompiled DTB still contained both CPR panic-register
    properties, so this was a concrete 4.9-vs-stock mismatch.
- Verification:
  - `mka bootimage -j4`: passed.
  - `git diff --check`: passed.
  - `bash -n` for the flash/BCB scripts passed.
  - `sha256sum -c SHA256SUMS`: passed.
  - Decompiled compiled DTB has no `qcom,cpr-panic-reg-addr-list` or
    `qcom,cpr-panic-reg-name-list`.
  - Decompiled `qcom,mss@4080000` still lacks `qcom,pas-id` and
    `qcom,pil-mss-memsetup`.
  - `kernel.config` confirms `CONFIG_PANIC_TIMEOUT=0` and pstore/ramoops
    remain enabled.
  - Repacked timeout/postcore cmdlines have no `panic=5`; postcore keeps
    `frgmark.force_panic_stage=0x52`.
- Runtime facts:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt79-20260529-nubia-cpr-panic-dt/runtime-postcore`.
  - Target gate accepted serial `30785d1a` in recovery before flash.
  - Host prewrite BCB was not used.
  - Preboot `misc` first 8 KiB were all zero.
  - Boot image was pushed and flashed to
    `/dev/block/bootdevice/by-name/boot`.
  - Remote pushed image SHA and boot partition prefix SHA both matched
    `9dc462717700edfcb5a957b88906780199e6674a2694eab67ca4a71a24b5e4a8`.
  - Waiter polled for recovery until timeout and recorded only
    `poll state=missing`.
  - After timeout, `adb devices -l` showed only non-target devices; target
    `30785d1a` was not visible.
- HYPOTHESIS: if the CPR panic-register properties route an early regulator
  panic/reset into a bad low-level path on NX549J, matching stock 3.18 may
  remove the 2-3 second bootlogo reset path or let the postcore BCB/pstore
  diagnostic run.
- INFERENCE: this hypothesis is now runtime-negative for automatic recovery.
  The CPR DT correction is still stock-truth and can stay, but it did not make
  the postcore BCB/pstore probe return to recovery. Continue with earlier
  evidence paths and remaining stock DT deltas.
- Rollback condition: revert the two-property `&apc_cpr` deletion if a fresh
  capture proves a new CPR regulator probe failure absent before this image.

2026-05-29 attempt80 stock Nubia PM8953 PON reset DT correction:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK candidate.
- Runtime status: flashed and verified; automatic recovery still failed.
- Attempt80 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt80-20260529-nubia-pm8953-pon-reset-dt`.
- Test image:
  `/srv/forge/work/nx549j-preserve/release-attempt80-20260529-nubia-pm8953-pon-reset-dt/boot-nubia-pm8953-pon-reset-dt-postcore-no-loop.img`.
- Test image SHA-256:
  `7ca03c9fb9e5cd91166981469268c38a3f476389e66b0df40cc9d251c99560f3`.
- Timeout-only image:
  `/srv/forge/work/nx549j-preserve/release-attempt80-20260529-nubia-pm8953-pon-reset-dt/boot-nubia-pm8953-pon-reset-dt-timeout-120s.img`.
- Timeout-only image SHA-256:
  `fe25fc93b32ad61cad8bc9e4b60c5be42f29de3135e72ac8951b9b5f230cd023`.
- Identity:
  - `kernel`: `03a2056621f5fb0b5051ec80f0b3dc6b6c78dfa5eaa2d67972b2230296af6ef4`
  - `System.map`: `4054fdd342c71b7ee1cdb55651f4bb7904ae0891ed95d386efaf826a29a4a3d3`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt79:
  - `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`
    now applies the stock 3.18 Nubia PM8953 `qcom,pon_1` reset setup:
    `qcom,support-reset = <1>`, `qcom,s1-timer = <10256>`,
    `qcom,s2-timer = <2000>`, and `qcom,s2-type = <7>`.
- Evidence:
  - Stock 3.18 reference:
    `highwaystar-nx549j/kernel/arch/arm/boot/dts/qcom/nubia_common/msm-pm8953.dtsi`
    sets the same properties under `qcom,pm8953@0/qcom,power-on@800/qcom,pon_1`.
  - attempt79 decompiled DTB had `qcom,pon_1` but no
    `qcom,support-reset`, `qcom,s1-timer`, `qcom,s2-timer`, or
    `qcom,s2-type`.
- Verification:
  - `mka bootimage -j4`: passed.
  - `git diff --check`: passed.
  - `sha256sum -c SHA256SUMS`: passed.
  - Decompiled DTB `qcom,power-on@800/qcom,pon_1` has the stock reset
    properties and timers.
  - Decompiled DTB still has no CPR panic-register properties and no
    `qcom,pil-mss-memsetup`.
  - Decompiled `qcom,mss@4080000` still lacks `qcom,pas-id`.
  - Repacked postcore cmdline adds `frgmark.force_panic_stage=0x52` but still
    has no `panic=5`.
- Runtime facts:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt80-20260529-nubia-pm8953-pon-reset-dt/runtime-postcore`.
  - Target gate accepted serial `30785d1a` in recovery before flash.
  - Host prewrite BCB was not used.
  - Preboot `misc` first 8 KiB were all zero.
  - Boot image was pushed and flashed to
    `/dev/block/bootdevice/by-name/boot`.
  - Remote pushed image SHA and boot partition prefix SHA both matched
    `7ca03c9fb9e5cd91166981469268c38a3f476389e66b0df40cc9d251c99560f3`.
  - Waiter polled for recovery until timeout and recorded only
    `poll state=missing`.
  - After timeout, `adb devices -l` did not show target `30785d1a`.
- HYPOTHESIS: if the missing PM8953 PON reset setup prevents correct
  reset/recovery behavior after an early target-kernel failure, matching stock
  may improve automatic recovery after BCB/panic paths.
- INFERENCE: this hypothesis is now runtime-negative for automatic recovery.
  The PM8953 PON setup is still stock-truth and can stay, but it did not make
  the postcore BCB/pstore probe return to recovery.
- Rollback condition: revert the PM8953 `qcom,pon_1` additions if a fresh
  capture proves a new power-key/RESIN handling regression or a new
  qpnp-power-on probe failure absent before this image.

2026-05-29 attempt81 late initcall panic probe:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; automatic recovery still failed.
- Attempt81 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt81-20260529-late-initcall-panic-probe`.
- Test image:
  `/srv/forge/work/nx549j-preserve/release-attempt81-20260529-late-initcall-panic-probe/boot-late-initcall-panic-probe.img`.
- Test image SHA-256:
  `f174bcb0748dd8a517ddcae7f6e63ee46f15256ddb2b1e6d0c3032cd63df3c9b`.
- Identity:
  - `kernel`: `03a2056621f5fb0b5051ec80f0b3dc6b6c78dfa5eaa2d67972b2230296af6ef4`
  - `System.map`: `4054fdd342c71b7ee1cdb55651f4bb7904ae0891ed95d386efaf826a29a4a3d3`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt80:
  - None. attempt81 only repacks the attempt80 base boot image with
    `frgmark.force_panic_stage=0x57` instead of `0x52`.
- Evidence:
  - `frgmark_maybe_checkpoint_bcb()` only attempts BCB writes for initcall
    stage markers `0x50..0x57`.
  - `frgmark_late_init()` also prewrites BCB at late initcall when recovery
    timeout is armed.
  - Therefore the previous postcore `0x52` image can refuse forced panic if
    BCB is not writable yet; `0x57` is a stronger probe for whether late
    initcalls and BCB are reached.
- Verification:
  - `sha256sum -c SHA256SUMS`: passed.
  - Unpacked cmdline contains `frgmark.force_panic_stage=0x57`.
  - Unpacked cmdline has no `panic=5`.
  - The kernel/DTB are exactly attempt80, so MSS, CPR, and PM8953 PON
    stock-truth corrections remain included.
- Runtime facts:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt81-20260529-late-initcall-panic-probe/runtime-late`.
  - Target gate accepted serial `30785d1a` in recovery before flash.
  - Host prewrite BCB was not used.
  - Preboot `misc` first 8 KiB were all zero.
  - Boot image was pushed and flashed to
    `/dev/block/bootdevice/by-name/boot`.
  - Remote pushed image SHA and boot partition prefix SHA both matched
    `f174bcb0748dd8a517ddcae7f6e63ee46f15256ddb2b1e6d0c3032cd63df3c9b`.
  - Waiter polled for recovery until timeout and recorded only
    `poll state=missing`.
  - After timeout, `adb devices -l` did not show target `30785d1a`.
- Expected next marker: automatic recovery with fresh pstore/ramoops would
  prove the kernel reaches late initcall and can write BCB there. A timeout
  means the failure is before late BCB write, the late BCB write still fails,
  or the boot chain still ignores the selector/reset path.
- INFERENCE: attempt81 does not prove stage `0x57` was reached. It proves only
  that flashing the late-initcall forced-panic probe did not return to recovery
  automatically. The next useful step is either a manual recovery capture for
  any retained pstore/misc evidence or a lower-level evidence path that does
  not depend on late initcalls.

2026-05-29 attempt82 visual initcall markers:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; recovery return was manual, not
  automatic.
- Attempt82 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt82-20260529-visual-initcall-markers`.
- Preferred test image:
  `/srv/forge/work/nx549j-preserve/release-attempt82-20260529-visual-initcall-markers/boot-visual-initcall-markers-timeout-120s.img`.
- Preferred test image SHA-256:
  `8dc2fd159a4168edcc0aec79126161ccfefad05b00b2a6cddf19cb2493d8e1c4`.
- Late forced-panic variant:
  `/srv/forge/work/nx549j-preserve/release-attempt82-20260529-visual-initcall-markers/boot-visual-late-panic-probe.img`.
- Late forced-panic variant SHA-256:
  `5a9dcab0ecc2d5c7be9fa16d59e14ad2624bab4bf1862799b44fde0c3c72d43b`.
- Identity:
  - `kernel`: `537f2f27499ce4fe88308b6bdcb876b3ed544f90d40620611af4e4684dfad23d`
  - `System.map`: `291307fe456912a64fa5cea163f1eb6a963e8bbd94d23d1d8b7c1164ba63b8f6`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt81:
  - `nx549j_splashprobe_frg_stage(stage)` paints the splash framebuffer prefix
    when FRG initcall stages `0x50..0x57` are reached.
  - `frgmark(stage)` calls the visual marker before attempting BCB checkpoint
    writes, so a visible color can prove a stage was reached even if BCB write
    later blocks or fails.
- Color map:
  - `0x50` red, `0x51` green, `0x52` blue, `0x53` yellow,
    `0x54` cyan, `0x55` magenta, `0x56` white, `0x57` orange.
- Verification:
  - `mka bootimage -j4`: passed.
  - `git diff --check`: passed.
  - `sha256sum -c SHA256SUMS`: passed.
  - Kernel strings include `NX549J splashprobe: frg_stage=%02x`.
  - Timeout image has no forced panic; late variant has
    `frgmark.force_panic_stage=0x57`.
  - Decompiled DTB keeps MSS, CPR, and PM8953 PON stock-truth corrections.
- Expected next marker: if the screen color changes, that color gives the
  latest visible initcall stage reached. If the screen stays as the original
  bootlogo, the failure is before these stages or splash framebuffer writes
  are not visible from Linux.
- Runtime evidence:
  - FACT: the preferred timeout image was flashed and the boot partition prefix
    SHA-256 matched
    `8dc2fd159a4168edcc0aec79126161ccfefad05b00b2a6cddf19cb2493d8e1c4`.
  - FACT: the user manually returned the phone to recovery; the script result
    `automatic-recovery-ok` in the capture-local README is invalid as
    automatic-recovery proof for this run.
  - FACT: the user observed no visible color mutation on the bootlogo.
  - FACT: post-recovery capture at
    `/srv/forge/work/nx549j-preserve/release-attempt82-20260529-visual-initcall-markers/runtime-visual-timeout/postrecovery`
    had empty pstore and empty `/proc/last_kmsg`.
  - INFERENCE: visual splash writes are not a reliable evidence channel on the
    current boot path. The next diagnostic must leave a recovery-readable
    marker in persistent RAM before setup/initcall assumptions are made.

2026-05-29 attempt83 early ramoops markers:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed and verified; automatic recovery timed out.
- Attempt83 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt83-20260529-early-ramoops-markers`.
- Preferred test image:
  `/srv/forge/work/nx549j-preserve/release-attempt83-20260529-early-ramoops-markers/boot-early-ramoops-markers.img`.
- Preferred image SHA-256:
  `08cfc8ec70210d868738145da90fa21f2c7161aee00810dde41fe766c3417cd6`.
- Identity:
  - `Image.gz-dtb`: `97ab861b9e9b5c583cfd97d5cb492d530242aca792b0266412c1338305417198`
  - `System.map`: `295563015d6d25e410f4b5850ca1870e9d45922adf48d90fbbe9e25d83f921bf`
  - `vmlinux`: `34e6934a79fe3047331a758aeef4ce7d36678232e49b4b15d637caab055063bd`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt82:
  - `arch/arm64/kernel/head.S` now writes non-reset markers at `stext`
    milestones `0x70..0x75`.
  - `arch/arm64/kernel/frgmark.c` now has `frgmark_early(stage)`, writes the
    latest stage into IMEM and writes a valid persistent_ram dmesg record into
    physical ramoops `0x9ff00000`.
  - `arch/arm64/kernel/setup.c` calls `frgmark_early()` at early setup stages
    `0x80..0x85`, before `early_ioremap_reset()`.
  - `/srv/forge/android/nx549j/scripts/nx549j-collect-early-markers.sh`
    collects and decodes pstore marker evidence after manual recovery.
- Stage map:
  - `0x70`: arm64 `stext` entry.
  - `0x71`: boot args preserved.
  - `0x72`: `el2_setup` returned.
  - `0x73`: page tables created.
  - `0x74`: `__cpu_setup` returned.
  - `0x75`: about to enter `__primary_switch`.
  - `0x80`: `setup_arch()` after `early_ioremap_init()`.
  - `0x81`: FDT parsed.
  - `0x82`: early params parsed.
  - `0x83`: `arm64_memblock_init()` done.
  - `0x84`: `paging_init()` done.
  - `0x85`: before `early_ioremap_reset()`.
- Expected next marker:
  - After manual recovery, `/sys/fs/pstore/dmesg-ramoops-*` or collector
    `pstore-cat.txt` should contain `FRGE`.
  - Latest stage is stored as a 32-bit word `0x465247XX`.
  - If pstore remains empty, the failure is before the ramoops write survives,
    the reset path clears this DRAM, or recovery's pstore backend rejects this
    manually seeded persistent_ram record.
- Verification:
  - `mka bootimage -j4`: passed.
  - `git diff --check`: passed.
  - `bash -n` for flash and marker collector scripts: passed.
  - `sha256sum -c SHA256SUMS`: passed.
  - `unpack_bootimg.py` decoded header/cmdline successfully. FACT: the packed
    cmdline does not include `frgmark.recovery_timeout_sec`, so this attempt
    depends on manual recovery entry after the bootlogo hang.
- Flash/runtime evidence:
  - Runtime directory:
    `/srv/forge/work/nx549j-preserve/release-attempt83-20260529-early-ramoops-markers/runtime-flash`.
  - FACT: host pushed image SHA-256 and boot partition prefix SHA-256 both
    matched
    `08cfc8ec70210d868738145da90fa21f2c7161aee00810dde41fe766c3417cd6`.
  - FACT: the helper rebooted without host-prewritten BCB and then timed out
    after polling `state=missing` for 20 seconds.
  - INFERENCE: no automatic recovery was observed. The next required action is
    manual recovery entry followed by
    `/srv/forge/android/nx549j/scripts/nx549j-collect-early-markers.sh`.
- Rollback condition: remove the head.S/setup_arch direct-marker patch after a
  capture proves the stop stage or if the manually seeded ramoops record
  corrupts recovery pstore handling.

2026-05-29 attempt84 BCB retry timeout image:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged; not flashed in this note.
- Attempt84 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt84-20260529-bcb-retry-timeout`.
- Preferred test image:
  `/srv/forge/work/nx549j-preserve/release-attempt84-20260529-bcb-retry-timeout/boot-bcb-retry-timeout-120s.img`.
- Preferred image SHA-256:
  `9456194afb760be1c36ffc1e1682919a7255eabccfa304cc5c4952e7857da432`.
- Identity:
  - `Image.gz-dtb`: `b485d5ee2563b7d610980f4351752685bd0d4ccb934754bf4bd140e7c7ceb44a`
  - `System.map`: `cfb8ebc2adc05b74f0b2a4923716a076d55ab8d32381ee824fdeeb46d59e7572`
  - `vmlinux`: `42b68fa5873433a5307244e7565914c46a38bf6975698cc7622df4e158eb20c9`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt83:
  - `device/nubia/nx549j/BoardConfig.mk` adds the target cmdline
    `frgmark.recovery_timeout_sec=120 frgmark.bcb_misc_devt=179:28 initcall_debug`.
  - `arch/arm64/kernel/frgmark.c` schedules a BCB retry work item after the
    recovery timeout is armed.
  - The timeout/timer paths defer reset while BCB is still not written, then
    reboot only after `frg_recovery_bcb_written` is true.
- Verification:
  - `mka bootimage -j4`: passed in `39:56`.
  - `git diff --check`: passed before build.
  - `bash -n /srv/forge/android/nx549j/scripts/nx549j-collect-early-markers.sh`: passed.
  - `unpack_bootimg.py` decoded the packed cmdline and confirmed
    `frgmark.recovery_timeout_sec=120`,
    `frgmark.bcb_misc_devt=179:28`, and `initcall_debug`.
  - `sha256sum -c SHA256SUMS`: passed.
- Expected next marker:
  - Flash this image to `boot` when NX549J is back in recovery.
  - Preferred command:
    `/srv/forge/android/nx549j/scripts/nx549j-run-attempt84.sh`.
  - Wait at least 120 seconds plus margin. A valid success requires automatic
    recovery entry without button input and a boot partition prefix matching
    `9456194afb760be1c36ffc1e1682919a7255eabccfa304cc5c4952e7857da432`.
  - If automatic recovery appears, inspect
    `runtime/flash-boot-bcb-*/after-recovery/marker-grep.txt` and
    `runtime/flash-boot-bcb-*/after-recovery/marker-od.txt`.
  - If automatic recovery times out and the user manually returns to recovery,
    run
    `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh <runtime/flash-boot-bcb-...>`
    to collect pstore/marker evidence and restore `misc`.
- Claim boundary: do not claim automatic recovery fixed from this build alone.
  The current state is "ready to flash", not "runtime proven".
- Rollback condition: if the image still loops or hangs without recovery, keep
  the earlier attempt83 marker path and collect manual-recovery pstore/IMEM
  evidence before moving the timeout earlier again.

2026-05-29 attempt85 bounded no-BCB fallback:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged; not flashed in this note.
- Attempt85 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt85-20260529-bounded-no-bcb-fallback`.
- Preferred test image:
  `/srv/forge/work/nx549j-preserve/release-attempt85-20260529-bounded-no-bcb-fallback/boot-bounded-no-bcb-fallback-120s.img`.
- Preferred image SHA-256:
  `cb310e2ff99bc3d69b986c3e4a2ed55d5b79c3a439351a5e06bb831682cd188c`.
- Identity:
  - `Image.gz-dtb`: `f87575622ca0b3f8abce9f3d0852dfeb9fbcb8a7d03c7a57f5a8442cd4540dc8`
  - `System.map`: `5da7a7e1d412f8531f4f2b6018cf2a00674834842fc8daa23b1557cbb174682f`
  - `vmlinux`: `bc6eedb9377c590f0cb3d598242228ca629deaa42c755feb11330858dc64c35d`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt84:
  - `include/linux/frgmark.h` adds timeout marker stages `0x5e`
    (`recovery_no_bcb_grace`) and `0x5f` (`recovery_no_bcb_reset`).
  - `arch/arm64/kernel/frgmark.c` keeps BCB-first behavior, but bounds the
    no-BCB case with `FRG_RECOVERY_NO_BCB_GRACE_SEC=60`.
  - If BCB is still unavailable after timeout plus grace, the kernel writes
    marker `0x5f`, refreshes recovery selectors, dumps kmsg, and forces reset
    instead of deferring forever.
- Verification:
  - `mka bootimage -j4`: passed in `03:01`.
  - `git diff --check`: passed.
  - `bash -n` for helper scripts: passed.
  - `unpack_bootimg.py` decoded the packed cmdline and confirmed
    `frgmark.recovery_timeout_sec=120`,
    `frgmark.bcb_misc_devt=179:28`, and `initcall_debug`.
  - `sha256sum -c SHA256SUMS`: passed.
  - `scripts/nx549j-verify-release-artifact.sh`: passed and wrote
    `VERIFY.md`, proving the packaged artifact contains the expected cmdline,
    `frgmark_recovery_timeout_fire`, `frgmark_userspace_reached`,
    `frg_recovery_no_bcb_reset_jiffies`, and the no-BCB/userspace marker
    strings in `vmlinux`. The same verifier also checks `CONFIG_PSTORE`,
    `CONFIG_PSTORE_CONSOLE`, `CONFIG_PSTORE_PMSG`, `CONFIG_PSTORE_RAM`, and
    compiled DTB `ramoops@9ff00000` with non-zero record/console/pmsg sizes.
- Expected next marker:
  - Preferred command:
    `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
    (currently delegates to `nx549j-run-attempt85.sh`).
  - The runner waits 220 seconds by default to cover the 120-second timeout,
    60-second no-BCB grace, and margin.
  - If recovery appears automatically, inspect
    `runtime/flash-boot-bcb-*/SUMMARY.md`,
    `runtime/flash-boot-bcb-*/boot-identity.env`,
    `runtime/flash-boot-bcb-*/after-recovery/marker-grep.txt` and
    `runtime/flash-boot-bcb-*/after-recovery/marker-od.txt` for `FRGE`,
    `recovery_no_bcb_reset`, or marker word `0x4652475f`.
  - If automatic recovery times out and the user manually returns to recovery,
    run
    `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh <runtime/flash-boot-bcb-...>`
    to collect pstore/marker evidence and restore `misc`.
- Claim boundary: this still does not prove automatic recovery or userspace.
  It only makes the no-BCB timeout path bounded and evidence-producing.
- Rollback condition: if marker `0x5f` proves repeated non-recovery loops,
  disable the no-BCB forced reset and focus on why BCB/block storage never
  becomes writable before timeout.

2026-05-29 attempt86 timer-safe no-BCB fallback:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged; not flashed in this note.
- Attempt86 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt86-20260529-timer-safe-no-bcb-fallback`.
- Preferred test image:
  `/srv/forge/work/nx549j-preserve/release-attempt86-20260529-timer-safe-no-bcb-fallback/boot-timer-safe-no-bcb-fallback-120s.img`.
- Preferred image SHA-256:
  `cbd808dbf40ae8e99c09ec6abbe71e1dd60f04c7e74e362397d69cbb0b2b8061`.
- Identity:
  - `Image.gz-dtb`: `0ad072d5d1b35942722fda363f45cb0f6e8dbe9b8fdb6a61e92e5382c9bfb1ea`
  - `System.map`: `534a7a816088d2b7b7d1f0f2af02319dde2b5c186bad3ef11acce6e8ef7550cc`
  - `vmlinux`: `a31a4ebfe4065cce71b0622f5442fe9b0930ea997bdb5b662c2dbf97f5e9599d`
  - `kernel.config`: `d4e2488ffbe1bd0a42214c2668cdb6ecef12e485c0b7e652d213f5ad06eb4c68`
  - `msm8953-mtp-nx549j.dtb`: `b4c5f2d03f3c9c23280936a01d40b0e8b477e49dc92228b0b49d34140be630f5`
- Source change over attempt85:
  - `arch/arm64/kernel/frgmark.c` now keeps QPNP PON restart-reason writes
    out of the timer/softirq no-BCB fallback path.
  - If the no-BCB fallback fires from delayed work, it still uses full
    recovery selector priming, including IMEM and QPNP PON.
  - If it fires from the timer callback, it refreshes only the IMEM recovery
    selector before raw reset/watchdog, because the PMIC/SPMI-backed
    `qpnp_pon_set_restart_reason()` path is not safe to call from timer
    context.
- Verification:
  - `mka bootimage -j4`: passed in `01:39`.
  - `scripts/nx549j-verify-release-artifact.sh`: passed and wrote
    `VERIFY.md`.
  - `sha256sum -c SHA256SUMS`: passed.
  - Packed cmdline still contains `frgmark.recovery_timeout_sec=120`,
    `frgmark.bcb_misc_devt=179:28`, and `initcall_debug`.
- Expected next marker:
  - Preferred command:
    `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
    (currently delegates to `nx549j-run-attempt86.sh`).
  - If recovery appears automatically, inspect
    `runtime/flash-boot-bcb-*/SUMMARY.md`,
    `runtime/flash-boot-bcb-*/boot-identity.env`,
    `runtime/flash-boot-bcb-*/after-recovery/marker-grep.txt`, and
    `runtime/flash-boot-bcb-*/after-recovery/marker-od.txt`.
  - If automatic recovery times out and the user manually returns to recovery,
    run
    `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh <runtime/flash-boot-bcb-...>`.
- Claim boundary: this still does not prove automatic recovery or userspace.
  It only removes a context bug from the diagnostic no-BCB reset path and
  packages the next image for flashing.
- Rollback condition: if attempt86 behaves worse than attempt85 before any
  marker evidence is captured, revert the attempt86 `frgmark.c` selector split
  and compare timer/workqueue reset behavior with the matching boot SHA.

2026-05-29 attempt65 recovery reboot with marker-before-reset:

- Patch category: DIAGNOSTIC.
- Runtime status: built and verified; not flashed in this note.
- Relationship to attempt64: supersedes attempt64. attempt65 keeps the same
  head.S recovery reboot gate, but additionally ensures
  `frgmark.force_reset_stage=<stage>` stores the reached marker before the
  forced recovery reset.
- FACT: the in-tree Qualcomm restart path for `cmd == "recovery"` writes
  recovery reboot magic `0x77665502` to
  `qcom,msm-imem-restart_reason`.
- FACT: `msm8953.dtsi` maps `qcom,msm-imem-restart_reason` at IMEM offset
  `0x65c`; physical address is `0x0860065c`.
- Source change for attempt65:
  - `arch/arm64/kernel/head.S` calls
    `nx549j_head_post_cpu_setup_recovery_probe` immediately after
    `bl __cpu_setup` and before `b __primary_switch`.
  - The head.S probe writes marker `FRG stage 0x64` at physical IMEM
    `0x08600040`, writes recovery reboot magic `0x77665502` at physical
    `0x0860065c`, writes a 512 KiB high-contrast prefix at physical splash
    address `0x90001000`, then writes `0` to MSM PS_HOLD physical
    `0x004ab000` and waits.
  - `arch/arm64/kernel/frgmark.c` changes `frgmark.force_reset_stage=<stage>`
    to write the stage marker first, then write recovery magic `0x77665502`,
    then drop PS_HOLD. If restart-reason ioremap fails, it refuses the
    non-recovery PS_HOLD reset rather than intentionally creating a boot loop.
- Attempt65 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt65-20260529-recovery-reboot-marker-first`.
- Attempt65 boot image:
  - `/srv/forge/work/nx549j-preserve/release-attempt65-20260529-recovery-reboot-marker-first/boot-head-post-cpusetup-recovery-markerfirst.img`
  - SHA-256:
    `5bc50394c4768bd8e7b280eec1bf0a566f2d10e03454ccfc9296d831f336ceea`.
  - size: `13746176`.
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`.
  - header version: `0`, page size: `2048`.
  - kernel load: `0x80008000`, ramdisk load: `0x81000000`, tags load:
    `0x80000100`.
- Attempt65 artifact identity:
  - `Image.gz-dtb` / `kernel` SHA-256:
    `08ec3e7317f5beca80e3322f7ed8292ac70f590ad39485f432e7e041d5e39817`.
  - `System.map` SHA-256:
    `494b6699a0a8ea3f497c7f94c7275d5f0aa3624982961519d9d615d9d7d3a735`.
  - `vmlinux` SHA-256:
    `ee35d3b10434f01177184f180cf33fa3b2c04c1ee5f62a1c8a202dcd60d6766a`.
  - `msm8953-mtp-nx549j.dtb` SHA-256:
    `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`.
- FACT: `System.map` contains:
  - `stext` at `ffffff8009a00000`.
  - `nx549j_head_post_cpu_setup_recovery_probe` at `ffffff8009a00030`.
  - `__primary_switch` at `ffffff80091cf2b0`.
- Verification:
  - `make ... Image.gz-dtb -j4`: passed.
  - `git diff --check`: passed.
  - `python3 ... unpack_bootimg.py --boot_img boot-head-post-cpusetup-recovery-markerfirst.img`:
    header geometry and cmdline matched the values above.
  - `sha256sum -c SHA256SUMS`: passed.
- Expected runtime result:
  - If the phone reboots into recovery, the MMU-off head path reaches
    page-table creation plus CPU setup, and LK honors the IMEM recovery reboot
    reason without a PMIC restart-reason write.
  - If it reboots back into the boot image, the same gate is reached but LK
    likely needs the PMIC restart reason in addition to IMEM.
  - If it stays on the unchanged bootlogo, the stop is between `stext` entry
    and the post-`__cpu_setup` gate.
- Flash safety:
  - Two devices were visible through reverse ADB at the transfer step:
    `30785d1a` is Nubia NX549J, `0123456789ABCDEF` is a non-target offline
    device.
  - Any ADB command that touches a device must use the explicit Nubia serial:
    `adb -H 127.0.0.1 -P 15037 -s 30785d1a ...`.
  - Do not flash this image with a bare `adb` command.
- Transfer evidence:
  - FACT: attempt65 was pushed to target path
    `/tmp/boot-head-post-cpusetup-recovery-markerfirst.img` on serial
    `30785d1a`.
  - FACT: target-side SHA-256 matched local artifact:
    `5bc50394c4768bd8e7b280eec1bf0a566f2d10e03454ccfc9296d831f336ceea`.
  - FACT: recovery by-name map reported
    `/dev/block/bootdevice/by-name/boot -> /dev/block/mmcblk0p21`.
  - FACT: `/proc/partitions` reported `mmcblk0p21` as `40960` KiB, larger
    than the attempt65 image size `13746176`.
  - Flash gate: waiting for explicit confirmation before writing attempt65 to
    `boot`.
- Rollback condition: revert the `head.S` probe after this single diagnostic
  flash, or immediately if attempt65 is not the next flashed image.

2026-05-29 attempt64 head.S post-cpu-setup recovery reboot probe:

- Patch category: DIAGNOSTIC.
- Runtime status: built and verified; not flashed in this note.
- FACT: the in-tree Qualcomm restart path for `cmd == "recovery"` writes
  recovery reboot magic `0x77665502` to
  `qcom,msm-imem-restart_reason`.
- FACT: `msm8953.dtsi` maps `qcom,msm-imem-restart_reason` at IMEM offset
  `0x65c`; physical address is `0x0860065c`.
- Source change for attempt64:
  - `arch/arm64/kernel/head.S` calls
    `nx549j_head_post_cpu_setup_recovery_probe` immediately after
    `bl __cpu_setup` and before `b __primary_switch`.
  - The head.S probe writes marker `FRG stage 0x64` at physical IMEM
    `0x08600040`, writes recovery reboot magic `0x77665502` at physical
    `0x0860065c`, writes a 512 KiB high-contrast prefix at physical splash
    address `0x90001000`, then writes `0` to MSM PS_HOLD physical
    `0x004ab000` and waits.
  - `arch/arm64/kernel/frgmark.c` changes `frgmark.force_reset_stage=<stage>`
    to write the same recovery magic before PS_HOLD. If the restart-reason
    ioremap fails, it refuses the non-recovery PS_HOLD reset rather than
    intentionally creating a boot loop.
- Attempt64 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt64-20260529-head-post-cpusetup-recovery`.
- Attempt64 boot image:
  - `/srv/forge/work/nx549j-preserve/release-attempt64-20260529-head-post-cpusetup-recovery/boot-head-post-cpusetup-recovery.img`
  - SHA-256:
    `f39d66517fef407902e96be04df8d83859afebf7140887982a3fdbc377c0b672`.
  - size: `13746176`.
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`.
  - header version: `0`, page size: `2048`.
  - kernel load: `0x80008000`, ramdisk load: `0x81000000`, tags load:
    `0x80000100`.
- Attempt64 artifact identity:
  - `Image.gz-dtb` / `kernel` SHA-256:
    `0e4c2754070726e33ddb2aa60cf8512ddda7eed07b3d930a8c04dee5941c87e9`.
  - `System.map` SHA-256:
    `2416c6fdf760ce2a3fc70ef762d832572309a1eceb26f3e161a8fdac96258593`.
  - `vmlinux` SHA-256:
    `84514764e60d35c8f0b239166ebe68ad3edba1a2e56053f036a93f867a874776`.
  - `msm8953-mtp-nx549j.dtb` SHA-256:
    `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`.
- FACT: `System.map` contains:
  - `stext` at `ffffff8009a00000`.
  - `nx549j_head_post_cpu_setup_recovery_probe` at `ffffff8009a00030`.
  - `__primary_switch` at `ffffff80091ce2b0`.
- Verification:
  - `make ... Image.gz-dtb -j4`: passed.
  - `git diff --check`: passed.
  - `python3 ... unpack_bootimg.py --boot_img boot-head-post-cpusetup-recovery.img`:
    header geometry and cmdline matched the values above.
  - `sha256sum -c SHA256SUMS`: passed.
- Expected runtime result:
  - If the phone reboots into recovery, the MMU-off head path reaches
    page-table creation plus CPU setup, and LK honors the IMEM recovery reboot
    reason without a PMIC restart-reason write.
  - If it reboots back into the boot image, the same gate is reached but LK
    likely needs the PMIC restart reason in addition to IMEM.
  - If it stays on the unchanged bootlogo, the stop is between `stext` entry
    and the post-`__cpu_setup` gate.
- Flash safety:
  - Two devices are currently visible through reverse ADB:
    `30785d1a` is Nubia NX549J, `810BBMM22D7S` is Meizu M2 Note.
  - Any ADB command that touches a device must use the explicit Nubia serial:
    `adb -H 127.0.0.1 -P 15037 -s 30785d1a ...`.
  - Do not flash this image with a bare `adb` command.
- Rollback condition: revert the `head.S` probe after this single diagnostic
  flash, or immediately if attempt64 is not the next flashed image.

2026-05-29 attempt63 head.S post-cpu-setup reset probe:

- Patch category: DIAGNOSTIC.
- Runtime status: built and verified; not flashed in this note.
- FACT: attempt62 produced an immediate cyclic reboot after about 2.5 seconds
  on the bootlogo, and recovery dmesg recorded PS_HOLD as the power-off
  reason.
- INFERENCE: LK enters the 4.9 image and reaches arm64 `stext`; the
  bootloader/no-entry branch is rejected for the current packaging.
- Source change for attempt63:
  - Removed the attempt62 `stext` entry call.
  - Added NX549J-only `nx549j_head_post_cpu_setup_probe` in
    `arch/arm64/kernel/head.S`.
  - Called it immediately after `bl __cpu_setup` and before
    `b __primary_switch`.
  - The probe writes physical IMEM `0x08600040`, writes a 512 KiB
    high-contrast prefix at physical splash address `0x90001000`, then writes
    `0` to MSM PS_HOLD physical `0x004ab000` and waits.
- Attempt63 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt63-20260529-head-post-cpusetup-reset`.
- Attempt63 boot image:
  - `/srv/forge/work/nx549j-preserve/release-attempt63-20260529-head-post-cpusetup-reset/boot-head-post-cpusetup-pshold.img`
  - SHA-256:
    `bdb0227f5e86c0c97fd140cd9baff71c3cda8a22e6d974316c3c55a8952215ce`.
  - size: `13746176`.
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`.
  - header version: `0`, page size: `2048`.
  - kernel load: `0x80008000`, ramdisk load: `0x81000000`, tags load:
    `0x80000100`.
- Attempt63 artifact identity:
  - `Image.gz-dtb` / `kernel` SHA-256:
    `e30d0b2c11980cee3712602aba77f92c03451f354bd2e3972fc543e89c7d6215`.
  - `System.map` SHA-256:
    `ed4cc12614d3b6465b7c3f117138cb605fd6f1fddb7c9b4951e02069008b82bc`.
  - `vmlinux` SHA-256:
    `cb7a76543e2b1b25c6f976e7f4bc7fd1679e4e4c69f12b2213ea995609500db0`.
  - `msm8953-mtp-nx549j.dtb` SHA-256:
    `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`.
- FACT: `System.map` contains:
  - `stext` at `ffffff8009a00000`.
  - `nx549j_head_post_cpu_setup_probe` at `ffffff8009a00030`.
  - `__primary_switch` at `ffffff80091d22b0`.
- Verification:
  - `make ... Image.gz-dtb -j4`: passed.
  - `git diff --check`: passed.
  - `python3 ... unpack_bootimg.py --boot_img boot-head-post-cpusetup-pshold.img`:
    header geometry and cmdline matched the values above.
  - `sha256sum -c SHA256SUMS`: passed.
- Expected runtime result:
  - If the phone immediately reboots/resets after flashing attempt63, the
    MMU-off head path reaches page-table creation plus CPU setup.
  - If it stays on the unchanged bootlogo, the stop is between `stext` entry
    and the post-`__cpu_setup` gate.
- Flash safety:
  - Two devices may be connected. Any ADB command that touches a device must
    use an explicit serial:
    `adb -H 127.0.0.1 -P 15037 -s <serial> ...`.
  - Do not flash this image with a bare `adb` command.
- Rollback condition: revert the `head.S` probe after this single diagnostic
  flash, or immediately if attempt63 is not the next flashed image.

2026-05-29 attempt62 head-entry reset probe:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed to `boot` from recovery via reverse ADB on
  2026-05-29; runtime observation is pending.
- FACT: attempt61 was flashed to `boot` and the user observed "no visible
  mutation, absolutely unchanged" on the bootlogo.
- INFERENCE: the previous setup_arch visual splash probe is not a useful next
  gate. It does not prove a later driver/DTB blocker; it only says that the
  kernel did not visibly reach the first C-level splash write, or that the
  selected splash memory is not visible.
- FACT: attempt60/attempt61 boot images were packed with the highwaystar
  cmdline shape:
  `console=null androidboot.console=ttyHSL0 ... firmware_class.path=/vendor/firmware_mnt/image loop.max_part=7`.
- FACT: attempt53/attempt55/attempt59 used the nearest verified cmdline shape:
  `androidboot.hardware=qcom ... loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`.
- INFERENCE: the cmdline drift is a real packaging regression even if it does
  not by itself explain a pre-setup_arch stop.
- Source change for attempt62:
  - Added an NX549J-only `nx549j_head_entry_probe` in
    `arch/arm64/kernel/head.S`.
  - Called it at `stext` before `preserve_boot_args`, `el2_setup`,
    page-table setup, `setup_arch()`, early_ioremap, pstore, and cmdline
    parsing.
  - The probe writes physical IMEM `0x08600040`, writes a 512 KiB
    high-contrast prefix at physical splash address `0x90001000`, then writes
    `0` to MSM PS_HOLD physical `0x004ab000` and waits.
- Attempt62 artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt62-20260529-head-entry-reset`.
- Attempt62 boot image:
  - `/srv/forge/work/nx549j-preserve/release-attempt62-20260529-head-entry-reset/boot-head-entry-pshold.img`
  - SHA-256:
    `d3bb0a5462ef12fca55859b8177c77488f87abdf08c3df18bff2261e1bf2a486`.
  - size: `13746176`.
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`.
  - header version: `0`, page size: `2048`.
  - kernel load: `0x80008000`, ramdisk load: `0x81000000`, tags load:
    `0x80000100`.
- Attempt62 artifact identity:
  - `Image.gz-dtb` / `kernel` SHA-256:
    `c145cd6ebc805892ce654cad9329ef7ad2f7d8c22cd5e9a0c3dccb5ac6f0662e`.
  - `System.map` SHA-256:
    `6f7f385b2288dd209b6d7e61846617fb3c1aea7295342f842567d1ad6aadb220`.
  - `vmlinux` SHA-256:
    `77f8cee7dece951bdeb4e94f9c0c2b0cf2ca2c08ef7ef94d555ed70f155e385e`.
  - `msm8953-mtp-nx549j.dtb` SHA-256:
    `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`.
- FACT: `System.map` contains:
  - `stext` at `ffffff8009a00000`.
  - `nx549j_head_entry_probe` at `ffffff8009a00030`.
- Verification:
  - `make ... Image.gz-dtb -j4`: passed.
  - `python3 ... unpack_bootimg.py --boot_img boot-head-entry-pshold.img`:
    header geometry and cmdline matched the values above.
  - `sha256sum -c SHA256SUMS`: passed.
- Flash evidence:
  - FACT: reverse ADB was available at `adb -H 127.0.0.1 -P 15037`, and
    `adb devices -l` reported `30785d1a recovery`.
  - FACT: recovery by-name map reported
    `/dev/block/bootdevice/by-name/boot -> /dev/block/mmcblk0p21`.
  - FACT: `/proc/partitions` reported `mmcblk0p21` as `40960` KiB, larger
    than the attempt62 image size `13746176`.
  - FACT: pushed `/tmp/boot-head-entry-pshold.img` SHA-256 matched
    `d3bb0a5462ef12fca55859b8177c77488f87abdf08c3df18bff2261e1bf2a486`.
  - FACT: recovery flash command completed:
    `dd if=/tmp/boot-head-entry-pshold.img of=/dev/block/bootdevice/by-name/boot bs=4096 conv=fsync`
    returned `3356+0 records in`, `3356+0 records out`,
    `13746176 bytes`.
  - FACT: on-device boot-prefix verification matched attempt62:
    `dd if=/dev/block/bootdevice/by-name/boot of=/tmp/boot-prefix-verify-attempt62.img bs=4096 count=3356`
    followed by `sha256sum /tmp/boot-prefix-verify-attempt62.img` returned
    `d3bb0a5462ef12fca55859b8177c77488f87abdf08c3df18bff2261e1bf2a486`.
  - FACT: after verification, `adb -H 127.0.0.1 -P 15037 reboot` returned
    successfully.
- Expected runtime result:
  - If the phone immediately reboots/resets after flashing attempt62, LK is
    entering the 4.9 image and reaching arm64 `stext`.
  - If it stays on the unchanged bootlogo, the next branch is bootloader/kernel
    handoff proof, not setup_arch, display, initcalls, or ramdisk.
- Rollback condition: revert the `head.S` probe after this single diagnostic
  flash, or immediately if attempt62 is not the next flashed image.
- Runtime result:
  - FACT: user observed an immediate cyclic reboot after about 2.5 seconds on
    the bootlogo.
  - FACT: recovery capture lives at
    `/srv/forge/work/nx549j-preserve/release-attempt62-20260529-head-entry-reset/recovery-capture-20260529-065700`.
  - FACT: post-boot recovery capture verified the boot partition prefix SHA-256
    as `d3bb0a5462ef12fca55859b8177c77488f87abdf08c3df18bff2261e1bf2a486`,
    matching attempt62.
  - FACT: recovery dmesg recorded `Power-off reason: Triggered from PS_HOLD`
    for both SID0 and SID2 PMIC power-on nodes.
  - FACT: pstore was empty, `/proc/last_kmsg` was absent, and direct IMEM
    readback was unavailable because recovery lacks a working `/dev/mem`
    backend even after a temporary char node was created.
  - INFERENCE: attempt62 reached arm64 `stext` and executed the diagnostic
    PS_HOLD write. The bootloader/no-entry branch is rejected for this image.
  - INFERENCE: the next diagnostic branch is later MMU-off `head.S` execution,
    not setup_arch/display/initcalls/ramdisk yet.

2026-05-29 attempt61 visual splash-probe cycle:

- Patch category: DIAGNOSTIC.
- Runtime status: flashed to `boot` from recovery via reverse ADB on
  2026-05-29; visual observation is still pending.
- FACT: attempt60 artifact integrity was re-verified with
  `sha256sum -c SHA256SUMS` in
  `/srv/forge/work/nx549j-preserve/release-attempt60-20260528-nubia-dtb-common`;
  every listed artifact returned `OK`.
- FACT: attempt59 and attempt60 both contain a boot image named
  `boot-force-reset-setuparch-stage02.img`, but they are different files:
  - attempt59 SHA-256:
    `bee89b9ec7f62eccd2140cdc96c8e1c338e103198f387bb91243c6f8b58723f2`,
    size `12920832`.
  - attempt60 SHA-256:
    `2e8a00f852c2b171f37ec3cd8cb1dbca184b3d3d17f4e5e84c539cf6a5764db9`,
    size `13746176`.
- INFERENCE: future runtime interpretation must use unique attempt61 filenames
  and hashes, not basename memory. The attempt61 boot image basename is reserved
  as `boot-splashprobe-setuparch.img`.
- FACT: the attempt61 preservation directory is
  `/srv/forge/work/nx549j-preserve/release-attempt61-20260529-splash-probe`.
- FACT: source-map gate for the splash probe found the first safe visual hook
  after `early_ioremap_init()` in
  `arch/arm64/kernel/setup.c`. The checkpoint order before code insertion is:
  - `setup.c:267` after `early_ioremap_init()`.
  - `setup.c:269` after `setup_machine_fdt(__fdt_pointer)`.
  - `setup.c:271` after `parse_early_param()`.
  - `setup.c:287` after `arm64_memblock_init()`.
  - `setup.c:289` after `paging_init()`.
  - `setup.c:305` before/around `early_ioremap_reset()`.
  - `setup.c:308` after `psci_dt_init()`.
  - `init/main.c:504` immediately after `setup_arch(&command_line)` returns.
- FACT: `include/asm-generic/early_ioremap.h` exposes `early_ioremap()` and
  `early_iounmap()`, and `arch/arm64/include/asm/fixmap.h` limits one early
  boot map slot to `SZ_256K`.
- INFERENCE: the splash probe must write the 19 MiB splash region in small
  mapped chunks. attempt61 will only touch a conservative visible prefix and
  remap/unmap each chunk.
- Source change for attempt61:
  - Added `include/linux/nx549j_splashprobe.h`.
  - Added `arch/arm64/kernel/nx549j_splashprobe.c`.
  - Built the helper only when `CONFIG_MACH_NUBIA_NX549J=y` through
    `arch/arm64/kernel/Makefile`.
  - Inserted checkpoint calls in `arch/arm64/kernel/setup.c` and one
    post-`setup_arch()` call in `init/main.c`.
- FACT: the helper writes a 2 MiB prefix of the splash framebuffer at
  `0x90001000`, remapped in `SZ_256K` chunks because one arm64 early-ioremap
  bootmap slot is 256 KiB.
- Visual checkpoint map for attempt61:
  - stage 1, after `early_ioremap_init()`: red fill.
  - stage 2, after `setup_machine_fdt()`: green fill.
  - stage 3, after `parse_early_param()`: blue fill.
  - stage 4, after `arm64_memblock_init()`: yellow fill.
  - stage 5, after `paging_init()`: white/black stripes.
  - stage 6, before `early_ioremap_reset()`: magenta fill.
  - stage 7, after `psci_dt_init()`: cyan fill.
  - stage 8, near end of `setup_arch()`: blue/white stripes.
  - stage 9, after `setup_arch()` returns to `start_kernel()`: black/white
    checker pattern.
- Expected next marker: the screen mutates away from the static Nubia bootlogo
  to the last reached stage color/pattern.
- Rollback condition: revert the attempt61 splashprobe source edits after the
  visual result is captured, or immediately if the diagnostic image fails to
  build.
- Attempt61 build result:
  - `make ... dtbs -j4`: passed.
  - `make ... Image.gz-dtb -j4`: passed.
  - `sha256sum -c SHA256SUMS`: passed in the attempt61 directory.
  - `System.map` contains `nx549j_splashprobe` at
    `ffffff8009a06c78`.
- Attempt61 boot image:
  - `/srv/forge/work/nx549j-preserve/release-attempt61-20260529-splash-probe/boot-splashprobe-setuparch.img`
  - SHA-256:
    `ae532516fed0a915fa6b2a78d8f97817907c8efba329b5e389f61f943ce51f77`.
  - size: `13746176`.
  - header version: `0`, page size: `2048`.
  - kernel load: `0x80008000`, ramdisk load: `0x81000000`, tags load:
    `0x80000100`.
  - cmdline:
    `console=null androidboot.console=ttyHSL0 androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 firmware_class.path=/vendor/firmware_mnt/image loop.max_part=7`.
- Attempt61 artifact identity:
  - `Image.gz-dtb` / `kernel` SHA-256:
    `e65d0b7fa179470844fd7a379fa72a70eca7ae79cef7642a32deb0cf58fde3bc`,
    size `12918446`.
  - `msm8953-mtp-nx549j.dtb` SHA-256:
    `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`.
  - `System.map` SHA-256:
    `a537fffe969f2cdbc434f5d33c6b412cf9981a57b8044cc74e0dcf7e7d7e3a06`.
  - `vmlinux` SHA-256:
    `d585db31c5285fad27ef9be62e5315c8fc9b9dd2b3b3b727b02b238ade9ebdde`.
  - `kernel.config` SHA-256:
    `f1b92d9073d21145b2bf1039aca0f4f8a3c778f61c45666ecae45833687856d2`.
- Attempt61 DTB facts:
  - `qcom,board-id = <0x8 0x0>` present.
  - `qcom,bark-time = <0x4e20>` present.
  - `qseecom@84a00000` present.
  - vendor fstab points to
    `/dev/block/platform/soc/7824900.sdhci/by-name/oem`.
  - `qseecom@85b00000` absent.
- Task 5 status: flashed; visual result captured.
  - FACT: reverse ADB was available at `adb -H 127.0.0.1 -P 15037`, and
    `adb devices -l` reported `30785d1a recovery`.
  - FACT: recovery by-name map reported
    `/dev/block/bootdevice/by-name/boot -> /dev/block/mmcblk0p21`.
  - FACT: `/proc/partitions` reported `mmcblk0p21` as `40960` KiB, larger
    than the attempt61 image size `13746176`.
  - FACT: pushed `/tmp/boot-splashprobe-setuparch.img` SHA-256 matched
    `ae532516fed0a915fa6b2a78d8f97817907c8efba329b5e389f61f943ce51f77`.
  - FACT: recovery flash command completed:
    `dd if=/tmp/boot-splashprobe-setuparch.img of=/dev/block/bootdevice/by-name/boot bs=4096 conv=fsync`
    returned `3356+0 records in`, `3356+0 records out`,
    `13746176 bytes`.
  - FACT: on-device boot-prefix verification matched attempt61:
    `dd if=/dev/block/bootdevice/by-name/boot of=/tmp/boot-prefix-verify.img bs=4096 count=3356`
    followed by `sha256sum /tmp/boot-prefix-verify.img` returned
    `ae532516fed0a915fa6b2a78d8f97817907c8efba329b5e389f61f943ce51f77`.
  - FACT: after verification, `adb -H 127.0.0.1 -P 15037 reboot` returned
    successfully.
  - FACT: user observation after reboot: no visible mutation; bootlogo stayed
    absolutely unchanged.
  - INFERENCE: setup_arch-level splash writes are not a reliable current
    branch; attempt62 moves the gate earlier to arm64 `stext`.

2026-05-28 update: the user flashed attempt59
`boot-force-reset-setuparch-stage02.img`, and the device still stayed on the
bootlogo until manual recovery entry. There was no obvious automatic reset.

INFERENCE: the marker-only/reset-only diagnostic path is now paused. The next
work item is a DTS/compiled-DTB reconstruction using Nubia 3.18 hardware truth
and nearby 3.18-to-4.9 MSM8953 references, not another forced-panic or PS_HOLD
stage image.

Reference audit:

- `/srv/forge/work/nx549j-reference-audit-20260528/reports/REFERENCE_AUDIT.md`

Main audit result:

- FACT: local Nubia 3.18 and current NX549J 4.9 both use MTP board identity
  with `qcom,board-id = <8 0>` and PMIC id
  `<0x010016 0x010011 0x0 0x0>`, so QRD SKU3 donor identity is not the first
  fix.
- FACT: current 4.9 NX549J DTS is a partial manual subset and does not carry
  the Nubia 3.18 `NX549/head.dtsi` plus `nubia_common/head.dtsi` overlay graph.
- FACT: a concrete low-level mismatch is qseecom/secure memory. Generic 4.9
  keeps `qseecom@85b00000` with `0x800000`, while Nubia 3.18 deletes that node
  and defines `qseecom@84a00000` with `0x1900000` plus Nubia-specific support.
- HYPOTHESIS: the 4.9 boot image may be handed a DTB that matches board/PMIC
  selection but is too far from Nubia hardware truth in secure-memory,
  firmware/fstab, QUSB, PM, charger, regulator, and related low-level overlay
  shape to reach a recoverable diagnostic point.

2026-05-28 attempt60 Nubia-common DTB candidate:

- Patch category: BOOT-UNBLOCK.
- Runtime status: not flashed yet.
- Source change:
  - Added
    `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`.
  - Included it from
    `arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dts`.
  - Kept MTP board identity unchanged.
  - Ported the first low-level Nubia 3.18 hardware-truth deltas into the 4.9
    DTB: firmware/fstab vendor on `oem`, qseecom relocation to
    `qseecom@84a00000`, `other_ext_mem` at `0x84a00000/0x1e00000`, watchdog
    timing, QUSB PHY init sequence, and NX549J charger / fuel-gauge properties.
- Build-only fix:
  - Wrapped the Xiaomi-only `memcpy_pstore()` helper in
    `fs/pstore/ram_core.c` with the same Xiaomi config guard that uses it. This
    removes the NX549J `-Wunused-function` forbidden warning without changing
    NX549J pstore behavior.
- Verification:
  - `make ... dtbs -j4` passed.
  - `make ... Image.gz-dtb -j4` passed.
  - Decompiled DTB contains `qseecom@84a00000`, vendor fstab `by-name/oem`,
    `qcom,bark-time = <0x4e20>`, and `qcom,board-id = <0x8 0x0>`.
  - Decompiled DTB does not contain `qseecom@85b00000`.
  - Boot header geometry matches attempt59: kernel `0x80008000`, ramdisk
    `0x81000000`, tags `0x80000100`, page size `2048`, header version `0`.
  - `sha256sum -c SHA256SUMS` passed.

Attempt60 preserved artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt60-20260528-nubia-dtb-common`

Attempt60 artifact identity:

- `boot-normal-nubia-dtb-common.img`
  - SHA-256: `77b966667ad277415429db459e734934d9b38dc7e3c4d9719771421f92f64e39`
  - size: `13746176`
- `boot-force-reset-setuparch-stage02.img`
  - SHA-256: `2e8a00f852c2b171f37ec3cd8cb1dbca184b3d3d17f4e5e84c539cf6a5764db9`
  - size: `13746176`
  - cmdline includes `frgmark.force_reset_stage=0x02`
- `boot-force-reset-consoleinit-stage14.img`
  - SHA-256: `fd0fa039cc5c4dabd1c182548e45a69ea65035c654340fb86506317ca06f2608`
  - size: `13746176`
  - cmdline includes `frgmark.force_reset_stage=0x14`
- `boot-force-reset-postcore-stage52.img`
  - SHA-256: `37bd9713c642b307ed9eed6c7eeaa058929bcc1eff5d91af6db67f1c211f248e`
  - size: `13746176`
  - cmdline includes `frgmark.force_reset_stage=0x52`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `ac310be516563615e1e81e68303cff1befe05126d21e15af2e5894d1dc9b67e7`
  - size: `12918387`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `76c754d63dde231ea913d11ed61c4b614e8c9e0c4a3dc26f76b25e9674c4bf92`
- `msm8953-mtp-nx549j.decompiled.dts`
  - SHA-256: `d01de751b61364906d4a73178a08deb6d442c118f5ed34ea76dbbd432a7ae619`
- `System.map`
  - SHA-256: `a5b1366d6048adcc2bc38a722d0e6d7de95f6ae4f32df774a206c79dd080500b`
- `vmlinux`
  - SHA-256: `82c961e0b51255c351e8245cf0ca32f70b13d1639a87dfec0700fd862b5a50bb`
- `kernel.config`
  - SHA-256: `f1b92d9073d21145b2bf1039aca0f4f8a3c778f61c45666ecae45833687856d2`

Recommended attempt60 test order:

1. Flash `boot-normal-nubia-dtb-common.img`.
2. If it still hangs on bootlogo, flash
   `boot-force-reset-setuparch-stage02.img`.
3. If stage02 visibly reboots, continue with stage14 then stage52 from the same
   attempt60 directory.

2026-05-28 PS_HOLD force-reset diagnostic build completed through direct Kbuild
target `Image.gz-dtb`. This supersedes attempt58 for the next reboot/reachability
test.

Reason:

- User-observed result for attempt58 `boot-forcepanic-setuparch-stage02.img`:
  the device still stayed on bootlogo until manual recovery entry; there was no
  obvious automatic panic reboot.
- Code audit found that a stage02 `panic()` is too early to prove hardware reset
  reachability. `CONFIG_PANIC_TIMEOUT=5` is enabled, but panic reboot eventually
  enters `machine_restart()`. On arm64 this calls `arm_pm_restart` only if a
  platform restart handler has already registered.
- The Qualcomm restart handler is registered by `drivers/power/reset/msm-poweroff.c`
  from the `qcom,pshold` platform driver. That driver probes from initcalls and
  sets `arm_pm_restart = do_msm_restart` only after mapping the PS_HOLD node.
  Stage02 is immediately after `setup_arch()`, before those initcalls, so a
  forced panic can fall through to `Reboot failed -- System halted` and look
  exactly like a bootlogo hang.
- Attempt59 keeps the normal FRGmark and forced-panic diagnostics, but adds a
  separate `frgmark.force_reset_stage=0xNN` cmdline gate that directly maps the
  MSM8953 PS_HOLD register (`restart@4ab000`, `qcom,pshold`) and writes `0`.
  This is DIAGNOSTIC only; it is meant to prove target-kernel reachability and
  bring back visible resets, not to fix Android boot.

Preserved diagnostic artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt59-20260528-force-reset-pshold`

Diagnostic artifact identity:

- `boot-normal-force-reset-capable.img`
  - SHA-256: `1c23e8f335552f09227d6afc1e2a16104e2c0aed929f008924e415fa2c7868d7`
  - size: `12920832`
- `boot-force-reset-setuparch-stage02.img`
  - SHA-256: `bee89b9ec7f62eccd2140cdc96c8e1c338e103198f387bb91243c6f8b58723f2`
  - size: `12920832`
  - cmdline includes `frgmark.force_reset_stage=0x02`
- `boot-force-reset-consoleinit-stage14.img`
  - SHA-256: `823c9c58da930293a79b8405dec88210137aea8399322ec142245619b0fc0a12`
  - size: `12920832`
  - cmdline includes `frgmark.force_reset_stage=0x14`
- `boot-force-reset-postcore-stage52.img`
  - SHA-256: `3c542addb48e4fc63f98d7b4f90b273d752975211d023cbba73463e0edd33bdf`
  - size: `12920832`
  - cmdline includes `frgmark.force_reset_stage=0x52`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `26533fbf3af15508c28e073973425cbf91b346de380ae37784bdc939755327c3`
  - size: `12091729`
- `System.map`
  - SHA-256: `45704937248708b10cd7997eef10b65d9edffca6b04c8efa2e9a68d28bdcdf1f`
- `vmlinux`
  - SHA-256: `d9fd1855f536d2053f6fa5ac843b6f1814b57ceb660f48cf41edf104c4bc8742`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
- `kernel.config`
  - SHA-256: `f1b92d9073d21145b2bf1039aca0f4f8a3c778f61c45666ecae45833687856d2`
- `nx549j-frgmark-decode-spm.sh`
  - SHA-256: `b4dd49c7b13501050f0092801465484bbf0496c5021f0781b319d9669c9b0f8d`

Diagnostic boot image header for attempt59 boot images:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- kernel size: `12091729`
- ramdisk size: `824801`
- OS version: `11.0.0`
- OS patch level: `2024-02`

Verification:

- `sha256sum -c SHA256SUMS` passed in the preserved attempt59 directory.
- Unpacked diagnostic boot images start with `ANDROID!`, have header version
  `0`, page size `2048`, kernel load `0x80008000`, ramdisk load `0x81000000`,
  and contain the expected `frgmark.force_reset_stage` cmdline value.
- Direct Kbuild succeeded after the same generated `.config` workaround used
  for attempt57/58:
  - `# CONFIG_MODULE_SIG is not set`
  - `# CONFIG_SYSTEM_TRUSTED_KEYRING is not set`

Recommended test order:

1. Flash `boot-force-reset-setuparch-stage02.img`.
   - If it visibly reboots or bootloops, target 4.9 reached
     `setup_arch_done`, and the previous stage02 hang was an early panic
     restart-handler limitation rather than proof that stage02 was not reached.
   - If it still hangs on bootlogo, either 4.9 is not reaching
     `setup_arch_done`, or direct PS_HOLD reset is not effective this early on
     NX549J.
2. Only after a stage02 reset works, flash
   `boot-force-reset-consoleinit-stage14.img`.
   - If stage02 resets but stage14 hangs, the failure window is between
     `setup_arch_done` and `console_init_done`.
3. Only after stage14 reset works, flash
   `boot-force-reset-postcore-stage52.img`.
   - If stage52 resets, the kernel reaches after postcore initcall completion
     and pstore/ramoops testing can resume from a better bracket.

2026-05-28 force-panic no-IMEM-gate diagnostic build completed through direct
Kbuild target `Image.gz-dtb`. This is superseded by attempt59 for reboot
diagnostics, but remains useful for forced-panic/pstore tests after reset
reachability is known.

Reason:

- User-observed result for `b3c70823` / attempt56 stage14: the device stayed on
  bootlogo until manual recovery entry; there was no obvious automatic panic
  reboot.
- Code audit found a diagnostic weakness in `frgmark()`: forced panic was
  checked only after `frg_imem` was available. If `frgmark_init_iomap()` failed
  or the code path never initialized IMEM, then even a reached forced stage
  would silently skip `panic()`.
- Attempt58 moves the forced-panic check before the `frg_imem` early return, so
  the diagnostic panic no longer depends on IMEM marker persistence.

Preserved diagnostic artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt58-20260528-forcepanic-noimemgate`

Diagnostic artifact identity:

- `boot-normal-forcepanic-noimemgate.img`
  - SHA-256: `2a52ee4bcdaf165a16d172535a1bad2a7adeac6d5b96be6da348c440e8a88d51`
  - size: `12920832`
- `boot-forcepanic-setuparch-stage02.img`
  - SHA-256: `c7d7746e64ba79972df052e35fe62a9fbf14bf8537afcbe4d2a13969df4301a7`
  - size: `12920832`
  - cmdline includes `frgmark.force_panic_stage=0x02`
- `boot-forcepanic-consoleinit-stage14.img`
  - SHA-256: `ca95d68d13a49f0e603b4fb033537a2c5e070bb260bc78c24ecd7559a3924d7f`
  - size: `12920832`
  - cmdline includes `frgmark.force_panic_stage=0x14`
- `boot-forcepanic-postcore-stage52.img`
  - SHA-256: `513d17beb49f1525bf593799c408a926ea1c41648c97221e5484a112e8a5ff07`
  - size: `12920832`
  - cmdline includes `frgmark.force_panic_stage=0x52`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `17d15e14d9af68c11f90dcd697970a10be30de02ced1838e4ae3ab0ae3294a17`
  - size: `12092343`
- `System.map`
  - SHA-256: `a4b0c2a88ef2968e2d116b12cde44844f32fc35af431648f4411f704dd74c034`
  - size: `5350766`
- `vmlinux`
  - SHA-256: `f42dcf498ed05cc7f7d93cc66a6f470b914fa5c27456c22adb5f2ddb2fc3d1e8`
  - size: `299113120`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
  - size: `276277`
- `kernel.config`
  - SHA-256: `f1b92d9073d21145b2bf1039aca0f4f8a3c778f61c45666ecae45833687856d2`
- `nx549j-frgmark-decode-spm.sh`
  - SHA-256: `b4dd49c7b13501050f0092801465484bbf0496c5021f0781b319d9669c9b0f8d`

Diagnostic boot image header for attempt58 boot images:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- kernel size: `12092343`
- ramdisk size: `824801`

Recommended test order:

1. Flash `boot-forcepanic-setuparch-stage02.img`.
   - If it still hangs on bootlogo with no automatic reboot, the failure is
     before or around `setup_arch_done`, or the early panic/restart path itself
     is not reliable enough for this device.
   - If it reboots, target 4.9 reached `setup_arch_done`.
2. Flash `boot-forcepanic-consoleinit-stage14.img`.
   - If stage02 reboots but stage14 hangs, the first failure window is between
     `setup_arch_done` and `console_init_done`.
3. Flash `boot-forcepanic-postcore-stage52.img`.
   - If stage52 reboots and recovery pstore contains FRGmark panic output,
     ramoops readback is usable.
   - If stage52 reboots but pstore is empty, pstore/ramoops readback is the
     blocker.

2026-05-28 initcall-level diagnostic build completed through direct Kbuild
target `Image.gz-dtb` after the Android ninja boot target hit a host OpenSSL
header blocker in `scripts/extract-cert.c` / `scripts/sign-file.c`. For this
diagnostic build only, the generated kernel `.config` was adjusted to avoid
host certificate tooling:

- `# CONFIG_MODULE_SIG is not set`
- `# CONFIG_SYSTEM_TRUSTED_KEYRING is not set`

The source defconfig was not changed for this workaround.

This build adds FRGmark initcall-level completion stages:

- `0x50` `initcall_early_done`
- `0x51` `initcall_core_done`
- `0x52` `initcall_postcore_done`
- `0x53` `initcall_arch_done`
- `0x54` `initcall_subsys_done`
- `0x55` `initcall_fs_done`
- `0x56` `initcall_device_done`
- `0x57` `initcall_late_done`

Preserved diagnostic artifact directory:

- `/srv/forge/work/nx549j-preserve/release-attempt57-20260528-initcall-pstore`

Diagnostic artifact identity:

- `boot-normal-initcall-markers.img`
  - SHA-256: `73c79b5882af0041d7744b8f71e5027ccb928571bbfd815d67e8f9c38bf04a38`
  - size: `12920832`
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug`
- `boot-forcepanic-initcall-postcore-stage52.img`
  - SHA-256: `629e0420ce764f230002d64401e518d3bc3fb17a8bdc5c11b3d90066742e89b3`
  - size: `12920832`
  - cmdline:
    `androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78af000 loop.max_part=16 androidboot.usbconfigfs=true androidboot.selinux=permissive buildvariant=userdebug frgmark.force_panic_stage=0x52`
- `kernel` / `Image.gz-dtb`
  - SHA-256: `e4a16bb318ee8367cdc11faa2a2931044f7ee6f83c8891730c6408f80b7ce4d9`
  - size: `12092951`
- `System.map`
  - SHA-256: `75e9f5964b2276235ea81a35bfe05428874ea65a5a6dec73e6150b9b893bfd06`
  - size: `5350809`
- `vmlinux`
  - SHA-256: `788e22c7f07bd2438a3fb6e4c841dfbfa45327cbda69816f5be7aaac6d82ce66`
  - size: `299113008`
- `msm8953-mtp-nx549j.dtb`
  - SHA-256: `38c6b8416cb8183d5b5b2d3d0feacfa504e9f15ca3622c58680ccec48dafda78`
  - size: `276277`
- `kernel.config`
  - SHA-256: `f1b92d9073d21145b2bf1039aca0f4f8a3c778f61c45666ecae45833687856d2`
- `nx549j-frgmark-decode-spm.sh`
  - SHA-256: `b4dd49c7b13501050f0092801465484bbf0496c5021f0781b319d9669c9b0f8d`

Diagnostic boot image header for both attempt57 boot images:

- kernel load address: `0x80008000`
- ramdisk load address: `0x81000000`
- tags load address: `0x80000100`
- page size: `2048`
- header version: `0`
- kernel size: `12092951`
- ramdisk size: `824801`

Expected capture for `boot-forcepanic-initcall-postcore-stage52.img`:

- Captured boot prefix SHA-256 should be
  `629e0420ce764f230002d64401e518d3bc3fb17a8bdc5c11b3d90066742e89b3`.
- The browser recovery collector should show
  `collector_version=2026-05-28-nx549j-initcall-v3`.
- Success marker: recovery pstore contains
  `FRGmark forced panic firing stage=52 name=initcall_postcore_done` or the
  panic reason
  `FRGmark forced panic stage=52 name=initcall_postcore_done`.
- If this capture still has empty pstore after a verified flash, then pstore
  persistence is a real blocker after ramoops registration and Task 7 should
  move to an alternate recovery-readable marker path.

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
  `collector_version=2026-05-28-nx549j-critical-first-v2`.
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
- current SHA-256: `b4dd49c7b13501050f0092801465484bbf0496c5021f0781b319d9669c9b0f8d`
- preserved copy:
  `/srv/forge/work/nx549j-preserve/release-attempt55-20260527-frgmark/nx549j-frgmark-decode-spm.sh`
- preserved attempt55 copy SHA-256:
  `5091ff56ce810fd43dc01b978ab23540cc3c6297fee7f424f7561780624fc8a3`

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
  `/srv/forge/work/nx549j-preserve/release-attempt57-20260528-initcall-pstore/boot-forcepanic-initcall-postcore-stage52.img`.
- Run the refreshed browser recovery collector version
  `2026-05-28-nx549j-initcall-v3`.
- Verify the captured boot prefix hash is
  `629e0420ce764f230002d64401e518d3bc3fb17a8bdc5c11b3d90066742e89b3`
  before drawing any runtime conclusion.
- Decode any crash address against
  `/srv/forge/work/nx549j-preserve/release-attempt57-20260528-initcall-pstore/System.map`.
- Decode marker text or raw IMEM marker words with
  `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`.
- Expected diagnostic improvement: recovery should expose a fresh forced-panic
  pstore record for stage `0x52` / `initcall_postcore_done`. If it does not,
  Task 7 should stop relying on recovery pstore/devmem and implement another
  recovery-readable marker path.

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

### 2026-05-28 - `14cbc448` attempt56 partial identity verification

FACT:

- Test: `14cbc448-07b4-404d-b749-d15ba692cf38`
- Capture:
  `/home/n8n/forge-work/gdrive-active/debug/7636c990-f363-48b2-8b9d-c8f7d1a4792f/eaa5e6a9-d89a-4f07-822f-421fca8f3e14/browser-debug-evidence-1780003252049.tar`
- Captured file:
  `evidence/adb/failed_boot/mtk/partitions/boot-full-16MB.img`
- Captured size: `428031` bytes
- Captured prefix SHA-256:
  `53ac5da7bf121f5a65b3abf57ddd54a9c71ca429164bb1237b67b048db860f12`
- The captured `428031` bytes match the first `428031` bytes of
  `/srv/forge/work/nx549j-preserve/release-attempt56-20260528-forcepanic/boot-forcepanic-consoleinit-stage14.img`.
- Full local attempt56 forced-panic artifact SHA-256:
  `030cde5bd1bd1888b299e4c96cf6d852574e708b338d9392f3ee9628383f315d`
- Collector version in this tar:
  `2026-05-28-nx549j-freshness-v1`

FACT:

- The tar does not contain `pstore_index.txt`, `pstore_dump.txt`,
  `nx549j/frgmark_raw.txt`, `frgmark_decoded.txt`, `boot-header-hex.txt`, or
  `recovery-header-hex.txt`.
- `boot_candidate_warning.txt` is not trusted: the captured boot bytes start
  with Android boot magic, but the recovery-side header command produced no
  header evidence before the warning.

INFERENCE:

- The forced-panic attempt56 image was at least partially flashed correctly.
- `14cbc448` cannot prove whether pstore/ramoops is empty or whether the
  target kernel reached the forced-panic stage, because the browser collector
  spent the unstable WebUSB transfer on the large boot dump before the critical
  small evidence.

### 2026-05-28 - `b3c70823` attempt56 full identity and pstore timing result

FACT:

- Test: `b3c70823-63ae-4c0a-80e2-1886cae5d5b5`
- Build Station status: `recovery_booted`
- Capture:
  `/home/n8n/forge-work/gdrive-active/debug/7636c990-f363-48b2-8b9d-c8f7d1a4792f/a4cfbf25-9665-4d7b-8c38-0303ede91869/browser-debug-evidence-1780004612526.tar`
- Extracted text:
  `/home/n8n/build-station/docs/run_reports/2026-05-28_nx549j_b3c70823_debug_text.txt`
- Preflight:
  `/home/n8n/build-station/docs/run_reports/2026-05-28_nx549j_b3c70823_preflight.json`
- Collector version:
  `2026-05-28-nx549j-critical-first-v2`

FACT:

- The tar contains the refreshed critical-first files:
  - `boot-header-hex.txt`
  - `recovery-header-hex.txt`
  - `pstore_index.txt`
  - `pstore_dump.txt`
  - `nx549j/frgmark_raw.txt`
  - `nx549j/frgmark_decoded.txt`
  - full `boot-full-16MB.img`
- Captured boot partition size: `16777216`
- The first `12929024` captured boot bytes match
  `/srv/forge/work/nx549j-preserve/release-attempt56-20260528-forcepanic/boot-forcepanic-consoleinit-stage14.img`
  exactly.
- Captured boot prefix SHA-256:
  `030cde5bd1bd1888b299e4c96cf6d852574e708b338d9392f3ee9628383f315d`
- Boot header magic is `ANDROID!`.
- Captured boot cmdline includes `frgmark.force_panic_stage=0x14`.
- Recovery live kernel is still TWRP `Linux version 3.18.31-lineageos...`.
- `/sys/fs/pstore` exists in recovery but is empty.
- `frgmark_decoded.txt` reports `no nx549j FRGmark markers decoded`.
- `frgmark_raw.txt` reports `no devmem command worked`.
- `pstore_dump.txt` contains recovery `/cache/recovery/last_log` / `log`,
  including the flash of `boot-forcepanic-consoleinit-stage14.img` to
  `/dev/block/mmcblk0p21`; it is not target 4.9 pstore output.

INFERENCE:

- The refreshed browser collector works for NX549J: small critical files are
  captured before the large boot dump, and the full boot partition transfer
  completed in this run.
- The forced-panic stage-14 image was flashed correctly.
- User observation after this run: the device stayed on bootlogo until manual
  recovery entry; there was no obvious automatic panic reboot.
- Empty pstore in `b3c70823` does not prove that pstore/ramoops is globally
  broken and does not prove that 4.9 failed before `console_init_done`.
  `console_init_done` is reached in `start_kernel()` before initcall execution,
  while this kernel registers ramoops from `postcore_initcall(ramoops_init)`.
- Because the original `frgmark()` returned before checking forced panic when
  `frg_imem` was unavailable, attempt56 stage14 cannot distinguish "did not
  reach stage14" from "reached stage14 but IMEM marker setup disabled the
  force-panic gate".
- The next falsifying tests should use attempt58, where forced panic no longer
  depends on IMEM marker availability.

### 2026-05-28 - Capture freshness gate for the next NX549J tar

FACT:

- Browser recovery collector now writes `collector_version` into
  `collection-report.json` and `collection-log.txt`.
- Expected browser version for the next capture:
  `2026-05-28-nx549j-initcall-v3`
- Browser NX549J failed-boot captures should include:
  - `evidence/adb/failed_boot/mtk/partitions/boot-header-hex.txt`
  - `evidence/adb/failed_boot/mtk/partitions/recovery-header-hex.txt`
- Browser NX549J failed-boot captures should collect `pstore_index.txt`,
  `pstore_dump.txt`, and `nx549j/frgmark_*` before large `boot-full` dumps.
- Local-agent failed-boot text writes its own collector version and captures
  boot/recovery 2048-byte partition headers in `mtk_partition_headers`.

INFERENCE:

- If the next browser tar lacks
  `collector_version=2026-05-28-nx549j-initcall-v3` or the NX549J header
  files, treat it as stale/incomplete before drawing a new boot-magic or
  pstore conclusion.
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
  `collector_version=2026-05-28-nx549j-initcall-v3`, boot/recovery header
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

### 2026-05-29 - Align NX549J 4.9 reserved-memory map to stock Nubia 3.18

Category: DIAGNOSTIC

Hypothesis: the remaining bootlogo hang may be caused by an incomplete
hardware-truth DT port rather than by the recovery reset path alone. The
current 4.9 DTB still carried donor MSM8953 fixed carveouts for modem, ADSP
firmware, WCNSS firmware, secure memory, and disabled DFPS data memory. Stock
Nubia 3.18 instead uses a smaller modem carveout plus a shared reloc carveout
for LPASS/Pronto firmware. The donor map can reserve or overlap memory that
Nubia firmware expects to own before userspace ever starts.

Evidence:

- Stock source:
  `/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/reference/nubia-stock-kernel/kernel/arch/arm/boot/dts/qcom/NX549/msm8953.dtsi`.
- FACT: stock `reserved-memory` defines:
  - `other_ext_mem` `0x84a00000/0x1e00000`;
  - `modem_mem` `0x86c00000/0x5600000` with `no-map-fixup`;
  - `reloc_mem` `0x8c200000/0x1800000`;
  - `secure_mem` size `0x09800000`;
  - `adsp_mem` size `0x400000`;
  - `dfps_data_mem` without donor `status = "disabled"`.
- FACT: stock `qcom,lpass@c200000` and `qcom,pronto@a21b000` both use
  `memory-region = <&reloc_mem>`.
- FACT: attempt86 compiled DTB still had donor `modem_mem` size `0x6a00000`,
  donor fixed `adsp_fw_mem` and `wcnss_fw_mem`, donor `secure_mem` size
  `0x0b400000`, donor `adsp_mem` size `0x800000`, and disabled DFPS data.
- FACT: attempt87 compiled DTB verification is in
  `/srv/forge/work/nx549j-preserve/release-attempt87-20260529-stock-reserved-memory-map/verify-stock-memory-map.txt`.
- FACT: attempt87 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt87-20260529-stock-reserved-memory-map/boot-stock-reserved-memory-map-120s.img`.
- FACT: attempt87 boot SHA-256:
  `512dc2f3777ec0dc042f9648c327a46fc82a0bc2ef5d8536ce0a8ff540fcaa79`.
- FACT: attempt87 was built and packaged offline only; it has not been
  flashed or runtime-proven.

Files changed:

- `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`
  - restores the stock Nubia modem/reloc/secure/adsp/dfps reserved-memory
    layout and redirects LPASS/Pronto to `reloc_mem`.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt87.sh`
  - adds a SHA-gated runner for the attempt87 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt87.
- `/srv/forge/work/nx549j-preserve/release-attempt87-20260529-stock-reserved-memory-map/README.md`
  - records artifact status, claim boundary, verification, and next device
    step.

Why each file changed:

- The NX549J overlay is the narrowest place to apply stock hardware-truth
  differences without mutating donor/reference sources.
- `adsp_fw_mem` and `wcnss_fw_mem` are marked `status = "disabled"` because
  keeping them enabled would reserve donor fixed ranges that overlap the stock
  `reloc_mem` range.
- The run scripts must follow the latest packaged artifact and refuse SHA
  mismatches before any future flash.

Expected next marker:

- If the memory-map mismatch is blocking early firmware/PIL/TZ setup, the next
  flash should advance beyond the previous bootlogo hang or produce fresher
  pstore/FRGmark evidence before the 120-second recovery fallback.
- If the hang is unrelated, the next runtime result should still be bootlogo
  hang or manual recovery with no userspace; then this diagnostic stays a
  rejected cause candidate unless another capture shows improved markers.

Rollback condition:

- Revert this diagnostic if attempt87 regresses earlier than attempt86, if a
  fresh capture proves the donor 4.9 ADSP/WCNSS fixed firmware regions are
  required on NX549J, or if the compiled DTB no longer matches the stock Nubia
  reserved-memory values documented above.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt87-20260529-stock-reserved-memory-map boot-stock-reserved-memory-map-120s.img
rg -n 'modem_region|reloc_region|secure_region|adsp_region|memory-region = <0x159>' /srv/forge/work/nx549j-preserve/release-attempt87-20260529-stock-reserved-memory-map/verify-stock-memory-map.txt
```

### 2026-05-29 - Queue BCB checkpoints off the initcall path

Category: DIAGNOSTIC

Hypothesis: the recovery fallback instrumentation should not perform blocking
storage I/O on the main initcall path while the current priority is to get the
kernel to userspace. The current code called `frgmark_write_recovery_bcb()`
from `frgmark_maybe_checkpoint_bcb()` after every initcall level until BCB
success. That helper opens `misc` with `blkdev_get_by_dev()`, writes with
`submit_bio_wait()`, and flushes with `blkdev_issue_flush()`. If eMMC or the
partition scan is slow or wedged, this diagnostic path can become a boot
blocker by itself.

Evidence:

- FACT: `init/main.c` runs `frgmark(FRGMARK_STAGE_INITCALL_LEVEL_BASE + level)`
  synchronously after each initcall level in `do_initcalls()`.
- FACT: before attempt88, `frgmark_maybe_checkpoint_bcb()` synchronously called
  `frgmark_write_recovery_bcb(frgmark_stage_name(stage))` on those stage
  markers.
- FACT: `frgmark_write_recovery_bcb()` reaches `blkdev_get_by_dev()`,
  `submit_bio_wait()`, and `blkdev_issue_flush()`.
- FACT: workqueue execution is available before `do_basic_setup()` because
  `kernel_init_freeable()` calls `workqueue_init()` before `do_basic_setup()`.
- FACT: attempt88 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt88-20260529-async-bcb-checkpoint/boot-async-bcb-checkpoint-120s.img`.
- FACT: attempt88 boot SHA-256:
  `72adcdcf6d301c02dc0d462ec4d6e78e4ef07e7dbffc476cc2a611b7396b1adb`.
- FACT: attempt88 was built and packaged offline only; it has not been
  flashed or runtime-proven.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - changes initcall checkpoint BCB handling from direct synchronous write to
    `mod_delayed_work(system_wq, &frg_recovery_bcb_work, 0)`;
  - changes late-init BCB prewrite from direct synchronous write to the same
    queued worker path;
  - adds a one-shot emergency log when an initcall checkpoint queues the BCB
    writer.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt88.sh`
  - adds a SHA-gated runner for the attempt88 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt88.
- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - now snapshots and clears recovery-visible pstore before reboot so any
    after-recovery pstore files are fresh for the flashed target boot.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - reports the preboot pstore clear status in `SUMMARY.md`.
- `/srv/forge/work/nx549j-preserve/release-attempt88-20260529-async-bcb-checkpoint/README.md`
  - records artifact status, claim boundary, verification, and next device
    step.

Why each file changed:

- Keeping BCB writes in a worker preserves the automatic recovery fallback when
  storage is healthy, but stops the diagnostic checkpoint from blocking the
  main boot path before userspace.
- The timeout timer path and delayed BCB retry path are left intact, so this is
  not a broad disable of recovery automation.

Expected next marker:

- If synchronous checkpoint I/O was stalling the boot path, attempt88 should
  advance farther than attempt87 or reach userspace.
- If storage is healthy, recovery logs should include
  `FRGmark: BCB checkpoint write queued` followed later by
  `FRGmark: BCB recovery command written`.
- If the device still hangs before workqueue/BCB write, the 120-second timeout
  may still fall back to no-BCB reset; then the next blocker remains earlier
  than available block I/O.

Rollback condition:

- Revert this diagnostic if attempt88 loses automatic recovery compared with
  attempt87 while attempt87 is proven not to block on checkpoint I/O, or if a
  fresh capture proves the synchronous initcall checkpoint is the only path
  that writes BCB before the target failure.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt88-20260529-async-bcb-checkpoint boot-async-bcb-checkpoint-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt88-20260529-async-bcb-checkpoint/SHA256SUMS
cat /srv/forge/work/nx549j-preserve/release-attempt88-20260529-async-bcb-checkpoint/verify-async-bcb-checkpoint.txt
bash -n /srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh
bash -n /srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh
```

2026-05-29 attempt89 BCB stage stamp:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged offline only; it has not been flashed or
  runtime-proven because the target is not attached.

Hypothesis: when pstore/ramoops remains empty or unreadable from recovery, a
successful kernel-side BCB write can still carry minimal target-kernel evidence
back to recovery. NX549J already proved the AOSP BCB selector at `misc` offset
0. The first BCB page has reserved space outside `command`, `status`, and the
main `recovery` message, so a text stamp there can report the last FRGmark
stage without changing the bootloader's `boot-recovery` selector.

Evidence:

- FACT: NX549J previously proved `boot-recovery` at `misc` page 0 selects
  recovery automatically.
- FACT: attempt88 still depended on pstore or printk evidence to know which
  target-kernel stage wrote BCB.
- FACT: attempt89 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt89-20260529-bcb-stage-stamp/boot-bcb-stage-stamp-120s.img`.
- FACT: attempt89 boot SHA-256:
  `887bca3481913f1a5006bd8fe22543bcbf085ebe1499a97909333876fa36e96b`.
- FACT: `verify-bcb-stage-stamp.txt` confirms `FRGMARK-BCB-v1`,
  `last_stage=0x%02x`, and `nx549j-frgmark-timeout` are present in `vmlinux`.
- FACT: `VERIFY.md` reports SHA256SUMS, boot cmdline, required symbols,
  marker strings, pstore config, and ramoops DTB checks as PASS.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - tracks the latest FRGmark stage in `frg_last_stage`;
  - writes a `FRGMARK-BCB-v1` text stamp at BCB offset `1024` whenever the
    kernel writes the recovery BCB command.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt89.sh`
  - adds a SHA-gated runner for the attempt89 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt89.
- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - runs marker grep after dumping `misc`, so the BCB stamp is included;
  - pulls the first `misc` page as
    `after-recovery/misc-first-page-before-restore.img` and extracts
    printable strings to `after-recovery/misc-first-page-strings.txt`.
- `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh`
  - also pulls/decodes the first `misc` page and greps after dumping `misc`
    for manual-recovery finishes.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - treats `FRGMARK-BCB`/`last_stage` as marker evidence and reports a
    dedicated `BCB stage stamp` verdict.
- `/srv/forge/work/nx549j-preserve/release-attempt89-20260529-bcb-stage-stamp/README.md`
  - records artifact status, claim boundary, verification, and next device
    step.

Why each file changed:

- The kernel BCB stamp is a recovery-readable fallback for the case where
  block storage is alive enough to write `misc`, but pstore is still empty or
  recovery cannot parse it.
- The host script changes make the existing recovery capture consume the new
  BCB evidence before restoring the saved `misc` backup.

Expected next marker:

- If the target reaches a kernel-side BCB write and later returns to recovery,
  `after-recovery/marker-grep.txt` or a manual-finish `marker-grep.txt` should
  contain `FRGMARK-BCB-v1` and `last_stage=0x..`.
  `misc-first-page-strings.txt` should decode the same stamp from the raw
  first `misc` page before restore.
- If automatic recovery still times out and manual recovery shows no BCB stamp,
  the hang remains before successful block-device BCB writes or in a reset path
  that clears/wipes the page before recovery can read it.

Rollback condition:

- Revert this diagnostic if recovery refuses the BCB page because reserved
  bytes are non-zero, if `boot-recovery` no longer selects recovery, or if a
  fresh capture proves the stamp overwrites any bootloader-owned field.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt89-20260529-bcb-stage-stamp boot-bcb-stage-stamp-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt89-20260529-bcb-stage-stamp/SHA256SUMS
cat /srv/forge/work/nx549j-preserve/release-attempt89-20260529-bcb-stage-stamp/verify-bcb-stage-stamp.txt
bash -n /srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh /srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh /srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh /srv/forge/android/nx549j/scripts/nx549j-run-attempt89.sh
```

2026-05-29 attempt90 early-stage BCB stamp:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged offline only; it has not been flashed or
  runtime-proven because the target is not attached.

Hypothesis: attempt89's BCB stamp is the right recovery-readable fallback, but
`frgmark_early()` wrote head/setup markers without updating `frg_last_stage`.
If a BCB write ever happens before the normal `frgmark()` path has refreshed
the variable, the BCB stamp could report stale stage `0x00` despite early
markers having run. Updating `frg_last_stage` in `frgmark_early()` keeps the
stamp coherent with the earliest marker path.

Evidence:

- FACT: `frgmark_early()` writes IMEM and ramoops records for early head/setup
  stages.
- FACT: before attempt90, only `frgmark()` and timeout marker helpers updated
  `frg_last_stage`.
- FACT: attempt90 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt90-20260529-early-stage-bcb-stamp/boot-early-stage-bcb-stamp-120s.img`.
- FACT: attempt90 boot SHA-256:
  `8925e0e84acee9a1b1397ad7307902bfd09daaff991c0aa2137ec4c7881df2f5`.
- FACT: `VERIFY.md` reports SHA256SUMS, boot cmdline, required symbols,
  marker strings, pstore config, and ramoops DTB checks as PASS.
- FACT: `verify-bcb-stage-stamp.txt` confirms `FRGMARK-BCB-v1`,
  `last_stage=0x%02x`, and `nx549j-frgmark-timeout` remain present in
  `vmlinux`.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - updates `frg_last_stage` at the start of `frgmark_early()`.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt90.sh`
  - adds a SHA-gated runner for the attempt90 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt90.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - now extracts BCB stamp fields from `misc-first-page-strings.txt` into
    `SUMMARY.md`: artifact, reason, `last_stage`, `last_name`, and `jiffies`.
- `/srv/forge/work/nx549j-preserve/release-attempt90-20260529-early-stage-bcb-stamp/README.md`
  - records artifact status, claim boundary, verification, and next device
    step.

Expected next marker:

- If BCB is written after any early `frgmark_early()` stage but before a later
  normal `frgmark()` stage, `misc-first-page-strings.txt` should report that
  early stage instead of `last_stage=0x00`.
  `SUMMARY.md` should mirror that value under `BCB Stage Stamp`.

Rollback condition:

- Revert this diagnostic if it changes runtime behavior before any BCB write
  evidence is produced; it should only update diagnostic state.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt90-20260529-early-stage-bcb-stamp boot-early-stage-bcb-stamp-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt90-20260529-early-stage-bcb-stamp/SHA256SUMS
cat /srv/forge/work/nx549j-preserve/release-attempt90-20260529-early-stage-bcb-stamp/verify-bcb-stage-stamp.txt
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt90.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh
```

Host-side summary verification:

- FACT: synthetic runtime directory
  `/tmp/nx549j-summary-bcb-fields-*` with `misc-first-page-strings.txt`
  containing `last_stage=0x55` and `last_name=initcall_fs_done` produced
  `SUMMARY.md` with:
  - `Target marker evidence: PRESENT`
  - `BCB stage stamp: PRESENT`
  - `Last stage: 0x55`
  - `Last stage name: initcall_fs_done`

2026-05-29 attempt91 serial earlycon repair:

- Patch category: DIAGNOSTIC.
- Runtime status: built and packaged offline only; it has not been flashed or
  runtime-proven because the target is expected to be disconnected from the
  forwarded ADB endpoint.

Hypothesis: the existing command line requested
`earlycon=msm_hsl_uart,0x78af000`, but this 4.9 tree does not declare an
`msm_hsl_uart` early console. Enabling the source-supported MSM serial console
path and using `earlycon=msm_serial_dm,0x78af000,115200n8` should make the
earliest UARTDM printk path available if the board exposes that UART. This is
diagnostic only; it does not by itself prove or fix the bootlogo hang.

Evidence:

- FACT: `drivers/tty/serial/msm_serial.c` declares
  `OF_EARLYCON_DECLARE(msm_serial_dm, "qcom,msm-uartdm", ...)`.
- FACT: `drivers/tty/serial/Kconfig` makes `CONFIG_SERIAL_MSM_CONSOLE` select
  `CONFIG_SERIAL_EARLYCON`.
- FACT: current `msm8953.dtsi` defines `blsp1_uart0` at `0x78af000` with
  compatible `"qcom,msm-uartdm-v1.4", "qcom,msm-uartdm"`.
- FACT: attempt91 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt91-20260529-serial-earlycon/boot-serial-earlycon-120s.img`.
- FACT: attempt91 boot SHA-256:
  `a60ab2a60c6a2bb48008bddd1a07243f3bd9ea7e4be9fcb16965c365af33b9f9`.
- FACT: `VERIFY.md` reports SHA256SUMS, boot cmdline, required symbols,
  marker strings, pstore config, serial early console config, and ramoops DTB
  checks as PASS.
- INFERENCE: previous `earlycon=msm_hsl_uart,0x78af000` was likely inert in
  this 4.9 tree because no matching earlycon id is declared in the source.

Files changed:

- `arch/arm64/configs/lineageos_nx549j_defconfig`
  - enables `CONFIG_SERIAL_MSM=y` and `CONFIG_SERIAL_MSM_CONSOLE=y`.
- `/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/device/nubia/nx549j/BoardConfig.mk`
  - replaces `earlycon=msm_hsl_uart,0x78af000` with
    `earlycon=msm_serial_dm,0x78af000,115200n8`.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - verifies the new earlycon cmdline token and serial early console config.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt91.sh`
  - adds a SHA-gated runner for the attempt91 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt91.
- `/srv/forge/work/nx549j-preserve/release-attempt91-20260529-serial-earlycon/README.md`
  - records artifact status, claim boundary, verification, and next device
    step.

Expected next marker:

- If UART is wired and readable, early printk should use the source-supported
  `msm_serial_dm` earlycon path instead of silently failing to match the old
  `msm_hsl_uart` token.
- Runtime verdict still depends on the normal capture stack:
  exact flashed SHA, boot partition prefix SHA, automatic/manual recovery
  classification, pstore/misc evidence, and fresh target markers.

Rollback condition:

- Revert this diagnostic if the serial driver introduction causes a new build
  or runtime regression unrelated to capture; the change is not required for
  storage, BCB, or pstore marker writes.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt91-20260529-serial-earlycon boot-serial-earlycon-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt91-20260529-serial-earlycon/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt91.sh
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh
bash -n /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
```

2026-05-29 attempt92 late panic pstore proof:

- Patch category: DIAGNOSTIC.
- Runtime status: repacked and verified offline only; it has not been flashed
  because the forwarded ADB endpoint has no attached target.
- This is not the preferred boot-to-userspace image. `nx549j-run-latest.sh`
  remains pointed at attempt91. attempt92 is an isolated pstore/recovery proof
  image.

Hypothesis: if the target reaches late initcall stage `0x57`, the async BCB
checkpoint should have had a chance to write the recovery selector. A controlled
`frgmark.force_panic_stage=0x57` panic can then prove whether target-kernel
pstore and automatic recovery capture work, without using host-prewritten BCB.

Evidence:

- FACT: attempt92 reuses the exact attempt91 kernel, ramdisk, `System.map`,
  `vmlinux`, `.config`, and DTB.
- FACT: `drivers/of/platform.c` explicitly creates a platform device for
  `compatible = "ramoops"` inside `/reserved-memory`.
- FACT: `fs/pstore/ram.c` parses the DT properties used by the NX549J DTB:
  `reg`, `record-size`, `console-size`, and `pmsg-size`.
- FACT: attempt92 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt92-20260529-late-panic-pstore-proof/boot-late-panic-pstore-proof.img`.
- FACT: attempt92 boot SHA-256:
  `4ee2bb41b75304e9fbbb9bc81dc93bf501fbff2722dc04107cc055a8dc410c81`.
- FACT: unpacked cmdline includes:
  `frgmark.force_panic_stage=0x57 panic=5 oops=panic panic_print=0x3f ignore_loglevel loglevel=8 printk.time=1`.
- FACT: `VERIFY.md` reports SHA256SUMS, boot cmdline, required symbols,
  marker strings, pstore config, serial early console config, and ramoops DTB
  checks as PASS.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt92-pstore-panic.sh`
  - adds a SHA-gated runner for the attempt92 panic-test boot image.
- `/srv/forge/work/nx549j-preserve/release-attempt92-20260529-late-panic-pstore-proof/README.md`
  - records artifact status, claim boundary, static pstore evidence, runtime
    pass criteria, and the exact runner command.

Runtime pass criteria:

- flashed boot SHA equals the attempt92 SHA above;
- boot partition prefix SHA equals the attempt92 SHA above;
- recovery was reached without button input;
- `misc-first-page-strings.txt` includes `FRGMARK-BCB-v1`;
- pstore contains fresh target-kernel text including
  `FRGmark forced panic stage=57` or `nx549j-frgmark-force-panic`.

Runtime command:

```sh
/srv/forge/android/nx549j/scripts/nx549j-run-attempt92-pstore-panic.sh
```

Rollback condition:

- Do not make attempt92 the default boot image. Remove or ignore this repack if
  it causes a loop or if it fails to add any pstore/BCB evidence beyond
  attempt91.

Verification commands:

```sh
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt92-20260529-late-panic-pstore-proof boot-late-panic-pstore-proof.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt92-20260529-late-panic-pstore-proof/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt92-pstore-panic.sh
```

2026-05-29 disconnected-device offline checkpoint:

- Patch category: documentation / evidence checkpoint only.
- Runtime status: no target is attached to the forwarded ADB endpoint; no new
  runtime claim can be made from this checkpoint.
- FACT: `adb -H 127.0.0.1 -P 15037 devices -l` returned only the header
  `List of devices attached`.
- FACT: no active `mka`, `ninja`, kernel `make`, `clang`, `gcc`, `ld.lld`,
  Soong, or Kati process was found by the current process check.
- FACT: attempt91 remains the latest normal boot-to-userspace candidate:
  `/srv/forge/work/nx549j-preserve/release-attempt91-20260529-serial-earlycon/boot-serial-earlycon-120s.img`.
- FACT: attempt91 SHA-256 remains:
  `a60ab2a60c6a2bb48008bddd1a07243f3bd9ea7e4be9fcb16965c365af33b9f9`.
- FACT: `sha256sum -c SHA256SUMS` and
  `scripts/nx549j-verify-release-artifact.sh` passed for attempt91.
- FACT: attempt92 remains a separate non-default pstore/recovery proof image:
  `/srv/forge/work/nx549j-preserve/release-attempt92-20260529-late-panic-pstore-proof/boot-late-panic-pstore-proof.img`.
- FACT: attempt92 SHA-256 remains:
  `4ee2bb41b75304e9fbbb9bc81dc93bf501fbff2722dc04107cc055a8dc410c81`.
- FACT: `sha256sum -c SHA256SUMS` and
  `scripts/nx549j-verify-release-artifact.sh` passed for attempt92.
- FACT: current DT audit rejected one suspected stocktruth omission:
  compiled attempt91 DTB already has `qcom,peripheral-size = <0x500000>` for
  `mem_client_3_size`.
- INFERENCE: with the device disconnected, the correct status is "offline
  artifacts are ready; flash/runtime proof is still required", not "boot,
  userspace, pstore, or automatic recovery is fixed".
- Next runtime action: flash attempt91 with
  `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh` when serial
  `30785d1a` is back in recovery. Run attempt92 only as the deliberate
  pstore/recovery proof test, not as the default boot image.

2026-05-29 attempt93 no-BCB no-loop timeout:

- Patch category: DIAGNOSTIC / recovery-safety.
- Runtime status: built and packaged offline only; it has not been flashed or
  runtime-proven because the forwarded ADB endpoint has no attached target.

Hypothesis:

The existing timeout path can still create a reboot loop when no recovery BCB
has been written: after the no-BCB grace window it primes only weak recovery
selectors and forces PS_HOLD/watchdog. Runtime evidence already showed raw
PS_HOLD/IMEM recovery selectors do not reliably select recovery on NX549J, so
resetting without BCB makes the requested "reboot to recovery, not loop"
less true. The safer diagnostic behavior is to reboot only after BCB is known
written; otherwise keep retrying BCB and emitting markers without raw reset.

Evidence:

- FACT: NX549J has a proven recovery selector only for AOSP BCB at `misc`
  offset 0, from `test-aosp-offset0-20260529-131128`.
- FACT: attempt68 proved early IMEM recovery magic plus PS_HOLD reset did not
  make the phone enter recovery automatically; manual recovery was required.
- FACT: current source before attempt93 had no-BCB paths that could call
  `kernel_restart("recovery")`, `frgmark_direct_pshold_reset()`, and
  `msm_trigger_wdog_bite()` after the grace window without BCB.
- FACT: recovery-side pstore was separately proven usable with the patched
  recovery DTB props image:
  `/srv/forge/work/nx549j-preserve/recovery-ramoops-cmdline-20260529/bootcheck-dtbprops-20260529-0828/dmesg.txt`
  contains `pstore: Registered ramoops as persistent store backend`.
- FACT: attempt93 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt93-20260529-no-bcb-no-loop/boot-no-bcb-no-loop-120s.img`.
- FACT: attempt93 boot SHA-256:
  `840fe9a2540f6798ff1dcc5a2a29e1195fecf18879b1d544cab3ca9099761360`.
- FACT: `VERIFY.md` reports SHA256SUMS, boot cmdline, required symbols,
  required marker strings, no-BCB no-loop gate, pstore config, serial early
  console config, and ramoops DTB checks as PASS.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - removes the no-BCB grace-window raw reset path.
  - timeout work and timer now keep retrying when BCB is absent.
  - reset to recovery remains allowed only after `frg_recovery_bcb_written`.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - verifies attempt93's no-BCB no-loop strings and rejects old reset-only
    no-BCB fallback strings.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt93.sh`
  - adds a SHA-gated runner for the attempt93 boot image.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - points the generic runner at attempt93.
- `/srv/forge/work/nx549j-preserve/release-attempt93-20260529-no-bcb-no-loop/README.md`
  - records artifact status, claim boundary, and runtime command.

Expected next marker:

- If the target can write BCB before the timeout, recovery should be entered
  via BCB and runtime capture should include a `FRGMARK-BCB-v1` stamp.
- If the target cannot write BCB, the device should not enter the previous
  no-BCB raw reset loop; manual recovery may still be required, but that would
  be an honest "BCB never became writable" signal.

Rollback condition:

- Revert attempt93 if runtime proves BCB is written but the image no longer
  resets into recovery, or if the no-reset behavior prevents collecting
  evidence that a later, proven selector could have collected safely.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt93-20260529-no-bcb-no-loop boot-no-bcb-no-loop-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt93-20260529-no-bcb-no-loop/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt93.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
```

2026-05-29 recovery ramoops preflight for boot-test runner:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script syntax and artifact identity verified offline; full
  recovery partition preflight still requires the device to be attached in
  recovery.

Hypothesis:

Even if attempt93 returns to recovery correctly, diagnostics are still weak if
the live recovery partition is the old image that fails to bind ramoops. The
runner should guarantee a pstore-readable recovery before flashing the broken
4.9 boot image, because recovery-side pstore registration is part of the
requested automatic diagnostic collection path.

Evidence:

- FACT: old recovery partition hash from capture `f89d96f3` was
  `15156f7e8a11a6cdbfa01191bd3e5b1d34c9568461769b0c4b4b45f54b29804a`.
- FACT: old recovery dmesg reported
  `ramoops: The memory size and the record/console size must be non-zero`.
- FACT: pstore-fixed recovery image is:
  `/srv/forge/work/nx549j-preserve/recovery-ramoops-cmdline-20260529/recovery-ramoops-dtbprops-headerorig.img`.
- FACT: pstore-fixed recovery SHA-256 is:
  `9700236dfa0d2057cf29c4247fa27a0d0b85a9e64e5033d511d6ac8f0469961c`.
- FACT: pstore-fixed recovery runtime check at
  `/srv/forge/work/nx549j-preserve/recovery-ramoops-cmdline-20260529/bootcheck-dtbprops-20260529-0828/dmesg.txt`
  contains `pstore: Registered ramoops as persistent store backend`.
- FACT: `bash -n` passed for
  `/srv/forge/android/nx549j/scripts/nx549j-ensure-recovery-ramoops.sh`,
  `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`,
  `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`, and
  `/srv/forge/android/nx549j/scripts/nx549j-run-attempt93.sh`.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-ensure-recovery-ramoops.sh`
  - verifies the known pstore-fixed recovery image SHA, checks the live
    recovery partition prefix, backs up a mismatching recovery prefix, flashes
    the pstore-fixed image, and verifies the partition prefix SHA.
- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - runs the recovery ramoops preflight before backing up `misc` and flashing
    the test boot image; it can be skipped only with
    `SKIP_RECOVERY_RAMOOPS_CHECK=1`.
- `/srv/forge/work/nx549j-preserve/release-attempt93-20260529-no-bcb-no-loop/README.md`
  - records that the latest runner includes recovery pstore preflight.

Expected next marker:

- A runtime directory from `nx549j-run-latest.sh` should include
  `recovery-ramoops-preflight/recovery-ramoops-identity.env`.
- If recovery was old, the preflight should preserve
  `recovery-ramoops-preflight/recovery-current-prefix.img` and verify the live
  recovery prefix SHA as
  `9700236dfa0d2057cf29c4247fa27a0d0b85a9e64e5033d511d6ac8f0469961c`.

Rollback condition:

- Disable with `SKIP_RECOVERY_RAMOOPS_CHECK=1` or revert the runner hook if
  flashing the pstore-fixed recovery image prevents entering recovery on this
  device. Keep the backup image from the runtime directory for manual restore.

Verification commands:

```sh
bash -n /srv/forge/android/nx549j/scripts/nx549j-ensure-recovery-ramoops.sh /srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-run-attempt93.sh
/srv/forge/android/nx549j/scripts/nx549j-ensure-recovery-ramoops.sh --help
sha256sum /srv/forge/work/nx549j-preserve/recovery-ramoops-cmdline-20260529/recovery-ramoops-dtbprops-headerorig.img
```

2026-05-29 attempt94 early BCB kick + no-reset kmsg dump:

- Patch category: DIAGNOSTIC target kernel + tooling.
- Runtime status: built and verified offline; not flashed because the
  device/tunnel was intentionally disconnected before runtime testing.

Hypothesis:

attempt93 correctly removed the unsafe no-BCB reset loop, but it could still
miss the best BCB write window if the normal retry work does not execute until
too late. Kicking the BCB retry immediately after `workqueue_init()` gives the
kernel an earlier blocking-capable write attempt. If the target still cannot
write BCB by the 120-second timeout, a one-shot `kmsg_dump(KMSG_DUMP_PANIC)`
should push target-kernel logs into the pstore/ramoops path without forcing a
raw reset loop.

Evidence:

- FACT: `mka bootimage -j4` completed successfully in 01:41.
- FACT: attempt94 is packaged at
  `/srv/forge/work/nx549j-preserve/release-attempt94-20260529-early-bcb-kick-kmsgdump/`.
- FACT: attempt94 boot image is
  `/srv/forge/work/nx549j-preserve/release-attempt94-20260529-early-bcb-kick-kmsgdump/boot-early-bcb-kick-kmsgdump-120s.img`.
- FACT: attempt94 boot SHA-256 is
  `ff94a5b81a6d4f7a17c785f109603eb4ff3c95e053b8f330f8f5c3cbc7da4489`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, no-BCB no-loop gate, pstore config, serial
  early console config, and ramoops DTB.
- FACT: `verify-symbols.txt` contains `frgmark_recovery_bcb_kick`.
- FACT: `verify-vmlinux-strings.txt` contains
  `FRGmark: BCB retry kicked` and
  `FRGmark: dumping kmsg without reset`.

Files changed:

- `arch/arm64/kernel/frgmark.c`
  - adds `frgmark_recovery_bcb_kick()` for an immediate BCB retry after the
    workqueue worker pool is ready;
  - adds a one-shot kmsg dump on the no-BCB timeout hold paths;
  - keeps the no-BCB no-loop guard from attempt93.
- `include/linux/frgmark.h`
  - declares `frgmark_recovery_bcb_kick()`.
- `init/main.c`
  - calls `frgmark_recovery_bcb_kick("kernel-init-workqueue-ready")` right
    after `workqueue_init()`.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - checks the new BCB kick symbol and marker strings.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt94.sh`
  - verifies the attempt94 boot image SHA and invokes the BCB/recovery runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - now delegates to attempt94.
- `/srv/forge/work/nx549j-preserve/release-attempt94-20260529-early-bcb-kick-kmsgdump/README.md`
  - records offline status, SHA, and flash path.

Expected next marker:

- On the next device run, `nx549j-run-latest.sh` should flash attempt94 after
  the recovery ramoops preflight.
- If BCB becomes writable, recovery should be selected by BCB and runtime
  capture should include `FRGMARK-BCB-v1`.
- If BCB still never becomes writable, the device should avoid the old 2-3s
  no-BCB raw reset loop and a manual recovery capture may contain a pstore kmsg
  dump from the target kernel.

Rollback condition:

- Revert attempt94 if runtime shows the early BCB kick blocks boot progress
  before later initcall markers, or if the kmsg dump path itself destabilizes
  the timeout hold path. In that case return to attempt93, which has the same
  no-BCB no-loop safety without the earlier kick/dump addition.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
export CCACHE_DIR=/srv/forge/android/ccache
source build/envsetup.sh
lunch lineage_nx549j-userdebug
mka bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt94-20260529-early-bcb-kick-kmsgdump boot-early-bcb-kick-kmsgdump-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt94-20260529-early-bcb-kick-kmsgdump/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt94.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
```

2026-05-29 continuation audit after attempt94 packaging:

- Patch category: DIAGNOSTIC evidence audit.
- Runtime status: no device available; no flash attempted.

Evidence:

- FACT: `adb devices -l`, `adb -H 127.0.0.1 -P 15037 devices -l`, and
  `adb -H 127.0.0.1 -P 5037 devices -l` all returned empty device lists.
- FACT: `ss -ltnp` still showed listeners on `127.0.0.1:15037` and
  `127.0.0.1:5037`, so the immediate blocker is the absent device, not a
  missing local ADB listener.
- FACT: `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh` currently
  exits with `ERROR: target 30785d1a is not online through ADB, state=''`
  when no target is attached.
- FACT: old NX549J recovery evidence still supports the target-kernel BCB
  device tuple used by attempt94:
  `/srv/forge/work/nx549j-preserve/misc-bcb/test-aosp-offset0-20260529-131128/by-name.txt`
  contains `misc -> /dev/block/mmcblk0p28`, and
  `/srv/forge/work/nx549j-preserve/misc-bcb/test-aosp-offset0-20260529-131128/after-recovery/proc-partitions.txt`
  contains `179 28 mmcblk0p28`.
- FACT: the same old BCB selector run proved `aosp-offset0` recovery by writing
  `boot-recovery` at misc page 0, normal-rebooting, observing recovery without
  button input, and restoring `misc`.

Interpretation:

- attempt94 is still the correct next flash candidate, because its BCB devt
  points at the proven `misc` partition and its runner refuses to act on the
  wrong or absent ADB target.
- Goal completion remains unproven: there is no current exact flashed-image
  identity, no current automatic recovery proof, and no fresh target-kernel
  pstore/BCB evidence from attempt94.

2026-05-29 flash runner userspace-hit audit:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script behavior checked offline; no device available.

Problem fixed:

The previous flash runner waited only for ADB `recovery`. If a future 4.9
candidate actually reaches Android/userspace and returns as normal ADB
`device`, the runner could misclassify that success as an automatic-recovery
timeout. That would directly hide the primary goal signal.

Files changed outside the kernel git repository:

- `/srv/forge/android/nx549j/scripts/nx549j-bcb-lib.sh`
  - adds `wait_for_target_recovery_or_device()`, recording
    `wait-boot-result.env` with `state=recovery` or `state=device`.
- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - uses the new wait helper;
  - keeps the existing automatic recovery capture path;
  - adds `after-userspace/` capture for normal Android `device`, including
    `cmdline.txt`, `dmesg.txt`, bounded `logcat.txt`, pstore files, misc first
    page before restore, marker grep, and misc restore.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - reports `Recovery result: USERSPACE` when the run result is
    `userspace-observed-within-wait-window`;
  - includes userspace marker/pstore/misc evidence in the summary.

Evidence:

- FACT: `bash -n` passed for `nx549j-bcb-lib.sh`,
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-latest.sh`, and
  `nx549j-run-attempt94.sh`.
- FACT: a synthetic summary fixture with
  `Result: userspace-observed-within-wait-window` produced `Recovery result:
  USERSPACE`, `Boot identity: PASS`, and `Target marker evidence: PRESENT`.
- FACT: with no device attached, `nx549j-run-latest.sh` still refuses to act on
  an absent target: `ERROR: target 30785d1a is not online through ADB,
  state=''`.

Expected next marker:

- If attempt94 reaches Android, the runtime directory should contain
  `after-userspace/`, `wait-boot-result.env` with `state=device`, and
  `SUMMARY.md` with `Recovery result: USERSPACE`.
- If attempt94 returns to recovery, the old `after-recovery/` path and
  automatic-recovery proof rules still apply.
- If neither state appears, the run remains a timeout and should be finished
  after manual recovery using `nx549j-finish-flash-timeout.sh`.

2026-05-29 userspace restore safety fix:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script behavior checked offline; no device available.

Problem fixed:

After Task 26, the runner could correctly detect ADB `device` as a userspace
hit, but the userspace branch still called `restore_misc` as a hard
requirement. Normal Android ADB may not have root/block write access, so a real
userspace success could be reported as a script failure only because misc
restore failed. That would hide the primary goal signal.

Files changed outside the kernel git repository:

- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - wraps userspace misc restore in a subshell so `restore_misc` failure is
    recorded without aborting the userspace-hit result;
  - writes `after-userspace/restore-optional-status.txt` when restore fails.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - reports `Misc restore: CHECK_USERSPACE_RESTORE_LOGS` when the userspace
    branch reached Android but optional misc restore failed.

Evidence:

- FACT: `bash -n` passed for `nx549j-bcb-lib.sh`,
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-latest.sh`, and
  `nx549j-run-attempt94.sh`.
- FACT: a synthetic userspace summary fixture with
  `Userspace misc restore: failed-see-after-userspace-restore-logs` produced
  `Recovery result: USERSPACE`, `Target marker evidence: PRESENT`, and
  `Misc restore: CHECK_USERSPACE_RESTORE_LOGS`.
- FACT: with no device attached, `nx549j-run-latest.sh` still refuses to flash:
  `ERROR: target 30785d1a is not online through ADB, state=''`.

Expected next marker:

- If attempt94 reaches Android but userspace ADB cannot write `misc`, the
  runtime `SUMMARY.md` should still report `USERSPACE` and point at
  `after-userspace/restore-optional-status.txt` instead of exiting before the
  result is written.

2026-05-29 recovery restore result-preservation fix:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script behavior checked offline; no device available.

Problem fixed:

The recovery branch still called `restore_misc` as a hard requirement before
writing the final result. If automatic recovery was observed but misc restore
failed, the script could exit before writing `README.md` and `SUMMARY.md`,
hiding the automatic-recovery evidence that the run was meant to prove.

Files changed outside the kernel git repository:

- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - sets `result=recovery-observed-within-wait-window` before restore;
  - records `Recovery misc restore: completed` or
    `failed-see-after-recovery-restore-logs`;
  - writes `after-recovery/restore-required-status.txt` on restore failure.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - reports `Misc restore: CHECK_RECOVERY_RESTORE_LOGS` for that case while
    preserving `Recovery result: CANDIDATE_AUTO`.

Evidence:

- FACT: `bash -n` passed for `nx549j-bcb-lib.sh`,
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-latest.sh`, and
  `nx549j-run-attempt94.sh`.
- FACT: a synthetic recovery summary fixture with
  `Recovery misc restore: failed-see-after-recovery-restore-logs` produced
  `Recovery result: CANDIDATE_AUTO`, `Target marker evidence: PRESENT`, and
  `Misc restore: CHECK_RECOVERY_RESTORE_LOGS`.
- FACT: with no device attached, `nx549j-run-latest.sh` still refuses to flash:
  `ERROR: target 30785d1a is not online through ADB, state=''`.

Expected next marker:

- If attempt94 returns to recovery but misc restore fails, the runtime
  `SUMMARY.md` should still preserve `CANDIDATE_AUTO` and point at
  `after-recovery/restore-required-status.txt` instead of losing the run result.

2026-05-29 recovery-only flash preflight:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script behavior checked offline; no device available.

Problem fixed:

The flash runner accepted either ADB `device` or `recovery` before writing
`boot`, checking/flashing recovery, and backing up/writing `misc`. That was too
loose for a block-write test: normal Android `device` is useful only after the
test boot as evidence that userspace was reached. The initial flashing context
should be recovery ADB, where the existing partition write/restore assumptions
were proven.

Files changed outside the kernel git repository:

- `/srv/forge/android/nx549j/scripts/nx549j-bcb-lib.sh`
  - adds `require_recovery_target()`, which keeps the existing NX549J identity
    checks but rejects initial state other than `recovery`.
- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - uses `require_recovery_target()` before any recovery/boot/misc block writes;
  - documents that Android `device` is accepted only as a post-reboot success
    result, not as the initial flashing state.

Evidence:

- FACT: `bash -n` passed for `nx549j-bcb-lib.sh`,
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-latest.sh`, and
  `nx549j-run-attempt94.sh`.
- FACT: with no device attached, `nx549j-run-latest.sh` still refuses to flash:
  `ERROR: target 30785d1a is not online through ADB, state=''`.
- FACT: attempt94 packaged files still pass `sha256sum -c SHA256SUMS`.

Expected next marker:

- The next attempt94 flash must start with `30785d1a` visible as ADB
  `recovery`. If it is already Android `device`, the runner should reject the
  flash instead of attempting block writes from the wrong context.

2026-05-29 flash runner crash-safe failure summary:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: script behavior checked offline; no device available.

Problem fixed:

The runner could still exit after creating a flash-run directory but before
writing the normal final `README.md` and `SUMMARY.md`. That matters for long
unattended builds or disconnected-device windows: a failed host-side phase
should leave an explicit "runner failed" report, not an ambiguous half-written
directory that looks like missing evidence.

Files changed outside the kernel git repository:

- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - adds an `EXIT` trap that writes a fallback `README.md` when the runner
    fails after the run directory exists but before the normal final summary;
  - tracks the current phase (`push-boot`, `flash-boot`, `wait-boot-result`,
    capture, restore, etc.) so the fallback report points at the failed stage;
  - fixes unescaped backticks in unquoted heredocs so README generation cannot
    accidentally execute inline text as shell command substitution.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - reports `Recovery result: RUNNER_FAILED` for
    `Result: runner-failed-before-final-summary`.

Evidence:

- FACT: `bash -n` passed for `nx549j-bcb-lib.sh`,
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-latest.sh`,
  `nx549j-run-attempt94.sh`, `nx549j-verify-release-artifact.sh`, and
  `nx549j-ensure-recovery-ramoops.sh`.
- FACT: `rg -n '\`[^\`]+\`|\\\`'` now shows only escaped README backticks in
  the flash runner and summary script.
- FACT: attempt94 packaged files pass `sha256sum -c SHA256SUMS`.

Expected next marker:

- If the runner fails mid-flow after creating the run directory, the directory
  should contain `README.md` with `Result: runner-failed-before-final-summary`,
  a `Failure phase:` line, and `SUMMARY.md` with `Recovery result:
  RUNNER_FAILED`.
- If the runner reaches a normal result, the normal final `README.md` remains
  authoritative and the fallback trap must not overwrite it.

2026-05-29 attempt95 userspace handoff stage marker:

- Patch category: DIAGNOSTIC.
- Runtime status: built, packaged, and locally verified; not flashed because
  the device/tunnel was disconnected before runtime testing.

Hypothesis:

- HYPOTHESIS: the current bootlogo hang may already reach the kernel-to-init
  exec boundary, but prior captures cannot prove that because the last
  persistent stage remained one of the `exec_*_init` markers and the
  "userspace reached" message was only a printk that can be lost when ADB is
  unavailable. A dedicated persistent stage at `frgmark_userspace_reached()`
  will distinguish "kernel successfully execed init" from "still died before
  userspace handoff" in the next recovery capture.

Evidence:

- FACT: attempt95 is packaged at
  `/srv/forge/work/nx549j-preserve/release-attempt95-20260529-userspace-stage-marker/`.
- FACT: attempt95 boot image is
  `/srv/forge/work/nx549j-preserve/release-attempt95-20260529-userspace-stage-marker/boot-userspace-stage-marker-120s.img`.
- FACT: attempt95 boot SHA-256 is
  `03735556830075ee4328c39799a4f6b26e21c779fcbf07feffaf2c5b4b66245e`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, no-BCB no-loop gate, pstore config, serial
  early console config, and ramoops DTB.
- FACT: `verify-vmlinux-strings.txt` contains both `userspace_reached` and
  `FRGmark: userspace reached`.
- FACT: the decoder shell script maps stage `0x15` to `userspace_reached`, and
  a stdin smoke test decoded `FRGmark stage=15` and `0x46524715` as
  `userspace_reached`.
- FACT: with no device attached on ADB port `15037`, `nx549j-run-latest.sh`
  refuses to flash with `ERROR: target 30785d1a is not online through ADB,
  state=''`.

Files changed:

- `include/linux/frgmark.h`
  - adds `FRGMARK_STAGE_USERSPACE_REACHED` as stage `0x15`.
- `arch/arm64/kernel/frgmark.c`
  - maps stage `0x15` to `userspace_reached`;
  - calls `frgmark(FRGMARK_STAGE_USERSPACE_REACHED)` from
    `frgmark_userspace_reached()` before the existing recovery timeout cleanup.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes stage `0x15` as `userspace_reached`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - shows stage `0x15` as `userspace_reached` in the debug page marker table.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the `userspace_reached` string in packaged vmlinux verification.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt95.sh`
  - verifies the attempt95 boot image SHA and invokes the BCB/recovery runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - now delegates to attempt95.
- `/srv/forge/work/nx549j-preserve/release-attempt95-20260529-userspace-stage-marker/README.md`
  - records offline status, SHA, expected evidence, and next flash action.

Expected next marker:

- If the target 4.9 kernel reaches Android init exec, the next recovery capture
  should decode `0x46524715` or `stage=0x15` as
  `name=userspace_reached`.
- If stage `0x15` is absent, the failure remains before userspace handoff and
  the previous highest persistent marker is the active boundary.

Rollback condition:

- Revert attempt95 if runtime shows that adding `frgmark()` inside
  `frgmark_userspace_reached()` prevents BCB cleanup, leaves the device stuck
  in recovery after a successful userspace handoff, or otherwise changes boot
  behavior before the userspace boundary. In that case return to attempt94 and
  keep the userspace-boundary evidence gap open.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/kernel/nubia/msm8953
env PATH=/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin:/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin:/home/n8n/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin CCACHE_DIR=/srv/forge/android/ccache ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- CROSS_COMPILE_ARM32=arm-linux-androideabi- HOST_EXTRACFLAGS=-I/usr/include/node make O=/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/out/target/product/nx549j/obj/KERNEL_OBJ Image.gz-dtb -j4 HOSTLOADLIBES_sign-file=/usr/lib/x86_64-linux-gnu/libcrypto.so.3 HOSTLOADLIBES_extract-cert=/usr/lib/x86_64-linux-gnu/libcrypto.so.3
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt95-20260529-userspace-stage-marker boot-userspace-stage-marker-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt95-20260529-userspace-stage-marker/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt95.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
printf 'FRGmark stage=15\n0x46524715\n' | /home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh -
```

2026-05-29 attempt96 recovery timeout reboot stage marker:

- Patch category: DIAGNOSTIC.
- Runtime status: built, packaged, and locally verified; not flashed because no
  device is attached on ADB port `15037`.

Hypothesis:

- HYPOTHESIS: the next capture needs to distinguish three outcomes without
  relying on transient printk only: userspace handoff (`0x15`), recovery
  timeout reaching the reboot/reset handoff after BCB is written (`0x16`), and
  a failure before either boundary. A dedicated timeout reboot marker will show
  whether automatic recovery failed before the reset handoff or after the
  kernel asked the platform to reboot to recovery.

Evidence:

- FACT: attempt96 is packaged at
  `/srv/forge/work/nx549j-preserve/release-attempt96-20260529-timeout-reboot-stage-marker/`.
- FACT: attempt96 boot image is
  `/srv/forge/work/nx549j-preserve/release-attempt96-20260529-timeout-reboot-stage-marker/boot-timeout-reboot-stage-marker-120s.img`.
- FACT: attempt96 boot SHA-256 is
  `b204f4b6a82ebe46a7ae742cdce82538d55aa09d2b8d3d15044f638e2956e4ad`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, no-BCB no-loop gate, pstore config, serial
  early console config, and ramoops DTB.
- FACT: `verify-vmlinux-strings.txt` contains `userspace_reached`,
  `recovery_timeout_reboot`, and the recovery timeout printk strings.
- FACT: the decoder shell script maps stage `0x16` to
  `recovery_timeout_reboot`, and a stdin smoke test decoded `FRGmark stage=16`
  and `0x46524716` as `recovery_timeout_reboot`.
- FACT: with no device attached on ADB port `15037`, `nx549j-run-latest.sh`
  refuses to flash with `ERROR: target 30785d1a is not online through ADB,
  state=''`.

Files changed:

- `include/linux/frgmark.h`
  - adds `FRGMARK_STAGE_RECOVERY_TIMEOUT_REBOOT` as stage `0x16`.
- `arch/arm64/kernel/frgmark.c`
  - maps stage `0x16` to `recovery_timeout_reboot`;
  - writes stage `0x16` immediately before the workqueue timeout path calls
    `kernel_restart("recovery")`;
  - writes stage `0x16` immediately before the timer fallback drops PS_HOLD or
    bites the watchdog after BCB has been written.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes stage `0x16` as `recovery_timeout_reboot`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - shows stage `0x16` as `recovery_timeout_reboot` in the debug page marker
    table.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the `recovery_timeout_reboot` string in packaged vmlinux
    verification.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt96.sh`
  - verifies the attempt96 boot image SHA and invokes the BCB/recovery runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - now delegates to attempt96.
- `/srv/forge/work/nx549j-preserve/release-attempt96-20260529-timeout-reboot-stage-marker/README.md`
  - records offline status, SHA, expected marker interpretation, and next flash
    action.

Expected next marker:

- `0x15 userspace_reached`: the kernel crossed into Android init/userspace and
  recovery timeout cleanup should run.
- `0x16 recovery_timeout_reboot`: userspace was not reached, BCB was written,
  and the timeout path attempted recovery reboot/reset.
- No `0x15` and no `0x16`: the failure remains before userspace handoff and
  before the recovery timeout reset handoff.

Rollback condition:

- Revert attempt96 if runtime shows the extra timeout marker changes timing or
  suppresses recovery reboot/reset behavior. Return to attempt95 in that case;
  attempt95 has the same userspace boundary marker without the timeout reboot
  marker.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/kernel/nubia/msm8953
env PATH=/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin:/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin:/home/n8n/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin CCACHE_DIR=/srv/forge/android/ccache ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- CROSS_COMPILE_ARM32=arm-linux-androideabi- HOST_EXTRACFLAGS=-I/usr/include/node make O=/srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot/out/target/product/nx549j/obj/KERNEL_OBJ Image.gz-dtb -j4 HOSTLOADLIBES_sign-file=/usr/lib/x86_64-linux-gnu/libcrypto.so.3 HOSTLOADLIBES_extract-cert=/usr/lib/x86_64-linux-gnu/libcrypto.so.3
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt96-20260529-timeout-reboot-stage-marker boot-timeout-reboot-stage-marker-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt96-20260529-timeout-reboot-stage-marker/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt96.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
printf 'FRGmark stage=16\n0x46524716\n' | /home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh -
```

2026-05-29 attempt97 boot-only userspace ACK timeout:

- Patch category: DIAGNOSTIC.
- Runtime status: built, packaged, and locally verified; not flashed because no
  device is attached on ADB port `15037`.

Hypothesis:

- HYPOTHESIS: attempt95/96 disarmed recovery timeout too early because
  `frgmark_userspace_reached()` ran as soon as the kernel successfully execed
  `/init`. If `/init` or first-stage mount then hangs before recovery-readable
  logs or ADB, the device can sit at bootlogo without automatic fallback. The
  timeout should remain armed until boot ramdisk userspace explicitly ACKs
  progress after `DoFirstStageMount()`.

Evidence:

- FACT: attempt97 is packaged at
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/`.
- FACT: attempt97 boot image is
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/boot-userspace-ack-timeout-120s.img`.
- FACT: attempt97 boot SHA-256 is
  `a0a40ab9539b4793ae772fa7a1f02793163fae653f9ef83805cbe51cf4666893`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk init userspace ACK, no-BCB no-loop
  gate, pstore config, serial early console config, and ramoops DTB.
- FACT: `verify-ramdisk-init-ack.txt` contains `/proc/frgmark_userspace_ack`
  and `first-stage-mounted` extracted from the boot image ramdisk `/init`.
- FACT: the decoder shell script and debug page marker table map stage `0x17`
  to `userspace_ack`, and a stdin smoke test decoded `FRGmark stage=17` and
  `0x46524717` as `userspace_ack`.

Files changed:

- `include/linux/frgmark.h`
  - adds `FRGMARK_STAGE_USERSPACE_ACK` as stage `0x17`.
- `arch/arm64/kernel/frgmark.c`
  - keeps recovery timeout armed after `frgmark_userspace_reached()`;
  - exposes `/proc/frgmark_userspace_ack`;
  - writes stage `0x17`, clears BCB/selectors, and cancels timeout work only
    after userspace writes the ACK.
- `system/core/init/first_stage_init.cpp`
  - writes `first-stage-mounted` to `/proc/frgmark_userspace_ack` after
    `DoFirstStageMount()` and before execing `/system/bin/init`.
- `system/core/init/init.cpp`
  - adds a later second-stage ACK action for full-ROM builds where
    `/system/bin/init` is updated too.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes stage `0x17` as `userspace_ack`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - shows stage `0x17` as `userspace_ack` in the debug page marker table.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the kernel ACK symbol/string and verifies the boot ramdisk `/init`
    contains the userspace ACK path/reason.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt97.sh`
  - verifies the attempt97 boot image SHA and invokes the BCB/recovery runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - now delegates to attempt97.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/README.md`
  - records offline status, SHA, expected marker interpretation, and next flash
    action.

Expected next marker:

- `0x15 userspace_reached`: the kernel crossed into `/init`; timeout remains
  armed and should still return to recovery if first-stage userspace hangs.
- `0x16 recovery_timeout_reboot`: no userspace ACK happened before 120 seconds;
  the kernel should select recovery and reset for evidence collection.
- `0x17 userspace_ack`: boot ramdisk first-stage init reached the post-mount
  handoff and disarmed the recovery timeout.

Rollback condition:

- Revert attempt97 if runtime shows that first-stage ACK fires too early for
  useful automation, prevents recovery fallback during a real bootlogo hang, or
  leaves recovery selectors uncleared after a successful boot. Return to
  attempt96 for timeout-reset marker coverage without userspace ACK gating.

Verification commands:

```sh
cd /srv/forge/android/nx549j/rom-nx549j-lineage-18.1-tissot
source build/envsetup.sh && lunch lineage_nx549j-userdebug
mka init_first_stage init bootimage -j4
/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout boot-userspace-ack-timeout-120s.img
sha256sum -c /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS
bash -n /srv/forge/android/nx549j/scripts/nx549j-run-attempt97.sh /srv/forge/android/nx549j/scripts/nx549j-run-latest.sh /srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh
printf 'FRGmark stage=17\n0x46524717\n' | /home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh -
```

2026-05-29 attempt97 runner decode update:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side capture tooling update only; not flashed because no
  device is attached on ADB port `15037`.

Evidence:

- FACT: `adb -H 127.0.0.1 -P 15037 -s 30785d1a get-state` returned
  `device '30785d1a' not found`.
- FACT: `bash -n` passed for
  `nx549j-flash-boot-with-bcb-fallback.sh`,
  `nx549j-finish-flash-timeout.sh`, `nx549j-collect-early-markers.sh`,
  `nx549j-summarize-flash-run.sh`, `nx549j-run-attempt97.sh`, and
  `nx549j-run-latest.sh`.
- FACT: decoder smoke decoded `0x15` as `userspace_reached`, `0x16` as
  `recovery_timeout_reboot`, and `0x17` as `userspace_ack`.
- FACT: `timeout 10s scripts/nx549j-run-latest.sh` refused to flash with
  `ERROR: target 30785d1a is not online through ADB, state=''`.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-flash-boot-with-bcb-fallback.sh`
  - automatic recovery and userspace capture paths now emit
    `marker-decode.txt` through the FRGmark decoder and grep the attempt97 ACK
    strings.
- `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh`
  - manual recovery finish now emits `marker-decode.txt` and greps attempt97
    ACK strings before restoring `misc`.
- `/srv/forge/android/nx549j/scripts/nx549j-collect-early-markers.sh`
  - standalone manual marker capture now emits `marker-decode.txt`.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - summary evidence now considers `marker-decode.txt` and recognizes
    attempt97 stage names/ACK strings.

Expected next marker:

- The next connected-device run should produce `marker-decode.txt` containing
  `0x15 userspace_reached`, `0x16 recovery_timeout_reboot`, or
  `0x17 userspace_ack` if any FRGmark evidence survives into recovery or
  userspace capture.

Rollback condition:

- Revert only if the decoder call makes capture scripts fail before `misc`
  restore. The decoder is best-effort and currently guarded with `|| true`.

2026-05-29 attempt97 reconnect helper:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side launch helper only; not flashed because no device
  is attached on ADB port `15037`.

Evidence:

- FACT: `ADB_SERVER_SOCKET=tcp:127.0.0.1:15037 adb devices -l` listed no
  devices.
- FACT: `sha256sum -c` still verifies all attempt97 release files under
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/`.
- FACT: kernel repo HEAD is `daa3655f3 docs: record nx549j attempt97 decode
  runner`; system/core HEAD is `dfec6ad diag: ack nx549j first-stage
  userspace`.
- FACT: `bash -n scripts/nx549j-wait-recovery-and-run-latest.sh
  scripts/nx549j-run-latest.sh scripts/nx549j-run-attempt97.sh` passed.
- FACT: `WAIT_ATTACH_SECONDS=3 POLL_SECONDS=1
  scripts/nx549j-wait-recovery-and-run-latest.sh` timed out after only
  `target-not-ready ... state='missing'` messages and did not flash.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-run-latest.sh`
  - waits for exact serial `30785d1a` to appear as ADB `recovery`, then execs
    `scripts/nx549j-run-latest.sh`.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/README.md`
  - documents the reconnect helper as the operator-facing launch path.

Expected next action:

- When the phone is reconnected in recovery, run
  `scripts/nx549j-wait-recovery-and-run-latest.sh` or direct
  `scripts/nx549j-run-latest.sh`. The first command is safer if the USB/ADB
  tunnel may appear late.

Rollback condition:

- Remove the helper if it ever launches on a non-recovery state or wrong
  serial. Current smoke proves it waits instead of flashing while the target is
  missing.

2026-05-29 attempt97 recovery-only manual capture helpers:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side helper hardening only; not flashed because no
  device is attached on ADB port `15037`.

Evidence:

- FACT: `bash -n scripts/nx549j-finish-flash-timeout.sh
  scripts/nx549j-collect-early-markers.sh
  scripts/nx549j-flash-boot-with-bcb-fallback.sh
  scripts/nx549j-wait-recovery-and-run-latest.sh` passed.
- FACT: `scripts/nx549j-collect-early-markers.sh /tmp/nx549j-collect-smoke`
  refused to run with `ERROR: target 30785d1a is not online through ADB,
  state=''`.
- FACT: `rg` confirms `nx549j-finish-flash-timeout.sh`,
  `nx549j-collect-early-markers.sh`, and
  `nx549j-flash-boot-with-bcb-fallback.sh` all call
  `require_recovery_target`.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-finish-flash-timeout.sh`
  - manual timeout finish now requires ADB `recovery` state before capture and
    `misc` restore.
- `/srv/forge/android/nx549j/scripts/nx549j-collect-early-markers.sh`
  - standalone early marker collection now requires ADB `recovery` state.

Expected next action:

- If attempt97 times out and the phone is manually returned to recovery, run
  `scripts/nx549j-finish-flash-timeout.sh <flash-run-dir>`. It should refuse
  to proceed unless the target serial is actually in recovery.

Rollback condition:

- Revert only if a known-good recovery exposes ADB state as `device` while
  still allowing recovery partition writes; current evidence and previous
  scripts treat `recovery` as the safe block-write state.

2026-05-29 attempt97 runner snapshot:

- Patch category: DIAGNOSTIC.
- Runtime status: release packaging/evidence snapshot only; not flashed because
  no device is attached on ADB port `15037`.

Evidence:

- FACT: `ADB_SERVER_SOCKET=tcp:127.0.0.1:15037 adb devices -l` listed no
  devices.
- FACT: `scripts/nx549j-run-latest.sh` delegates to
  `scripts/nx549j-run-attempt97.sh`, which verifies boot SHA
  `a0a40ab9539b4793ae772fa7a1f02793163fae653f9ef83805cbe51cf4666893`.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passes and now includes `RUNNER_SNAPSHOT_SHA256SUMS`.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passes for the copied runner scripts and snapshot README.
- FACT: `bash -n runner-snapshot-20260529/*.sh` passes.
- FACT: `scripts/nx549j-verify-release-artifact.sh
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout
  boot-userspace-ack-timeout-120s.img` still reports PASS.

Files changed:

- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the current host-side scripts expected for attempt97 flashing and
    recovery/userspace evidence collection.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  - records checksums for the copied runner scripts.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - now includes the runner snapshot manifest checksum.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/README.md`
  - documents the runner snapshot.

Expected next action:

- Use live scripts from `/srv/forge/android/nx549j/scripts/` for the actual
  attempt. Use the runner snapshot to audit exact intended behavior if the live
  scripts drift before the phone is reconnected.

Rollback condition:

- Remove the snapshot only if a newer attempt supersedes attempt97 or the live
  runner changes enough to require a new release-local snapshot.

2026-05-29 attempt97 decoded-stage summary verdict:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side summarizer/snapshot update only; not flashed
  because the phone/tunnel is unavailable.

Evidence:

- FACT: `scripts/nx549j-summarize-flash-run.sh` now reads the last
  `stage=0x.. name=..` line from `marker-decode.txt`.
- FACT: synthetic summary evidence classified `0x16
  recovery_timeout_reboot` as `RECOVERY_TIMEOUT_REBOOT`.
- FACT: synthetic summary evidence classified `0x17 userspace_ack` as
  `USERSPACE_ACK`.
- FACT: `bash -n scripts/nx549j-summarize-flash-run.sh` passed.
- FACT: `bash -n runner-snapshot-20260529/*.sh` passed for the release-local
  runner snapshot.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passed.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passed.
- FACT: latest release verifier writes
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/VERIFY.md`
  with PASS for checksums, cmdline, required symbols, required marker strings,
  ramdisk userspace ACK, no-BCB no-loop gate, pstore config, serial early
  console config, and ramoops DTB. The file timestamp changes on every
  verifier rerun.
- FACT: top-level release `SHA256SUMS` now records runner snapshot manifest
  SHA-256
  `ee612de504050795ef09c22b49dfee95e479bd593ebf8784b3ac6d25cb9343f7`.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - adds `Last decoded marker` and `Decoded FRGmark` summary fields.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/nx549j-summarize-flash-run.sh`
  - preserves the same summarizer behavior in the release snapshot.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - records the updated runner snapshot manifest hash.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/README.md`
  - documents decoded marker verdicts and the current local verification
    boundary.

Expected next action:

- When the phone is reconnected in recovery, run
  `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-run-latest.sh`
  or direct `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`.
  If the phone/tunnel remains disconnected, report the state as
  "artifact ready, runtime not tested" rather than claiming device success.

Rollback condition:

- Revert this summary-only update if real capture output uses a different
  decoder line format and the generated `SUMMARY.md` loses the raw evidence
  paths or fails before writing the summary. It does not affect boot image
  contents.

2026-05-29 attempt97 unattended wait log:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side wait/capture wrapper update only; not flashed
  because ADB port `15037` currently lists no devices.

Evidence:

- FACT: `WAIT_ATTACH_SECONDS=2 POLL_SECONDS=1
  WAIT_LOG=/tmp/nx549j-wait-log.N7t5fp/wait.log
  scripts/nx549j-wait-recovery-and-run-latest.sh` exited with status `1`
  after logging two `state='missing'` polls and `wait-timeout`; no flash
  runner handoff occurred.
- FACT: the same smoke wrote timestamped lines to the persistent `WAIT_LOG`
  file, so unattended waiting leaves evidence even if terminal output is lost.
- FACT: `bash -n scripts/nx549j-wait-recovery-and-run-latest.sh` passed.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passed after updating the release-local runner snapshot.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passed.
- FACT: `ADB_SERVER_SOCKET=tcp:127.0.0.1:15037 adb devices -l` listed no
  attached devices, so no runtime flash/capture was attempted.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-run-latest.sh`
  - writes wait-state lines to `$WAIT_LOG`, defaulting under attempt97
    `runtime/wait-logs/`, and tees the eventual runner output into the same
    file while preserving the runner exit code.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the updated wait helper and snapshot README.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - records updated runner snapshot manifest checksum
    `2bf4a95e34eb3b6058260b5d8307e859dd76f49727f21a01e48fa5c5efde93db`.

Expected next action:

- Leave `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-run-latest.sh`
  running when the phone may be reconnected later in recovery. If it never
  sees serial `30785d1a` in recovery, it should only accumulate wait evidence
  and must not flash.

Rollback condition:

- Revert if the persistent logging wrapper changes the flash runner exit code
  or starts flashing while the target is missing/non-recovery. The current
  no-device smoke proves the missing-target path does not flash.

2026-05-29 attempt97 unattended finish wait helper:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side post-timeout helper only; not flashed because ADB
  port `15037` currently lists no devices.

Evidence:

- FACT: `scripts/nx549j-wait-recovery-and-finish-latest.sh` was added to wait
  for exact serial `30785d1a` in recovery, then run
  `scripts/nx549j-finish-flash-timeout.sh` for a timed-out attempt97
  flash-run directory.
- FACT: if no `flash-run-dir` is given, the helper selects the newest
  attempt97 `runtime/flash-boot-bcb-*` directory with `misc-backup.img` whose
  README result is empty, `automatic-recovery-timeout`, or
  `runner-failed-before-final-summary`.
- FACT: a no-device smoke with synthetic timeout directory
  `/tmp/nx549j-finish-wait.HBKb2s/runtime/flash-boot-bcb-test` exited with
  status `1`, wrote persistent wait log
  `/tmp/nx549j-finish-wait.HBKb2s/wait-finish.log`, and did not call the
  finish helper because the target remained missing.
- FACT: a synthetic run whose README reported
  `userspace-observed-within-wait-window` was not selected for finish.
- FACT: `bash -n scripts/nx549j-wait-recovery-and-finish-latest.sh` passed.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passed after updating the release-local runner snapshot.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passed.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-finish-latest.sh`
  - new unattended helper for the manual-recovery finish path after timeout.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the new finish wait helper and documents it in the snapshot
    README.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - records updated runner snapshot manifest checksum
    `106c76e905db0ba63149aa3429488de7a86a3747b921a01ad8765cd3b8f04a9b`.

Expected next action:

- If a flash run times out and the phone is later returned to recovery, run
  `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-finish-latest.sh`
  without arguments to finish the newest timed-out attempt97 run, or pass the
  exact flash-run directory to avoid ambiguity.

Rollback condition:

- Revert if the helper selects a run that already reached userspace/automatic
  recovery, or if it starts the finish helper while the target is missing or in
  normal Android `device` state.

2026-05-29 attempt97 one-command unattended runner:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side orchestration wrapper only; not flashed because
  ADB port `15037` currently lists no devices.

Evidence:

- FACT: `scripts/nx549j-run-attempt97-unattended.sh` invokes the existing
  recovery wait + flash runner first, preserving the normal attempt97 flashing
  path and boot-image SHA check.
- FACT: the wrapper switches to
  `scripts/nx549j-wait-recovery-and-finish-latest.sh` only when the run helper
  exits with status `2`, the current flash-runner code for
  `automatic-recovery-timeout`.
- FACT: no-device smoke with `WAIT_ATTACH_SECONDS=2 POLL_SECONDS=1` exited
  status `1`, wrote an unattended log, did not create a finish wait log, and
  did not flash because the initial recovery wait failed.
- FACT: synthetic run helper exit `2` started the finish helper and returned
  success when the finish helper returned success.
- FACT: synthetic run helper exit `0` returned success without starting the
  finish helper.
- FACT: `bash -n scripts/nx549j-run-attempt97-unattended.sh` passed.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passed after updating the release-local runner snapshot.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passed.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt97-unattended.sh`
  - new one-command orchestration wrapper for recovery wait, flash, timeout
    classification, and finish wait handoff.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the unattended runner and documents it in the snapshot README.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - records updated runner snapshot manifest checksum
    `ee612de504050795ef09c22b49dfee95e479bd593ebf8784b3ac6d25cb9343f7`.

Expected next action:

- For an unattended bench run, use
  `/srv/forge/android/nx549j/scripts/nx549j-run-attempt97-unattended.sh`.
  It should either finish after userspace/automatic recovery evidence, or
  keep waiting for the post-timeout manual recovery entry and then collect
  recovery-readable markers and restore `misc`.

Rollback condition:

- Revert if the wrapper starts finish on any non-timeout run status, hides the
  original flash runner exit code, or starts flashing while the target is
  missing/non-recovery. Current synthetic tests cover all three control-flow
  branches.

2026-05-29 attempt97 latest unattended entrypoint:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side entrypoint alias only; not flashed because ADB
  port `15037` currently lists no devices.

Evidence:

- FACT: `scripts/nx549j-run-unattended-latest.sh` now delegates to
  `scripts/nx549j-run-attempt97-unattended.sh`, making the current unattended
  path independent of the attempt number in the operator command.
- FACT: no-device smoke with `WAIT_ATTACH_SECONDS=2 POLL_SECONDS=1` exited
  status `1`, wrote an unattended log, did not create a finish wait log, and
  did not flash because the initial recovery wait failed.
- FACT: `bash -n scripts/nx549j-run-unattended-latest.sh` passed.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  passed after updating the release-local runner snapshot.
- FACT: `sha256sum -c
  /srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/RUNNER_SNAPSHOT_SHA256SUMS`
  passed.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-run-unattended-latest.sh`
  - stable latest unattended entrypoint for the current recovery wait, flash,
    timeout handoff, finish wait path.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the latest unattended entrypoint and documents it in the snapshot
    README.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/SHA256SUMS`
  - records updated runner snapshot manifest checksum
    `106c76e905db0ba63149aa3429488de7a86a3747b921a01ad8765cd3b8f04a9b`.

Expected next action:

- For the next Windows/reverse-ADB bench run, use
  `/srv/forge/android/nx549j/scripts/nx549j-run-unattended-latest.sh`.
  This is the stable current command; it currently targets attempt97.

Rollback condition:

- Revert or retarget this alias if a newer attempt supersedes attempt97 or if
  the wrapper stops preserving the underlying attempt runner exit behavior.

2026-05-29 attempt97 latest unattended Windows ADB wait:

- Patch category: DIAGNOSTIC.
- Runtime status: attempted stable unattended runner against the Windows
  reverse ADB tunnel; no flash occurred because the target serial never
  appeared.

Evidence:

- FACT: `ss -ltnp | rg ':15037\b'` showed a listener on `127.0.0.1:15037`,
  so the reverse tunnel endpoint existed on this host.
- FACT: `ADB_SERVER_SOCKET=tcp:127.0.0.1:15037 adb devices -l` listed no
  attached devices before and after the run.
- FACT: ran `WAIT_ATTACH_SECONDS=300 POLL_SECONDS=2
  scripts/nx549j-run-unattended-latest.sh`.
- FACT: unattended log
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/wait-logs/unattended-20260529-231800.log`
  records start at `2026-05-29T23:18:00Z`, helper exit status `1`, and
  `unattended-result run-failed-before-timeout status=1`.
- FACT: recovery wait log
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/wait-logs/wait-recovery-20260529-231800.log`
  records only `target-not-ready serial=30785d1a state='missing'`, then
  `wait-timeout serial=30785d1a timeout=300s` at `2026-05-29T23:23:00Z`.
- FACT: because the run never reached `recovery-online`, it did not enter
  `nx549j-run-latest.sh`, did not call `require_recovery_target`, and did not
  write boot or misc.

Expected next action:

- On the Windows USB host, confirm `adb devices -l` shows serial `30785d1a`
  in `recovery`, then keep the reverse tunnel open. From this host rerun
  `/srv/forge/android/nx549j/scripts/nx549j-run-unattended-latest.sh`.

Rollback condition:

- None; this is an evidence-only wait run with no device writes.

2026-05-29 attempt97 reverse ADB preflight helper:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side reverse ADB preflight only; not flashed because
  ADB port `15037` currently lists no devices.

Evidence:

- FACT: added `scripts/nx549j-check-reverse-adb.sh`; it never writes to the
  phone and only runs listener/ADB visibility checks.
- FACT: current reverse tunnel check wrote
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/reverse-adb-check-20260529-232651/`
  with `verdict=NO_ADB_DEVICES`, `listener=present`, `adb_status=ok`,
  `device_count=0`, and empty `target_state`.
- FACT: no-listener smoke with `ADB_PORT=15038` returned status `2` and
  `verdict=NO_REVERSE_LISTENER`, `listener=missing`, `adb_status=failed`,
  `device_count=0`.
- FACT: `bash -n scripts/nx549j-check-reverse-adb.sh` passed.
- FACT: release-local runner snapshot and top-level `SHA256SUMS` were updated
  with runner snapshot manifest checksum
  `2bf4a95e34eb3b6058260b5d8307e859dd76f49727f21a01e48fa5c5efde93db`.

Files changed:

- `/srv/forge/android/nx549j/scripts/nx549j-check-reverse-adb.sh`
  - new preflight helper for Windows/reverse ADB readiness.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runner-snapshot-20260529/`
  - preserves the reverse ADB checker and documents it in the snapshot README.
- `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/README.md`
  - documents the preflight helper before starting a flash run.

Expected next action:

- Run `/srv/forge/android/nx549j/scripts/nx549j-check-reverse-adb.sh
  <out-dir>` before the next unattended flash attempt. Proceed to
  `nx549j-run-unattended-latest.sh` only when the preflight verdict is
  `TARGET_RECOVERY_READY`.

Rollback condition:

- Revert if the helper misclassifies a visible recovery device or introduces
  any device-write operation. Current implementation does not call `adb shell`
  or write to any block device.

2026-05-29 attempt97 reverse ADB preflight on port 15038:

- Patch category: DIAGNOSTIC.
- Runtime status: host-side reverse ADB preflight only; not flashed because
  ADB port `15038` currently lists no devices.

Evidence:

- FACT: user reported a new Windows reverse tunnel command on port `15038`
  was started after port `15037` was occupied.
- FACT: `scripts/nx549j-check-reverse-adb.sh` wrote
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/reverse-adb-check-20260529-235011/`.
- FACT: `status.env` reports `verdict=NO_ADB_DEVICES`, `listener=present`,
  `adb_status=ok`, `device_count=0`, empty `target_state`,
  `serial=30785d1a`, `adb_host=127.0.0.1`, and `adb_port=15038`.
- FACT: `adb-devices.txt` contains only `List of devices attached`.
- FACT: because the preflight verdict is not `TARGET_RECOVERY_READY`, no
  unattended flash was started and no boot/misc writes occurred.

Expected next action:

- On the Windows USB host, make `adb devices -l` show `30785d1a` in
  `recovery` first. Then keep the `15038` reverse tunnel open and rerun:
  `ADB_PORT=15038 scripts/nx549j-check-reverse-adb.sh <out-dir>`.
  Proceed with `ADB_PORT=15038 scripts/nx549j-run-unattended-latest.sh` only
  after `TARGET_RECOVERY_READY`.

Rollback condition:

- None; this is an evidence-only preflight with no device writes.

2026-05-30 attempt97 runtime capture and attempt98 setup-tail markers:

- Patch category: DIAGNOSTIC.
- Runtime status: attempt97 was flashed and captured after manual recovery;
  attempt98 is built, verified, and ready to flash, but not yet flashed because
  the current ADB listener does not expose serial `30785d1a`.

Hypothesis:

- HYPOTHESIS: the current earliest proven blocker is in the setup tail at or
  immediately after `early_ioremap_reset()`. Attempt97 proved execution reaches
  `FRGMARK_STAGE_SETUP_BEFORE_IOREMAP_RESET` (`0x85`) and then does not reach
  userspace ACK, the 120s recovery timeout marker, or automatic recovery.
  Attempt98 adds only setup-tail markers after `early_ioremap_reset()` to split
  that gap before changing behavior.

Evidence:

- FACT: attempt97 flash run directory:
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/flash-boot-bcb-20260529-235419`.
- FACT: attempt97 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/boot-userspace-ack-timeout-120s.img`.
- FACT: attempt97 boot SHA-256:
  `a0a40ab9539b4793ae772fa7a1f02793163fae653f9ef83805cbe51cf4666893`.
- FACT: attempt97 recovery capture:
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/flash-boot-bcb-20260529-235419/after-manual-recovery-20260529-235419`.
- FACT: attempt97 summary:
  `/srv/forge/work/nx549j-preserve/release-attempt97-20260529-userspace-ack-timeout/runtime/flash-boot-bcb-20260529-235419/SUMMARY.md`.
- FACT: attempt97 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, last decoded marker
  `0x85 setup_before_ioremap_reset`, BCB stage stamp `ABSENT`, pstore file
  count `1`, preboot pstore clear `PASS`, and misc restore `PASS`.
- FACT: attempt98 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers`.
- FACT: attempt98 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/boot-setup-tail-markers-120s.img`.
- FACT: attempt98 boot SHA-256:
  `2675aec9942893a45f30de6fe476466101d2d11429322e46e2b17ceac626ef99`.
- FACT: attempt98 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk init userspace ACK,
  no-BCB no-loop gate, pstore config, serial early console config, and ramoops
  DTB.
- FACT: attempt98 `SHA256SUMS` passed from the release directory, and
  `runner-snapshot-20260530/SHA256SUMS` also passed.
- FACT: the current preflight on `ADB_PORT=15038` reports
  `NO_REVERSE_LISTENER`; local `adb devices -l` on port `5037` lists two other
  serials and not `30785d1a`, so no flash was attempted.

Files changed:

- `include/linux/frgmark.h`
  - adds setup-tail marker IDs `0x86..0x8c`.
- `arch/arm64/kernel/frgmark.c`
  - adds decoder names for setup-tail marker IDs.
- `arch/arm64/kernel/setup.c`
  - stamps setup-tail markers after `early_ioremap_reset()`, PSCI init,
    boot CPU ops, SMP CPU enumeration, MPIDR hash build, boot-args check, and
    random pool init.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - updates the host marker decoder for the same setup-tail IDs.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - updates the debug UI marker-name table for the same setup-tail IDs.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt98.sh`
  - adds the attempt98 flash runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt98-unattended.sh`
  - adds the attempt98 unattended runner.
- `/srv/forge/android/nx549j/scripts/nx549j-run-latest.sh`
  - retargets latest to attempt98.
- `/srv/forge/android/nx549j/scripts/nx549j-run-unattended-latest.sh`
  - retargets latest unattended to attempt98.
- `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-run-latest.sh`
  - updates the default attempt directory to attempt98 and keeps env override.
- `/srv/forge/android/nx549j/scripts/nx549j-wait-recovery-and-finish-latest.sh`
  - updates the default attempt directory to attempt98.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - updates the marker coverage note for head/setup/timeout/initcall/userspace
    and display markers.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the new setup-tail marker strings in the release verification
    gate.

Why each file changed:

- The kernel marker edits are the narrowest instrumentation for the proven
  `0x85` gap; they do not intentionally change the success path except for
  writing marker bytes.
- The decoder/UI/script edits keep the marker table synchronized with the
  kernel header and prevent later captures from being misclassified as unknown
  stages.
- The attempt98 runner edits make the stable latest command flash the exact
  verified artifact and keep the existing timeout/manual-recovery collection
  behavior.

Expected next marker:

- If attempt98 still stops at `0x85 setup_before_ioremap_reset`, investigate
  `early_ioremap_reset()` return path and `frgmark_init_iomap()` availability
  immediately after it.
- If attempt98 reaches `0x86..0x8c`, continue from the last named setup-tail
  marker.
- If attempt98 reaches `0x15`, `0x16`, or `0x17`, classify userspace handoff,
  recovery timeout, or userspace ACK respectively before changing the kernel.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers boot-setup-tail-markers-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- `ADB_PORT=15038 scripts/nx549j-check-reverse-adb.sh /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/runtime/reverse-adb-check-$(date -u +%Y%m%d-%H%M%S)`
- `ADB_PORT=15038 scripts/nx549j-run-unattended-latest.sh`

Rollback condition:

- Revert the setup-tail marker patch if attempt98 proves the markers
  themselves are the regression, if marker IDs diverge from the decoder table,
  or after a later capture advances far enough that this diagnostic split is no
  longer needed.

2026-05-30 attempt98 reverse ADB watcher:

- Patch category: DIAGNOSTIC tooling.
- Runtime status: host-side automation only; no device write occurred because
  the current `15038` preflight still reports `NO_REVERSE_LISTENER`.

Evidence:

- FACT: added `/srv/forge/android/nx549j/scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- FACT: the watcher polls `ADB_PORTS`, runs the read-only
  `nx549j-check-reverse-adb.sh` for each port, and only hands off to
  `nx549j-run-unattended-latest.sh` after the checker reports
  `TARGET_RECOVERY_READY` for serial `30785d1a`.
- FACT: no-listener smoke command
  `WAIT_TUNNEL_SECONDS=1 POLL_SECONDS=1 ADB_PORTS='15038'
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh` exited status `2`,
  recorded only `NO_REVERSE_LISTENER` preflight rows, and did not start the
  flash runner.
- FACT: release-local runner snapshot now includes
  `nx549j-watch-reverse-adb-and-run-latest.sh`.
- FACT: `sha256sum -c` passed for both attempt98 `SHA256SUMS` and
  `runner-snapshot-20260530/SHA256SUMS` after adding the watcher.
- FACT: `scripts/nx549j-verify-release-artifact.sh
  /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers
  boot-setup-tail-markers-120s.img` regenerated `VERIFY.md` with all gates
  still PASS.

Expected next action:

- If the Windows tunnel timing is uncertain, run
  `ADB_PORTS="15038 15037"
  /srv/forge/android/nx549j/scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
  It should wait until `30785d1a` is visible in recovery, then flash attempt98
  and collect the same unattended evidence as the latest runner.

Rollback condition:

- Revert the watcher if it can start the flash runner without
  `TARGET_RECOVERY_READY`, scans an unintended serial, or hides the runner exit
  code. Current smoke covered the no-listener path.

2026-05-30 attempt98 blocked on missing reverse ADB listener:

- Patch category: DIAGNOSTIC evidence.
- Runtime status: no flash occurred.

Evidence:

- FACT: preflight command `ADB_PORT=15038
  scripts/nx549j-check-reverse-adb.sh
  /srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/runtime/reverse-adb-check-20260530-014628`
  reported `NO_REVERSE_LISTENER`.
- FACT: local `adb devices -l` on this host listed serials
  `86641c36687700000000` and `91HEBNL163XD`, but not NX549J serial
  `30785d1a`.
- FACT: watcher command `WAIT_TUNNEL_SECONDS=60 POLL_SECONDS=5
  ADB_PORTS='15038 15037'
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh` timed out at
  `watch-timeout timeout=60s`.
- FACT: every watcher preflight row for both `15038` and `15037` reported
  `NO_REVERSE_LISTENER`.

Expected next action:

- Restore the Windows reverse ADB tunnel for the phone-side ADB server, then
  rerun `ADB_PORTS="15038 15037"
  /srv/forge/android/nx549j/scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
  The current attempt98 artifact and latest runner are already verified and
  ready.

Rollback condition:

- None; this is no-write evidence of the current external connectivity
  blocker.

2026-05-30 attempt98/99 setup-tail capture and attempt100 pre-reset split:

- Patch category: DIAGNOSTIC.
- Runtime status: attempt98 and attempt99 were flashed and captured after
  manual recovery; attempt100 is flashed and boot-partition verified, but its
  post-timeout recovery capture is still blocked on `30785d1a` being missing
  from the reverse ADB server.

Hypothesis:

- HYPOTHESIS: the current earliest proven target-kernel blocker is between
  `FRGMARK_STAGE_SETUP_BEFORE_IOREMAP_RESET` (`0x85`) and the first marker
  after `early_ioremap_reset()`. Attempt99 added a linear ramoops marker after
  `early_ioremap_reset()` to avoid relying on normal ioremap. Attempt100 adds
  `0x8e setup_before_reset_splash_done` immediately after the pre-reset
  splashprobe and before `early_ioremap_reset()` to split splashprobe from the
  reset call itself.

Evidence:

- FACT: attempt98 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers`.
- FACT: attempt98 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/boot-setup-tail-markers-120s.img`.
- FACT: attempt98 boot SHA-256:
  `2675aec9942893a45f30de6fe476466101d2d11429322e46e2b17ceac626ef99`.
- FACT: attempt98 flash summary:
  `/srv/forge/work/nx549j-preserve/release-attempt98-20260530-setup-tail-markers/runtime/flash-boot-bcb-20260530-015437/SUMMARY.md`.
- FACT: attempt98 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, last decoded marker
  `0x85 setup_before_ioremap_reset`, pstore file count `1`, preboot pstore
  clear `PASS`, and misc restore `PASS`.
- FACT: attempt99 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt99-20260530-linear-post-ioremap-reset`.
- FACT: attempt99 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt99-20260530-linear-post-ioremap-reset/boot-linear-post-ioremap-reset-120s.img`.
- FACT: attempt99 boot SHA-256:
  `899b0eb55aa5f473fd54458480381b47a9f83b06665e0b3690be3f33531e2f6d`.
- FACT: attempt99 flash summary:
  `/srv/forge/work/nx549j-preserve/release-attempt99-20260530-linear-post-ioremap-reset/runtime/flash-boot-bcb-20260530-021249/SUMMARY.md`.
- FACT: attempt99 summary again reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, last decoded marker
  `0x85 setup_before_ioremap_reset`, pstore file count `1`, preboot pstore
  clear `PASS`, and misc restore `PASS`.
- FACT: attempt100 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker`.
- FACT: attempt100 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker/boot-before-reset-splash-marker-120s.img`.
- FACT: attempt100 boot SHA-256:
  `b125778622e855afebf60c11565d25c8ba0f5914078aaef68b6250cd7e0759d4`.
- FACT: attempt100 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.
- FACT: attempt100 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker/runtime/flash-boot-bcb-20260530-022707`.
- FACT: attempt100 boot partition prefix SHA-256 matched the local boot image:
  `b125778622e855afebf60c11565d25c8ba0f5914078aaef68b6250cd7e0759d4`.
- FACT: attempt100 timed out waiting for automatic recovery and is currently
  waiting for manual recovery capture; local ADB on reverse port `15038`
  reports no attached devices, so target marker evidence is not yet available.

Files changed:

- `include/linux/frgmark.h`
  - adds `0x8d setup_ioremap_reset_returned_linear` and
    `0x8e setup_before_reset_splash_done`, plus
    `frgmark_linear_ramoops()`.
- `arch/arm64/kernel/frgmark.c`
  - adds names for `0x8d` and `0x8e`, allows setup-stage ramoops slots through
    `0x8e`, and writes a linear phys-to-virt ramoops record after
    `early_ioremap_reset()` can no longer rely on early ioremap.
- `arch/arm64/kernel/setup.c`
  - stamps `0x8e` after the pre-reset splashprobe and writes `0x8d` via the
    linear ramoops helper after `early_ioremap_reset()` returns.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - keeps host marker decoding in sync for `0x8d` and `0x8e`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - keeps the debug UI marker table in sync for `0x8d` and `0x8e`.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the new marker strings in release verification.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - updates the setup marker coverage note to `0x80..0x8e`.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt100*.sh` and latest
  runner scripts
  - retarget stable flash automation to attempt100.

Why each file changed:

- The kernel marker edits are the narrowest split for the repeated `0x85`
  stop and do not intentionally skip or disable a subsystem.
- The linear ramoops helper avoids using the normal `ioremap()` path for the
  first post-`early_ioremap_reset()` marker, so a missing `0x8d` is meaningful
  for the reset/splashprobe split instead of only proving normal mapping is
  unavailable.
- The decoder/UI/verification edits keep artifact and marker identity
  synchronized with the kernel patch, as required before interpreting the next
  capture.
- The runner edits ensure the user-facing `latest` flash path writes the
  exact verified attempt100 image.

Expected next marker:

- If attempt100 still decodes as `0x85 setup_before_ioremap_reset`, the
  blocker is inside or before `nx549j_splashprobe(NX549J_SPLASH_STAGE_BEFORE_IOREMAP_RESET)`.
- If attempt100 decodes as `0x8e setup_before_reset_splash_done`, the
  blocker is inside `early_ioremap_reset()` before return.
- If attempt100 decodes as `0x8d setup_ioremap_reset_returned_linear`, the
  blocker moved after `early_ioremap_reset()` and before normal marker
  remapping or the next setup-tail stage.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker boot-before-reset-splash-marker-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- `ADB_PORT=15038 ADB_PORTS='15038' scripts/nx549j-watch-reverse-adb-and-run-latest.sh`
- After manual recovery appears, inspect:
  `/srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker/runtime/flash-boot-bcb-20260530-022707/SUMMARY.md`.

Rollback condition:

- Revert this diagnostic split if attempt100 proves the added markers cause a
  regression before `0x85`, if marker IDs diverge from host decoders, or after
  the next capture identifies the exact side of the `0x85` gap.

2026-05-30 attempt101/102 early-ioremap and frgmark-iomap isolation:

- Patch category: ISOLATION.
- Runtime status: attempt101 was flashed and captured after manual recovery;
  attempt102 is built, verified, flashed, and timed out waiting for automatic
  recovery, but its post-timeout capture is still pending because
  `30785d1a` has not yet reappeared in recovery.

Hypothesis:

- HYPOTHESIS: the repeated bootlogo hang is currently dominated by diagnostic
  marker plumbing in the `setup_arch()` tail rather than a later userspace
  handoff. attempt100 proved the pre-reset splashprobe returns and reaches
  `0x8e`. attempt101 skipped `early_ioremap_reset()` and proved execution
  reaches `0x8f`, then stops before `0x86`. That isolates the next blocker to
  `frgmark_init_iomap()` or the first normal `frgmark()` call after it.
  attempt102 defers that normal-ioremap diagnostic setup and uses the already
  proven early marker path for the rest of `setup_arch()`.

Evidence:

- FACT: attempt100 flash summary:
  `/srv/forge/work/nx549j-preserve/release-attempt100-20260530-before-reset-splash-marker/runtime/flash-boot-bcb-20260530-022707/SUMMARY.md`.
- FACT: attempt100 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, last decoded marker
  `0x8e setup_before_reset_splash_done`, preboot pstore clear `PASS`, and misc
  restore `PASS`.
- FACT: attempt101 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt101-20260530-skip-early-ioremap-reset-isolation`.
- FACT: attempt101 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt101-20260530-skip-early-ioremap-reset-isolation/boot-skip-early-ioremap-reset-isolation-120s.img`.
- FACT: attempt101 boot SHA-256:
  `c854fe29e8953bfec0dc3fe691d0c29ad679ab5a8d79c0c1c32003d5fa4a8e8e`.
- FACT: attempt101 flash summary:
  `/srv/forge/work/nx549j-preserve/release-attempt101-20260530-skip-early-ioremap-reset-isolation/runtime/flash-boot-bcb-20260530-131139/SUMMARY.md`.
- FACT: attempt101 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, last decoded marker
  `0x8f setup_ioremap_reset_skipped_isolation`, preboot pstore clear `PASS`,
  and misc restore `PASS`.
- FACT: attempt102 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap`.
- FACT: attempt102 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap/boot-defer-frgmark-normal-iomap-120s.img`.
- FACT: attempt102 boot SHA-256:
  `064dea4b53f4baa4efa326ece43627ac5570bfca7ec5995f013654e5b2120a8b`.
- FACT: attempt102 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.
- FACT: attempt102 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap/runtime/flash-boot-bcb-20260530-132928`.
- FACT: attempt102 boot partition prefix SHA-256 matched the local boot image:
  `064dea4b53f4baa4efa326ece43627ac5570bfca7ec5995f013654e5b2120a8b`.
- FACT: attempt102 timed out waiting for automatic recovery; the wait loop was
  stopped after repeated `target-not-ready serial=30785d1a state='missing'`.
  No attempt102 target marker has been decoded yet.

Files changed:

- `include/linux/frgmark.h`
  - adds `0x8f setup_ioremap_reset_skipped_isolation`.
- `arch/arm64/kernel/frgmark.c`
  - adds the `0x8f` marker name, allows the setup ramoops slot through `0x8f`,
    and makes `frgmark()` fall back to `frgmark_early()` when normal IMEM
    mapping is unavailable.
- `arch/arm64/kernel/setup.c`
  - temporarily skips `early_ioremap_reset()`, defers `frgmark_init_iomap()`,
    and uses early marker writes for the remaining setup-tail stages.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - keeps host marker decoding in sync for `0x8f`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - keeps the debug UI marker table in sync for `0x8f`.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the `0x8f` marker string in release verification.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - updates the setup marker coverage note to `0x80..0x8f`.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt101*.sh`,
  `/srv/forge/android/nx549j/scripts/nx549j-run-attempt102*.sh`, and latest
  runner scripts
  - retarget flash automation to the exact verified boot images.

Why each file changed:

- The kernel edits are scoped isolation of the earliest proven hang. They do
  not disable unrelated drivers or late subsystems.
- The fallback to `frgmark_early()` preserves marker visibility after deferring
  normal ioremap, so a future stop point is still recovery-readable.
- The decoder/UI/verification edits prevent the new marker from being decoded
  as an unknown stage.

Expected next marker:

- If attempt102 reaches `0x86..0x8c`, then diagnostic normal-ioremap setup was
  blocking the setup tail.
- If attempt102 reaches `0x02` or later low-numbered `init/main.c` markers,
  `setup_arch()` returned and the next blocker is after it.
- If attempt102 still stops at `0x8f`, the blocker is immediately after the
  skip marker before the first deferred early marker.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap boot-defer-frgmark-normal-iomap-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- After manual recovery appears:
  `ADB_PORT=15038 scripts/nx549j-wait-recovery-and-finish-latest.sh /srv/forge/work/nx549j-preserve/release-attempt102-20260530-defer-frgmark-normal-iomap/runtime/flash-boot-bcb-20260530-132928`.

Rollback condition:

- Revert attempt101/102 isolation once the next capture shows whether
  `setup_arch()` advances past `0x8f`, or if the early fallback creates a new
  marker regression before `0x8f`.

2026-05-30 attempt103 late setup diagnostic hook isolation:

- Patch category: ISOLATION.
- Runtime status: built, verified, flashed to `30785d1a`, timed out waiting
  for automatic recovery or userspace, then captured after manual recovery.

Hypothesis:

- HYPOTHESIS: attempt102 proved the setup tail advances through
  `0x8c setup_random_pool_done`, so the next possible local blockers are the
  late diagnostic `nx549j_splashprobe()` at the end of `setup_arch()` and the
  immediate post-`setup_arch()` `frgmark_init_iomap()` call in `start_kernel()`.
  attempt103 removes those diagnostic hooks so the next recovery-readable marker
  should be `0x02 setup_arch_done` if `setup_arch()` returns normally.

Evidence:

- FACT: attempt103 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap`.
- FACT: attempt103 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/boot-skip-late-splash-frgmark-iomap-120s.img`.
- FACT: attempt103 boot SHA-256:
  `dcf45cb65a47f7954975fa22de6be8a1fcdd97834eb28e601e76788274f25fa9`.
- FACT: attempt103 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.
- FACT: attempt103 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/runtime/flash-boot-bcb-20260530-135630`.
- FACT: attempt103 boot partition prefix SHA-256 matched the local boot image:
  `dcf45cb65a47f7954975fa22de6be8a1fcdd97834eb28e601e76788274f25fa9`.
- FACT: attempt103 wait result was `automatic-recovery-timeout`; no recovery or
  Android `device` ADB state appeared inside the 260-second window.
- FACT: attempt103 manual recovery capture:
  `/srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/runtime/flash-boot-bcb-20260530-135630/after-manual-recovery`.
- FACT: attempt103 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, preboot pstore clear
  `PASS`, and misc restore `PASS`.
- FACT: attempt103 raw marker latest slot at `marker-od.txt` offset `0x40`
  contains `4652470b`, decoded as `0x0b kernel_init_begin`. The summary's
  `0x8f` line is an older table entry from grep order and is not the latest
  slot.
- INFERENCE: attempt103 passed `setup_arch()`, `trap_init()`, `mm_init()`,
  scheduler setup, timer setup, `console_init()`, and reached `kernel_init()`.
  The next unproven area starts inside `kernel_init_freeable()`, before
  `0x0c kernel_init_freeable_done`.

Files changed:

- `arch/arm64/kernel/setup.c`
  - removes the late `NX549J_SPLASH_STAGE_SETUP_ARCH_DONE` splashprobe after
    `init_random_pool()`.
- `init/main.c`
  - removes the immediate post-`setup_arch()` splashprobe and
    `frgmark_init_iomap()` call, leaving the normal `frgmark(0x02)` marker as
    the next test point.

Why each file changed:

- Both edits are scoped to diagnostic marker plumbing on the earliest proven
  still-unpassed edge between the setup tail and the first normal
  `start_kernel()` marker.

Expected next marker:

- If attempt103 reaches `0x02 setup_arch_done`, then late diagnostic hook
  plumbing was the blocker and the next failure is after `setup_arch()`.
- If attempt103 still stops at `0x8c`, then the blocker is in `setup_arch()`
  return or an uninstrumented path immediately before the first normal
  `frgmark()` after it.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap boot-skip-late-splash-frgmark-iomap-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- After manual recovery appears:
  `ADB_HOST=127.0.0.1 ADB_PORT=15038 SERIAL=30785d1a /srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/runner-snapshot-20260530/nx549j-finish-flash-timeout.sh /srv/forge/work/nx549j-preserve/release-attempt103-20260530-skip-late-splash-frgmark-iomap/runtime/flash-boot-bcb-20260530-135630`

Rollback condition:

- Revert attempt103 if the capture shows no advancement past `0x8c`, or after
  the next marker proves whether the late diagnostic hooks are safe to remove
  from the active debug branch.

2026-05-30 attempt104 rest_init/kernel_init_freeable marker split:

- Patch category: DIAGNOSTIC.
- Runtime status: built, verified, flashed to `30785d1a`, and timed out waiting
  for automatic recovery or userspace, then captured after manual recovery.

Hypothesis:

- HYPOTHESIS: because attempt103 reaches `0x0b kernel_init_begin` but never
  reaches `0x0c kernel_init_freeable_done`, the active hang is now inside
  `kernel_init_freeable()` or the corresponding `rest_init()` handoff that
  completes `kthreadd_done`. attempt104 adds markers around the first blocking
  candidates instead of disabling any subsystem.

Evidence:

- FACT: attempt103 marker decode includes `0x0b kernel_init_begin` as the
  latest raw marker slot.
- FACT: attempt103 does not include `0x0c kernel_init_freeable_done`, initcall
  markers, userspace markers, or recovery timeout reset markers.
- FACT: attempt104 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers`.
- FACT: attempt104 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/boot-kernel-init-freeable-markers-120s.img`.
- FACT: attempt104 boot SHA-256:
  `43f0458cbd629653a27c69ed534fc55c7ae8d604326657314fb4b9c534e11a0f`.
- FACT: attempt104 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.
- FACT: attempt104 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runtime/flash-boot-bcb-20260530-144044`.
- FACT: attempt104 boot partition prefix SHA-256 matched the local boot image:
  `43f0458cbd629653a27c69ed534fc55c7ae8d604326657314fb4b9c534e11a0f`.
- FACT: attempt104 wait result was `automatic-recovery-timeout`; no recovery or
  Android `device` ADB state for `30785d1a` appeared inside the 260-second
  window.
- FACT: attempt104 manual recovery capture:
  `/srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runtime/flash-boot-bcb-20260530-144044/after-manual-recovery`.
- FACT: attempt104 summary reports boot identity `PASS`, recovery result
  `MANUAL_RECOVERY`, target marker evidence `PRESENT`, latest decoded marker
  `0x62 kernel_init_bcb_kick_done`, preboot pstore clear `PASS`, and misc
  restore `PASS`.
- FACT: attempt104 raw marker latest slot at `marker-od.txt` offset `0x40`
  contains `46524762`, decoded as `0x62 kernel_init_bcb_kick_done`.
- INFERENCE: attempt104 passed `smp_prepare_cpus()`, `workqueue_init()`, and
  `frgmark_recovery_bcb_kick()`, then stopped before
  `0x63 kernel_init_pre_smp_initcalls_done`, so the next blocker is inside
  `do_pre_smp_initcalls()`.

Files changed:

- `include/linux/frgmark.h`
  - adds `0x18..0x1f` rest/kernel-init handoff markers and `0x60..0x69`
    later `kernel_init_freeable()` markers.
- `arch/arm64/kernel/frgmark.c`
  - adds names for the new markers and preserves `0x60..0x69` in separate
    ramoops table slots.
- `init/main.c`
  - marks `rest_init()` thread creation/completion points and
    `kernel_init_freeable()` milestones through `do_basic_setup()`.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes the new marker IDs.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - displays the new marker names in the debug UI.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - classifies `rest_*` markers as earlier-stage target markers.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the new critical marker strings in release verification.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt104*.sh` and latest
  runner scripts
  - retarget flashing to the exact attempt104 image.

Why each file changed:

- The kernel edits are diagnostic-only markers on the earliest proven
  unpassed path. No driver, DTS, Kconfig, or subsystem behavior is disabled.
- Decoder/UI/verification changes keep the marker table synchronized with the
  kernel marker header.
- Runner changes ensure future flashes use the exact verified attempt104 image
  and still refuse non-`30785d1a` serials.

Expected next marker:

- `0x1d` without `0x1e` means `kernel_init` is stuck waiting for
  `kthreadd_done`.
- `0x1f` without `0x60` means `smp_prepare_cpus()` is the next blocker.
- `0x68` without `0x69` means the blocker is inside `do_basic_setup()` or one
  of its initcall stages.
- `0x0f`/`0x0c` or later means `kernel_init_freeable()` completes and the next
  blocker is after basic setup.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers boot-kernel-init-freeable-markers-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- Flash from recovery:
  `ADB_PORT=15038 ADB_PORTS=15038 /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runner-snapshot-20260530/nx549j-run-attempt104.sh`
- After manual recovery appears:
  `ADB_HOST=127.0.0.1 ADB_PORT=15038 SERIAL=30785d1a /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runner-snapshot-20260530/nx549j-finish-flash-timeout.sh /srv/forge/work/nx549j-preserve/release-attempt104-20260530-kernel-init-freeable-markers/runtime/flash-boot-bcb-20260530-144044`

Rollback condition:

- Revert attempt104 diagnostic markers after the next capture narrows the
  `kernel_init_freeable()` blocker, or if marker insertion changes the last
  stage before `0x0b`.

2026-05-30 attempt105 pre-SMP initcall index markers:

- Patch category: DIAGNOSTIC.
- Runtime status: built and verified. Flash/capture is pending.

Hypothesis:

- HYPOTHESIS: attempt104 proves `do_pre_smp_initcalls()` is the next active
  hang region. There are 48 early initcall entries between `__initcall_start`
  and `__initcall0_start`; attempt105 writes marker `0xa0 + index` before each
  entry so the next capture identifies the running/hung initcall by index.

Evidence:

- FACT: attempt104 latest raw marker is `0x62 kernel_init_bcb_kick_done`.
- FACT: attempt104 has no `0x63 kernel_init_pre_smp_initcalls_done`, no
  `smp_init` marker, and no later initcall/userspace markers.
- FACT: attempt104 `System.map` places `__initcall_start` at
  `ffffff8009a93dd8` and `__initcall0_start` at `ffffff8009a93f58`, a 48-entry
  span when interpreted as 8-byte function pointers.
- FACT: attempt105 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers`.
- FACT: attempt105 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers/boot-pre-smp-initcall-index-markers-120s.img`.
- FACT: attempt105 boot SHA-256:
  `b433c0d1ef0b15a3851d027ced55b4f4fc8b0bb2ab55c5d96919bfd390ec7b5b`.
- FACT: attempt105 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.

Files changed:

- `include/linux/frgmark.h`
  - adds `0xa0..0xcf` as pre-SMP initcall index markers.
- `arch/arm64/kernel/frgmark.c`
  - names `0xa0..0xcf` as `pre_smp_initcall` and reserves separate ramoops
    table slots for them.
- `init/main.c`
  - writes `0xa0 + index` before each `do_pre_smp_initcalls()` entry.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes `0xa0..0xcf` as `pre_smp_initcall_00..47`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - displays the new pre-SMP initcall index names.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - prefers the latest raw `marker-od.txt` slot and classifies
    `pre_smp_initcall_*` as earlier-stage target markers.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the new marker string in release verification.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt105*.sh` and latest
  runner scripts
  - retarget flashing to the exact attempt105 image.

Why each file changed:

- The kernel edits are diagnostic-only markers for the exact next proven
  blocker region. No early initcall is skipped or changed.
- Decoder/UI/verification changes keep the marker table synchronized with the
  kernel marker header.
- The summarizer change fixes the observed stale-grep problem where older table
  entries could hide the actual latest marker slot.

Expected next marker:

- Latest `0xa0..0xcf` identifies the running or hung pre-SMP initcall by index.
- `0x63` means all pre-SMP initcalls returned and the next blocker is after
  `do_pre_smp_initcalls()`.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers boot-pre-smp-initcall-index-markers-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers && sha256sum -c SHA256SUMS)`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers/runner-snapshot-20260530 && sha256sum -c SHA256SUMS)`
- Flash from recovery:
  `ADB_PORT=15038 ADB_PORTS=15038 /srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers/runner-snapshot-20260530/nx549j-run-attempt105.sh`

Rollback condition:

- Revert attempt105 diagnostic markers after the next capture identifies the
  pre-SMP initcall index, or if marker insertion changes the last stage before
  `0x62`.

Runtime result:

- FACT: attempt105 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt105-20260530-pre-smp-initcall-index-markers/runtime/flash-boot-bcb-20260530-150019`.
- FACT: attempt105 boot partition prefix SHA-256 matched the local boot image:
  `b433c0d1ef0b15a3851d027ced55b4f4fc8b0bb2ab55c5d96919bfd390ec7b5b`.
- FACT: attempt105 summary reports boot identity `PASS`, recovery result
  `CANDIDATE_AUTO`, target marker evidence `PRESENT`, preboot pstore clear
  `PASS`, and misc restore `PASS`.
- FACT: because user/manual interaction happened around the wait window, the
  attempt105 recovery result is not accepted as proof of automatic recovery.
- FACT: attempt105 latest decoded marker remained
  `0x62 kernel_init_bcb_kick_done`.
- FACT: no `0x6a`, `0x6b`, or `0xa0..0xcf` markers were present in attempt105.
- FACT: attempt105 vmlinux disassembly shows the pre-SMP loop and first
  `0xa0` marker code are compiled inline immediately after marker `0x62`.
- INFERENCE: attempt105 does not prove that the first pre-SMP initcall hangs.
  It instead points to either the return path from `frgmark(0x62)` or the
  immediate early BCB retry kick/workqueue side effect after `workqueue_init()`.

2026-05-30 attempt106 defer BCB before pre-SMP markers:

- Patch category: DIAGNOSTIC / ISOLATION.
- Runtime status: built and verified. Flash/capture is next.

Hypothesis:

- HYPOTHESIS: attempt105 stopped after the immediate early BCB retry kick, not
  because a pre-SMP initcall body ran. Kicking a BCB write worker immediately
  after `workqueue_init()` may be too early for the block layer and can stop
  progress before the next marker. attempt106 defers BCB writes until later
  device/late initcall checkpoints or after `do_basic_setup()`, then adds two
  markers to prove whether execution gets past the old boundary.

Evidence:

- FACT: attempt106 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt106-20260530-defer-bcb-pre-smp-markers`.
- FACT: attempt106 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt106-20260530-defer-bcb-pre-smp-markers/boot-defer-bcb-pre-smp-markers-120s.img`.
- FACT: attempt106 boot SHA-256:
  `f664f9913a872867b22fec0d38a0e659e201cc3699e5f2b5844d53a58259054b`.
- FACT: attempt106 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.
- FACT: attempt106 vmlinux disassembly shows the marker sequence after
  `workqueue_init()` as `0x61`, `0x62`, `0x6a`, `0x6b`, then pre-SMP
  `0xa0 + index`; the old immediate call to
  `frgmark_recovery_bcb_kick("kernel-init-workqueue-ready")` is gone.

Files changed:

- `include/linux/frgmark.h`
  - adds `0x6a kernel_init_pre_smp_call_begin` and
    `0x6b kernel_init_pre_smp_loop_begin`.
- `arch/arm64/kernel/frgmark.c`
  - names the new markers, preserves them in dedicated ramoops slots, stops
    scheduling the BCB retry worker during early timeout arming, and only kicks
    BCB work from safer later checkpoints.
- `init/main.c`
  - removes the immediate BCB kick after `workqueue_init()`, adds markers
    before and inside the pre-SMP loop, and moves the explicit BCB kick after
    `do_basic_setup()`.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes `0x6a` and `0x6b`.
- `/home/n8n/build-station/apps/web/src/app/debug/page.tsx`
  - displays `0x6a` and `0x6b` marker names.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires the new marker strings in release verification.
- `/srv/forge/android/nx549j/scripts/nx549j-summarize-flash-run.sh`
  - documents the `0x60..0x6b` kernel-init marker range.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt106*.sh` and latest
  runner scripts
  - retarget flashing to the exact attempt106 image.

Expected next marker:

- Latest `0x6a` means `frgmark(0x62)` returned and execution reached the
  pre-SMP call boundary.
- Latest `0x6b` means the inlined `do_pre_smp_initcalls()` loop started but no
  indexed pre-SMP marker was written.
- Latest `0xa0..0xcf` identifies the running or hung pre-SMP initcall by index.
- Latest `0x62` after attempt106 means the blocker is still inside
  `frgmark(0x62)` or the instruction path immediately after it, not the early
  BCB worker kick.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt106-20260530-defer-bcb-pre-smp-markers boot-defer-bcb-pre-smp-markers-120s.img`
- `(cd /srv/forge/work/nx549j-preserve/release-attempt106-20260530-defer-bcb-pre-smp-markers && sha256sum -c SHA256SUMS)`
- Flash from recovery:
  `ADB_PORT=15038 ADB_PORTS=15038 /srv/forge/android/nx549j/scripts/nx549j-run-attempt106.sh`

Rollback condition:

- Revert attempt106 diagnostic isolation after the next capture proves whether
  early BCB kick was the blocker. If the marker advances past `0x62`, keep the
  BCB deferral direction and then debug the new latest marker.

Runtime result:

- FACT: attempt106 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt106-20260530-defer-bcb-pre-smp-markers/runtime/flash-boot-bcb-20260530-152216`.
- FACT: attempt106 boot partition prefix SHA-256 matched the local boot image:
  `f664f9913a872867b22fec0d38a0e659e201cc3699e5f2b5844d53a58259054b`.
- FACT: no host-prewritten BCB was used.
- FACT: the device returned to recovery inside the wait window, and the runner
  restored `misc` to backup SHA
  `30e14955ebf1352266dc2ff8067e68104607e750abb9d3b36582b8af909fcb58`.
- FACT: attempt106 pstore captured real target-kernel logs, not just raw
  marker slots.
- FACT: attempt106 advanced through `0x6a`, `0x6b`, every pre-SMP initcall
  marker `0xa0..0xcf`, `0x63`, `0x64`, `0x65`, `0x66`, `0x67`, `0x68`,
  `0x09`, `0x0a`, `0x0d`, initcall levels `0x50..0x57`, `0x0e`, `0x69`,
  `0x0f`, and `0x0c`.
- FACT: attempt106 wrote the BCB recovery command after
  `kernel-init-basic-setup-done`, then panic recovery returned to recovery.
- FACT: attempt106 panic:
  `Unable to handle kernel paging request at virtual address ffffff8009a08d40`;
  `PC is at frgmark_early+0x0/0xec`, `LR is at frgmark+0x10c/0x188`,
  after `Freeing unused kernel memory: 6144K`.
- INFERENCE: attempt106 fixed the early `0x62` stall by deferring the early BCB
  worker. The remaining crash is caused by diagnostic code calling the
  `__init` `frgmark_early()` fallback after `free_initmem()`, because normal
  FRGmark iomap was still not prepared.

2026-05-30 attempt107 FRGmark post-init iomap:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for diagnostic code.
- Runtime status: built and verified. Flash/capture is next.

Hypothesis:

- HYPOTHESIS: the next boot should no longer panic at `frgmark_early+0x0`
  after `free_initmem()`. If userspace still fails, the next pstore should
  show either `0x10 exec_ramdisk_init`, a userspace ACK marker, or a real
  post-init failure unrelated to the freed `__init` diagnostic path.

Evidence:

- FACT: attempt107 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt107-20260530-frgmark-post-init-iomap`.
- FACT: attempt107 boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt107-20260530-frgmark-post-init-iomap/boot-frgmark-post-init-iomap-120s.img`.
- FACT: attempt107 boot SHA-256:
  `454366ed83b56fadfb4c7079b9c8b79f38750d434e071eb0b08fe088b27876e8`.
- FACT: attempt107 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  required symbols, required marker strings, ramdisk userspace ACK, no-BCB
  no-loop gate, pstore config, serial early console config, and ramoops DTB.

Files changed:

- `include/linux/frgmark.h`
  - declares `frgmark_prepare_post_init()`.
- `arch/arm64/kernel/frgmark.c`
  - adds `frgmark_prepare_post_init()`, prepares normal iomaps before initmem
    is freed, and refuses to call `frgmark_early()` if initmem is unavailable.
- `init/main.c`
  - calls `frgmark_prepare_post_init()` after async init synchronization and
    before `free_initmem()`, then marks `0x0c`.
- `/home/n8n/build-station/scripts/nx549j-frgmark-decode-spm.sh`
  - decodes `FRGmark early stage=..` pstore lines and no longer uses command
    substitution on binary pstore input.
- `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
  - requires `frgmark_prepare_post_init` in release `System.map`.
- `/srv/forge/android/nx549j/scripts/nx549j-run-attempt107*.sh` and latest
  runner scripts
  - retarget flashing to the exact attempt107 image.

Expected next marker:

- `0x10 exec_ramdisk_init` means we passed `free_initmem()` and started init
  execution.
- `0x15 userspace_reached` or `0x17 userspace_ack` means the kernel reached
  Android init far enough for the ramdisk ACK path.
- Another panic with PC outside `frgmark_early` is now the next real blocker.

Verification commands:

- `scripts/nx549j-verify-release-artifact.sh /srv/forge/work/nx549j-preserve/release-attempt107-20260530-frgmark-post-init-iomap boot-frgmark-post-init-iomap-120s.img`
- Flash from recovery:
  `ADB_PORT=15038 ADB_PORTS=15038 /srv/forge/android/nx549j/scripts/nx549j-run-attempt107.sh`

Rollback condition:

- Keep this guard unless a later run proves normal FRGmark iomap itself causes
  a new earlier boot regression. The freed-`__init` fallback is a confirmed
  diagnostic bug.

2026-05-30 attempt108 BCB late-stage refresh:

- Patch category: DIAGNOSTIC / USERSpace boundary proof.
- Runtime status: flashed twice and returned to recovery automatically inside
  the wait window.
- Attempt108 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt108-20260530-bcb-late-stage-refresh`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt108-20260530-bcb-late-stage-refresh/boot-bcb-late-stage-refresh-120s.img`.
- SHA-256:
  `2e97602d9832f8656fa01466a47fefd6e9075fe21a0a48b14432c2918187b0f2`.
- FACT: post-format flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt108-20260530-bcb-late-stage-refresh/runtime/flash-boot-bcb-20260530-155100`.
- FACT: boot partition prefix SHA-256 matched the local attempt108 boot image.
- FACT: recovery summary reports boot identity `PASS`, recovery result
  `CANDIDATE_AUTO`, target marker evidence `PRESENT`, BCB stage stamp
  `PRESENT`, pstore file count `0`, preboot pstore clear `PASS`, and misc
  restore `PASS`.
- FACT: BCB stage stamp recorded reason `userspace-reached`, last stage
  `0x15 userspace_reached`, jiffies `4294879300`.
- FACT: `/cache` is mountable in recovery as ext4 on `/dev/block/mmcblk0p25`.
- FACT: after the user formatted `/data`, recovery no longer found a crypto
  footer, but attempt108 still did not reach Android ADB.
- INFERENCE: the 4.9 kernel now reaches `execve("/init")`; the remaining
  blocker is first-stage init / first-stage mount / early userspace before the
  existing `/proc/frgmark_userspace_ack` write.

2026-05-30 attempt109 force ADB and cache trace:

- Patch category: DIAGNOSTIC / BOOT-ONLY USERSpace observability.
- Runtime status: built and verified. Flash/capture is next.
- Attempt109 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt109-20260530-force-adb-cache-trace`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt109-20260530-force-adb-cache-trace/boot-force-adb-cache-trace-120s.img`.
- SHA-256:
  `2e17552fb2d562129a4129eefccd760e3e7b7887668ccfcbf42d9c57b517df26`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostic strings, no-BCB no-loop
  gate, pstore config, serial early console config, and ramoops DTB.
- FACT: ramdisk `init` strings include `ro.secure=0`, `ro.adb.secure=0`,
  `persist.sys.usb.config=adb`, `NX549J forcing adb debug properties from
  first-stage init`, `/cache/nx549j-bootdiag/first_stage.log`,
  `before-do-first-stage-mount`, and `after-do-first-stage-mount`.
- Source changes:
  - `system/core/init/first_stage_init.cpp` writes
    `/debug_ramdisk/adb_debug.prop`, sets `INIT_FORCE_DEBUGGABLE=true`, and
    writes first-stage trace records to `/cache/nx549j-bootdiag/first_stage.log`
    when `/dev/block/mmcblk0p25` can be mounted.
  - `device/nubia/nx549j/prop.mk` forces `ro.secure=0`,
    `ro.adb.secure=0`, and `persist.sys.usb.config=adb` for future full/system
    builds.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    requires the new ramdisk diagnostic strings.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt109*.sh` and latest
    runner scripts target the exact attempt109 image.
- Expected runtime evidence:
  - If Android ADB appears before the timeout, inspect userspace logs directly.
  - If recovery returns, pull `/cache/nx549j-bootdiag/first_stage.log` from
    recovery and compare the latest first-stage trace with the BCB
    `userspace-reached` stamp.
- Flash command:
  `ADB_HOST=127.0.0.1 ADB_PORT=15038 ADB_PORTS=15038 SERIAL=30785d1a /srv/forge/android/nx549j/scripts/nx549j-run-attempt109.sh`.

Runtime result:

- FACT: attempt109 flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt109-20260530-force-adb-cache-trace/runtime/flash-boot-bcb-20260530-161055`.
- FACT: boot partition prefix SHA-256 matched the local attempt109 boot image:
  `2e17552fb2d562129a4129eefccd760e3e7b7887668ccfcbf42d9c57b517df26`.
- FACT: result was `USERSPACE`; Android ADB appeared as normal `device`.
- FACT: forced debug properties were live in Android userspace:
  `ro.secure=0`, `ro.adb.secure=0`, `ro.debuggable=1`,
  `ro.force.debuggable=1`, and `persist.sys.usb.config=adb`.
- FACT: `adb shell id` returned root context `u:r:su:s0`.
- FACT: `/cache/nx549j-bootdiag/first_stage.log` was present and contained
  `first-stage-start`, `after-load-kernel-modules`,
  `before-do-first-stage-mount`, and `after-do-first-stage-mount`.
- FACT: dmesg shows second-stage init loaded `/debug_ramdisk/adb_debug.prop`.
- FACT: Android services reached at least zygote, SurfaceFlinger, bootanim,
  vold, netd, and root adbd. `sys.boot_completed` was still empty during the
  short capture window, with several vendor HAL services restarting.
- FACT: `frgmark_late_init` returned early before normal FRGmark iomap was
  prepared, so `/proc/frgmark_userspace_ack` was missing.
- FACT: BCB stayed sticky with `reason=userspace-reached`; the device later
  returned to recovery from the old timeout path.
- INFERENCE: attempt109 proves the kernel can boot Android userspace with ADB,
  and proves the old "no secure 0 / no forced adb" hypothesis. It also exposes
  a diagnostic bug: successful userspace handoff must disarm/clear BCB without
  waiting for a proc ACK that does not exist yet.

2026-05-30 attempt110 userspace auto-ack:

- Patch category: DIAGNOSTIC / STABILITY after userspace hit.
- Runtime status: built and verified. Flash/capture is next.
- Attempt110 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt110-20260530-userspace-auto-ack`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt110-20260530-userspace-auto-ack/boot-userspace-auto-ack-120s.img`.
- SHA-256:
  `9e638a59b68a6082a5249df8c7e307911f1db8354140717eb4a63f4ade1871b3`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostic strings, no-BCB no-loop
  gate, pstore config, serial early console config, and ramoops DTB.
- Source changes:
  - `arch/arm64/kernel/frgmark.c` now auto-calls
    `frgmark_userspace_ack("userspace-reached")` when userspace exec succeeds.
  - `init/main.c` now calls `frgmark_prepare_post_init()` before
    `do_basic_setup()`, allowing `frgmark_late_init` to create the proc ACK
    hook instead of returning early.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now requires the auto-ack/disarm and BCB-clear strings.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt110*.sh` and latest
    runner scripts target the exact attempt110 image.
- Expected runtime evidence:
  - Android ADB should return as `device`, with the same forced debug props as
    attempt109.
  - Dmesg should show `userspace exec reached, auto-acking recovery timeout`,
    `userspace ack reason=userspace-reached`, and `BCB command cleared`.
  - Live `misc` first page should not contain `boot-recovery` after userspace
    is reached.
- Flash command:
  `ADB_HOST=127.0.0.1 ADB_PORT=15038 ADB_PORTS=15038 SERIAL=30785d1a /srv/forge/android/nx549j/scripts/nx549j-run-attempt110.sh`.

2026-05-30 attempt111 MDSS SMMU CB21 scanout diagnostic:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for physical display scanout.
- Runtime status: built and verified. Flash/capture is next.
- Attempt114 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt114-20260530-skip-wfd-lcd-backlight`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt114-20260530-skip-wfd-lcd-backlight/boot-skip-wfd-lcd-backlight-120s.img`.
- Boot SHA-256:
  `50af3d5c3636874a26cc20911450606fe3523f9bda940483cbbb16ba86bdda9a`.
- Image.gz-dtb SHA-256:
  `ffac929a2e4713bd502f355abe5dec4b432a662b9929707a9e35664dfa022122`.
- vmlinux SHA-256:
  `50100d492508a3302c8c77004e87aa1e42084e77dc28ce9eefc69e25066bd740`.
- System.map SHA-256:
  `bdc826f7f77af4f4e448b4658a5da5af8c8c5f029f4ee6ed49dc0ea2d8c32ac2`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostics, no-BCB gate, pstore
  config, serial early console config, and ramoops DTB.
- FACT: `unpack_bootimg.py` confirmed the boot header still uses the attempt113
  legacy layout: header version 0, page size 2048, kernel load
  `0x80008000`, ramdisk load `0x81000000`, tags `0x80000100`, OS version
  `11.0.0`, and patch level `2024-02`.
- FACT: `scripts/nx549j-run-attempt114.sh`,
  `scripts/nx549j-run-attempt114-unattended.sh`, and latest runner aliases now
  target the exact attempt114 boot SHA above.
- Live evidence before this patch:
  - FACT: Android userspace reached SetupWizard and `sys.boot_completed=1`.
  - FACT: `screencap` after wake showed the Lineage SetupWizard UI, while the
    physical panel still showed only the bootloader logo.
  - FACT: `dmesg` repeatedly showed
    `mdss_smmu_attach_v2: iommu attach device failed for domain[0] with err:-22`
    and `mdss_mdp_overlay_kickoff: iommu attach failed rc=-22`.
  - FACT: TrustZone SMMU programming warned
    `Format change failed for CB 21 with ret -22`.
  - FACT: `/d/mdp/stat` reported `intf2: play: 00000000`, so SurfaceFlinger
    composition was alive but MDP never played frames to the panel.
- Reference check:
  - FACT: local msm8953 references under
    `/srv/forge/work/nx549j-reference-audit-20260528/refs` do not show a
    different `msm_tz_smmu.c` or MDSS SMMU DTS binding for this path.
  - INFERENCE: this is likely a bootloader/TZ/static-context-bank mismatch on
    NX549J rather than an obvious copied DTS typo.
- Source change:
  - `drivers/soc/qcom/msm_tz_smmu.c` now tolerates only
    `TZ_DEVICE_APPS + CB21 + -EINVAL` from
    `SMMU_CHANGE_PAGETABLE_FORMAT`, logs
    `NX549J: ignoring TZ APPS CB21 format failure`, and returns success.
  - Other TrustZone SMMU format failures still warn and remain fatal.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now requires the diagnostic marker string in `vmlinux`.
- Expected runtime evidence:
  - `dmesg` should contain the new `NX549J: ignoring TZ APPS CB21 format
    failure` marker and should no longer contain MDSS SMMU attach `-22` for
    the display path.
  - `/d/mdp/stat` should show `play` advancing above `0` if this was the
    blocking scanout failure.
  - If `play` advances but the user still sees bootlogo, the next blocker is
    likely panel command/DSI/fb-index handoff rather than SurfaceFlinger.
- Rollback condition:
  - Revert this diagnostic tolerance if it introduces SMMU faults, memory
    corruption, earlier boot failure, or proves unrelated after `play` remains
    `0` and the same MDSS attach errors persist.

Runtime result:

- FACT: attempt111 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt111-20260530-mdss-smmu-cb21-scanout`.
- FACT: boot image SHA-256:
  `59228916ce5d414ed696da8d33a785d176322070a6991660bde1fbe4707eb225`.
- FACT: runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt111-20260530-mdss-smmu-cb21-scanout/runtime/flash-boot-bcb-20260530-203005`.
- FACT: late recovery evidence directory:
  `/srv/forge/work/nx549j-preserve/release-attempt111-20260530-mdss-smmu-cb21-scanout/runtime/flash-boot-bcb-20260530-203005/after-late-recovery`.
- FACT: recovery capture boot prefix SHA matched attempt111:
  `59228916ce5d414ed696da8d33a785d176322070a6991660bde1fbe4707eb225`.
- FACT: pstore confirmed userspace auto-ack and BCB clear:
  `FRGmark stage=15 name=userspace_reached`,
  `userspace exec reached, auto-acking recovery timeout`,
  `FRGmark stage=17 name=userspace_ack`,
  `BCB command cleared reason=userspace-ack`, and
  `recovery timeout disarmed`.
- FACT: the new CB21 diagnostic marker appeared:
  `NX549J: ignoring TZ APPS CB21 format failure ret -22 for MDSS scanout diagnostic`.
- FACT: after that, MDSS hit a later SMMU/scanout failure:
  `mdss_smmu_fault_handler: mdss_smmu: iova:0x8807000 flags:0x25`,
  `Unhandled context fault: iova=0x08807000 ... cb=21`,
  `kernel BUG at drivers/iommu/arm-smmu.c:1552`, and
  `Kernel panic - not syncing: mdss_mdp_wait_for_xin_halt`.
- INFERENCE: the CB21 TZ tolerance advanced the display path past the previous
  attach `-22` blocker. The next diagnostic blocker is that the first MDSS
  scanout translation fault and follow-up XIN halt timeout force a panic before
  live Android evidence can be collected.

2026-05-30 attempt112 MDSS SMMU nonfatal + XIN no-panic diagnostic:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for live display evidence.
- Runtime status: built and verified. Flash/capture is next.
- Attempt112 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt112-20260530-mdss-smmu-nonfatal-xin-no-panic`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt112-20260530-mdss-smmu-nonfatal-xin-no-panic/boot-mdss-smmu-nonfatal-xin-no-panic-120s.img`.
- Boot SHA-256:
  `05f426d457f1731eb7f4dce64bea0aea23c69c2b73b16a8bb14d4dd59a82279c`.
- Image.gz-dtb SHA-256:
  `23ea8b9ce3d8e82760023fc88407d39aae63cfe4b7fb94a31af2a24210365c75`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostics, no-BCB gate, pstore
  config, serial early console config, and ramoops DTB.
- Source changes:
  - `drivers/video/fbdev/msm/mdss_smmu.c` sets
    `DOMAIN_ATTR_NON_FATAL_FAULTS` on MDSS SMMU mappings and logs
    `NX549J: MDSS SMMU non-fatal faults enabled`.
  - `drivers/video/fbdev/msm/mdss_mdp.c` keeps the VBIF/XIN timeout register
    dump but removes the `panic` token from that one timeout handler and logs
    `NX549J: MDSS xlog suppressed panic for VBIF XIN halt timeout`.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now requires both new MDSS marker strings.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt112*.sh` and latest
    runner scripts target the exact attempt112 image.
- Expected next marker:
  - No `Kernel panic - not syncing: mdss_mdp_wait_for_xin_halt`.
  - Dmesg should contain `NX549J: MDSS SMMU non-fatal faults enabled` and, if
    the XIN timeout repeats, `NX549J: MDSS xlog suppressed panic`.
  - If Android survives, collect `/d/mdp/stat`, `/d/mdp/xlog/dump`, dmesg,
    logcat, framebuffer/display service state, and user-visible panel status.
- Rollback condition:
  - Revert this diagnostic if nonfatal MDSS SMMU fault handling causes a new
    earlier boot failure, repeated IRQ storms that kill userspace, memory
    corruption, or hides the fault without leaving useful MDSS evidence.
- Verification commands:
  - `ADB_HOST=127.0.0.1 ADB_PORT=15038 ADB_PORTS=15038 SERIAL=30785d1a /srv/forge/android/nx549j/scripts/nx549j-run-attempt112.sh`
  - `rg -n "NX549J: MDSS SMMU non-fatal|NX549J: MDSS xlog suppressed|arm-smmu|mdss_smmu|mdss_mdp_wait_for_xin_halt|Kernel panic" <capture>/dmesg.txt <capture>/pstore-cat.txt`
  - `adb -H 127.0.0.1 -P 15038 -s 30785d1a shell 'cat /d/mdp/stat; cat /d/mdp/xlog/dump | tail -200'`

Runtime result:

- FACT: attempt112 runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt112-20260530-mdss-smmu-nonfatal-xin-no-panic/runtime/flash-boot-bcb-20260530-222744`.
- FACT: boot partition prefix SHA-256 matched attempt112:
  `05f426d457f1731eb7f4dce64bea0aea23c69c2b73b16a8bb14d4dd59a82279c`.
- FACT: the runner observed Android `device`, but the phone later returned to
  recovery; late recovery evidence directory:
  `/srv/forge/work/nx549j-preserve/release-attempt112-20260530-mdss-smmu-nonfatal-xin-no-panic/runtime/flash-boot-bcb-20260530-222744/after-late-recovery`.
- FACT: user-visible result: panel blinked once, then continued showing the
  bootloader logo; no visible boot animation.
- FACT: pstore confirms Android userspace and boot animation started:
  `BootAnimationShownTiming`, `SurfaceFlinger Enter boot animation`, and HWC
  created display 0.
- FACT: recovery timeout was disarmed correctly:
  `FRGmark stage=15 name=userspace_reached`,
  `FRGmark stage=17 name=userspace_ack`, `BCB command cleared`, and
  `recovery timeout disarmed`.
- FACT: the attempt112 markers fired: `NX549J: MDSS SMMU non-fatal faults
  enabled`, `NX549J: ignoring TZ APPS CB21 format failure`, and
  `NX549J: MDSS xlog suppressed panic for VBIF XIN halt timeout`.
- FACT: the old `mdss_mdp_wait_for_xin_halt` panic was bypassed, but the next
  MDSS command-mode timeout panicked:
  `Kernel panic - not syncing: mdss_mdp_cmd_wait4pingpong`.
- FACT: this occurred with repeated CB21 MDSS SMMU translation faults and
  pingpong/fence timeouts:
  `Unhandled context fault ... cb=21`, `mdp-fence: frame timeout`, and
  `mdss_mdp_cmd_wait4pingpong:wait4pingpong timed out`.
- INFERENCE: display has advanced from attach failure to live HWC/bootanim and
  physical panel power activity, but scanout is still blocked by MDSS SMMU
  translation faults and command-mode pingpong timeout.

2026-05-30 attempt113 MDSS command pingpong no-panic diagnostic:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for live display evidence.
- Runtime status: built and verified. Flash/capture is next.
- Attempt113 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt113-20260530-mdss-cmd-pingpong-no-panic`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt113-20260530-mdss-cmd-pingpong-no-panic/boot-mdss-cmd-pingpong-no-panic-120s.img`.
- Boot SHA-256:
  `7d06df1a669b2e991f3510fcec65815560d1d30928dd810472deb98ab822b522`.
- Image.gz-dtb SHA-256:
  `38d4935139acedfa18a211ef24d481b15261fb10b4755c34f6ae78fc0b46e32f`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostics, no-BCB gate, pstore
  config, serial early console config, and ramoops DTB.
- Source changes:
  - `drivers/video/fbdev/msm/mdss_mdp_intf_cmd.c` keeps command-mode timeout
    xlog/register dumps but removes the `panic` token from pingpong,
    autorefresh-pp, autorefresh-done, and line-out timeout handlers.
  - New marker strings include
    `NX549J: MDSS xlog suppressed panic for command pingpong timeout` plus
    autorefresh and line-out variants.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    requires the command pingpong no-panic marker.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt113*.sh` and latest
    runner scripts target the exact attempt113 image.
- Expected next marker:
  - No `Kernel panic - not syncing: mdss_mdp_cmd_wait4pingpong`.
  - If Android survives longer, collect live `/d/mdp/stat`, `/d/mdp/xlog/dump`,
    SurfaceFlinger/HWC dumps, dmesg/logcat, and framebuffer nodes before the
    next timeout path.
- Rollback condition:
  - Revert this diagnostic if command-mode no-panic causes a hard hang with no
    ADB/pstore gain, masks all useful MDSS fault evidence, or introduces a new
    non-display regression.

Runtime result:

- FACT: attempt113 runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt113-20260530-mdss-cmd-pingpong-no-panic/runtime/flash-boot-bcb-20260530-224820`.
- FACT: live black-screen capture directory:
  `/srv/forge/work/nx549j-preserve/release-attempt113-20260530-mdss-cmd-pingpong-no-panic/runtime/flash-boot-bcb-20260530-224820/live-black-20260530-175634`.
- FACT: local and live boot partition prefix SHA-256 both matched attempt113:
  `7d06df1a669b2e991f3510fcec65815560d1d30928dd810472deb98ab822b522`.
- FACT: Android was alive in the capture: `sys.boot_completed=1`,
  `init.svc.bootanim=stopped`, root ADB worked, and SurfaceFlinger/HWC could
  be dumped.
- FACT: before a wake poke, SurfaceFlinger had internal display power mode
  `Off`, `isEnabled=false`, display power state `OFF`, and `/d/mdp/stat`
  showed `intf2 play=0x0 vsync=0x1795 user_bl=0`.
- FACT: after `svc power stayon true`, wake keyevents, and brightness writes,
  SurfaceFlinger switched to power mode `On`, display power state `ON`, and
  `/d/mdp/stat` advanced to `intf2 play=0x2`, but `user_bl` and
  `/sys/class/leds/lcd-backlight/brightness` stayed `0`.
- FACT: the live `lcd-backlight` symlink pointed at the writeback/WFD
  framebuffer parent:
  `.../qcom,mdss_fb_wfd/leds/lcd-backlight`, not the 1080x1920 panel fb.
- FACT: `/proc/fb` listed fb0 as `mdssfb_a0000` with 640x640 and fb1 as
  `mdssfb_90000` with 1080x1920; runtime logs showed WFD probing before the
  real panel.
- FACT: dmesg/logcat still contained repeated MDSS SMMU CB21 translation
  faults, command/fence timeouts, and
  `mdss_fb_report_panel_dead: Panel has gone bad, sending uevent - PANEL_ALIVE=0`.
- INFERENCE: attempt113 successfully converted the earlier display panic into
  a live Android evidence window. The current visible black-screen symptom is
  at least partly a backlight routing bug: the single Android `lcd-backlight`
  classdev is bound to WFD because WFD probes first, so userspace brightness
  writes do not target the physical panel. The remaining lower-layer blocker is
  still MDSS SMMU/DSI command-mode timeout.

2026-05-30 attempt114 skip WFD lcd-backlight diagnostic:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for physical display backlight
  routing.
- Runtime status: built, verified, flashed, and captured. Backlight routing is
  fixed; physical scanout is still blocked by MDSS/APPS SMMU CB21 faults.
- Source changes:
  - `drivers/video/fbdev/msm/mdss_fb.c` now registers the global
    `lcd-backlight` classdev only for real display panel types
    `MIPI_VIDEO_PANEL`, `MIPI_CMD_PANEL`, `EDP_PANEL`, and `SPI_PANEL`.
  - Non-panel framebuffer types, including WFD/writeback, log
    `NX549J: skipping lcd-backlight for fb...` and leave the single
    backlight slot free for the later physical panel probe.
  - Successful physical-panel registration logs
    `NX549J: registered lcd-backlight for fb...`.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now requires the registration marker in `vmlinux`.
- Expected next marker:
  - `/sys/class/leds/lcd-backlight` should point at the 1080x1920 panel fb
    parent instead of `qcom,mdss_fb_wfd`.
  - A brightness write should no longer read back as permanently `0` solely
    because WFD owns the classdev.
  - If the physical panel remains black, keep chasing the repeated
    `cb=21` SMMU faults, `mdp-fence` timeouts, and `PANEL_ALIVE=0` as the
    next lower-layer display blocker.
- Rollback condition:
  - Revert this diagnostic if `lcd-backlight` disappears completely, the panel
    fb fails to register, userspace boot regresses before SurfaceFlinger/HWC,
    or brightness routing is proven unrelated and the WFD binding is required.

Runtime result:

- FACT: attempt114 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt114-20260530-skip-wfd-lcd-backlight`.
- FACT: boot image SHA-256:
  `50af3d5c3636874a26cc20911450606fe3523f9bda940483cbbb16ba86bdda9a`.
- FACT: live display capture directory:
  `/srv/forge/work/nx549j-preserve/release-attempt114-20260530-skip-wfd-lcd-backlight/runtime/flash-boot-bcb-20260531-023725/live-display-20260531-023913`.
- FACT: Android reached userspace in the live capture: SurfaceFlinger and HWC
  were alive, display power was `ON`, and `sys.boot_completed=1`.
- FACT: `/sys/class/leds/lcd-backlight` now points at the primary panel fb
  parent, not the WFD framebuffer:
  `.../qcom,mdss_fb_primary/leds/lcd-backlight`.
- FACT: `/proc/fb` still listed WFD first as fb0 at 640x640 and the physical
  1080x1920 panel as fb1, but the global `lcd-backlight` classdev was no
  longer stolen by WFD.
- FACT: brightness was writable and readable on the physical panel path:
  `brightness=127`, `max_brightness=255`, and `/d/mdp/stat` showed
  `user_bl=127`.
- FACT: the visible screen remained black because MDSS still hit repeated APPS
  SMMU CB21 scanout faults:
  `mdss_smmu_fault_handler: iova:0x8807000 flags:0x25`,
  `arm-smmu ... Unhandled context fault ... SID=0xc00 ... cb=21`,
  `mdss_mdp_cmd_wait4pingpong:wait4pingpong timed out`, `mdp-fence` timeout,
  and `PANEL_ALIVE=0`.
- INFERENCE: WFD/backlight misbinding was a real bug and is now cleared from
  the primary blocker list. The remaining black-screen root blocker is the
  APPS SMMU static context-bank handoff for MDSS unsecure CB21: software can
  resolve the IOVA, but the hardware translation path faults at scanout.

2026-05-31 attempt115 MDSS APPS CB21 AArch32-LPAE isolation:

- Patch category: DIAGNOSTIC / ISOLATION for the MDSS/APPS SMMU handoff.
- Runtime status: built, verified, flashed, and captured. The CB21 SMMU fault
  pattern is gone in the live capture; if the physical panel is still black,
  the next blocker is below SurfaceFlinger/HWC/MDP scanout, in DSI panel power
  or init.
- Attempt115 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt115-20260531-mdss-cb21-aarch32-lpae`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt115-20260531-mdss-cb21-aarch32-lpae/boot-mdss-cb21-aarch32-lpae-120s.img`.
- Boot SHA-256:
  `9586fa0b9e22e8dde41195511e1f5e7f0978f8c143190e7ae9fdddeffff28acd`.
- Image.gz-dtb SHA-256:
  `4dd561cd8379ee73f256e4dc90b6a1e8fef44adfa48cc59487a2bada553fda9c`.
- vmlinux SHA-256:
  `9ace10a3722ef99649df130e18f4504a932c9362ccfd9a4eb57b30cebc051e0d`.
- DTB SHA-256:
  `0456a1dabcf37e77bcdab206c8e61582b18dcdec12f3880bf9a6af457ac5400d`.
- FACT: `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline, required
  symbols, required marker strings, ramdisk diagnostics, no-BCB gate, pstore
  config, serial early console config, and ramoops DTB.
- Source changes:
  - `drivers/iommu/arm-smmu.c` detects `TZ_DEVICE_APPS` + static CB21 + S1
    MDSS domain allocation and forces that context to `ARM_SMMU_CTX_FMT_AARCH32_L`
    when AArch32 LPAE is supported.
  - The diagnostic caps MDSS CB21 input address size to 32 bits and output
    address size to 40 bits for the matching 32-bit LPAE translation format.
  - The new marker is
    `NX549J: forcing APPS CB21 MDSS context to AArch32 LPAE`.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    requires the new marker, and the latest runner scripts target the exact
    attempt115 boot SHA above.
- Reasoning:
  - FACT: the active DTB maps MDSS unsecure SID `0xc00` through APPS SMMU CB21.
  - FACT: the 3.18 stock/highwaystar SMMU DTS also places `mdp_0` at APPS CB21
    (`0x1e35000` with `qcom,cb-base-offset = <0x20000>`), so the context-bank
    number itself is not random.
  - FACT: TrustZone returned `-22` for the APPS CB21 AArch64 format switch, and
    attempt111+ only tolerated that failure to get farther.
  - INFERENCE: firmware likely leaves CB21 in the 3.18/32-bit LPAE format.
    Masking the TZ error while keeping 4.9's AArch64 software context may leave
    the hardware and software page-table formats mismatched, causing the
    repeated scanout faults.
- Expected next marker:
  - Dmesg should contain
    `NX549J: forcing APPS CB21 MDSS context to AArch32 LPAE`.
  - If the hypothesis is right, the repeated `cb=21` MDSS context faults should
    disappear or change substantially before the next DSI/panel failure.
  - If the screen remains black, collect fresh `/d/mdp/stat`, `/d/mdp/xlog/dump`,
    dmesg, logcat, SurfaceFlinger/HWC dumps, `/proc/fb`, and the
    `lcd-backlight` symlink/brightness values.
- Rollback condition:
  - Revert this diagnostic if it regresses boot before Android userspace,
    prevents MDSS attach, introduces new APPS SMMU faults outside MDSS CB21, or
    leaves the same `cb=21` fault pattern unchanged after a verified attempt115
    boot.

Runtime result:

- FACT: attempt115 runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt115-20260531-mdss-cb21-aarch32-lpae/runtime/flash-boot-bcb-20260531-032131`.
- FACT: live display capture directory:
  `/srv/forge/work/nx549j-preserve/release-attempt115-20260531-mdss-cb21-aarch32-lpae/runtime/flash-boot-bcb-20260531-032131/live-display-20260531-032343`.
- FACT: live boot partition prefix SHA-256 matched attempt115:
  `9586fa0b9e22e8dde41195511e1f5e7f0978f8c143190e7ae9fdddeffff28acd`.
- FACT: Android reached userspace and boot completed:
  `sys.boot_completed=1`, `init.svc.bootanim=stopped`, SurfaceFlinger had an
  enabled internal 1080x1920 display, and HWC display 0 was present.
- FACT: a valid `screencap -p` image was captured at
  `screencap-execout.png`; the compositor content is normal Android lock/home
  UI at 1080x1920.
- FACT: the new marker fired:
  `NX549J: forcing APPS CB21 MDSS context to AArch32 LPAE`.
- FACT: fresh `dmesg-after.txt` and `logcat-all-after.txt` no longer contain
  the old MDSS/APPS CB21 blockers: no `Unhandled context fault`, no
  `mdss_smmu_fault_handler`, no `wait4pingpong`, no `mdp-fence` timeout, and
  no `PANEL_ALIVE=0`.
- FACT: backlight routing stayed fixed: `/sys/class/leds/lcd-backlight` points
  at `qcom,mdss_fb_primary`, and a write/read test changed brightness from
  `14` to `180` with `max_brightness=255`.
- FACT: MDP debugfs shows active scanout rather than a stalled pipe:
  `Control path #0 - MIPI_CMD`, `Panel #0 1080x1920p60`, `Play Count=2`,
  `Underrun Count=0`, four active SSPP pipes, and `domain=mdp_unsecure`.
- FACT: DSI registration still shows suspicious panel-power/init evidence:
  the first DSI probe hit `lab get failed. rc=-517`, then the later probe
  registered the panel with `Continuous splash enabled`; the active panel DTS
  lacks several stock Nubia sideband properties such as CE/CABC commands and
  the Nubia backlight curve.
- INFERENCE: attempt115 likely fixed the IOMMU-format blocker rather than just
  hiding it. If the physical LCD still shows black while screencap/MDP are
  correct, the remaining issue is DSI panel bring-up state: LAB/IBB sequencing,
  reset/on-command timing, or missing Nubia panel extension behavior.

Component sidecar notes for post-display batch:

- FACT: Wi-Fi userspace opens `/dev/wcnss_wlan` and `/dev/wcnss_ctrl`; current
  runtime `Bad address` maps to the WCNSS char device/open path, with firmware
  and `/persist/WCNSS_qcom_wlan_nv.bin` still needing confirmation.
- FACT: BT init expects `/system/vendor/bin/wcnss_filter`, but active vendor
  prebuilts appear to lack `wcnss_filter`; BT protocol config also needs
  generated `.config` review before changing.
- FACT: RIL/vendor packaging appears incomplete for Android 11 radio:
  `qcrild`, `qmuxd`, `port-bridge`, `ipacm`, and several init rc sidecars are
  declared/expected but not present in active vendor prebuilts.
- FACT: Touch DTS node `synaptics_dsx@20` names pinctrl states
  `pmx_ts_int_active` / `pmx_ts_int_suspend` while the active driver expects
  `synaptics_pin_active` / `synaptics_pin_suspend`; first patch should be
  DTS-only after display evidence is stable.
- FACT: Goodix fingerprint kernel/DTS wiring exists, but `fps_hal` is disabled
  with no active start trigger; the first userspace fix should start
  `goodix_script` and `fps_hal` when `sys.fp.vendor=goodix`, then verify the
  selected `fingerprint.default.so` vs Goodix module mapping.

2026-05-31 attempt116 DSI stock vddio/l22 parity diagnostic:

- Patch category: DIAGNOSTIC / ISOLATION for physical DSI panel power wiring
  after SurfaceFlinger/HWC/MDP scanout became healthy.
- Runtime status: built and verified; latest runner scripts target attempt116.
- Attempt116 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt116-20260531-dsi-stock-vddio-l22`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt116-20260531-dsi-stock-vddio-l22/boot-dsi-stock-vddio-l22-120s.img`.
- Boot SHA-256:
  `aa7f48de432b0813526cc0c014075fb7929ae1c418fddf97b61366741943ffe2`.
- Image.gz-dtb SHA-256:
  `e064c8536c950cacab7663687b99776371e5626b3819672f1cdfd08253926c12`.
- DTB SHA-256:
  `caeb703f02f33aff182cc8b1697b725fd5722cc95a8b04e69cb4af382f2ccf08`.
- FACT: stock 3.18 NX549J DTS deletes `vdd-supply` on `mdss_dsi0` and
  `mdss_dsi1`, and sets `vddio-supply = <&pm8953_l22>`.
- FACT: attempt115 live regulator dump showed `pm8953_l6` enabled as the
  inherited 4.9 DSI IO rail while stock `pm8953_l22` stayed disabled.
- Source changes:
  - `arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dts` now deletes inherited
    `vdd-supply` and assigns `vddio-supply = <&pm8953_l22>` for both DSI0 and
    DSI1, matching stock NX549J 3.18.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now decompiles the packaged DTB and fails the release if either DSI node
    still has inherited `vdd-supply` or if `vddio-supply` does not point at
    `regulator-l22`.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt116*.sh` and latest
    runner scripts target the exact attempt116 boot SHA above.
- FACT: attempt116 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  symbols, marker strings, ramdisk diagnostics, no-BCB gate, pstore config,
  serial console config, ramoops DTB, and `DSI supply DTB parity`.
- Expected next marker:
  - Fresh dmesg should still contain
    `NX549J: forcing APPS CB21 MDSS context to AArch32 LPAE`.
  - Regulator runtime evidence should show `pm8953_l22` enabled during panel
    bring-up, with no regression to old MDSS SMMU context faults.
  - If physical LCD remains black while `screencap`, HWC, MDP, and backlight
    are healthy, continue to stock panel reset/on-command timing and Nubia
    panel sideband properties.
- Rollback condition:
  - Revert this diagnostic if the DSI controller probe fails earlier than
    attempt115, Android no longer reaches userspace, the panel loses LAB/IBB
    rails, or runtime evidence proves the stock l22 wiring is not used by this
    hardware revision.

Runtime result:

- FACT: attempt116 runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt116-20260531-dsi-stock-vddio-l22/runtime/flash-boot-bcb-20260531-034016`.
- FACT: live display capture directory:
  `/srv/forge/work/nx549j-preserve/release-attempt116-20260531-dsi-stock-vddio-l22/runtime/flash-boot-bcb-20260531-034016/live-display-20260531-034250`.
- FACT: live boot partition prefix SHA-256 matched attempt116:
  `aa7f48de432b0813526cc0c014075fb7929ae1c418fddf97b61366741943ffe2`.
- FACT: attempt116 regressed before normal display registration:
  `pm8953_l22: requested voltage range [1800000, 1800000] does not fit within
  constraints: [2800000, 2850000]`, followed by
  `vddio set vltg fail` and `mdss_dsi_ctrl0 failed with error -22`.
- FACT: `sys.boot_completed` stayed empty, SurfaceFlinger was `restarting`,
  `screencap-execout.png` was empty, and `/sys/class/leds/lcd-backlight`
  was absent in the attempt116 capture.
- INFERENCE: DSI controller phandle parity alone is incomplete in 4.9. If the
  stock L22 rail is used, the NX549J panel-supply voltage table must also be
  made compatible with L22's 2.8-2.85 V regulator constraints.

2026-05-31 attempt117 DSI L22 voltage-table diagnostic:

- Patch category: DIAGNOSTIC / ISOLATION for the same physical-panel power
  hypothesis, now including the voltage table required for 4.9 regulator
  validation.
- Runtime status: built and verified; latest runner scripts target attempt117.
- Attempt117 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt117-20260531-dsi-l22-voltage-table`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt117-20260531-dsi-l22-voltage-table/boot-dsi-l22-voltage-table-120s.img`.
- Boot SHA-256:
  `f64e65f7a298ba3befdecc812351cdb023644fa900e5bc086f5e8fa6bdff7938`.
- Image.gz-dtb SHA-256:
  `ec67afb48c722e5ba4a56761b5473508c836767021c768a4139bc5bd3ca8736d`.
- DTB SHA-256:
  `c3dea165f9877a4ab5ec1f530ebd9546981e40755c95cde184353050a825a237`.
- Source changes:
  - `arch/arm64/boot/dts/qcom/msm8953-mdss-panels-nx549j.dtsi` sets the
    NX549J JDI `vddio` panel supply range to `2800000..2850000`, matching
    `pm8953_l22` constraints in this 4.9 tree.
  - `/srv/forge/android/nx549j/scripts/nx549j-verify-release-artifact.sh`
    now fails the release unless the compiled DTB has DSI0/DSI1 on L22 and
    NX549J panel entry@0 voltage range `2800000..2850000`.
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt117*.sh` and latest
    runner scripts target the exact attempt117 boot SHA above.
- FACT: attempt117 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  symbols, marker strings, ramdisk diagnostics, no-BCB gate, pstore config,
  serial console config, ramoops DTB, and `DSI supply DTB parity`.
- Expected next marker:
  - DSI probe should no longer fail at `vddio set vltg fail`.
  - If panel registration succeeds, collect whether `pm8953_l22`, LAB, and IBB
    are enabled and whether `screencap`, SurfaceFlinger, MDP, and physical
    panel behavior match or diverge.
- Rollback condition:
  - Revert attempt117 if it still fails at DSI regulator setup, regresses
    before userspace compared with attempt115, or runtime proves the hardware
    needs the old L6 1.8 V path rather than L22.

Runtime result:

- FACT: attempt117 runtime flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt117-20260531-dsi-l22-voltage-table/runtime/flash-boot-bcb-20260531-035218`.
- FACT: live display capture directory:
  `/srv/forge/work/nx549j-preserve/release-attempt117-20260531-dsi-l22-voltage-table/runtime/flash-boot-bcb-20260531-035218/live-display-20260531-035348`.
- FACT: live boot partition prefix SHA-256 matched attempt117:
  `f64e65f7a298ba3befdecc812351cdb023644fa900e5bc086f5e8fa6bdff7938`.
- FACT: Android reached normal userspace: `sys.boot_completed=1`,
  `dev.bootcomplete=1`, `init.svc.bootanim=stopped`, and
  `init.svc.surfaceflinger=running`.
- FACT: `screencap-execout.png` is a valid 1080x1920 PNG showing the Android
  launcher/loading UI.
- FACT: the attempt116 regulator failure is gone on the successful second DSI
  probe. The first early probe still logs transient `lab get failed. rc=-517`,
  but the later probe reaches
  `mdss_dsi_ctrl_probe: Dsi Ctrl->0 initialized, DSI rev:0x10040002, PHY rev:0x2`.
- FACT: `pm8953_l22` is enabled with two users at 2800 mV, and debugfs shows
  `1a94000.qcom,mdss_dsi_ctrl0-vddio` under regulator-l22. LAB and IBB are
  also enabled at 5500 mV.
- FACT: backlight remains correctly routed to the primary physical panel fb:
  `/sys/class/leds/lcd-backlight -> ...qcom,mdss_fb_primary/leds/lcd-backlight`,
  with brightness `180` and max `255`.
- FACT: MDP/SF state is healthy: SurfaceFlinger display 0 is enabled and
  `powerMode=On`, display service reports the internal 1080x1920 display
  `state ON`, `/d/mdp/stat` has `play=2`, `underrun=0`, `user_bl=180`, and
  `/d/mdp/dump` shows active MIPI_CMD scanout.
- FACT: fresh dmesg/logcat retain the CB21 format marker:
  `NX549J: forcing APPS CB21 MDSS context to AArch32 LPAE`.
- INFERENCE: attempt117 closes the DSI regulator mismatch introduced by
  attempt116 and preserves the attempt115 SMMU/backlight fixes. If the
  physical LCD is still black while the capture is this healthy, the next
  display blocker is likely panel command/init parity or Nubia-specific panel
  extension behavior, not APPS SMMU, generic MDP scanout, or regulator
  enablement.

2026-05-31 attempt118 touch pinctrl and userspace component batch:

- Patch category: PROPER-FIX for Synaptics pinctrl naming; PROPER-FIX /
  packaging repair for userspace radio/qrtr/sensor sidecars that were missing
  or zero-byte in the active runtime.
- Kernel GitHub branch:
  `https://github.com/nomorecoolnicknames/android_kernel_nubia_msm8953/tree/nx549j-port-4.9`.
- Kernel checkpoint:
  `ce2332fcc nx549j: fix synaptics pinctrl states`.
- Attempt118 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt118-20260531-touch-pinctrl`.
- Boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt118-20260531-touch-pinctrl/boot-touch-pinctrl-120s.img`.
- Boot SHA-256:
  `63e35205c64fff9b50aee7c097b400cff5b56a52576d81fdcd49ef3c7fa9ce52`.
- FACT: attempt118 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  symbols, marker strings, ramdisk diagnostics, no-BCB gate, pstore config,
  serial console config, ramoops DTB, and DSI supply parity.
- FACT: packaged DTB contains `synaptics_dsx@20` with
  `pinctrl-names = "synaptics_pin_active", "synaptics_pin_suspend"` and each
  state now includes the IRQ, reset, and power pinctrl groups expected by the
  active 4.9 Synaptics DSX driver.
- Evidence for userspace component repair:
  `/srv/forge/work/nx549j-preserve/release-attempt117-20260531-dsi-l22-voltage-table/runtime/userspace-components-20260531-live`.
- FACT: attempt117 userspace reached boot complete and display scanout was
  healthy, so non-display component failures are now actionable.
- FACT: runtime `/vendor/firmware_mnt` was mounted from the modem partition
  but lacked `image/wcnss.mdt`; WCNSS Wi-Fi/BT firmware loading failed before
  normal WLAN/BT bring-up.
- FACT: runtime lacked real `qcrild`, `qrtr-ns`, `port-bridge`, and
  `sensors.qti` binaries; the source vendor package also lacked those blobs
  before this batch.
- FACT: TheMuppets Xiaomi lineage-18.1 blob sources provided SHA1-matching
  RIL blobs already named in `device/nubia/nx549j/proprietary-files.txt`:
  `qcrild` `c0db5589a45874c31c0bb7e8a7151511505c1e04`,
  `qcrild.rc` `f9c32f5e7a8e7f4a0df1dcce174de169b8d7c060`,
  `libqcrilFramework.so` `3d8f25d13533f6fa45a36b23feb250291b663370`,
  `libril-qc-hal-qmi.so` `bf1a176d3890eaba94a115a311f98efe1010aeed`,
  and `qcrild_librilutils.so` `c23470619690a34f7442fb6b26345de5dc4de880`.
- Source/package changes outside the kernel git repo:
  - `vendor/nubia/msm8953-common/proprietary/vendor/bin/qrtr-ns`
  - `vendor/nubia/msm8953-common/proprietary/vendor/bin/port-bridge`
  - `vendor/nubia/msm8953-common/proprietary/vendor/bin/sensors.qti`
  - `vendor/nubia/msm8953-common/proprietary/vendor/etc/init/port-bridge.rc`
  - `vendor/nubia/nx549j/proprietary/vendor/bin/hw/qcrild`
  - `vendor/nubia/nx549j/proprietary/vendor/etc/init/qcrild.rc`
  - `vendor/nubia/nx549j/proprietary/vendor/lib64/libqcrilFramework.so`
  - `vendor/nubia/nx549j/proprietary/vendor/lib64/libril-qc-hal-qmi.so`
  - `vendor/nubia/nx549j/proprietary/vendor/lib64/qcrild_librilutils.so`
  - `vendor/nubia/msm8953-common/msm8953-common-vendor.mk`
  - `vendor/nubia/nx549j/nx549j-vendor.mk`
  - `device/nubia/msm8953-common/rootdir/etc/init.qcom.rc`
- FACT: `device/nubia/msm8953-common/rootdir/etc/init.qcom.rc` no longer
  starts the stale `vendor.ril-daemon2 /vendor/bin/hw/rild -c 2`; the copied
  `qcrild.rc` supplies `vendor.qcrild`, `vendor.qcrild2`, and disabled
  `vendor.qcrild3`.
- FACT: `TARGET_COPY_OUT_VENDOR=system/vendor`; this tree has no separate
  `vendor.img`, so these component fixes require a fresh `system.img` or full
  ROM package, not only a boot flash.
- FACT: during the 2026-05-31 `mka systemimage -j1` run, build output copied
  non-zero `qcrild`, `qrtr-ns`, `port-bridge`, `sensors.qti`,
  `qcrild.rc`, `libqcrilFramework.so`, `libril-qc-hal-qmi.so`, and
  `qcrild_librilutils.so` into `out/target/product/nx549j/system/vendor`.
- Expected next marker:
  - After flashing attempt118 boot plus the matching freshly built system
    image/full package, touch input should create a Synaptics input device or
    move to a concrete I2C/firmware/power error.
  - Radio should no longer fail at missing `qcrild` / missing
    `android.hardware.radio@1.4::IRadio/slot1` service startup due absent
    daemon binaries.
  - QRTR, port bridge, and sensors should no longer be zero-byte exec failures.
  - Wi-Fi/BT will still need a valid stock NX549J modem/NON-HLOS firmware
    mount containing `/vendor/firmware_mnt/image/wcnss.mdt`; do not fake this
    by copying `wcnss.mdt` into the normal vendor filesystem.
- Rollback condition:
  - Revert the Synaptics DTS pinctrl change if the touch controller regresses
    from probe-visible to absent for reasons unrelated to pinctrl lookup.
  - Revert the RIL/blob wiring only if fresh boot logs show these Q RIL blobs
    are ABI-incompatible with the active userspace; do not restore the old
    zero-byte or missing executables.

Final packaged artifact for this batch:

- Attempt119 release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt119-20260531-ril-qrtr-systemimage`.
- Verified boot image:
  `/srv/forge/work/nx549j-preserve/release-attempt119-20260531-ril-qrtr-systemimage/boot-ril-qrtr-system-120s.img`.
- Boot SHA-256:
  `dd09e2ceb5174c05845754e51c6f4918abba5ebacb87c33ea8121b122dd25419`.
- OTA zip:
  `/srv/forge/work/nx549j-preserve/release-attempt119-20260531-ril-qrtr-systemimage/lineage-18.1-20260531-UNOFFICIAL-nx549j-ril-qrtr.zip`.
- OTA SHA-256:
  `589a22fc9cf2020b69795b7fd92dbef5b2ab92f2f70ff6f9541272673e5b972e`.
- Sparse system image:
  `/srv/forge/work/nx549j-preserve/release-attempt119-20260531-ril-qrtr-systemimage/system-ril-qrtr.img`.
- System image SHA-256:
  `94a1db5ac6b649ebafa1027469da9bb23363ec0834998f9e415d2b94787cc0f1`.
- FACT: `mka systemimage -j1` and `mka otapackage -j1` completed
  successfully on 2026-05-31.
- FACT: attempt119 `VERIFY.md` reports PASS for SHA256SUMS, boot cmdline,
  symbols, marker strings, ramdisk diagnostics, no-BCB gate, pstore config,
  serial early console config, ramoops DTB, and DSI supply parity.
- Flashing note: `system-ril-qrtr.img` is Android sparse. Do not write it with
  plain recovery `dd` unless first converted with `simg2img`; the OTA zip is
  the safer artifact for full userspace component testing.

2026-05-31 4PDA NON-HLOS and runtime vendor follow-up:

- FACT: 4PDA official firmware topic `782268`, post `58717801`
  (`https://4pda.to/forum/index.php?showtopic=782268&view=findpost&p=58717801`)
  publishes NX549J radio packages for TWRP. The latest global package checked
  in this pass is `Radio_NX549J_update_V2.04EN_Crocodil.zip`.
- Local 4PDA radio package:
  `/srv/forge/work/nx549j-preserve/4pda-radio/Radio_NX549J_update_V2.04EN_Crocodil.zip`.
- Radio package SHA-256:
  `8e4e987ed411538df01deac5be37cf43c97600637ad67e345a562a3b8e178770`.
- Extracted NON-HLOS:
  `/srv/forge/work/nx549j-preserve/4pda-radio/v2.04en/NON-HLOS.bin`.
- NON-HLOS SHA-256:
  `b18a12d7bdf1a9b8bc451227a1566a2938784b0d1569839c7776749916d270e2`.
- FACT: the full 4PDA radio zip writes `aboot`, `rpm`, `tz`, `cmnlib`,
  `modem`, `lksecapp`, `cmnlib64`, `dsp`, `keymaster`, `devcfg`, and `sbl1`.
  Only `NON-HLOS.bin` was flashed in this pass; the bootloader chain was not
  overwritten.
- Modem backup before 4PDA flash:
  `/srv/forge/work/nx549j-preserve/4pda-radio/modem-before-4pda-v2.04en-30785d1a.img`.
- Modem backup SHA-256:
  `8efe995f781773479193a4b56b7fc316a956ee00f3383b0e410f68864fa06a95`.
- FACT: `/dev/block/bootdevice/by-name/modem` size was `88080384`, matching
  `NON-HLOS.bin`, and recovery readback after `dd` matched the NON-HLOS
  SHA-256 `b18a12d7bdf1a9b8bc451227a1566a2938784b0d1569839c7776749916d270e2`.
- FACT: the 4PDA NON-HLOS FAT image contains `image/wcnss.mdt`,
  `image/modem.mdt`, WCNSS segment files, modem segment files, and
  `verinfo/ver_info.txt`; extracted `ver_info.txt` reports
  `MSM8953.LA.1.0-01075-STD.PROD-3`.
- FACT: after booting Android, raw reads from the modem partition show the
  `IMAGE` and `VERINFO` FAT root entries, and a loop mount of
  `/cache/NON-HLOS-v2.04EN.bin` shows `/image/wcnss.mdt`.
- FACT: runtime mount table contained both
  `/dev/block/mmcblk0p1 on /system/vendor/firmware_mnt type vfat` and a later
  `overlay on /system/vendor type overlay`; path lookup through
  `/vendor/firmware_mnt` was empty until the NON-HLOS image was loop-mounted
  over `/vendor/firmware_mnt` after boot.
- INFERENCE: the current WCNSS firmware blocker is not missing stock firmware
  anymore. The modem partition contains the expected files, but the parent
  `/system/vendor` overlay hides the nested first-stage `firmware_mnt` mount.
- Required next fix: make the modem firmware mount visible after vendor
  overlay setup, either by moving/remounting the modem mount to a later init
  phase on `/vendor/firmware_mnt` or by disabling the vendor overlay for this
  bring-up image. Do not install the full 4PDA radio zip unless the bootloader
  chain update is explicitly intended.
- FACT: attempt119 OTA updater-script only updates
  `/dev/block/bootdevice/by-name/system` and `boot`; it does not update the
  separate runtime `/dev/block/bootdevice/by-name/oem` partition mounted at
  `/system/vendor`.
- FACT: live `/vendor/bin/qrtr-ns` and `/vendor/bin/sensors.qti` were still
  zero-byte files after attempt119 artifact creation, while the matching local
  build output under `out/target/product/nx549j/system/vendor/bin/` contained
  valid non-zero AArch64 ELFs.
- INFERENCE: the qcrild/qrtr/sensors userspace batch is not proven bad. It is
  currently blocked by the device's separate `oem` vendor partition hiding the
  rebuilt `system/vendor` payload from the system image/OTA.
- Required next fix: produce and flash a targeted `oem`/runtime vendor update
  or otherwise make the rebuilt vendor payload land on the actual
  `/dev/block/bootdevice/by-name/oem` filesystem. The full rebuilt
  `system/vendor` tree is larger than the 256 MiB `oem` partition, so a blind
  raw copy is not valid.

2026-05-31 attempt128 runtime codec/userspace follow-up:

- Active boot image before this pass:
  `/srv/forge/work/nx549j-preserve/release-attempt128-20260531-audio-max989-undef/boot.img`.
- Active boot SHA-256:
  `f5d8a1393c95662bcdf638cc92e98165befc5cd985579cc7fe647ec8c1d774cb`.
- Runtime capture:
  `/srv/forge/work/nx549j-preserve/release-attempt128-20260531-audio-max989-undef/runtime-20260531-172002`.
- Fresh codec capture:
  `/srv/forge/work/nx549j-preserve/release-attempt128-20260531-audio-max989-undef/runtime-20260531-172002/scrcpy-codec-20260531-173357`.
- FACT: user reported Windows `scrcpy` now fails after server upload with
  `Capture/encoding error: java.lang.IllegalStateException: null`, then retries
  with lower `-m1600`.
- FACT: `dumpsys media.metrics` records repeated failures of
  `OMX.qcom.video.encoder.avc` at scrcpy-like `bitrate=8000000` for
  1080x1920, 904x1600, 720x1280, 576x1024, and 448x800. Each failure carries
  `errcode=-2147479543` and `errstate=STARTED`.
- FACT: a direct device-side `screenrecord --verbose --time-limit 3
  /sdcard/nx549j-screenrecord-codec-test.mp4` completed at 1080x1920/20 Mbps
  and produced `/sdcard/nx549j-screenrecord-codec-test.mp4`, but only recorded
  8 frames in 3 seconds.
- FACT: VIDC/OMX kernel logs during the failure include
  `HFI_EVENT_SESSION_ERROR`, `HFI_EVENT_SYS_ERROR`,
  `Unknown session error (-1879048190)`, buffer return error `1006`, and a
  Venus firmware fatal message from `VIDEO.VE.4.2.1-00008`.
- FACT: boot logs also show `batched_hyp_assign: Failed to assign memory
  protection, ret = -5` around secure VIDC context-bank setup.
- INFERENCE: hardware AVC encode is not completely absent because
  `screenrecord` can create a file, but the VIDC low-latency/repeated session
  path used by scrcpy is unstable and falls into a Qualcomm OMX hardware error
  before usable frames are delivered.
- HYPOTHESIS: VIDC context-bank / secure memory assignment mismatch between
  the 4.9 DTS and NX549J firmware may be part of the encoder instability. Do
  not replace the VIDC DTS with the old 3.18 `qcom,vidc-iommu-domains` format
  without also proving that the matching 4.9 SMMU nodes exist or adding a
  narrow diagnostic.
- FACT: live runtime still misses blobs that the local build output expects
  under the active `/system/vendor`/`oem` vendor filesystem:
  `vendor.qti.hardware.perf@1.0.so`, `lib-rtpdaemoninterface.so`,
  `com.quicinc.cne.api@1.0.so`, `com.quicinc.cne.api@1.1.so`,
  `vendor.qti.hardware.perf@1.0-service`, and its init rc.
- FACT: logs show corresponding linker failures for `ims_rtp_daemon`,
  `imsdatadaemon`, and perf clients; these are live vendor payload mismatch
  evidence, not missing source entries, because the blobs exist in
  `vendor/nubia/msm8953-common/proprietary/vendor`.
- FACT: live `/cache` has only about 6.5 MiB free, but the confirmed missing
  perf/CNE/RTP blobs are small enough for a temporary overlay-upper test.
- Planned live test: push the missing small blobs into
  `/cache/overlay/vendor/upper`, fix permissions, reboot, then retest the
  codec path and linker spam. This is a diagnostic overlay test, not a
  substitute for producing the correct runtime `oem` vendor update.
- FACT: before the overlay push could start, the target `30785d1a` went
  `offline` and then disappeared from the reverse ADB server. Other connected
  devices must not be touched.

2026-05-31 pending attempt129 camera DTS batch:

- Patch category: PROPER-FIX for camera DTS parity with NX549J stock sensor
  board wiring.
- Kernel source changes:
  - `arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dts` now includes
    `nx549j/msm8953-camera-sensor-nx549j.dtsi`.
  - `arch/arm64/boot/dts/qcom/nx549j/msm8953-camera-sensor-nx549j.dtsi`
    removes generic MTP camera flash/sensors and adds NX549J-specific rear and
    front camera nodes with stock GPIO/regulator/CSI mount-angle data.
- Expected next marker after booting an image containing this DTS:
  camera services should move from generic MTP sensor/power assumptions to
  NX549J-specific regulator/GPIO probe results. Any remaining failure should
  reference concrete sensor identity, regulator, CCI, CSIPHY, or actuator/eeprom
  errors rather than duplicate generic MTP camera nodes.
- Rollback condition:
  revert the NX549J camera include only if boot logs prove it regresses kernel
  init or camera probe before the previous generic MTP behavior.

2026-06-01 attempt130 real vendor image and minimal oem resize:

- Patch category: PROPER-FIX for runtime vendor payload delivery.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt130-20260601-real-vendor-repartition`.
- Built artifacts:
  - boot image:
    `boot-real-vendor-repartition.img`
    SHA-256 `684f7734e96b8229da8a60b3b38b0adf0882351f61aa239cbf5c7e3cc6403ff9`.
  - sparse vendor image:
    `vendor-real-vendor-repartition.img`
    size `267100308`
    SHA-256 `9318f2f8326e9312c59d2a975f621cdbf9f4025c75483907686fbfd3d55dece0`.
  - raw vendor image for recovery `dd`:
    `vendor-real-vendor-repartition.raw.img`
    size `300384256`
    SHA-256 `f1d82b85e4faadea41ac2335bec881ae545d6cdbbc0084fc3912f29ffde0a4cd`.
  - recovery image:
    `recovery-real-vendor-repartition.img`
    SHA-256 `f88c2d535e15294dcfdb967aa9bb4af3019640d9165f627a8d94bf9d15d65183`.
- Source changes in this attempt:
  - `device/nubia/nx549j/BoardConfig.mk` enables real vendor image output and
    sets `BOARD_VENDORIMAGE_PARTITION_SIZE := 300384256`.
  - `vendor/nubia/msm8953-common/proprietary/priv-app/ims/ims.apk` was rebuilt
    with required `uses-library` `ims-ext-common`.
  - `vendor/nubia/msm8953-common/Android.mk` sets
    `LOCAL_USES_LIBRARIES := ims-ext-common` only for the `ims` module.
  - `device/nubia/nx549j/device.mk` copies `sensors/hals.conf` to
    `$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf`.
  - `device/nubia/msm8953-common/msm8953.mk` now packages
    `android.hardware.sensors@1.0-service.nubia8953`.
  - `arch/arm64/boot/dts/qcom/msm8953-mtp-nx549j.dts` no longer directly
    includes generic `msm8953-camera-sensor-mtp.dtsi`; the generic include is
    already inherited through `msm8953-mtp.dtsi`, and the NX549J camera overlay
    deletes/replaces those nodes.
- FACT: building `vendorimage` at the stock `oem` size failed:
  - `vendor_size=268435456`.
  - `build_image` reported vendor tree size `274885632` bytes.
  - `e2fsdroid` failed while writing `libril-qc-qmi-1.so`.
- FACT: the live GPT has `oem` as partition 31:
  - start sector `10030144`.
  - old end sector `10554431`.
  - old size `268435456` bytes.
  - type GUID `7DB6AC55-ECB5-4E02-80DA-4D335B973332`.
  - unique GUID `18427616-B840-273C-6745-7ABA2F96560B`.
- FACT: partition 32 `limits` starts at sector `10616832`, so partition 31 can
  be enlarged to end sector `10616831` without moving later partitions or
  `userdata`.
- FACT: `sgdisk --pretend` validated the exact resize command with preserved
  type GUID and unique GUID.
- FACT: direct `build_image` with `vendor_size=300384256` succeeded, and
  `simg2img` confirms the raw vendor image is exactly `300384256` bytes.
- FACT: GPT backup before any repartition was captured as
  `gpt-before-attempt130.bin` with SHA-256
  `992a248158d0fa29c598ffc4099f0d4563e2f15fcda2e0f8e10ff0f3476e94b1`.
- FACT: a large `adb exec-out dd` attempt from the booted system made serial
  `30785d1a` go `offline`. No repartition or attempt130 image flash was done
  after that.
- Flashing tool:
  - `flash-attempt130-from-recovery.sh` is a host-side script that targets only
    serial `30785d1a` via `127.0.0.1:15038`.
  - It refuses a normally booted Android system unless
    `ALLOW_LIVE_REPARTITION=1` is set.
  - It backs up GPT, optionally backs up current `oem`, resizes only partition
    31, reboots recovery to reread GPT, flashes boot plus raw vendor, and
    verifies sha256 from block devices when available.
- Required next step:
  boot the phone to recovery, verify ADB is online for serial `30785d1a`, then
  run the attempt130 flash script. After boot, collect logcat/dmesg and verify:
  `qcrild`, IMS linker failures, sensors service, camera provider, Wi-Fi/BT,
  VIDC/scrcpy, light/flashlight, fingerprint, audio, and vibrator state.

2026-06-01 attempts131-133 runtime vendor component fixes:

- Patch category: PROPER-FIX for the active `/system/vendor` payload on the
  enlarged runtime `oem` partition.
- Release directories:
  - `/srv/forge/work/nx549j-preserve/release-attempt131-20260601-wcnss-vendor`
  - `/srv/forge/work/nx549j-preserve/release-attempt132-20260601-secconfig-camera-protobuf`
  - `/srv/forge/work/nx549j-preserve/release-attempt133-20260601-gps-seccomp-vendor`
- Final flashed artifact in this batch:
  - sparse vendor image:
    `vendor-attempt133-gps-seccomp-vendor.img`
    SHA-256 `cbad9ca355cb0ff83b630c3b4b26694cd5a9a13413d8e4214cbd08fafc287d3a`
  - raw vendor image:
    `vendor-attempt133-gps-seccomp-vendor.raw.img`
    SHA-256 `54363bbcaca365d4e0380188bcc460dd4ecc3b3561f64d189d55686156c48b37`
- FACT: attempt131 installed `wcnss_service` into vendor. Runtime then had
  `wcnss_service`, `wlan0`, `p2p0`, `wpa_supplicant`, and WCNSS firmware
  loaded.
- FACT: attempt132 added `sec_config`, fixed the 32-bit protobuf-lite
  compatibility symlink, and added the missing camera `libfastcvopt.so`.
  Runtime no longer had IPC_RTR permission spam, and `mm-qcamera-daemon`
  stopped linker-crashing.
- FACT: attempt133 added media seccomp policy files and `libgps.utils.so`.
  Runtime files verified present:
  `/vendor/etc/sec_config`,
  `/vendor/etc/seccomp_policy/mediacodec.policy`,
  `/vendor/etc/seccomp_policy/mediaextractor.policy`,
  `/vendor/lib64/libgps.utils.so`,
  `/vendor/lib/libprotobuf-cpp-lite.so -> libprotobuf-cpp-lite-3.9.1.so`,
  and `/vendor/lib64/libprotobuf-cpp-lite.so -> libprotobuf-cpp-lite-v29.so`.
- FACT: all attempt131-133 raw vendor images were flashed to
  `/dev/block/bootdevice/by-name/oem` and readback SHA-256 matched the local
  raw image.
- FACT: attempt133 runtime capture:
  `/srv/forge/work/nx549j-preserve/release-attempt133-20260601-gps-seccomp-vendor/runtime-20260531-234353`.
- FACT: fresh live Wi-Fi/RIL capture:
  `/srv/forge/work/nx549j-preserve/live-wifi-ril-check-20260531-234854`.
- FACT: Wi-Fi scanning is now functional. `cmd wifi list-scan-results` returns
  visible 2.4 GHz and 5 GHz networks, `dumpsys wifi` reports five non-empty
  scans, `numHalCrashes=0`, `numWificondCrashes=0`, and
  `numSupplicantCrashes=0`.
- FACT: screen recording through the hardware encoder is now at least
  minimally functional after the seccomp update:
  `screenrecord --time-limit 2 /data/local/tmp/screenrecord-test.mp4`
  returned `rc=0` and produced a non-empty file.
- FACT: RIL is past the earlier linker/IPC_RTR blockers. The live system has
  both `rild` processes running, baseband
  `NX549J_Z69_EN_TSXPR1JE11L204`, `vendor.peripheral.modem.state=ONLINE`,
  `ro.vendor.ril.mbn_copy_completed=1`, and `persist.radio.multisim.config=dsds`.
- FACT: live telephony still reports `gsm.sim.state=ABSENT,ABSENT` and no
  operator/network type. If a SIM is physically installed, the next RIL task is
  SIM/UIM detection rather than generic modem boot.
- FACT: `dpmd` still restarts from the system partition, not vendor, and fails
  with:
  `CANNOT LINK EXECUTABLE "/system/bin/dpmd": cannot locate symbol "strdup8to16" referenced by "/system/lib64/libdpmframework.so"`.
  The prior source-side init rc edit is not effective until a matching system
  image or direct system-side blob/init update is flashed.
- FACT: camera is no longer blocked by missing userspace blobs, but kernel
  logs repeatedly show:
  `MSM-SENSOR-INIT msm_sensor_wait_for_probe_done wait timeout` and
  `VIDIOC_MSM_SENSOR_INIT_CFG failed`.
  This points back to NX549J camera DTS sensor/actuator/eeprom/flash tables.
- Current working status after attempt133:
  boot to Android, display/touch, Wi-Fi scan, WCNSS, basic hardware AVC encode,
  RIL process startup, and camera userspace linker load are working.
- Current unresolved component blockers:
 camera sensor DTS/probe timeout, SIM/UIM/RIL service state, system-side
 `dpmd`, audio HAL/routing, sensors registry, LED/flashlight pinctrl/HAL, and
 final full-build hygiene once disk pressure is reduced.

2026-06-01 attempt134 live IMS/audio/RIL patch and component triage:

- Patch category: DIAGNOSTIC live system/vendor patch plus source-side
  PROPER-FIX candidates.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt134-20260601-live-ims-audio`.
- Fresh component capture:
  `/srv/forge/work/nx549j-preserve/live-component-check-20260601-054938`.
- Live-patched files:
  - `/system/priv-app/ims/ims.apk` from the source prebuilt with
    `uses-library:'ims-ext-common'`.
  - `/vendor/lib/hw/audio.primary.msm8953.so` from the built 32-bit CAF audio
    HAL.
  - `/system/etc/permissions/qti_libpermissions.xml` with a compatibility
    alias for `android.hidl.manager@1.0-java`.
  - `/system/etc/init/dpmd.rc` from source with
    `LD_PRELOAD=/system/lib64/libshim_dpmframework.so`.
- FACT: `com.android.phone` no longer crashes on missing
  `org/codeaurora/ims/utils/QtiCarrierConfigHelper` after replacing `ims.apk`.
- FACT: `com.qualcomm.qcrilmsgtunnel` no longer crashes on missing
  `android.hidl.manager.V1_0.IServiceNotification$Stub` after adding the
  `android.hidl.manager@1.0-java` alias. The service process is running.
- FACT: `dpmd` no longer linker-crashes on `strdup8to16`. The live system has
  parent and worker `dpmd` processes and the service sockets exist, but SELinux
  denials remain permissive-only policy debt.
- FACT: RIL transport is much cleaner than attempt133. The live system has
  both `rild` processes, `com.android.phone`,
  `com.qualcomm.qcrilmsgtunnel`, `imsqmidaemon`, `imsdatadaemon`,
  `ims_rtp_daemon`, and `vendor.peripheral.modem.state=ONLINE`.
- FACT: live telephony still reports `gsm.sim.state=ABSENT,ABSENT`. The fresh
  radio log also shows a WCDMA cell-info response, so modem/radio is not just
  dead. If a SIM is physically installed, the next RIL task is UIM/SIM-slot
  detection; if no SIM is installed, this is expected.
- FACT: Wi-Fi scan is working. Fresh `cmd wifi list-scan-results` returns
  multiple 2.4 GHz and 5 GHz networks, `dumpsys wifi` reports
  `numNonEmptyScanResults=6`, `SCAN_SUCCESS: 6`, `numEmptyScanResults=0`,
  `numHalCrashes=0`, `numWificondCrashes=0`, and
  `numSupplicantCrashes=0`.
- FACT: sensors services now start. Fresh `ps -A` shows both
  `android.hardware.sensors@1.0-service.nubia8953` and `sensors.qcom.real`.
  Runtime functionality still needs `dumpsys sensorservice` and movement tests.
- FACT: Bluetooth userspace comes up far enough to run
  `android.hardware.bluetooth@1.0-service-qti` and `com.android.bluetooth`.
  Pairing/scan/audio routing still needs a focused test.
- FACT: the light HAL is still crashing at runtime on:
  `Failed to open /sys/class/leds/nubia_led/blink_mode`.
  Source-side patch from the audio/light subagent makes missing `nubia_led`
  nodes optional in `device/nubia/msm8953-common/light/service.cpp`, but this
  has not yet been built/flashed.
- FACT: `qti_gnss_service` still restarts with status 255. Logs show the
  current shipped service tries to register
  `android.hardware.gnss@1.1::IGnss/default`, falls back to 1.0, then fails
  because there is no matching framework GNSS implementation entry. The source
  blob tree currently contains only the old `vendor.qti.gnss@1.0-service`
  files even though `proprietary-files.txt` references the newer
  `android.hardware.gnss@2.0-service-qti` stack.
- FACT: `.dataservices` is still running but restarts/crashes around the
  Android 11 IMS API mismatch
  `android.telephony.ims.stub.ImsRegistrationImplBase$Callback`. A RIL
  subagent is now assigned to decide whether to shim, replace, or temporarily
  disable this CNE/dataservices package.
- FACT: camera is still blocked below userspace camera app behavior. Kernel
  logs still show missing camera vreg/gpio tables, flash GPIO data problems,
  and the prior `MSM-SENSOR-INIT` wait timeout path. The camera subagent found
  no safe blind DTS replacement; the next safe step is more targeted kernel
  logging around sensor init/probe rather than copying another board's camera
  table.
- FACT: audio is partially advanced only by a live 32-bit HAL replacement.
  Audio HAL source wiring now exists for `msm8953`, but the module/full build
  was interrupted while `ckati` was regenerating under disk pressure. The live
  64-bit HAL slot remains the default stub hardlink because the direct push ran
  out of vendor space.
- Current working status after attempt134:
  boot to Android, display/touch, Wi-Fi scan, WCNSS, basic hardware AVC encode,
  RIL process startup, qcril message tunnel, dpmd linker startup, sensors
  service startup, Bluetooth service startup, and camera userspace linker load
  are working or at least past their earlier hard blockers.
- Current unresolved component blockers:
  camera sensor DTS/probe, SIM/UIM only if a SIM is inserted, CNE/dataservices
  IMS API mismatch, GNSS HAL service selection/blob set, light HAL build/flash,
  audio routing and final 64-bit HAL packaging, flashlight pinctrl, SELinux
  cleanup, and final full image rebuild once disk pressure is lower.

2026-06-01 attempt135 light/CNE/GNSS follow-up:

- Patch category: PROPER-FIX candidates plus one interrupted DIAGNOSTIC live
  patch.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt135-20260601-live-light-hal`.
- FACT: a first module build exposed a bad source edit: adding `msm8953` to
  `hardware/qcom/audio` duplicated `audio.primary.msm8953`, which is already
  defined by `hardware/qcom-caf/msm8996/audio`. The `hardware/qcom/audio`
  additions were removed, and a second module build passed.
- FACT: `m android.hardware.light@2.0-service.nubia -j1` completed
  successfully after the duplicate audio module fix.
- FACT: live-pushed light HAL binary SHA-256 matched on-device:
  `0f7d67ff56275273f0cc6b66b6ddbfb967d1d82f3de176f10ac5bf1861413a0a`.
- FACT: after the light binary push, `vendor.light-hal-2-0` no longer failed
  on missing `/sys/class/leds/nubia_led/*`; logs changed to optional-node
  warnings. The next light blocker was VINTF registration:
  `Service android.hardware.light@2.0::ILight/default must be in VINTF manifest`.
- FACT: source `device/nubia/msm8953-common/manifest.xml` now declares
  `android.hardware.light@2.0::ILight/default`.
- URGENT FACT: the attempted live push of the expanded vendor VINTF manifest
  failed with `No space left on device` and removed the overlay-visible
  `/vendor/etc/vintf/manifest.xml`. A minified manifest payload was prepared at
  `vendor-manifest-light-min.xml`, but a second push also hit ENOSPC, then
  serial `30785d1a` dropped from ADB.
- FACT: no cache files were deleted before ADB dropped. Large freeable cache
  files were identified but not removed:
  `/cache/NON-HLOS-v2.04EN.bin`,
  `/cache/modem-before-4pda-v2.04en.img`,
  `/cache/attempt120-vendor-upper-payload.tar`, and
  `/cache/boot-before-attempt120.img`.
- Required immediate recovery when `30785d1a` returns to ADB:
  1. Free cache space after backing up those large `/cache` files, or delete
     only if a host backup already exists.
  2. Push
     `/srv/forge/work/nx549j-preserve/release-attempt135-20260601-live-light-hal/vendor-manifest-light-min.xml`
     to `/vendor/etc/vintf/manifest.xml`.
  3. Verify `sha256sum /vendor/etc/vintf/manifest.xml`.
  4. Restart `vendor.light-hal-2-0` and verify it registers in `lshal`.
- FACT: CNE/dataservices root cause is Android API skew, not a missing jar.
  `CNEService.apk` targets Android P-era IMS APIs and crashes on Android 11
  missing `ImsRegistrationImplBase$Callback`.
- Source change: `device/nubia/nx549j/device.mk` now filters out
  `CNEService`, and `device/nubia/msm8953-common/vendor.prop` sets
  `persist.vendor.cne.feature=0`. Native CNE/DPM libraries remain packaged.
- FACT: GNSS root cause is an incomplete local blob set. The shipped
  `vendor.qti.gnss@1.0-service` wrapper expects an Android GNSS passthrough
  implementation, but the tree lacked `android.hardware.gnss@1.0-impl-qti.so`.
- Source change: matching 32-bit and 64-bit
  `android.hardware.gnss@1.0-impl-qti.so` blobs were imported from the local
  `/srv/forge/work/nx549j-crdroid-mine/mpfs/vendor` tree, not from an
  unrelated internet blob set.
  - 32-bit SHA-256:
    `17c61d2359682c78e301e38a4dcfd410e9413a82df365ba02c46cfbd6987387d`.
  - 64-bit SHA-256:
    `3e45a3ac93d7061ed52de251fa2286ce322e1f7a7dae0abce6ef462863b17696`.
- Source change: `proprietary-files.txt`, `msm8953-common-vendor.mk`, and
  `device/nubia/msm8953-common/manifest.xml` now package and declare
  `android.hardware.gnss@1.0::IGnss/default`.
- Next full-image requirement: rebuild vendor/system images once disk pressure
  is lower. The live device is ahead of source in some places and behind source
  in others; attempt135 source fixes need a clean image flash or a carefully
  repaired live overlay.

2026-06-03 attempt149 userspace-timeout boot-complete ack:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt149-20260602-userspace-timeout-boot-ack`.
- Build logs:
  `/srv/forge/work/nx549j-preserve/build-attempt149-20260602-userspace-timeout-boot-ack/build.log`
  and
  `/srv/forge/work/nx549j-preserve/build-attempt149-20260602-userspace-timeout-boot-ack/vendorimage.log`.
- Evidence behind the change:
  old runtime dmesg from attempt113 showed
  `userspace exec reached, auto-acking recovery timeout` at about 10 seconds,
  then `userspace ack reason=userspace-reached, recovery timeout disarmed`.
  That explained why a later bootlogo/userspace hang needed a manual return to
  recovery and did not leave an automatic recovery timeout trace.
- Files changed:
  - `arch/arm64/kernel/frgmark.c`: keep the early recovery timeout before
    userspace, reschedule it to 900 seconds at `userspace_reached`, and wait
    for an explicit userspace ack instead of auto-acking.
  - `device/nubia/msm8953-common/rootdir/etc/init.qcom.rc`: write
    `/proc/frgmark_userspace_ack boot-completed` only when
    `sys.boot_completed=1`.
  - `arch/arm64/boot/dts/qcom/nx549j/msm8953-nubia-common-nx549j.dtsi`:
    keep the attempt148 recovery DT fstab fix, `/system_root` with
    `wait,recoveryonly`.
  - `bootable/recovery/minui/graphics_fbdev.cpp`: keep the attempt148 recovery
    primary framebuffer selection fix, choosing physical DSI fb1 instead of
    writeback fb0.
- Artifact identity:
  - `boot.img` SHA-256:
    `3b22bd2647d94f51a2f5d90a1962a0616364e0b1221e9f3591953c677d0bd460`.
  - `recovery.img` SHA-256:
    `f0595ad52efc7c990e40e80a799c2058e83230549a6c1f2eacb0c8bc7585421d`.
  - `vendor.img` SHA-256:
    `870072c4a9f917ec3ce9c45014edf7bb0eb962053876cb1bb6e8820f11cedf19`.
  - `vendor.raw.img` SHA-256:
    `2ba15eabae478975dfcd4fbda7657f30e0e493ed573303bc4a2eff3749c58911`.
- Flash result:
  flash helper completed from recovery on serial `30785d1a`; post-dd prefix
  hashes matched for `boot`, `recovery`, and `oem`.
- Runtime result:
  after `adb -s 30785d1a reboot`, the postflash monitor recorded 120 samples
  over about 20 minutes and every sample was `state=missing`. It observed no
  system `device` state and no recovery fallback state. Capture-local report:
  `/srv/forge/work/nx549j-preserve/capture-attempt149-postflash-monitor-20260602-1855/REPORT.md`.
- Expected next marker:
  if a post-reboot kernel log is collected, it should show either
  `recovery timeout rescheduled reason=userspace-reached seconds=900` followed
  by a late timeout/restart path, or the absence of that marker proving the
  boot failed before `userspace_reached`.
- Rollback condition:
  revert only if the new logs show that waiting for explicit
  `boot-completed` ack prevents recovery fallback on a path that previously
  could recover, or if `sys.boot_completed=1` cannot write the proc ack.
- Next required step:
  manually return the device to recovery and pull pstore/dmesg/recovery logs
  before changing more boot watchdog logic. The current postflash evidence does
  not prove whether the late timer failed internally or whether recovery/USB
  simply never became visible.

2026-06-03 attempt149 returned-recovery pstore result:

- Capture directory:
  `/srv/forge/work/nx549j-preserve/capture-attempt149-returned-recovery-20260603-005122`.
- FACT: `adb pull` from recovery was flaky, but `exec-out` captured
  `execout-pstore/console-ramoops-0` and `execout-pstore/dmesg-ramoops-0`.
- FACT: pstore proves attempt149 reached userspace:
  `FRGmark stage=15 name=userspace_reached`,
  `FRGmark: recovery timeout rescheduled reason=userspace-reached seconds=900`,
  and `FRGmark: userspace exec reached, waiting for explicit ack late_timeout=900`.
- FACT: the timeout was then disabled too early by the stale first-stage init
  ack:
  `FRGmark stage=17 name=userspace_ack` and
  `FRGmark: userspace ack reason=first-stage-mounted, recovery timeout disarmed`.
- FACT: the same pstore tail shows orderly shutdown and
  `reboot: Restarting system with command 'bootloader'` at about 12.6 seconds.
- Updated hypothesis:
  attempt149's recovery fallback logic was not stuck before userspace; it was
  disarmed by a stale ack before `sys.boot_completed=1`. The next patch should
  let only the explicit `boot-completed` ack disarm recovery fallback.

2026-06-03 attempt150 ack allowlist patch:

- Patch category: BOOT-UNBLOCK with DIAGNOSTIC logging.
- Build directory:
  `/srv/forge/work/nx549j-preserve/build-attempt150-20260603-ack-allowlist`.
- Files changed:
  - `arch/arm64/kernel/frgmark.c`: accept `/proc/frgmark_userspace_ack` only
    when the trimmed reason is `boot-completed`; print ignored ack reasons so
    the next pstore proves whether old userspace is still writing stale values.
  - `system/core/init/first_stage_init.cpp`: remove the stale
    `first-stage-mounted` ack source from the source tree.
  - `system/core/init/init.cpp`: remove the stale `init-main-queue` ack source
    from the source tree.
- Immediate test scope:
  build and flash only `bootimage recoveryimage`; do not build a full ROM unless
  later evidence proves the on-device `/system/bin/init` must be replaced for
  this specific watchdog test.
- Expected next markers:
  stale acks should log as
  `FRGmark: userspace ack ignored reason=... waiting_for=boot-completed`.
  Only `reason=boot-completed` may disarm the recovery timeout.
- Rollback condition:
  revert the allowlist only if a fresh attempt150 pstore proves Android reached
  `sys.boot_completed=1` but the proc ack was blocked or malformed despite the
  vendor init trigger writing `boot-completed`.

2026-06-03 attempt150 boot-only artifact and monitor result:

- FACT: the normal Android `m bootimage recoveryimage -j1` attempt was stopped
  after Kati spent more than 10 minutes in filesystem wait during graph regen.
  No full ROM, systemimage, or vendorimage build was run for attempt150.
- FACT: a kernel-only build was run against the existing
  `out/target/product/nx549j/obj/KERNEL_OBJ` and completed successfully:
  `/srv/forge/work/nx549j-preserve/build-attempt150-20260603-ack-allowlist/kernel-only.log`.
  It rebuilt `arch/arm64/kernel/frgmark.o`, linked `vmlinux`, generated a fresh
  `System.map`, and rebuilt `arch/arm64/boot/Image.gz-dtb`.
- Artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt150-20260603-ack-allowlist`.
- Artifact identity:
  - `boot.img` SHA-256:
    `8e59dbf798d25fbc86e74c1c8ffb28901f9b69bdae93ab3b5ece8df2cb52e441`.
  - `Image.gz-dtb` SHA-256:
    `668058d6260df365591eb9192e9f75be9741087915c09b702813f848e69f0425`.
  - `System.map` SHA-256:
    `47313b669cacca949fe7b38f3cb76b1c7e1c909c41c4717b6034404ebf47e48d`.
- FACT: `strings vmlinux` confirmed the new allowlist marker strings are in the
  built kernel:
  `boot-completed` and
  `FRGmark: userspace ack ignored reason=%s waiting_for=%s artifact=%s`.
- FACT: `boot.img` was repacked from attempt149 boot header/ramdisk with only
  the kernel replaced. Recovery/vendor/system were not flashed.
- Flash result:
  `flash-attempt150-boot-only-from-codex-recovery.sh` flashed boot from
  recovery on serial `30785d1a`; post-dd boot partition prefix SHA matched
  `8e59dbf798d25fbc86e74c1c8ffb28901f9b69bdae93ab3b5ece8df2cb52e441`.
- Runtime monitor:
  `/srv/forge/work/nx549j-preserve/capture-attempt150-postflash-monitor-20260602-2112`.
  After `adb -s 30785d1a reboot`, 180 samples over about 30 minutes were all
  `error: device '30785d1a' not found`. No system ADB and no recovery fallback
  ADB appeared during the monitor window.
- Current interpretation:
  attempt150 likely closed the immediate stale-ack disarm path, because the
  attempt149 12.6-second `bootloader` reboot pattern did not reappear as a
  visible recovery/system ADB state. However, the run does not prove whether
  the 900-second timeout fired. Fresh pstore after manual recovery is required
  before changing the timer/restart path again.
- Next required step:
  manually return `30785d1a` to recovery and pull pstore with `exec-out` before
  any new boot watchdog patch. The next pstore should be checked for
  `userspace ack ignored`, `recovery timeout firing seconds=900`, and the final
  reboot command.

2026-06-03 attempt151 raw-WDT userspace-120 diagnostic fallback:

- Patch category: DIAGNOSTIC / BOOT-UNBLOCK for recovery automation only.
- Evidence for change:
  attempt149 pstore proved BCB was written by 8.54s and later cleared by stale
  `first-stage-mounted` ack. Attempt150 blocked stale ack in the kernel, but
  its boot cmdline still had no `frgmark.raw_wdt=1`, and the 30-minute monitor
  never saw `30785d1a` return over ADB. This left an unbounded hang path if the
  kernel/userspace stopped before the 900-second fallback could reboot.
- Files changed:
  - `arch/arm64/kernel/frgmark.c`: shorten
    `FRG_RECOVERY_USERSPACE_TIMEOUT_SEC` from 900 to 120 seconds, call
    `frgmark_arm_early_recovery_guard("raw-wdt-param")` when raw WDT is parsed,
    and disable the raw WDT only after an allowed `boot-completed` ack.
  - `device/nubia/nx549j/BoardConfig.mk`: add `frgmark.raw_wdt=1` to the
    reproducible boot cmdline.
- Build:
  `/srv/forge/work/nx549j-preserve/build-attempt151-20260603-raw-wdt-userspace120/kernel-only-clang.log`.
  This was a kernel-only `Image.gz-dtb` build against the existing
  `out/target/product/nx549j/obj/KERNEL_OBJ`; no full ROM, system, vendor, or
  recovery build was run.
- Artifact directory:
  `/srv/forge/work/nx549j-preserve/release-attempt151-20260603-raw-wdt-userspace120`.
- Artifact identity:
  - `boot.img` SHA-256:
    `2759a887d7ff0132efba7dda814cac5b2dde5311cb26294f6c457a9931b130eb`.
  - `Image.gz-dtb` SHA-256:
    `1afccae3e3e0d73c653588e72cb92cf5731ddf444996fcc88a9597dabfb23617`.
  - `System.map` SHA-256:
    `595bdd0b1427aa28ce4b4f30fe29352e6ea7308a7be17d8d4cb7e65e8c1abca7`.
  - `vmlinux` SHA-256:
    `099792ddc0dece77c1b36b69b9feb2bb806e93dc9f5c432a6ee2812e5d9edee4`.
- Verification:
  `sha256sum -c SHA256SUMS` passed. `unpack_bootimg.py` confirmed header
  version 0, page size 2048, os version 11.0.0, os patch level 2024-02, and
  cmdline containing `frgmark.raw_wdt=1 frgmark.recovery_timeout_sec=120
  frgmark.bcb_misc_devt=179:28`. Unpacked kernel and ramdisk matched the
  packaged `Image.gz-dtb` and attempt150 ramdisk. `strings vmlinux` confirmed
  `raw-wdt-param`, `early raw WDT disabled`, `boot-completed`, and
  `userspace ack ignored`.
- Current device status:
  a watcher on reverse ADB port 15038 is running for serial `30785d1a` and
  currently sees only non-target devices `711HEBSR277K5` and `91HEBNL163XD`.
  When `30785d1a` appears in recovery, first pull attempt150 pstore, then flash
  attempt151 boot-only if recovery shell is usable.
- Expected next markers:
  pstore after attempt151 should show `FRGmark: raw WDT enabled`,
  `FRGmark: early raw WDT armed seconds=120`, stale ack rejection, and
  `late_timeout=120`. If `boot-completed` happens, it should also show
  `FRGmark: early raw WDT disabled reason=userspace-ack`.
- Rollback condition:
  if attempt151 resets before BCB is written or repeatedly prevents a real
  `boot-completed` path, remove `frgmark.raw_wdt=1` and restore the longer
  userspace timeout after collecting fresh pstore.

2026-06-03 attempt151 reverse-ADB flash result:

- FACT: reverse ADB port `15038` became reachable and showed the target serial
  `30785d1a` in recovery at `20260603-124744`.
- Watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt151-20260603-raw-wdt-userspace120/runtime/wait-logs/watch-tmux-20260603-121840-15038.log`.
- Flash directory:
  `/srv/forge/work/nx549j-preserve/release-attempt151-20260603-raw-wdt-userspace120/runtime/flash-boot-bcb-20260603-121840-15038`.
- FACT: recovery-ramoops preflight was flashed and verified before the boot
  test. The recovery prefix SHA-256 after flashing matched
  `9700236dfa0d2057cf29c4247fa27a0d0b85a9e64e5033d511d6ac8f0469961c`.
- FACT: `attempt151` boot was pushed and flashed to the real `boot`
  partition. The boot partition prefix SHA-256 matched
  `2759a887d7ff0132efba7dda814cac5b2dde5311cb26294f6c457a9931b130eb`.
- Runtime result:
  after normal reboot, the runner waited 260 seconds and timed out:
  `RESULT=automatic-recovery-timeout`. No Android userspace ADB state and no
  automatic recovery ADB state appeared during that window.
- Current status after timeout:
  the unattended runner is still waiting for manual recovery in tmux session
  `nx549j-attempt151-15038`, but the current `adb -H 127.0.0.1 -P 15038
  devices -l` output shows only non-target devices `711HEBSR277K5` and
  `91HEBNL163XD`. Serial `30785d1a` is missing.
- Interpretation:
  this run proves the guarded flash path worked and the boot partition identity
  was correct. It does not yet prove whether the raw WDT marker path fired,
  because pstore after the failed boot has not been collected. The next action
  is to let the waiting finish helper capture pstore/logs when `30785d1a`
  returns to recovery.

2026-06-03 attempt152 current targeted boot/recovery/vendor package:

- Patch category: SOURCE-SYNC / TARGETED-IMAGE REFRESH. This package was built
  to remove stale mixing between older flashed images and the current source
  tree. It is not a full OTA.
- Build directory:
  `/srv/forge/work/nx549j-preserve/build-attempt152-20260603-current-targeted-images`.
- Build command:
  `source build/make/envsetup.sh && lunch lineage_nx549j-userdebug && m bootimage recoveryimage vendorimage -j1`.
- Build log:
  `/srv/forge/work/nx549j-preserve/build-attempt152-20260603-current-targeted-images/build-rerun1.log`.
- FACT: the targeted build completed successfully:
  `#### build completed successfully (18:37 (mm:ss)) ####`.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images`.
- Artifact identity:
  - `boot.img` SHA-256:
    `52ea754301ee0792bc489d535a7e81d3dffcce3906f5b3c9a433618baaa6e43e`.
  - `recovery.img` SHA-256:
    `61e42358e689182bd4bb1627f835b9de182d20715f76f53726a10e0b341e3162`.
  - `vendor.img` SHA-256:
    `85c4bbff83e5af0e3a21684daecbbd069534412d3a239a69bf1c215e1c4f852d`.
  - `vendor.raw.img` SHA-256:
    `61c5967bff6ed0161701279bde694f58a85af38ffcdf2c0c28ea30521cc13318`.
  - `Image.gz-dtb` SHA-256:
    `208ecf535fb16bc0341fca23fd214986e3281818532de67c08544bcc514b6b41`.
  - `System.map` SHA-256:
    `8c899387f999ece3e4573b50261f9fedffa0da84c9d5d55f941f0a3166e4427d`.
  - `vmlinux` SHA-256:
    `5f112cffc3f8f81c794f2bc857f080a563b5a52192fbd0c180ff2a4969d8716b`.
- Validation:
  `sha256sum -c SHA256SUMS` passed for all listed artifacts.
  `vendor.raw.img` size is `300384256`, matching the known real `oem`
  partition size. `e2fsck -fn vendor.raw.img` passed with
  `vendor: 1342/73344 files (0.7% non-contiguous), 73070/73336 blocks`.
- Flash helper:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images/flash-attempt152-from-codex-recovery.sh`.
  The helper is executable, passed `bash -n`, refuses unless serial `30785d1a`
  is in recovery, verifies `SHA256SUMS`, and refuses unless the `oem` partition
  size is exactly `300384256` before writing `vendor.raw.img`.
- Current status:
  attempt152 has not been flashed yet. Preserve and inspect attempt151 pstore
  first if `30785d1a` returns to recovery, then flash attempt152 from the
  guarded helper if recovery remains reachable.

2026-06-03 attempt151-to-attempt152 unattended chain:

- Patch category: DIAGNOSTIC / AUTOMATION. This is runner infrastructure, not
  a kernel proper-fix.
- New scripts:
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt152-unattended.sh`:
    flashes the attempt152 package from recovery, reboots, waits for Android
    `device` or recovery, then captures either userspace component logs or
    recovery-readable early markers. If the boot disappears past the wait
    window, it waits for manual recovery and captures pstore then.
    Before the attempt152 reboot it now snapshots and clears recovery pstore
    and backs up `misc`; after userspace/recovery capture it restores that
    `misc` backup where possible. Recovery-side restore is strict because a
    stale recovery BCB can poison the next boot test.
  - `/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt152.sh`:
    waits for recovery, finishes the timed-out attempt151 run first, then
    starts the attempt152 unattended run only after attempt151 pstore capture
    succeeds.
- Updated helper:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images/flash-attempt152-from-codex-recovery.sh`
  now writes a runtime flash log and verifies partition-prefix SHA-256 for
  `boot`, `recovery`, and `oem` after dd. Its output identity file is
  `runtime/flash-attempt152-$RUN_ID/flash-identity.env`.
- Validation:
  `bash -n` passed for both new scripts and the updated attempt152 flash
  helper, including the later pstore-clear/misc-restore hardening in
  `nx549j-run-attempt152-unattended.sh`. `sha256sum -c SHA256SUMS` still
  passed for the attempt152 package.
- Active watcher:
  the old `nx549j-attempt151-15038` tmux session was replaced by
  `nx549j-chain151-152-15038`.
- Active watcher command context:
  `RUN_ID=20260603-130621-15038 ADB_HOST=127.0.0.1 ADB_PORT=15038
  SERIAL=30785d1a POLL_SECONDS=5 BOOT_WAIT_SECONDS=420
  MANUAL_RECOVERY_WAIT_SECONDS=0
  scripts/nx549j-chain-attempt151-to-attempt152.sh`.
- Active chain log:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images/runtime/wait-logs/chain-attempt151-to-152-20260603-130621-15038.log`.
- Active finish log for attempt151:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images/runtime/wait-logs/finish-attempt151-20260603-130621-15038.log`.
- Current live ADB state when the chain was started:
  reverse ADB port `15038` was reachable but showed only non-target devices
  `711HEBSR277K5` and `91HEBNL163XD`; serial `30785d1a` was still missing.
- Expected next flow when `30785d1a` appears in recovery:
  1. finish helper captures attempt151 pstore into the existing timed-out flash
     directory:
     `/srv/forge/work/nx549j-preserve/release-attempt151-20260603-raw-wdt-userspace120/runtime/flash-boot-bcb-20260603-121840-15038/after-manual-recovery-attempt151-before-152-20260603-130621-15038`;
  2. attempt152 boot/recovery/vendor.raw are flashed and post-dd verified;
  3. recovery pstore is snapshotted/cleared and `misc` is backed up before the
     attempt152 reboot, so a later pstore capture should belong to attempt152
     rather than attempt151;
  4. the device is rebooted and watched for 420 seconds;
  5. if Android `device` appears, broad userspace component logs are captured;
     if recovery appears, early markers/pstore are captured; if neither
     appears, the script waits indefinitely for manual recovery and captures
     pstore then;
  6. `misc` is restored after the capture when the current ADB state allows
     block writes.
- Rollback condition:
  if this automation races with manual recovery work or the target appears on
  a different ADB port, stop `nx549j-chain151-152-15038` and restart the same
  chain with the correct `ADB_PORT`; no source/kernel rollback is required.

2026-06-03T13:16Z live reverse ADB check:

- Port `15038` is open and the remote ADB server responds.
- Current visible devices are still only non-target Meizu serials
  `711HEBSR277K5` and `91HEBNL163XD`; NX549J serial `30785d1a` is not visible
  yet.
- `nx549j-chain151-152-15038` remains the active watcher and is still waiting
  for `30785d1a` before it captures attempt151 pstore and flashes attempt152.

2026-06-03T13:21Z multiport reverse ADB watcher:

- Patch category: DIAGNOSTIC / AUTOMATION. No kernel behavior change.
- The single-port tmux watcher `nx549j-chain151-152-15038` was stopped and
  replaced with `nx549j-multiport-chain151-152`.
- Active command context:
  `RUN_ID=20260603-132037-multiport ADB_HOST=127.0.0.1 ADB_PORT=15038
  ADB_PORTS="15038 15032 15037" SERIAL=30785d1a POLL_SECONDS=5
  WAIT_TUNNEL_SECONDS=0 BOOT_WAIT_SECONDS=420
  MANUAL_RECOVERY_WAIT_SECONDS=0
  ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt152.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images/runtime/wait-logs/watch-reverse-adb-20260603-132037-multiport.log`.
- FACT: first preflight pass showed `TARGET_SERIAL_MISSING` on listeners
  `15038` and `15032`; `15037` had `NO_REVERSE_LISTENER`.
- Expected next flow is unchanged: the watcher starts the attempt151-to-152
  chain only after `nx549j-check-reverse-adb.sh` reports
  `TARGET_RECOVERY_READY` for exact serial `30785d1a`.

2026-06-03 attempt153 systemimage delivery package:

- Patch category: PROPER-FIX delivery artifact for system-side userspace files
  already present in the LineageOS output tree. This is not a kernel behavior
  change and it is not wired into the active attempt152 watcher.
- Reason:
  attempt152 contains boot/recovery/vendor only. Local audit showed DPM/shim
  files such as `system/bin/dpmd`, `system/lib64/libshim_dpmframework.so`, and
  `system/etc/init/dpmd.rc` exist under `out/target/product/nx549j/system`, but
  there was no current `system.img` artifact to deliver them if the phone's
  system partition is stale.
- Build command:
  `source build/make/envsetup.sh && lunch lineage_nx549j-userdebug &&
  m systemimage -j1`.
- Build log:
  `/srv/forge/work/nx549j-preserve/build-attempt153-20260603-systemimage/build-rerun2.log`.
- Build result:
  completed successfully in `05:19` and produced
  `out/target/product/nx549j/system.img`.
- Release directory:
  `/srv/forge/work/nx549j-preserve/release-attempt153-20260603-systemimage`.
- Artifacts:
  - sparse `system.img`, size `1611797012`, SHA-256
    `eb563a14d64fcde6d93432cf8f9d26b2eb8255e99a586a2f8940c6ee9dfa628a`;
  - raw `system.raw.img`, size `3221225472`, SHA-256
    `93182b52d4bc76f5774efc21b1e1db1fb35b0165a6d7dabbdeeaa17edbce2220`.
- Validation:
  `sha256sum -c SHA256SUMS` passed for both images.
  `/sbin/e2fsck -fn system.raw.img` passed with
  `/: 4968/196608 files (0.3% non-contiguous), 405515/786432 blocks`.
- Flash helper:
  `/srv/forge/work/nx549j-preserve/release-attempt153-20260603-systemimage/flash-system-from-codex-recovery.sh`.
  It passed `bash -n`, refuses non-`30785d1a`, refuses non-recovery state,
  checks the model/device/product identity, checks the system partition size
  is `3221225472`, streams raw ext4 to
  `/dev/block/bootdevice/by-name/system` with `adb exec-in`, then verifies the
  flashed partition by streaming it back through `adb exec-out | sha256sum`.
- Current use rule:
  keep this as the next delivery tool if fresh attempt152 runtime evidence
  shows stale system-side DPM/shim/system files. Do not flash it merely to
  change the early boot experiment unless a full image refresh is intentionally
  being tested.

2026-06-03 attempt154 full targeted flash flow:

- Patch category: PROPER-FIX delivery automation. This changes the next flash
  flow only; no kernel behavior is changed.
- Reason:
  the active attempt152 chain would test boot/recovery/vendor without the fresh
  system-side fixes delivered by attempt153. That would leave a real risk of
  re-testing stale `system/bin/dpmd`, `system/lib64/libshim_dpmframework.so`,
  `system/etc/init/dpmd.rc`, and related system files while judging the current
  ROM.
- New release manifest:
  `/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images/README.md`.
- New scripts:
  - `/srv/forge/android/nx549j/scripts/nx549j-run-attempt154-full-unattended.sh`
    flashes attempt152 boot/recovery/vendor, then attempt153 system, then
    snapshots/clears recovery pstore, backs up `misc`, reboots, waits for
    userspace or recovery, captures logs/markers, and restores `misc`.
  - `/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt154.sh`
    waits for recovery, finishes the timed-out attempt151 pstore capture first,
    then starts the full attempt154 runner.
- Validation:
  `bash -n` passed for both new scripts, and
  `nx549j-run-attempt154-full-unattended.sh --help` prints the expected
  attempt154/attempt152/attempt153 paths.
- Active watcher:
  `nx549j-multiport-chain151-152` was stopped and replaced by
  `nx549j-multiport-chain151-154`.
- Active command context:
  `RUN_ID=20260603-134224-full ADB_HOST=127.0.0.1 ADB_PORT=15038
  ADB_PORTS="15038 15032 15037" SERIAL=30785d1a POLL_SECONDS=5
  WAIT_TUNNEL_SECONDS=0 BOOT_WAIT_SECONDS=420
  MANUAL_RECOVERY_WAIT_SECONDS=0
  ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt154.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images/runtime/wait-logs/watch-reverse-adb-20260603-134224-full.log`.
- FACT: first preflight passes still show `TARGET_SERIAL_MISSING` on listeners
  `15038` and `15032`; `15037` has `NO_REVERSE_LISTENER`.
- Expected next flow when `30785d1a` appears in recovery:
  1. capture attempt151 pstore before any new flash;
  2. flash and verify attempt152 boot/recovery/vendor;
  3. flash and verify attempt153 system;
  4. clear stale recovery pstore and back up `misc`;
  5. reboot and capture either userspace component logs or recovery markers.

2026-06-03T13:47Z attempt154 rolling reverse ADB watcher:

- Patch category: DIAGNOSTIC / AUTOMATION. No kernel, recovery, or image
  behavior change.
- Reason:
  the first attempt154 watcher was started before the watcher script gained
  `PREFLIGHT_ROLLING=1`; while target serial `30785d1a` was missing, it could
  keep creating timestamped preflight directories. The old watcher was stopped
  and restarted with rolling per-port preflight directories only.
- Active session:
  `nx549j-multiport-chain151-154`.
- Active command context:
  `RUN_ID=20260603-134654-full-rolling ADB_HOST=127.0.0.1 ADB_PORT=15038
  ADB_PORTS="15038 15032 15037" SERIAL=30785d1a POLL_SECONDS=5
  PREFLIGHT_ROLLING=1 WAIT_TUNNEL_SECONDS=0 BOOT_WAIT_SECONDS=420
  MANUAL_RECOVERY_WAIT_SECONDS=0
  ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt154.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images/runtime/wait-logs/watch-reverse-adb-20260603-134654-full-rolling.log`.
- FACT:
  first rolling preflight pass showed `TARGET_SERIAL_MISSING` on open
  listeners `15038` and `15032`; `15037` still had `NO_REVERSE_LISTENER`.
- FACT:
  rolling preflight root contains only `port-15038-latest`,
  `port-15032-latest`, and `port-15037-latest`.
- Expected next flow remains unchanged: when exact serial `30785d1a` appears in
  recovery, the chain captures attempt151 pstore first, then flashes and
  verifies the full attempt154 targeted set.

2026-06-03T13:53Z attempt154 watcher device-list logging:

- Patch category: DIAGNOSTIC / AUTOMATION. No image flashing behavior changed.
- File changed:
  `/srv/forge/android/nx549j/scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Reason:
  `TARGET_SERIAL_MISSING` alone was ambiguous when multiple reverse ADB servers
  were open. The watcher now logs a compact `devices=serial:state,...` field
  from each port's `adb devices -l` result, filtered to real ADB states only.
- Validation:
  `bash -n scripts/nx549j-watch-reverse-adb-and-run-latest.sh` passed.
- Active session:
  `nx549j-multiport-chain151-154`.
- Active command context:
  `RUN_ID=20260603-135238-full-rolling-devices2 ADB_HOST=127.0.0.1
  ADB_PORT=15038 ADB_PORTS="15038 15032 15037" SERIAL=30785d1a
  POLL_SECONDS=5 PREFLIGHT_ROLLING=1 WAIT_TUNNEL_SECONDS=0
  BOOT_WAIT_SECONDS=420 MANUAL_RECOVERY_WAIT_SECONDS=0
  ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-chain-attempt151-to-attempt154.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt154-20260603-full-targeted-images/runtime/wait-logs/watch-reverse-adb-20260603-135238-full-rolling-devices2.log`.
- FACT:
  first pass showed `15038` listener present with
  `devices=711HEBSR277K5:device,91HEBNL163XD:device`,
  `15032` listener present with
  `devices=L91HVCNS22KS5:device,ZY32LLVPXR:device`, and `15037`
  `NO_REVERSE_LISTENER` with `devices=none`.
- Current blocker:
  exact NX549J serial `30785d1a` is still absent from every reverse ADB port,
  so attempt154 has not started and no new runtime evidence exists yet.

2026-06-03T14:46Z attempt154/156 system delivery evidence:

- Patch category: PROPER-FIX delivery fix plus DIAGNOSTIC/AUTOMATION resume
  tooling.
- FACT:
  attempt154 found exact serial `30785d1a` in recovery on reverse ADB port
  `15038`, captured/finished attempt151 pstore/misc first, then flashed and
  verified attempt152 boot/recovery/vendor:
  - boot SHA-256:
    `52ea754301ee0792bc489d535a7e81d3dffcce3906f5b3c9a433618baaa6e43e`;
  - recovery SHA-256:
    `61e42358e689182bd4bb1627f835b9de182d20715f76f53726a10e0b341e3162`;
  - vendor raw SHA-256:
    `61c5967bff6ed0161701279bde694f58a85af38ffcdf2c0c28ea30521cc13318`.
- FACT:
  attempt154 system delivery refused before writing because recovery reported
  `/dev/block/bootdevice/by-name/system` size `4294967296`, while the attempt153
  helper expected `3221225472`.
- Source fix:
  `device/nubia/msm8953-common/BoardConfigCommon.mk` and
  `device/nubia/nx549j/BoardConfig.mk` now set
  `BOARD_SYSTEMIMAGE_PARTITION_SIZE := 4294967296`.
- Build note:
  a direct `m systemimage -j1` rebuild was started in
  `/srv/forge/work/nx549j-preserve/build-attempt155-20260603-systemimage-4g`,
  but the host entered severe block I/O wait (`ckati` in `ext4_read_bh` /
  `rq_qos_wait`, load average around 70-80). The agent stopped its own stuck
  build session to avoid adding more I/O pressure.
- Artifact fix:
  `/srv/forge/work/nx549j-preserve/release-attempt155-20260603-systemimage-4g`
  contains a 4GiB `system.raw.img` made by sparse-copying the verified attempt153
  raw image, truncating to `4294967296`, and running `resize2fs`.
- attempt155 system raw identity:
  - size: `4294967296`;
  - filesystem block count: `1048576`;
  - filesystem block size: `4096`;
  - SHA-256:
    `c57ccae87455a9d45608da7452ecd047579ee429af1cb5213c717d1c24067aaf`;
  - `/sbin/e2fsck -fn` result: clean,
    `/: 4968/262144 files (0.3% non-contiguous), 409641/1048576 blocks`.
- Flash helper fix:
  `release-attempt155-20260603-systemimage-4g/flash-system-from-codex-recovery.sh`
  now expects system size `4294967296`, checks SHA
  `c57ccae87455a9d45608da7452ecd047579ee429af1cb5213c717d1c24067aaf`, verifies
  the full partition hash on-device with `dd ... | sha256sum` instead of
  streaming 4GiB back over USB, and supports `VERIFY_ONLY=1`.
- Runner fix:
  `/srv/forge/android/nx549j/scripts/nx549j-run-attempt154-full-unattended.sh`
  now supports `SKIP_BOOT_VENDOR_FLASH=1` and `SYSTEM_VERIFY_ONLY=1` for resume
  after a USB drop.
- attempt156:
  `/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g`
  re-used attempt152 boot/recovery/vendor plus attempt155 4GiB system.
- FACT:
  attempt156 wrote the 4GiB system image from `14:37:00Z` until the helper
  reached `stream-verify-start` at `14:46:14Z`; this means the write phase
  completed, but the old helper then attempted USB readback verification.
- Current blocker:
  at `14:46:14Z`, USB/ADB went `device offline` exactly when full readback
  verification started. The system partition write is therefore likely complete
  but not identity-verified yet.
- Active resume watcher:
  `nx549j-attempt156-resume-watch`.
- Active resume watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g/runtime/wait-logs/watch-reverse-adb-20260603-145100-attempt156-resume-verify.log`.
- Resume command context:
  `RUN_ID=20260603-145100-attempt156-resume-verify ADB_HOST=127.0.0.1
  ADB_PORT=15038 ADB_PORTS="15038 15032 15037" SERIAL=30785d1a
  PREFLIGHT_ROLLING=1 ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g
  BOOT_VENDOR_DIR=/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images
  SYSTEM_DIR=/srv/forge/work/nx549j-preserve/release-attempt155-20260603-systemimage-4g
  SKIP_BOOT_VENDOR_FLASH=1 SYSTEM_VERIFY_ONLY=1
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-run-attempt154-full-unattended.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Expected next flow:
  when `30785d1a` returns as `recovery`, the watcher should run verify-only
  system SHA on the device, then snapshot/clear pstore, reboot, and capture
  userspace logs or recovery markers.

2026-06-03T14:56Z attempt156 offline reconnect watcher:

- Patch category: DIAGNOSTIC / AUTOMATION. No image write behavior changed.
- File changed:
  `/srv/forge/android/nx549j/scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Reason:
  after the 4GiB system write completed, the old helper started a 4GiB USB
  readback verify and the target fell to `offline`. A passive watcher would
  wait forever even if the remote ADB server only needed a reconnect.
- Change:
  the watcher now supports `RECONNECT_OFFLINE=1`; when the target serial is
  visible as `offline`, it runs `adb -H $ADB_HOST -P $port reconnect offline`
  and records the output path.
- Validation:
  `bash -n scripts/nx549j-watch-reverse-adb-and-run-latest.sh` passed.
- Active resume watcher:
  `nx549j-attempt156-resume-watch`.
- Active command context:
  `RUN_ID=20260603-150000-attempt156-resume-reconnect ADB_HOST=127.0.0.1
  ADB_PORT=15038 ADB_PORTS="15038 15032 15037" SERIAL=30785d1a
  POLL_SECONDS=5 PREFLIGHT_ROLLING=1 RECONNECT_OFFLINE=1
  ATTEMPT_DIR=/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g
  BOOT_VENDOR_DIR=/srv/forge/work/nx549j-preserve/release-attempt152-20260603-current-targeted-images
  SYSTEM_DIR=/srv/forge/work/nx549j-preserve/release-attempt155-20260603-systemimage-4g
  SKIP_BOOT_VENDOR_FLASH=1 SYSTEM_VERIFY_ONLY=1
  RUNNER=/srv/forge/android/nx549j/scripts/nx549j-run-attempt154-full-unattended.sh
  scripts/nx549j-watch-reverse-adb-and-run-latest.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g/runtime/wait-logs/watch-reverse-adb-20260603-150000-attempt156-resume-reconnect.log`.
- FACT:
  the watcher is issuing `reconnecting 30785d1a`, but direct ADB still reports
  `30785d1a offline` on port `15038`; ports `15032` and `15037` do not expose
  the target.
- Current blocker:
  target USB/ADB transport is still offline after the completed system write.
  A physical USB reconnect or reboot back to recovery is likely needed. The
  watcher is already armed to resume with verify-only and no repeat 4GiB write.

2026-06-03T15:02Z attempt156 offline retry status:

- Patch category: DIAGNOSTIC / AUTOMATION status only. No image behavior
  changed.
- FACT:
  active watcher `nx549j-attempt156-resume-watch` still sees
  `30785d1a:offline` on reverse ADB port `15038`; ports `15032` and `15037`
  do not expose the target.
- FACT:
  repeated `adb reconnect offline` attempts return status `0` and print
  `reconnecting 30785d1a`, but the target stays `offline`.
- FACT:
  manual `adb -H 127.0.0.1 -P 15038 reconnect device` also returned success
  status but left `30785d1a` in `offline` state.
- Current blocker:
 the host cannot make further device progress until the physical USB/ADB
 transport is reset or the phone is rebooted back to recovery. The active
 watcher remains armed and will resume with `SKIP_BOOT_VENDOR_FLASH=1` and
  `SYSTEM_VERIFY_ONLY=1` when `30785d1a` becomes `recovery` again.

2026-06-03T16:10Z attempt156 direct resume / verified raw system reflash:

- Patch category: DIAGNOSTIC / AUTOMATION status only. No kernel/image source
  changed in this step.
- Recovery return:
  `30785d1a` returned on reverse ADB port `15038` as `recovery`; the previous
  tmux watcher was gone (`tmux list-sessions` reported no server), so a direct
  resume was started.
- Direct verify-only run:
  `RUN_ID=20260603-152835-attempt156-direct-resume` with
  `SKIP_BOOT_VENDOR_FLASH=1 SYSTEM_VERIFY_ONLY=1`.
- Direct verify-only result:
  the 4GiB system partition full-block SHA was
  `fd2eb50dd96595581ded0137263debcbb63405b9f9040b26ff43820d11147670`, not the
  expected raw SHA
  `c57ccae87455a9d45608da7452ecd047579ee429af1cb5213c717d1c24067aaf`.
- Interpretation:
  the earlier completed write could not be treated as identity-verified. Note
  that sparse-image flashing can leave DONT_CARE/free blocks with non-zero old
  contents, so full-block SHA is stricter than filesystem validity; however this
  run needed deterministic raw identity before reboot testing.
- Reflash run:
  `RUN_ID=20260603-153255-attempt156-reflash-system` with
  `SKIP_BOOT_VENDOR_FLASH=1 SYSTEM_VERIFY_ONLY=0`, using the same
  `release-attempt155-20260603-systemimage-4g/system.raw.img`.
- Reflash evidence:
  `stream-flash-start` at `2026-06-03T15:34:02Z`; host fd progress reached the
  end of the 4GiB image; `device-local-verify-start` at `16:00:36Z`;
  `system-flashed-and-verified` at `16:01:55Z`.
- Verified system evidence:
  `/srv/forge/work/nx549j-preserve/release-attempt155-20260603-systemimage-4g/runtime/flash-system-4g-20260603-153255-attempt156-reflash-system/flash-system.log`.
- Reboot evidence:
  the runner backed up misc, rebooted at `16:01:59Z`, waited 420 seconds, and
  timed out at `16:09:04Z` with only `poll state=missing` for `30785d1a`.
- Active wait:
  tmux session `nx549j-attempt156-reflash-system` is now in the
  `manual-recovery` wait loop (`MANUAL_RECOVERY_WAIT_SECONDS=0`). It will
  capture recovery markers and restore misc automatically when the user returns
  the phone to recovery.
- Active log:
  `/srv/forge/work/nx549j-preserve/release-attempt156-20260603-full-targeted-system4g/runtime/wait-logs/attempt154-full-unattended-20260603-153255-attempt156-reflash-system.log`.
- Current blocker:
  after a fully verified raw 4GiB `system` flash, the phone does not enumerate
  as ADB `device` or `recovery` within 420 seconds. Need manual recovery return
  to collect pstore/early markers from this verified-system boot attempt.

2026-06-04T22:58Z attempt157 charger / battery-data source restore:

- Patch category: PROPER-FIX / POWER-DT source restore.
- Trigger:
  the user reported the phone likely stopped charging and disappeared twice due
  to battery level while being moved back to recovery.
- FACT:
  the latest available preboot pstore from attempt156 only showed normal
  shutdown callbacks for `qcom,qpnp-fg`, `qpnp-smbcharger`, and `qpnp-typec`;
  it did not contain a direct "charging disabled" charger failure.
- FACT:
  current 4.9 `.config` and `lineageos_nx549j_defconfig` both enable
  `CONFIG_QPNP_FG=y`, `CONFIG_QPNP_SMBCHARGER=y`, `CONFIG_QPNP_TYPEC=y`,
  PMIC revid, ADC, and thermal support, so this was not a missing-driver
  config issue.
- FACT:
  stock/highwaystar NX549 DTS uses
  `NX549/batterydata-ztemt-4v4-3000mah.dtsi` with `qcom,batt-id-kohm = <47>`
  and `qcom,max-voltage-uv = <4400000>`, but the 4.9 NX549 DTS was still using
  generic `batterydata-itech-3000mah.dtsi` and `batterydata-ascent-3450mAh.dtsi`
  profiles (`100k`/`60k`, 4.35V).
- Change:
  added `arch/arm64/boot/dts/qcom/nx549j/batterydata-ztemt-4v4-3000mah.dtsi`
  from the stock NX549 source and switched `msm8953-mtp-nx549j.dts`
  `mtp_batterydata` to include only that NX549 profile.
- Type-C note:
  stock NX549 deletes `qcom,ssmux-gpio` and related pinctrl because ssmux GPIO
  conflicts with Nubia Synaptics touch. The 4.9 NX549 DTB does not contain that
  GPIO/pinctrl path, only `ss-mux-supply`, so no Type-C GPIO change was made in
  this attempt.
- Build:
  `source build/make/envsetup.sh && lunch lineage_nx549j-userdebug &&
  mka bootimage -j4` completed successfully in `05:53`.
- Verification:
  `git diff --check` passed; decompiled DTB contains
  `qcom,batt-id-kohm = <0x2f>`, `qcom,max-voltage-uv = <0x432380>`, and
  `qcom,battery-type = "ztemt_lg_3000mah"`.
- Release:
  `/srv/forge/work/nx549j-preserve/release-attempt157-20260604-batterydata-ztemt`
- Artifact SHA-256:
  - `boot-batterydata-ztemt.img`:
    `bda1e9e9eea62d7df5a6c648c450722d4ac190657cf716277ef41909c97f8180`
  - `Image.gz-dtb`:
    `21d5842ca3da84a757df47da2280f9cb776858894a1ba9c7ae31ce2b14520d34`
  - `msm8953-mtp-nx549j.dtb`:
    `51ce8b893e98e2053ed46ba131d3ab9c876b3a12e35d53bf9e696a02b0eb3b9f`
- FACT:
  `sha256sum -c SHA256SUMS` passed in the release directory.
- Current device state:
  reverse ADB port `15038` currently exposes other devices
  (`711HEBSR277K5`, `91HEBNL163XD`) but not NX549J `30785d1a`; ports `15030`,
  `15031`, and `15034` also did not expose the NX549J target during this pass.
- Next when `30785d1a` returns to recovery:
  collect `/sys/class/power_supply/*` and charger/typec/fg dmesg before
  rebooting, then flash `boot-batterydata-ztemt.img` to the boot partition and
  retest whether USB/charge stability improves.
- Automation:
  added `scripts/nx549j-run-attempt157-batterydata.sh`. It refuses non-NX549J
  serials, requires recovery, captures `power_supply`, charger/typec/fg dmesg,
  and then delegates to `nx549j-flash-boot-with-bcb-fallback.sh`.
- Active watcher:
  tmux session `nx549j-attempt157-batterydata-watch` is scanning ports
  `15038 15031 15034 15030` with exact serial gate `30785d1a` and runner
  `scripts/nx549j-run-attempt157-batterydata.sh`.
- Active watcher log:
  `/srv/forge/work/nx549j-preserve/release-attempt157-20260604-batterydata-ztemt/runtime/wait-logs/watch-reverse-adb-20260604-2258-attempt157-batterydata.log`.
- Watcher first-cycle evidence:
  `15038` had only `711HEBSR277K5` and `91HEBNL163XD`; `15031` had only
  `L91HVCNS22KS5`; `15034` and `15030` had no ADB devices. The watcher reported
  `TARGET_SERIAL_MISSING` / `NO_ADB_DEVICES` and did not write to any device.
