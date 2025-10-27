#!/usr/bin/env python3
"""
create_new_repo.py
Interactive helper to create a new Git repository locally and publish it to the
GitHub organization 'holland-mechanics' using gh.exe and git.

Requirements:
- Python 3.8+
- git and gh available in PATH
- You are authenticated with gh for the right org permissions
"""

import os
import re
import sys
import subprocess
from pathlib import Path

ORG = "holland-mechanics"
ALLOWED_RE = re.compile(r"^[A-Za-z0-9._-]+$")  # only letters, numbers, dot, underscore, hyphen

def run(cmd, cwd=None, check=True):
    print(f"\n$ {' '.join(cmd)}")
    try:
        subprocess.run(cmd, cwd=cwd, check=check)
    except subprocess.CalledProcessError as e:
        print(f"\n[ERROR] Command failed with exit code {e.returncode}")
        sys.exit(e.returncode)

def clear_screen():
    # Cross-platform clear
    os.system('cls' if os.name == 'nt' else 'clear')

def interactive_add(cwd: Path):
    """Recursively ask user per file whether to add it, ignoring .git folder."""
    files = [
        f for f in cwd.rglob("*")
        if f.is_file()
        and not f.name.startswith(".")
        and ".git" not in f.parts
    ]
    if not files:
        print("[WARN] No files found to add.")
        return

    for fpath in files:
        rel_path = fpath.relative_to(cwd)
        choice = input(f"Add '{rel_path}' to repo? [Y/n]: ").strip().lower()
        if choice == "y":
            run(["git", "add", str(rel_path)], cwd=cwd)
        elif choice == "n":
            print(f"Skipped: {rel_path}")
        else:
            print("Please type Y or N.")
            while True:
                choice = input(f"Add '{rel_path}' to repo? [Y/n]: ").strip().lower()
                if choice == "y":
                    run(["git", "add", str(rel_path)], cwd=cwd)
                    break
                elif choice == "n":
                    print(f"Skipped: {rel_path}")
                    break

def main():
    print("=== Create a new GitHub repository in the holland-mechanics org ===")
    print("Name rules: only letters, numbers, hyphen (-), underscore (_), and dot (.)")
    print("No spaces; max recommended length: 100 characters.\n")

    name = input("Enter repository name: ").strip()

    if not name:
        print("[ERROR] Name cannot be empty.")
        sys.exit(1)
    if not ALLOWED_RE.match(name):
        print("[ERROR] Invalid name. Allowed: letters, numbers, hyphen, underscore, dot. No spaces.")
        sys.exit(1)

    workdir = Path.cwd()
    # Check if .git already exists
    if (workdir / ".git").exists():
        print(f"[ERROR] This folder is already a git repository.")
        sys.exit(1)

    # Warn if folder is not empty
    files = [f for f in os.listdir(workdir) if not f.startswith(".")]
    if files:
        print(f"[WARN] Current folder is not empty. Existing files: {files}")

    # 1) Initialize repo in current directory
    run(["git", "init"], cwd=workdir)

    # 2) Create README.md if not exists
    readme_path = workdir / "README.md"
    if not readme_path.exists():
        readme_path.write_text("hello world\n", encoding="utf-8")

    # 3) Interactively add files
    interactive_add(workdir)

    # 4) Commit
    run(["git", "commit", "-m", "Initial commit"], cwd=workdir)

    # 5) Create GitHub repo and push
    run(["gh", "repo", "create", f"{ORG}/{name}", "--private", "--source", ".", "--remote", "origin", "--push"], cwd=workdir)

    print("\n=== Done ===")
    print(f"Repo URL: https://github.com/{ORG}/{name}")
    print(f"Local path: {workdir}")
    print("Tip: 'git branch -M main && git push -u origin main' if you want 'main' as default branch.")

if __name__ == '__main__':
    main()
