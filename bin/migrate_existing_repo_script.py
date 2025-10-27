#!/usr/bin/env python3
"""
migrate_existing_repo.py
Migrate the current local repository to the GitHub organization 'holland-mechanics'.
- Does NOT keep the old remote: replaces 'origin' with the new HTTPS URL.
- Creates the GitHub repository if it does not exist.
- Pushes ALL branches and ALL tags.
- Uses HTTPS and (optionally) Git Credential Manager via gh.
"""

import os
import sys
import subprocess
from pathlib import Path

ORG = "holland-mechanics"

def run(cmd, cwd=None, check=True):
    print(f"\n$ {' '.join(cmd)}")
    try:
        subprocess.run(cmd, cwd=cwd, check=check)
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Command failed with exit code {e.returncode}")
        sys.exit(e.returncode)

def gh_repo_exists(full_name: str) -> bool:
    try:
        subprocess.run(["gh", "repo", "view", full_name],
                       check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return True
    except subprocess.CalledProcessError:
        return False

def is_git_repo(path: Path) -> bool:
    return (path / ".git").is_dir()

def main():
    print("=== Migrate existing local repo to GitHub org (no old remote kept) ===\n")
    cwd = Path.cwd()
    if not is_git_repo(cwd):
        print(f"[ERROR] No .git directory found in {cwd}. Run this script from the ROOT of your git repository.")
        sys.exit(1)

    repo_name = cwd.name.strip()
    print(f"Detected repository name from folder: '{repo_name}'")
    resp = input("Use this as GitHub repo name? [Y/n]: ").strip().lower()
    if resp == "n":
        repo_name = input("Enter desired GitHub repository name: ").strip()
        if not repo_name:
            print("[ERROR] Repository name cannot be empty.")
            sys.exit(1)

    full_name = f"{ORG}/{repo_name}"
    https_url = f"https://github.com/{full_name}.git"

    # Ensure gh is authenticated and git uses credential manager (optional but recommended)
    try:
        run(["gh", "auth", "status"])
        run(["git", "config", "--global", "credential.helper", "manager"], check=False)
        run(["gh", "auth", "setup-git"], check=False)
    except SystemExit:
        raise
    except Exception:
        print("[WARN] Could not verify or configure authentication helpers; continuing.")

    # Create the repo on GitHub if missing
    if gh_repo_exists(full_name):
        print(f"[INFO] GitHub repo already exists: https://github.com/{full_name}")
    else:
        run(["gh", "repo", "create", full_name, "--private"])

    # Replace origin with new HTTPS remote
    run(["git", "remote", "remove", "origin"], check=False)
    run(["git", "remote", "add", "origin", https_url])
    run(["git", "remote", "-v"])

    # Push all branches and all tags
    run(["git", "push", "--all", "origin"])
    run(["git", "push", "--tags", "origin"])

    print("\n=== Done ===")
    print(f"Repository is available at: https://github.com/{full_name}")
    print("If you need 'main' as default branch:")
    print("  git branch -M main && git push -u origin main")

if __name__ == '__main__':
    main()
