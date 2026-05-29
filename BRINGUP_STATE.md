# NX549J 4.9 Bring-up State

Last updated: 2026-05-29

## Objective

Bring the Nubia NX549J / Z11 mini S LineageOS 18.1 4.9 kernel to a bootable,
diagnosable state. The current priority is to recover an automatic reboot/reset
signal from the target 4.9 kernel, then use that signal to bracket how far
early boot gets before returning to pstore/ramoops or another recovery-readable
persistence path.

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
