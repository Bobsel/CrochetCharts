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
    virtiofsd
    virt-viewer
)

TARGET_USER="${SUDO_USER:-$USER}"

echo "==> apt update + install ${#PACKAGES[@]} packages"
apt-get update -q
apt-get install -y "${PACKAGES[@]}"

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
