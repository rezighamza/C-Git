# C-Git: A Git Clone in C++

C-Git is a learning project to build a simplified clone of the Git version control system from scratch in C++. The primary goal is to gain a deep understanding of Git's internal data model and mechanics, such as its content-addressable filesystem, object types (blobs, trees, commits), and the staging area (index).

---

## Features

This implementation currently supports the core, fundamental Git commands:

*   `c-git init`: Initializes a new, empty C-Git repository by creating the `.cgit` directory structure.
*   `c-git add <file>`: Adds a file to the staging area (the index), preparing it for the next commit.
*   `c-git commit -m "<message>"`: Creates a new commit object, snapshotting the staged files and linking it to the project's history.
*   `c-git hash-object <file>`: A "plumbing" command that takes a file, creates a blob object, and prints its SHA-1 hash.

---

## Getting Started

### Prerequisites

To build and run this project, you will need a C++ compiler and the following libraries installed:
*   A C++17 compliant compiler (e.g., GCC/G++)
*   CMake (version 3.10+)
*   OpenSSL (for SHA-1 hashing)
*   Zlib (for object compression)

On Arch Linux, you can install these with:
```bash
sudo pacman -S gcc cmake openssl zlib
```

### Building the Project

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/[your-username]/C-Git.git
    cd C-Git
    ```
2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```
3.  **Run CMake and build:**
    ```bash
    cmake ..
    make
    ```
    The `c-git` executable will be located in the `build` directory.

---

## Usage Example

Here is a quick example of how to use C-Git to track a new project.

```bash
# 1. Create a new directory for your project and cd into it
mkdir my_project
cd my_project

# 2. Initialize a C-Git repository
/path/to/C-Git/build/c-git init

# 3. Create a file and add it to the staging area
echo "Hello, C-Git!" > file1.txt
/path/to/C-Git/build/c-git add file1.txt

# 4. Commit the file
/path/to/C-Git/build/c-git commit -m "Add initial file"
```

The repository objects are stored in a format compatible with real Git. You can inspect them using `git --git-dir=.cgit cat-file -p <hash>`.

---


### Future Work

The following features are planned for future development:
- [ ] `c-git status`: Show the state of the working directory and staging area.
- [ ] `c-git log`: Display the commit history.
- [ ] `c-git checkout`: Switch branches or restore working tree files.
- [ ] `c-git branch`: List, create, or delete branches.
