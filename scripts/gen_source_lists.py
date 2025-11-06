#!/bin/env python3
#
# Usage:
# gen_sourse_lists.py path/to/starting/folder

import os
import sys
from collections import defaultdict

my_extensions = [".c",".h",".cpp",".hpp",".s",".asm"]

def list_filtered_files_by_folder(start_path, extensions):
    """Return a dict where keys = folder paths, values = list of matching files."""
    files_by_folder = defaultdict(list)
    extensions = [ext.lower() for ext in extensions]

    for root, _, files in os.walk(start_path):
        # Collect only files that match extensions
        matching = [
            name for name in files
            if os.path.splitext(name)[1].lower() in extensions
        ]
        if matching:
            # Use relative folder path for cleaner output
            relative_folder = os.path.relpath(root, start_path)
            files_by_folder[relative_folder] = matching

    return files_by_folder

def main():
    if len(sys.argv) != 2:
        print("Usage: python gen_sourse_lists.py <start_folder>")
        print("Example: python gen_sourse_lists.py /path/to/start")
        sys.exit(1)

    start_folder = os.path.normpath(sys.argv[1])

    print(f"Starting from a folder: {start_folder}")

    grouped_name = os.path.basename(os.path.abspath(start_folder))
    grouped_name_upper = grouped_name.upper()

    cmakefile = "sources_list.cmake"

    if not os.path.isdir(start_folder):
        print(f"Error: '{start_folder}' is not a valid directory.")
        sys.exit(1)

    files_by_folder = list_filtered_files_by_folder(start_folder, my_extensions)
    output_file = os.path.join(start_folder, cmakefile)

    CMCLD = "${CMAKE_CURRENT_LIST_DIR}"
    total_files = 0
    groups = []
    with open(output_file, "w", encoding="utf-8") as f:
        f.write("message(STATUS \"Processing ${CMAKE_CURRENT_LIST_FILE}...\")\n")
        for folder, files in sorted(files_by_folder.items()):
            folder_display = f"{grouped_name_upper}" if folder == "." else folder
            folder_display = folder_display.upper()
            folder_display = folder_display.replace("/", "_")
            folder_display = folder_display.replace("\\", "_")
            folder_display = folder_display.replace("-", "_")

#            folder = "" if folder == "." else folder

            group_name = f"GROUP_{folder_display}_SRC"
            groups.append(group_name)
            f.write(f"SET({group_name}\n")
            for name in sorted(files):
                if folder == ".":
                    f.write(f"\t{CMCLD}/{name}\n")
                else:
                    f.write(f"\t{CMCLD}/{folder}/{name}\n")
                total_files += 1
            finalizer = f") # of {group_name}\n\n"
            f.write(finalizer)

# include
        f.write(f"\nSET(INCLUDE_DIRS_{grouped_name_upper}_LIST\n")
        for folder, _ in sorted(files_by_folder.items()):
            if folder == ".":
                f.write(f"\t{CMCLD}\n")
            else:
                f.write(f"\t{CMCLD}/{folder}\n")
        f.write(f") # of INCLUDE_DIRS_{grouped_name_upper}_LIST\n")
# alltogether
        alltogether = f"\nSET({grouped_name_upper}_SOURCES_LIST\n"
        f.write(alltogether)
        for gn in groups:
            f.write(f"\t${{{gn}}}\n")
        f.write(f") # of {grouped_name_upper}_SOURCES_LIST")

    print(f"Found {total_files} matching files in {len(files_by_folder)} folders.")
    print(f"Grouped file list saved to: {output_file}")

if __name__ == "__main__":
    main()
