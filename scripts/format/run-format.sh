#!/bin/bash

# run-format.sh - A shell wrapper for automatic C++ code formatting in Git repositories

# Default parameters
REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null)
PYTHON_SCRIPT="$REPO_ROOT/scripts/format/format_tool.py"
EXCLUDE_DIRS=()
EXCLUDE_FILES=()
CLANG_FORMAT_CONFIG=""
DRY_RUN=false
AUTO_GIT_MODE=true

# Display help information
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  -h, --help                 Show this help message"
    echo "  -e, --exclude-dirs DIRS    Comma-separated list of directories to exclude"
    echo "  -f, --exclude-files FILES  Comma-separated list of files to exclude"
    echo "  -c, --config PATH          Path to .clang-format config file"
    echo "  -n, --dry-run              Show what would be formatted without making changes"
    echo "  -m, --manual-mode          Disable auto Git mode and specify targets manually"
    echo
    echo "Examples:"
    echo "  $0                         Format all C++ files in Git repository"
    echo "  $0 -e build,third_party    Exclude build and third_party directories"
    echo "  $0 -c ~/myconfig/.clang-format -n  Dry run with custom config"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            show_help
            exit 0
            ;;
        -e|--exclude-dirs)
            IFS=',' read -r -a EXCLUDE_DIRS <<< "$2"
            shift 2
            ;;
        -f|--exclude-files)
            IFS=',' read -r -a EXCLUDE_FILES <<< "$2"
            shift 2
            ;;
        -c|--config)
            CLANG_FORMAT_CONFIG="$2"
            shift 2
            ;;
        -n|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -m|--manual-mode)
            AUTO_GIT_MODE=false
            shift
            ;;
        *)
            echo "Error: Unknown option $1"
            show_help
            exit 1
            ;;
    esac
done

# Build Python command
PYTHON_CMD=("python3" "$PYTHON_SCRIPT")

# Add targets based on mode
if [ "$AUTO_GIT_MODE" = true ]; then
    # Check if we're in a Git repository
    if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
        echo "Error: Not in a Git repository. Use -m for manual mode."
        exit 1
    fi
    
    # Get all C/C++ files in the repository
    CPP_FILES=$(git ls-files | grep -E '\.(cpp|hpp|h|c|cc|cxx|hxx|m|mm)$')
    
    if [ -z "$CPP_FILES" ]; then
        echo "No C++ files found in the Git repository."
        exit 0
    fi
    
    # Add files to command
    while IFS= read -r file; do
        PYTHON_CMD+=("$file")
    done <<< "$CPP_FILES"
else
    # Manual mode - use current directory as target
    PYTHON_CMD+=("$REPO_ROOT")
    PYTHON_CMD+=("--config" "$REPO_ROOT/.clang-format")
fi

# Add exclude directories
if [ ${#EXCLUDE_DIRS[@]} -gt 0 ]; then
    PYTHON_CMD+=("--exclude-dirs")
    for dir in "${EXCLUDE_DIRS[@]}"; do
        PYTHON_CMD+=("$dir")
    done
fi

# Add exclude files
if [ ${#EXCLUDE_FILES[@]} -gt 0 ]; then
    PYTHON_CMD+=("--exclude-files")
    for file in "${EXCLUDE_FILES[@]}"; do
        PYTHON_CMD+=("$file")
    done
fi

# Add config file path
if [ -n "$CLANG_FORMAT_CONFIG" ]; then
    PYTHON_CMD+=("--config" "$CLANG_FORMAT_CONFIG")
fi

# Add dry-run parameter
if [ "$DRY_RUN" = true ]; then
    PYTHON_CMD+=("--dry-run")
fi

# Execute Python script
echo "Running command: ${PYTHON_CMD[*]}"
exec "${PYTHON_CMD[@]}"
