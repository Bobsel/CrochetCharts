#!/usr/bin/env bash
set -euo pipefail

VM_NAME="${VM_NAME:-crochet-win11-test}"
SHARE_DIR="${SHARE_DIR:-$(cd "$(dirname "$0")/../../.." && pwd)/artifacts}"
TAG="${TAG:-crochet-artifacts}"

if ! virsh dominfo "$VM_NAME" &>/dev/null; then
    echo "ERROR: VM '$VM_NAME' does not exist." >&2
    exit 1
fi

if [[ ! -d "$SHARE_DIR" ]]; then
    echo "ERROR: share source directory does not exist: $SHARE_DIR" >&2
    exit 1
fi

STATE=$(virsh domstate "$VM_NAME")
if [[ "$STATE" == "running" ]]; then
    echo "VM is running; attaching share live + persistent."
    SCOPE=(--live --config)
else
    echo "VM is off; attaching share to persistent config only."
    SCOPE=(--config)
fi

echo "==> attaching virtiofs share"
echo "    host:  $SHARE_DIR"
echo "    tag:   $TAG"
echo "    VM:    $VM_NAME"

TMP_XML=$(mktemp)
trap 'rm -f "$TMP_XML"' EXIT

cat > "$TMP_XML" <<EOF
<filesystem type='mount' accessmode='passthrough'>
  <driver type='virtiofs'/>
  <source dir='$SHARE_DIR'/>
  <target dir='$TAG'/>
</filesystem>
EOF

virsh attach-device "$VM_NAME" "$TMP_XML" "${SCOPE[@]}"

echo
echo "=== attached ==="
echo
echo "NEXT STEPS (inside Windows):"
echo "  1. Install the virtio-win guest tools if not done yet (virtio-win.iso)."
echo "  2. Install 'WinFsp' from https://winfsp.dev/rel/ (required for virtiofs on Windows)."
echo "  3. Start the VirtioFS service: services.msc -> 'VirtioFsSvc' -> Start + Automatic."
echo "  4. The share appears as a new drive letter automatically (usually Z:)."
echo "     If not: 'virtio-fs.exe -d -t $TAG -m Z:'"
echo "  5. Browse Z:\\ to see CrochetCharts-*.exe from the host."
