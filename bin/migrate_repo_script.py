#!/usr/bin/env python3
"""
migrate_repo.py
Interactive helper to mirror-clone a repository from local Gitea and publish branches+tags
to GitHub organization 'holland-mechanics' over HTTPS (avoids SSH issues).

Changes vs. previous:
- Work inside a dedicated subfolder '.mirror_work'
- Force-clean removal that fixes Windows read-only attributes
- Cleanup runs in a 'finally' to ensure deletion even on failures
"""

import json
import re
import sys
import os
import stat
import shutil
import subprocess
from pathlib import Path

ORG = "holland-mechanics"
ALLOWED_RE = re.compile(r"^[A-Za-z0-9._-]+$")

def run(cmd, cwd=None, check=True):
    print(f"\n$ {' '.join(cmd)}")
    try:
        subprocess.run(cmd, cwd=cwd, check=check)
    except subprocess.CalledProcessError as e:
        print(f"\n[ERROR] Command failed with exit code {e.returncode}")
        raise

def gh_repo_exists(full_name: str) -> bool:
    try:
        subprocess.run(
            ["gh", "repo", "view", full_name],
            check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
        )
        return True
    except subprocess.CalledProcessError:
        return False

def _handle_remove_readonly(func, path, exc):
    # Make read-only files writable and retry
    try:
        os.chmod(path, stat.S_IWRITE)
        func(path)
    except Exception:
        raise

def rmtree_force(path: Path):
    if path.exists():
        shutil.rmtree(path, onerror=_handle_remove_readonly)

def main():
    here = Path.cwd()
    json_path = here / "gitea_repos.json"
    if not json_path.exists():
        alt = here / "repos.json"
        if alt.exists():
            json_path = alt
        else:
            print(f"[ERROR] Cannot find '{json_path.name}' (or 'repos.json') in {here}")
            sys.exit(1)

    data = json.loads(json_path.read_text(encoding="utf-8"))
    if not isinstance(data, list):
        print("[ERROR] JSON must be a list of objects.")
        sys.exit(1)

    names = [item["name"] for item in data if "name" in item]
    unique_names = sorted(set(names), key=str.lower)

    print("=== Available repositories from gitea_repos.json ===")
    for n in unique_names:
        print(f" - {n}")
    print("")

    target = input("Type the repository name to migrate: ").strip()

    if not target:
        print("[ERROR] Name cannot be empty.")
        sys.exit(1)
    if not ALLOWED_RE.match(target):
        print("[ERROR] Invalid name. Allowed: letters, numbers, hyphen, underscore, dot. No spaces.")
        sys.exit(1)
    if target not in unique_names:
        print("[ERROR] Name not found in JSON list.")
        sys.exit(1)

    entry = next((x for x in data if x.get("name") == target), None)
    if not entry or not entry.get("ssh"):
        print("[ERROR] Selected repo has no 'ssh' URL in JSON.")
        sys.exit(1)

    ssh_source = entry["ssh"]

    # Work inside a dedicated subfolder
    work_root = here / ".mirror_work"
    temp_dir = work_root / f"{target}.git"

    # Start with a clean workspace
    rmtree_force(temp_dir)
    work_root.mkdir(exist_ok=True)
    try:
        print("\n=== Step 1: Mirror-clone from Gitea (bare) ===")
        run(["git", "clone", "--mirror", ssh_source, str(temp_dir)])

        print("\n=== Step 2: Ensure GitHub repo exists (private) ===")
        full_name = f"{ORG}/{target}"
        if gh_repo_exists(full_name):
            print(f"GitHub repo already exists: https://github.com/{full_name}")
        else:
            run(["gh", "repo", "create", full_name, "--private"])

        print("\n=== Step 3: Configure HTTPS credentials ===")
        try:
            run(["git", "config", "--global", "credential.helper", "manager"])
            run(["gh", "auth", "setup-git"])
        except Exception:
            print("[WARN] credential helper setup encountered an issue; continuing...")

        print("\n=== Step 4: Add HTTPS remote and push branches + tags ===")
        https_url = f"https://github.com/{full_name}.git"
        try:
            run(["git", "remote", "remove", "github"], cwd=temp_dir, check=False)
            run(["git", "remote", "add", "github", https_url], cwd=temp_dir)
            run(["git", "remote", "-v"], cwd=temp_dir)
            run(["git", "push", "--all", "github"], cwd=temp_dir)
            run(["git", "push", "--tags", "github"], cwd=temp_dir)
        except Exception:
            print("[ERROR] Push failed.")
    finally:
        print("\n=== Step 5: Cleanup temporary mirror folder ===")
        # Remove the repo folder; if work_root becomes empty, remove it too
        rmtree_force(temp_dir)
        # Try to remove work_root only if empty; if not empty, force-remove anyway
        try:
            work_root.rmdir()
            print(f"Removed empty folder: {work_root}")
        except OSError:
            # Not empty or still has artifacts; force remove
            rmtree_force(work_root)
        print("Cleanup completed.")

    print("\n=== Done ===")
    print(f"Repo URL: https://github.com/{full_name}")
    print("Branches and tags should now be visible.")

if __name__ == '__main__':
    main()
