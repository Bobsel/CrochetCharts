#!/usr/bin/env bash
set -euo pipefail

VM_NAME="${VM_NAME:-crochet-win11-test}"
SNAPSHOT_NAME="${SNAPSHOT_NAME:-clean-install}"

usage() {
    cat <<EOF >&2
Usage: $0 <command>

Commands:
  create   Create snapshot '$SNAPSHOT_NAME' of VM '$VM_NAME'
  revert   Revert VM to snapshot '$SNAPSHOT_NAME' (destructive; shuts VM off first)
  list     List all snapshots of VM '$VM_NAME'
  delete   Delete snapshot '$SNAPSHOT_NAME'

Environment overrides: VM_NAME, SNAPSHOT_NAME
EOF
    exit 1
}

[[ $# -eq 1 ]] || usage

if ! virsh dominfo "$VM_NAME" &>/dev/null; then
    echo "ERROR: VM '$VM_NAME' does not exist." >&2
    exit 1
fi

case "$1" in
    create)
        echo "==> creating snapshot '$SNAPSHOT_NAME' of '$VM_NAME'"
        virsh snapshot-create-as "$VM_NAME" "$SNAPSHOT_NAME" \
            --description "Clean Windows install, no test artefacts yet"
        virsh snapshot-list "$VM_NAME"
        ;;
    revert)
        STATE=$(virsh domstate "$VM_NAME")
        if [[ "$STATE" == "running" ]]; then
            echo "==> shutting down VM before revert"
            virsh shutdown "$VM_NAME" || true
            for _ in {1..30}; do
                [[ "$(virsh domstate "$VM_NAME")" == "shut off" ]] && break
                sleep 2
            done
            [[ "$(virsh domstate "$VM_NAME")" == "shut off" ]] || virsh destroy "$VM_NAME"
        fi
        echo "==> reverting to '$SNAPSHOT_NAME'"
        virsh snapshot-revert "$VM_NAME" "$SNAPSHOT_NAME"
        echo "==> starting VM"
        virsh start "$VM_NAME"
        ;;
    list)
        virsh snapshot-list "$VM_NAME"
        ;;
    delete)
        virsh snapshot-delete "$VM_NAME" "$SNAPSHOT_NAME"
        ;;
    *)
        usage
        ;;
esac
