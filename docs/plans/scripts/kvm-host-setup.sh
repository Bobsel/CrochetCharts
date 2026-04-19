#!/usr/bin/env bash
set -euo pipefail

if [[ $EUID -ne 0 ]]; then
    echo "Re-running under sudo..." >&2
    exec sudo -E "$0" "$@"
fi

PACKAGES=(
    virt-manager
    qemu-kvm
    libvirt-daemon-system
    libvirt-clients
    bridge-utils
    ovmf
    swtpm
    swtpm-tools
    virt-viewer
)

TARGET_USER="${SUDO_USER:-$USER}"

echo "==> apt update + install ${#PACKAGES[@]} packages"
apt-get update -q
apt-get install -y "${PACKAGES[@]}"

# virtiofsd has no standalone package on Ubuntu jammy; it ships inside
# qemu-system-common (pulled in by qemu-kvm) at /usr/lib/qemu/virtiofsd.
# Verify it's there so vm-share-artifacts.sh doesn't fail later with a
# confusing libvirt error.
if [[ ! -x /usr/lib/qemu/virtiofsd && ! -x /usr/libexec/virtiofsd && -z "$(command -v virtiofsd)" ]]; then
    echo "WARNING: virtiofsd binary not found. virtiofs share won't work." >&2
    echo "         Expected at /usr/lib/qemu/virtiofsd (from qemu-system-common)." >&2
fi

echo "==> adding $TARGET_USER to libvirt,kvm"
usermod -aG libvirt,kvm "$TARGET_USER"

echo "==> enabling libvirtd"
systemctl enable --now libvirtd

echo "==> preparing ~/ISOs"
install -d -o "$TARGET_USER" -g "$TARGET_USER" "/home/$TARGET_USER/ISOs"

echo
echo "=== done ==="
systemctl is-active libvirtd
ls -la /dev/kvm
echo
echo "NEXT: log out and log back in (or run 'newgrp libvirt' in a fresh shell)"
echo "      then download the Windows 11 eval ISO into ~/ISOs/Win11_Eval.iso"
echo "      from https://www.microsoft.com/en-us/evalcenter/"
