# Windows VM Acceptance Test Plan

**Status:** Draft
**Last updated:** 2026-04-19
**Scope:** Set up a QEMU/KVM + Windows 10/11 evaluation VM on the Ubuntu host and
define the acceptance-test procedure for the cross-compiled Windows installer
(`artifacts/CrochetCharts-<ver>-win64.exe`).

This is the "proper" runtime test that must pass before cutting a release tag.
Wine smoke-testing (documented separately) catches obvious build breakage but
cannot certify release quality.

---

## 1. Goal

Verify that the NSIS installer produced by `task package:win`:

1. Installs correctly on stock Windows 10 and/or 11 with no prerequisites.
2. Launches without errors or missing-DLL dialogs.
3. Can open, edit, save, and export a crochet chart.
4. Uninstalls cleanly (no leftover files, registry entries, Start Menu shortcuts).

Pass criteria are a concrete checklist in section 6.

## 2. Prerequisites

### Hardware / OS

- Ubuntu 22.04+ on x86_64.
- CPU with Intel VT-x or AMD-V (any desktop/laptop CPU from ~2010 onwards).
- ≥ 8 GB RAM free while the VM is running (VM itself uses 4 GB).
- ≥ 60 GB free disk space (40 GB for the VM disk + ISO + headroom).
- Network access to `microsoft.com/evalcenter`.

### Verify virt support

```bash
grep -E 'vmx|svm' /proc/cpuinfo > /dev/null && echo "virt supported" || echo "MISSING"
kvm-ok 2>/dev/null || true
```

If `vmx`/`svm` is absent, enable it in BIOS/UEFI (Intel: "Intel Virtualization
Technology"; AMD: "SVM Mode"). Without it, the VM will work but at 1/10 speed
via pure emulation.

### Evaluation ISO

Microsoft offers time-limited evaluation ISOs at
<https://www.microsoft.com/en-us/evalcenter/evaluate-windows-10-enterprise> and
<https://www.microsoft.com/en-us/evalcenter/evaluate-windows-11-enterprise>.

- Both are 90-day evaluations — no product key, no activation.
- Windows 11 enterprise eval: ~5 GB download.
- Pick Windows 11 unless you specifically need to test Windows 10.

Save to `~/ISOs/Win11_Eval.iso` (any path works — referenced below).

## 3. Host setup (one-time, ~15 min)

### 3.1 Install the stack

```bash
sudo apt-get update
sudo apt-get install -y virt-manager qemu-kvm libvirt-daemon-system libvirt-clients \
                        bridge-utils \
                        ovmf \
                        swtpm swtpm-tools \
                        virt-viewer
```

> **Note on `virtiofsd`:** Ubuntu jammy (22.04) does **not** ship `virtiofsd` as a standalone package — it lives inside `qemu-system-common` at `/usr/lib/qemu/virtiofsd` (pulled in transitively by `qemu-kvm`). Newer Debian/Ubuntu releases split it out as a separate apt package. If you adapt this for a 24.04+ host, add `virtiofsd` to the apt list. UEFI firmware (`ovmf`) is required because Win11 boots EFI; TPM emulation (`swtpm`+`swtpm-tools`) because Win11 requires TPM 2.0.

### 3.2 Add your user to the virt groups

```bash
sudo usermod -aG libvirt,kvm "$USER"
```

Log out / log in (or `newgrp libvirt` in a new shell).

### 3.3 Verify

```bash
virsh list --all             # should succeed without sudo, empty list is fine
systemctl is-active libvirtd # should print "active"
```

## 4. Create the VM (one-time, ~30-45 min)

### 4.1 Through virt-manager (GUI, recommended first time)

```bash
virt-manager
```

1. **File → New Virtual Machine → Local install media (ISO)**.
2. Point at `~/ISOs/Win11_Eval.iso`. The dialog auto-detects "Microsoft Windows 11".
3. **Memory / CPU:** 4 GB RAM, 4 vCPUs (adjust up if your host has >= 32 GB / 16 cores; down if less).
4. **Storage:** create a 40 GB qcow2 image (default location `/var/lib/libvirt/images/`).
5. **Name:** `crochet-win11-test`. Tick "Customize configuration before install".
6. **In the customize screen:**
   - **Overview → Firmware:** `UEFI x86_64` (required by Windows 11; pick an `OVMF_CODE.fd` entry).
   - **CPUs → Configuration:** tick "Copy host CPU configuration" (enables VT-x/SVM passthrough).
   - **Add Hardware → TPM:** Windows 11 requires TPM 2.0. Type `Emulated`, Model `CRB`, Version `2.0`.
   - **Boot Options:** ensure the CD-ROM is first.
7. Click **Begin Installation**.
8. Walk through the Windows installer normally. Skip the Microsoft account step
   by using "I don't have internet" → "Continue with limited setup" (offline
   local account is fine for testing).

### 4.2 Or via `virt-install` (scripted, reproducible)

```bash
virt-install \
  --name crochet-win11-test \
  --memory 4096 \
  --vcpus 4 \
  --cpu host-passthrough \
  --os-variant win11 \
  --disk size=40,format=qcow2 \
  --cdrom "$HOME/ISOs/Win11_Eval.iso" \
  --network default \
  --graphics spice \
  --video qxl \
  --boot uefi \
  --tpm backend.type=emulator,backend.version=2.0,model=tpm-crb \
  --sound ich9
```

Then open the console:

```bash
virt-viewer crochet-win11-test
```

### 4.3 Inside the guest — post-install setup

Once Windows desktop appears:

1. **virtio-win drivers** (optional but improves performance):
   ```
   # On host:
   sudo apt-get install -y virtio-win
   # Then attach /usr/share/virtio-win/virtio-win.iso to the VM's CD drive
   # and run virtio-win-guest-tools.exe inside Windows.
   ```
2. **Filesystem sharing** — the easiest way to get `.exe` from host to VM:
   - **Option A (simplest):** drop `artifacts/CrochetCharts-0.0.0-win64.exe`
     on a web server and download it inside the VM with Edge.
   - **Option B:** shared folder via virtiofs. In virt-manager: Add Hardware →
     Filesystem → Driver `virtiofs`, Source `/home/stefan/devboost/private/CrochetCharts/artifacts`,
     Target `crochet-artifacts`. Inside Windows after virtio-win tools install,
     mount it via `net use Z: \\crochet-artifacts`.
   - **Option C:** `scp` from the host into the VM if you enable OpenSSH server
     in Windows Settings → Apps → Optional features.
3. **Snapshot now.** Before installing CrochetCharts — creates a rollback point
   for repeatable test cycles:
   ```bash
   virsh snapshot-create-as crochet-win11-test clean-install \
       --description "Windows 11 installed, virtio drivers, no app"
   ```

## 5. Running the acceptance test (per release candidate, ~10-15 min)

### 5.1 Preparation on host

```bash
# Start from a clean state each time
virsh snapshot-revert crochet-win11-test clean-install
virsh start crochet-win11-test
virt-viewer crochet-win11-test

# Build the latest Windows artifact (on host)
docker run --rm -v "$PWD:/src" -w /src crochet-win-build task package:win:inner
```

### 5.2 Transfer + install

Inside the VM, copy `CrochetCharts-<ver>-win64.exe` from whichever share you
configured. Then:

1. Double-click the installer. Accept UAC prompt.
2. Step through NSIS: license, destination, Start Menu, install.
3. Leave "Run CrochetCharts" ticked → click **Finish**.
4. App should launch within ~5 s on a warm VM.

### 5.3 Functional checks (see section 6)

Go through the checklist. Note defects with screenshots (`Win+Shift+S`).

### 5.4 Uninstall check

- Control Panel → Programs → Uninstall CrochetCharts.
- Confirm the install directory under `C:\Program Files\CrochetCharts\` is gone.
- Confirm Start Menu shortcut is gone.
- `reg query "HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\CrochetCharts*"`
  should return no matches (run in `cmd` as administrator).

### 5.5 Tear-down

```bash
virsh shutdown crochet-win11-test
# Next test cycle starts with snapshot-revert again; no state leaks.
```

## 6. Acceptance checklist

Tick each before signing off on a beta/release candidate. Run in the order
listed; each step depends on the previous one not failing.

### Install
- [ ] Installer runs without UAC errors or unsigned-binary warnings beyond the
      standard SmartScreen prompt.
- [ ] Install completes in < 30 s on a 4 vCPU VM.
- [ ] Target directory (`C:\Program Files\CrochetCharts\`) contains
      `bin\CrochetCharts.exe` and `docs\homepage.html`.
- [ ] Start Menu entry `CrochetCharts` is present.
- [ ] Desktop shortcut (if CPACK_CREATE_DESKTOP_LINKS is honored by the NSIS
      installer) is present.

### First launch
- [ ] App window appears within 10 s.
- [ ] No "missing DLL" dialog.
- [ ] No Qt error dialog (`qWarning` / `qFatal` output).
- [ ] Splash screen renders correctly (SVG-based, see `src/splashscreen.cpp`).
- [ ] Default stitch library loads (look for stitch icons in the palette).

### Basic editing
- [ ] `File → New Chart` opens a new blank chart canvas.
- [ ] Click a stitch in the palette, click on the canvas — stitch is placed.
- [ ] Zoom in/out via mouse wheel + Ctrl works.
- [ ] Pan via middle-mouse works.
- [ ] Undo (`Ctrl+Z`) removes the last stitch.
- [ ] Redo (`Ctrl+Y`) re-adds it.

### File I/O
- [ ] `File → Save` writes a `.pattern` file to Documents.
- [ ] Close the file, reopen it via `File → Open` — canvas restores correctly.
      (Regression surface: `file_v1.cpp` / `file_v2.cpp` / `filefactory.cpp`.)
- [ ] Open one of the sample patterns from an earlier build (if any are shipped).

### Export
- [ ] `File → Export → PDF` produces a readable PDF.
- [ ] `File → Export → PNG` / `SVG` produce non-empty images.
- [ ] Exported files match the canvas visually.

### Properties / library dialogs
- [ ] Properties dock shows selected-stitch metadata.
- [ ] `Stitches → Manage Stitch Library` dialog opens without crash. Note: a
      known FIXME lives at `src/stitchlibraryui.cpp:590` — "crashes when
      removing sets with sts in the master list". **Don't remove a stitch set**
      unless specifically testing that fix.

### Networking (updater)
- [ ] `Help → Check for Updates` either shows a dialog or fails gracefully. The
      Windows build was compiled with `-no-openssl` (Qt4 + OpenSSL 3 are
      incompatible), so any HTTPS check will fail at the TLS step. Pass
      criterion: app does **not** crash on this operation. Plain-HTTP updater
      URLs work; HTTPS returns a clear error.

### Uninstall
- [ ] Add/Remove Programs → Uninstall removes every file + shortcut.
- [ ] Re-running the installer after uninstall works (no leftover registry
      blocking re-install).

### Cosmetic (optional)
- [ ] Fonts render smoothly (no missing-glyph boxes).
- [ ] HiDPI scaling — if the VM is set to 1.5x or 2x, the UI should not be
      blurry. Qt 4.8 pre-dates modern DPI awareness, so some blurriness is
      acceptable; outright broken layouts are not.

## 7. Time budget

| Phase | Frequency | Time |
|-------|-----------|------|
| Install virt-manager + ISO download | once | ~20 min (mostly ISO download) |
| Create VM + install Windows | once | ~30-45 min (Windows setup) |
| Configure snapshot `clean-install` | once | ~2 min |
| One acceptance run | per RC | ~10-15 min |
| Re-run after fix (snapshot-revert → test) | per fix | ~5-10 min |

## 8. Troubleshooting

### `KVM: unable to open /dev/kvm` or virt-manager says "no connection"
- Check `/dev/kvm` permissions: `ls -la /dev/kvm` — should show group `kvm`.
- Confirm your user is in the `kvm` and `libvirt` groups (`groups $USER`).
- `sudo systemctl restart libvirtd`.

### Windows 11 installer complains about TPM or Secure Boot
- TPM: verify emulated TPM 2.0 is attached (section 4.1 step 6).
- Secure Boot: use the `OVMF_CODE.secboot.fd` firmware variant instead of plain
  `OVMF_CODE.fd`.
- Fallback: use the **Windows 10** eval ISO instead — same process, no TPM/SB
  requirement.

### VM is unusably slow
- Confirm "Copy host CPU configuration" is enabled (hardware virt passthrough).
- Give the VM more RAM (8 GB) or vCPUs (8).
- Install virtio-win drivers (disk + network) inside Windows for drastic
  improvement.

### `CrochetCharts.exe` fails to launch with "entry point not found" or missing DLL
- Should not happen — the MXE build is statically linked. If it does, MXE
  generated a shared build instead of static. Re-check
  `.devcontainer/win/Dockerfile` → `MXE_TARGET=x86_64-w64-mingw32.static`.

### Updater crashes (not just errors, actually crashes)
- The cross-compile dropped OpenSSL from Qt. The updater's `updater.cpp:168`
  already has a known "crash on cancel" FIXME (see `AGENTS.md`). If it crashes
  immediately when opened, we need to either:
  1. `#ifdef` the HTTPS call path out of the Windows build, or
  2. Switch MXE Qt to build against OpenSSL 1.1 (older MXE commit or custom
     package).
- Document which path you picked in a follow-up plan.

## 9. Automation scripts

The inline commands in sections 3-5 are wrapped in four helper scripts under
`docs/plans/scripts/`. Run them in this order:

| Script | When | Needs sudo | What it does |
|--------|------|------------|--------------|
| `kvm-host-setup.sh` | once | yes (auto `exec sudo`) | apt install all packages, add you to `libvirt` + `kvm`, enable `libvirtd`, create `~/ISOs/` |
| `vm-create-win11.sh` | once | no | `virt-install` with UEFI + TPM 2.0 + host-passthrough CPU + memfd shared memory (for virtiofs) |
| `vm-share-artifacts.sh` | once, after VM created | no | attach `./artifacts/` as virtiofs share named `crochet-artifacts` |
| `vm-snapshot.sh create\|revert\|list\|delete` | on demand | no | manage the `clean-install` snapshot |

Environment overrides (defaults in parens): `VM_NAME` (`crochet-win11-test`),
`ISO_PATH` (`~/ISOs/Win11_Eval.iso`), `MEMORY_MB` (4096), `VCPUS` (4),
`DISK_GB` (40), `SNAPSHOT_NAME` (`clean-install`),
`SHARE_DIR` (`<repo>/artifacts`), `TAG` (`crochet-artifacts`).

### Flow from zero to test-ready

```bash
# on host, repo root
docs/plans/scripts/kvm-host-setup.sh       # prompts for sudo password
# log out / log in so group membership takes effect
# download Win11_Eval.iso into ~/ISOs/
docs/plans/scripts/vm-create-win11.sh
# walk through Windows 11 installer in virt-viewer (~30 min)
# install virtio-win guest tools + WinFsp inside the VM
docs/plans/scripts/vm-share-artifacts.sh
docs/plans/scripts/vm-snapshot.sh create
```

For each release candidate afterwards:

```bash
docs/plans/scripts/vm-snapshot.sh revert   # back to clean Windows
docker run --rm -v "$PWD:/src" -w /src crochet-win-build task package:win:inner
# install + run CrochetCharts-<ver>-win64.exe inside the VM per §6 checklist
```

## 10. Future work

- **Automate smoke-test** with `winrm` or `psexec` driving the VM from the
  host (install + launch + screenshot + diff). Enables CI without a Windows
  runner.
- **Shrink the VM image** by using Differencing disks after snapshot so repeat
  cycles don't grow the backing file.
- **Publish a base VM image** to a team-internal share so contributors don't
  each build their own Windows install from scratch.
- **Signed installer:** add `signtool` step to the Windows build once a code
  signing cert is in place. Tested in this VM with the Windows SDK's
  `signtool.exe`.
