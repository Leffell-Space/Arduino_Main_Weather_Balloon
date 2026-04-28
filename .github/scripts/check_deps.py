#!/usr/bin/env python3
"""Checks that the library list in README.md matches compile-sketch.yml.

If they differ, updates README.md and regenerates install_libraries.py.
Writes 'needs_update=true' to GITHUB_OUTPUT when changes are made.
"""

import os
import re
import sys

import yaml
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

BUILD_WORKFLOW = REPO_ROOT / ".github" / "workflows" / "compile-sketch.yml"
README = REPO_ROOT / "README.md"
INSTALL_SCRIPT = REPO_ROOT / "install_libraries.py"

# Libraries that are built-in to the Arduino IDE but not in the build workflow.
# These are always kept in the README for reference and skipped in the install script.
TRULY_BUILTIN = ["SPI", "Wire"]

# Named libraries from the build workflow that are pre-bundled with Arduino IDE.
# They appear in the workflow (needed for CI) but are marked as "(built-in)" in README
# and skipped by the install script.
WORKFLOW_BUILTIN = {"SD"}


# ---------------------------------------------------------------------------
# Parsing
# ---------------------------------------------------------------------------


def parse_build_workflow_libs():
    """Return (named_libs, url_libs) from compile-sketch.yml."""
    with open(BUILD_WORKFLOW) as fh:
        data = yaml.safe_load(fh)

    steps = data["jobs"]["compile-sketches"]["steps"]
    libs_str = None
    for step in steps:
        if step.get("uses", "").startswith("arduino/compile-sketches"):
            libs_str = step["with"]["libraries"]
            break

    if libs_str is None:
        sys.exit("ERROR: arduino/compile-sketches step not found in compile-sketch.yml")

    libs_data = yaml.safe_load(libs_str)
    named = [lib["name"] for lib in libs_data if "name" in lib]
    urls = [lib["source-url"] for lib in libs_data if "source-url" in lib]
    return named, urls


def parse_readme_libs():
    """Return (named, builtin, urls) parsed from the README installation section.

    named   – plain names (no built-in marker, no URL)
    builtin – names with the "(built-in)" marker
    urls    – URLs extracted from "(from <url>)" entries
    """
    content = README.read_text()
    match = re.search(
        r"Install the necessary libraries[^\n]*\n((?:   - [^\n]*\n)+)",
        content,
    )
    if not match:
        return [], [], []

    named, builtin, urls = [], [], []
    for line in match.group(1).splitlines():
        stripped = line.strip()
        if not stripped.startswith("- "):
            continue
        entry = stripped[2:]
        url_match = re.search(r"\(from (https://\S+)\)", entry)
        if url_match:
            urls.append(url_match.group(1).rstrip(")"))
        elif "(built-in)" in entry:
            name = re.sub(r"\s*\(built-in\)", "", entry).strip()
            builtin.append(name)
        else:
            named.append(entry.strip())

    return named, builtin, urls


# ---------------------------------------------------------------------------
# Comparison
# ---------------------------------------------------------------------------


def libs_match(wf_named, wf_urls, readme_named, readme_builtin, readme_urls):
    """Return True when README libraries match the build workflow."""
    # All names the workflow declares should appear somewhere in README
    # (either as a regular entry or as "(built-in)").  SPI/Wire live only in
    # README and are fine to ignore on the workflow side.
    readme_all_names = set(readme_named) | set(readme_builtin) - set(TRULY_BUILTIN)
    if set(wf_named) != readme_all_names:
        return False
    if set(wf_urls) != set(readme_urls):
        return False
    return True


# ---------------------------------------------------------------------------
# README update
# ---------------------------------------------------------------------------


def _lib_name_from_url(url):
    """Derive a display name from a git URL."""
    name = url.rstrip("/").split("/")[-1]
    if name.endswith(".git"):
        name = name[:-4]
    return name


def _build_readme_lib_lines(wf_named, wf_urls):
    """Return the replacement library list lines for README."""
    lines = []
    # Workflow-declared built-ins first
    for name in wf_named:
        if name in WORKFLOW_BUILTIN:
            lines.append(f"   - {name} (built-in)")
    # Truly built-in (not in workflow)
    for name in TRULY_BUILTIN:
        lines.append(f"   - {name} (built-in)")
    # Regular named libs
    for name in wf_named:
        if name not in WORKFLOW_BUILTIN:
            lines.append(f"   - {name}")
    # URL-based libs
    for url in wf_urls:
        display = _lib_name_from_url(url)
        lines.append(f"   - {display} (from {url})")
    return lines


def update_readme(wf_named, wf_urls):
    """Rewrite the README library list to match the build workflow.

    Returns True when the file was actually changed.
    """
    content = README.read_text()
    new_lines = _build_readme_lib_lines(wf_named, wf_urls)
    new_block = "\n".join(new_lines) + "\n"

    new_content = re.sub(
        r"(Install the necessary libraries[^\n]*\n)((?:   - [^\n]*\n)+)",
        lambda m: m.group(1) + new_block,
        content,
    )
    if new_content == content:
        return False
    README.write_text(new_content)
    print("  Updated README.md")
    return True


# ---------------------------------------------------------------------------
# Install-script generation
# ---------------------------------------------------------------------------


def generate_install_script(wf_named, wf_urls):
    """Write install_libraries.py with the current library list.

    Returns True when the file content changed.
    """
    install_named = [n for n in wf_named if n not in WORKFLOW_BUILTIN and n not in TRULY_BUILTIN]
    named_repr = repr(install_named)
    urls_repr = repr(wf_urls)

    # Use a plain string without nested f-string placeholders so that the
    # {…} inside the embedded script are literal and not expanded here.
    script_lines = [
        "#!/usr/bin/env python3",
        '"""',
        "Cross-platform Arduino library installer for this project.",
        "Auto-generated by the check-dependencies workflow — do not edit manually.",
        "",
        "Usage:",
        "  python install_libraries.py",
        "  python install_libraries.py --path /custom/arduino/libraries",
        '"""',
        "",
        "import argparse",
        "import os",
        "import platform",
        "import subprocess",
        "import sys",
        "from pathlib import Path",
        "",
        "# Libraries to install by name via arduino-cli (or Arduino Library Manager)",
        f"NAMED_LIBRARIES = {named_repr}",
        "",
        "# Libraries to install by cloning from git",
        f"URL_LIBRARIES = {urls_repr}",
        "",
        "",
        "def get_arduino_libraries_path():",
        '    """Return the default Arduino libraries path for the current OS."""',
        "    system = platform.system()",
        "    home = Path.home()",
        '    if system == "Windows":',
        '        docs = Path(os.environ.get("USERPROFILE", str(home))) / "Documents"',
        '    elif system == "Darwin":  # macOS',
        '        docs = home / "Documents"',
        "    else:  # Linux",
        '        return home / "Arduino" / "libraries"',
        '    return docs / "Arduino" / "libraries"',
        "",
        "",
        "def _run(cmd):",
        "    print(f\"    $ {' '.join(str(c) for c in cmd)}\")",
        "    result = subprocess.run(cmd, capture_output=True, text=True)",
        "    if result.stdout.strip():",
        "        print(result.stdout.rstrip())",
        "    if result.returncode != 0 and result.stderr.strip():",
        "        print(f\"    stderr: {result.stderr.rstrip()}\", file=sys.stderr)",
        "    return result.returncode == 0",
        "",
        "",
        "def _has_arduino_cli():",
        "    try:",
        '        subprocess.run(["arduino-cli", "version"], capture_output=True, check=True)',
        "        return True",
        "    except (subprocess.CalledProcessError, FileNotFoundError):",
        "        return False",
        "",
        "",
        "def install_named_libraries(libs_path):",
        '    """Install named libraries via arduino-cli, or print manual instructions."""',
        "    if not NAMED_LIBRARIES:",
        "        return",
        '    print("\\nInstalling named libraries...")',
        "    if _has_arduino_cli():",
        "        for name in NAMED_LIBRARIES:",
        "            print(f\"  Installing {name} via arduino-cli...\")",
        '            _run(["arduino-cli", "lib", "install", name])',
        "    else:",
        '        print("  arduino-cli not found.")',
        '        print("  Please install the following via the Arduino Library Manager:")',
        "        for name in NAMED_LIBRARIES:",
        "            print(f\"    - {name}\")",
        "",
        "",
        "def install_url_libraries(libs_path):",
        '    """Clone or update git-based libraries into the Arduino libraries folder."""',
        "    if not URL_LIBRARIES:",
        "        return",
        "    libs_path.mkdir(parents=True, exist_ok=True)",
        "    print(f\"\\nInstalling URL-based libraries into: {libs_path}\")",
        "    for url in URL_LIBRARIES:",
        '        repo_name = url.rstrip("/").split("/")[-1]',
        '        if repo_name.endswith(".git"):',
        "            repo_name = repo_name[:-4]",
        "        dest = libs_path / repo_name",
        "        if dest.exists():",
        "            print(f\"  Updating {repo_name}...\")",
        '            _run(["git", "-C", str(dest), "pull"])',
        "        else:",
        "            print(f\"  Cloning {repo_name} from {url}...\")",
        '            _run(["git", "clone", url, str(dest)])',
        "",
        "",
        "def main():",
        "    parser = argparse.ArgumentParser(",
        '        description="Install Arduino libraries required by this project."',
        "    )",
        "    parser.add_argument(",
        '        "--path",',
        "        type=Path,",
        "        default=None,",
        '        help="Arduino libraries folder (default: auto-detected per OS)",',
        "    )",
        "    args = parser.parse_args()",
        "",
        "    libs_path = args.path or get_arduino_libraries_path()",
        "    print(f\"Arduino libraries directory: {libs_path}\")",
        "",
        "    install_named_libraries(libs_path)",
        "    install_url_libraries(libs_path)",
        "",
        '    print("\\nInstallation complete.")',
        "",
        "",
        'if __name__ == "__main__":',
        "    main()",
        "",
    ]
    new_content = "\n".join(script_lines)
    old_content = INSTALL_SCRIPT.read_text() if INSTALL_SCRIPT.exists() else ""
    if new_content == old_content:
        return False
    INSTALL_SCRIPT.write_text(new_content)
    print("  Generated install_libraries.py")
    return True


# ---------------------------------------------------------------------------
# GitHub Actions output helper
# ---------------------------------------------------------------------------


def set_output(name, value):
    output_file = os.environ.get("GITHUB_OUTPUT")
    if output_file:
        with open(output_file, "a") as fh:
            fh.write(f"{name}={value}\n")
    else:
        print(f"[output] {name}={value}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    print("Parsing build workflow libraries...")
    wf_named, wf_urls = parse_build_workflow_libs()
    print(f"  Named : {wf_named}")
    print(f"  URLs  : {wf_urls}")

    print("Parsing README libraries...")
    readme_named, readme_builtin, readme_urls = parse_readme_libs()
    print(f"  Named   : {readme_named}")
    print(f"  Builtin : {readme_builtin}")
    print(f"  URLs    : {readme_urls}")

    changes_made = False

    if not libs_match(wf_named, wf_urls, readme_named, readme_builtin, readme_urls):
        print("Libraries do not match — updating README.md...")
        changes_made |= update_readme(wf_named, wf_urls)
    else:
        print("README libraries are already in sync.")

    print("Regenerating install_libraries.py...")
    changes_made |= generate_install_script(wf_named, wf_urls)

    set_output("needs_update", "true" if changes_made else "false")

    if changes_made:
        print("Changes detected — a PR will be created.")
    else:
        print("Everything is up to date — no PR needed.")


if __name__ == "__main__":
    main()
