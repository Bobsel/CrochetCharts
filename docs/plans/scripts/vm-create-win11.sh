#!/usr/bin/env bash
set -euo pipefail

VM_NAME="${VM_NAME:-crochet-win11-test}"
ISO_PATH="${ISO_PATH:-$HOME/ISOs/Win11_Eval.iso}"
MEMORY_MB="${MEMORY_MB:-4096}"
VCPUS="${VCPUS:-4}"
DISK_GB="${DISK_GB:-40}"
VARIANT="${VARIANT:-win11}"

# Force system-mode libvirt. In session mode swtpm runs as the user and cannot
# write to /var/lib/swtpm-localca (root-owned), so TPM setup fails.
export LIBVIRT_DEFAULT_URI="${LIBVIRT_DEFAULT_URI:-qemu:///system}"

if ! virsh list --all &>/dev/null; then
    echo "ERROR: cannot connect to $LIBVIRT_DEFAULT_URI." >&2
    echo "       Ensure your user is in the 'libvirt' group and you have a fresh" >&2
    echo "       login (not just 'newgrp') so the group is effective:" >&2
    echo "         id -nG | grep libvirt" >&2
    exit 1
fi

# osinfo-db on Ubuntu jammy (libosinfo 1.9) doesn't know win11; fall back to
# win10 — Win11 still installs, just without UEFI/TPM hints from osinfo (we
# pass those explicitly via --boot uefi and --tpm below).
if ! osinfo-query os short-id="$VARIANT" 2>/dev/null | grep -q "$VARIANT"; then
    if [[ "$VARIANT" == "win11" ]]; then
        echo "WARN: osinfo-db doesn't know 'win11' on this host; falling back to 'win10'." >&2
        VARIANT=win10
    fi
fi

if [[ ! -r "$ISO_PATH" ]]; then
    echo "ERROR: ISO not found or unreadable: $ISO_PATH" >&2
    echo "       Download from https://www.microsoft.com/en-us/evalcenter/" >&2
    exit 1
fi

if virsh dominfo "$VM_NAME" &>/dev/null; then
    echo "ERROR: VM '$VM_NAME' already exists." >&2
    echo "       Delete with: virsh destroy $VM_NAME; virsh undefine $VM_NAME --nvram --remove-all-storage" >&2
    exit 1
fi

echo "==> creating VM '$VM_NAME' from $ISO_PATH"
echo "    ${MEMORY_MB} MB RAM, ${VCPUS} vCPU, ${DISK_GB} GB disk, os-variant=${VARIANT}"

virt-install \
    --name "$VM_NAME" \
    --memory "$MEMORY_MB" \
    --vcpus "$VCPUS" \
    --cpu host-passthrough \
    --os-variant "$VARIANT" \
    --disk size="$DISK_GB",format=qcow2,bus=virtio \
    --cdrom "$ISO_PATH" \
    --network default,model=virtio \
    --graphics spice \
    --video qxl \
    --channel spicevmc \
    --boot uefi \
    --tpm backend.type=emulator,backend.version=2.0,model=tpm-crb \
    --sound ich9 \
    --memorybacking source.type=memfd,access.mode=shared \
    --noautoconsole

echo
echo "=== VM started ==="
echo "Open the console:  virt-viewer $VM_NAME"
echo
echo "NEXT STEPS (inside the Windows installer window):"
echo "  1. Walk through Windows 11 setup (partition, account, region, ~30 min)"
echo "  2. Skip Microsoft account: when prompted for internet, pick 'I don't have internet'"
echo "     (on recent builds: Shift+F10 → 'OOBE\\BYPASSNRO' → Enter → reboot)"
echo "  3. After first-boot desktop: install virtio-win tools (optional but faster)"
echo "     Attach virtio-win ISO via virt-manager and run virtio-win-guest-tools.exe"
echo "  4. Return here and run vm-share-artifacts.sh + vm-snapshot.sh"
