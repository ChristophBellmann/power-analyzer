#!/usr/bin/env python3
"""Verify preserved canonical artifacts; optionally audit the original source checkout."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, help='Original SpectrumAnalyzer checkout')
    args = parser.parse_args()
    manifest = json.loads((ROOT / 'docs/source/spectrum-analyzer/migration-manifest.json').read_text())
    errors = []
    actual = subprocess.check_output(['git', '-C', str(ROOT / 'components/esp-dsp'), 'rev-parse', 'HEAD'], text=True).strip()
    if actual != manifest['esp_dsp_commit']:
        errors.append('ESP-DSP commit differs from migration pin')
    for entry in manifest['files']:
        if 'destination' in entry:
            target = ROOT / entry['destination']
            if not target.is_file() or sha(target) != entry['destination_sha256']:
                errors.append('Destination differs: ' + entry['destination'])
        if args.source:
            source = subprocess.check_output(['git', '-C', str(args.source), 'show', manifest['source_commit'] + ':' + entry['source']])
            if hashlib.sha256(source).hexdigest() != entry['source_sha256']:
                errors.append('Source differs: ' + entry['source'])
    if args.source:
        tracked = set(subprocess.check_output(['git', '-C', str(args.source), 'ls-tree', '-r', '--name-only', manifest['source_commit']], text=True).splitlines())
        accounted = {e['source'] for e in manifest['files']}
        if tracked != accounted:
            errors.append('Source revision/file inventory differs from migration audit')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f"Verified {len(manifest['files'])} source entries, canonical artifact hashes and ESP-DSP pin.")


if __name__ == '__main__':
    main()
