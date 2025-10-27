#!/usr/bin/env python3
# Purpose:
# - Initialize a local Git repo
# - Create a minimal commit with README.md and .gitignore only (no 'git add --all')
# - Set default branch to 'master'
# - Create lightweight tag v0.0.0 (if not already present)
# - Create and push a GitHub repo under the Open-Source-Model-Railway-Electronics org
#
# Requirements:
# - Git and GitHub CLI ('gh') must be installed and authenticated (`gh auth login`).
# - You must have permission to create repositories in the target organization.

import subprocess
from pathlib import Path
import sys
import shutil
import os

ORG = "Open-Source-Model-Railway-Electronics"
DEFAULT_BRANCH = "master"
TAG = "v0.0.0"

IGNORE_PATTERNS = [
    "date.h",
    "io.cpp",
    "io.h",
    "*.hex",
    "*.bin",
    "*.elf",
    "*.eep",
    "*cache",
    "*.zip",
    "*.kicad_prl",
    "fp-info-cache",
    "/release/",
]

def run(cmd, check=True):
    return subprocess.run(cmd, text=True, capture_output=True, check=check)

def ensure_git_repo():
    if not (Path(".") / ".git").exists():
        run(["git", "init"])

def ensure_branch_master():
    run(["git", "branch", "-M", DEFAULT_BRANCH])

def ensure_tag(tag: str):
    res = run(["git", "tag", "--list", tag], check=False)
    if res.returncode == 0 and tag not in res.stdout.split():
        run(["git", "tag", tag])

def write_file(path: Path, content: str):
    if not path.exists():
        path.write_text(content, encoding="utf-8")

def ensure_gitignore(patterns):
    p = Path(".gitignore")
    existing = set()
    if p.exists():
        existing = {line.rstrip("\n") for line in p.read_text(encoding="utf-8").splitlines()}
    new_lines = []
    for pat in patterns:
        if pat not in existing:
            new_lines.append(pat)
    if not p.exists():
        p.write_text("\n".join(patterns) + "\n", encoding="utf-8")
    elif new_lines:
        with p.open("a", encoding="utf-8") as f:
            for line in new_lines:
                f.write(line + "\n")

def stage_and_commit():
    # Only add README.md and .gitignore
    files_to_add = []
    if Path("README.md").exists():
        files_to_add.append("README.md")
    if Path(".gitignore").exists():
        files_to_add.append(".gitignore")
    if not files_to_add:
        print("Nothing to commit (no README.md or .gitignore).")
        sys.exit(1)
    run(["git", "add"] + files_to_add)
    # Commit if there is something staged
    status = run(["git", "diff", "--cached", "--name-only"])
    if status.stdout.strip():
        run(["git", "commit", "-m", "first commit"])

def repo_name_from_cwd():
    return Path.cwd().name

def create_remote_and_push(repo_name: str):
    if shutil.which("gh") is None:
        print("\nGitHub CLI (gh) not found.")
        print("Manual steps:")
        print(f"  gh repo create {ORG}/{repo_name} --public")
        print(f"  git remote add origin https://github.com/{ORG}/{repo_name}.git")
        print(f"  git push -u origin {DEFAULT_BRANCH}")
        print(f"  git push origin {TAG}")
        return

    # Create repo in org and push current branch; only README.md and .gitignore are in the commit
    # --source=. will push the current commit after creating the remote
    cmd = [
        "gh", "repo", "create", f"{ORG}/{repo_name}",
        "--public",
        "--source=.",
        f"--remote=origin",
        "--push",
        "--confirm",
    ]
    res = run(cmd, check=False)
    if res.returncode != 0:
        # If it exists already, just set remote and push
        print("gh repo create failed, attempting to set remote and push...", file=sys.stderr)
        run(["git", "remote", "remove", "origin"], check=False)
        run(["git", "remote", "add", "origin", f"https://github.com/{ORG}/{repo_name}.git"], check=False)
        run(["git", "push", "-u", "origin", DEFAULT_BRANCH], check=False)
    # Push the tag explicitly
    run(["git", "push", "origin", TAG], check=False)

def main():
    ensure_git_repo()
    ensure_branch_master()

    # Create minimal README and .gitignore
    write_file(Path("README.md"), f"# {repo_name_from_cwd()}\n\nInitial repository skeleton.\n")
    ensure_gitignore(IGNORE_PATTERNS)

    # Only add README.md and .gitignore, then commit
    stage_and_commit()

    # Ensure tag exists locally
    ensure_tag(TAG)

    # Create/org repo and push
    create_remote_and_push(repo_name_from_cwd())

    print("\nRepository initialized locally and on GitHub (organization).")
    print(f"Branch: {DEFAULT_BRANCH}")
    print(f"Tag:    {TAG}\n")
    print("You can now add files manually and push them when ready.")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print("\nCommand failed:")
        print(" ", " ".join(e.cmd))
        print("STDOUT:\n", e.stdout)
        print("STDERR:\n", e.stderr)
        sys.exit(e.returncode)
