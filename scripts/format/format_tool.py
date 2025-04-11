#!/usr/bin/env python3
import os
import subprocess
import argparse
from pathlib import Path

def find_clang_format():
    """Try to find clang-format in common locations or system PATH."""
    possible_paths = [
        "/usr/local/bin/clang-format",
        "/usr/bin/clang-format",
        "clang-format",
    ]
    for path in possible_paths:
        try:
            subprocess.run([path, "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            return path
        except (subprocess.CalledProcessError, FileNotFoundError):
            continue
    raise FileNotFoundError("clang-format not found. Please install clang-format.")

def find_clang_format_config(config_path=None):
    """Find the .clang-format config file."""
    if config_path:
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Specified .clang-format config not found: {config_path}")
        return os.path.abspath(config_path)
    
    # Search in current directory and parent directories
    current_dir = Path.cwd()
    for parent in [current_dir] + list(current_dir.parents):
        config_file = parent / ".clang-format"
        if config_file.exists():
            return str(config_file)
    
    raise FileNotFoundError(".clang-format config file not found. Please create one or specify with --clang-format-config")

def should_skip(path, exclude_dirs, exclude_files):
    """Check if the path should be skipped based on exclusion rules."""
    path_str = str(path)
    # Check if any excluded dir is part of the path
    for exclude_dir in exclude_dirs:
        if exclude_dir in path_str:
            return True
    # Check if the file is in the excluded files list
    if path_str in exclude_files:
        return True
    return False

def format_files(target_paths, exclude_dirs, exclude_files, clang_format_path, config_path, dry_run=False):
    """Format files using clang-format."""
    cpp_extensions = {'.cpp', '.hpp', '.h', '.c', '.cc', '.cxx', '.hxx', '.m', '.mm'}
    formatted_files = []
    skipped_files = []

    for target_path in target_paths:
        if not os.path.exists(target_path):
            print(f"Warning: Path does not exist: {target_path}")
            continue

        if os.path.isfile(target_path):
            # Handle single file
            path = Path(target_path)
            if path.suffix in cpp_extensions:
                if should_skip(path, exclude_dirs, exclude_files):
                    skipped_files.append(str(path))
                else:
                    formatted_files.append(str(path))
                    if not dry_run:
                        subprocess.run([clang_format_path, "-i", f"-style=file:{config_path}", str(path)], check=True)
        else:
            # Handle directory
            for root, dirs, files in os.walk(target_path):
                # Skip excluded directories during traversal
                dirs[:] = [d for d in dirs if not should_skip(Path(root) / d, exclude_dirs, exclude_files)]
                
                for file in files:
                    file_path = Path(root) / file
                    if file_path.suffix in cpp_extensions:
                        if should_skip(file_path, exclude_dirs, exclude_files):
                            skipped_files.append(str(file_path))
                        else:
                            formatted_files.append(str(file_path))
                            if not dry_run:
                                subprocess.run([clang_format_path, "-i", f"-style=file:{config_path}", str(file_path)], check=True)

    return formatted_files, skipped_files

def main():
    parser = argparse.ArgumentParser(description="Format C++ files using clang-format.")
    parser.add_argument("paths", nargs="+", help="Files or directories to format")
    parser.add_argument("-e", "--exclude-dirs", nargs="*", default=[], help="Directories to exclude from formatting")
    parser.add_argument("-f", "--exclude-files", nargs="*", default=[], help="Files to exclude from formatting")
    parser.add_argument("-c", "--config", help="Path to .clang-format config file")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be formatted without actually doing it")
    args = parser.parse_args()

    try:
        clang_format_path = find_clang_format()
        print(f"Using clang-format at: {clang_format_path}")
        
        config_path = find_clang_format_config(args.config)
        print(f"Using .clang-format config at: {config_path}")
        
        # Convert exclude paths to absolute paths for consistent comparison
        exclude_dirs = [os.path.abspath(d) for d in args.exclude_dirs]
        exclude_files = [os.path.abspath(f) for f in args.exclude_files]
        
        formatted_files, skipped_files = format_files(
            args.paths, exclude_dirs, exclude_files, clang_format_path, config_path, args.dry_run
        )

        print("\nFormatted files:")
        for file in formatted_files:
            print(f"  {file}")

        if skipped_files:
            print("\nSkipped files:")
            for file in skipped_files:
                print(f"  {file}")

        print(f"\nTotal formatted: {len(formatted_files)}")
        print(f"Total skipped: {len(skipped_files)}")

        if args.dry_run:
            print("\nDry run completed. No files were actually modified.")
    except Exception as e:
        print(f"Error: {e}")
        exit(1)

if __name__ == "__main__":
    main()
