# Version Control System (jit)

A lightweight, Git-inspired version control system implemented in C++ as part of CSCE 2211 - Applied Data Structures course at AUC.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Custom Data Structures](#custom-data-structures)
- [Architecture](#architecture)
- [Building the Project](#building-the-project)
- [Usage](#usage)
- [Commands](#commands)
- [Project Structure](#project-structure)

##  Overview

JIT is a version control system that mimics core Git functionality. It demonstrates practical applications of fundamental data structures including hash maps, vectors, deques, and trees in building a real-world system.

The project implements Git's internal object model (blobs, trees, commits) and provides essential version control operations like staging files, creating commits, viewing history, and computing diffs.

## Features

- **Repository Initialization**: Create new `.jit` repositories
- **File Staging**: Add files and directories to the staging area
- **Commit Creation**: Save snapshots of your project with commit messages
- **Commit History**: View the log of all commits
- **Diff Computation**: Calculate differences between commits and files
- **Branch Management**: Create and switch between branches
- **Checkout**: Restore working directory to specific commits
- **Status Tracking**: View repository and staging area status
- **Object Storage**: Efficient content-addressable storage system

## Custom Data Structures

All core data structures are implemented from scratch without using STL containers:

### Vector (`src/vector.h`)
- Dynamic array implementation with automatic resizing
- Growth factor: 2x capacity when full
- Supports: `push_back`, `pop_back`, `pop_front`, sorting, iterators
- Used throughout the project for dynamic collections

### HashMap (`src/hashmap.h`)
- Hash table using chaining for collision resolution
- MurmurHash3 32-bit hash function for key hashing
- Automatic resizing when load factor exceeds threshold
- Supports generic key-value pairs with iterator support
- Used for: object storage, file tracking, index management

### Deque (`src/deque.h`)
- Double-ended queue with circular buffer implementation
- Supports: `push_front`, `push_back`, `pop_front`, `pop_back`
- Used for efficient queue operations in diff algorithms

## Architecture

### Core Components

#### GitObjects (`src/gitobjects.h`)
Implements Git's internal object model:
- **Blob**: Stores file content
- **Tree**: Represents directory structure with entries
- **TreeEntry**: Individual entries in a tree (files/subdirectories)
- **Commit**: Contains tree hash, parent commits, author, message, timestamp

#### ObjectStore (`src/objectstore.h`)
- Content-addressable storage using SHA-256 hashing
- Stores and retrieves Git objects by hash
- Handles serialization/deserialization
- Implements `retrieveLog()` for commit history
- Implements `reconstruct()` for checkout operations

#### IndexStore (`src/index.h`)
- Manages staging area
- Tracks files to be committed
- Converts staged files into tree objects
- Persists index state to disk

#### Refs (`src/refs.h`)
- Manages branches and HEAD pointer
- Resolves symbolic references
- Updates branch pointers on commit
- Handles branch creation and switching

#### ArgParser (`src/argparser.h`)
- Custom command-line argument parser
- Supports commands, options, and arguments
- Provides help message generation
- Type-safe argument binding

#### Helpers (`src/helpers.h`)
- SHA-256 hash computation
- String manipulation utilities
- File I/O operations
- Path handling utilities

#### Diff (`src/diff.cpp`)
- Myers diff algorithm implementation
- Line-by-line comparison of files
- Generates unified diff format output

## 🔨 Building the Project

### Prerequisites
- CMake 3.24 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Git (for cloning the repository)

### Build Steps

```bash
# Clone the repository
git clone https://github.com/ziad-eliwa/Distributed-Version-Control-System.git
cd ADSProject

# Create build directory
mkdir -p build
cd build

# Generate build files
cmake ..

# Compile the project
make

# The executable will be at build/jit
```

## 💻 Usage

After building, the `jit` executable will be in the `build` directory.

### Basic Workflow

```bash
# Navigate to your project directory
cd /path/to/your/project

# Initialize a new repository
../build/jit init

# Add files to staging area
../build/jit add file.txt
../build/jit add .  # Add all files in current directory

# Commit changes
../build/jit commit -m "Initial commit"

# View commit history
../build/jit log

# Check repository status
../build/jit status

# View differences
../build/jit diff  # Between HEAD and staging area
```

##  Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `help` | Show this help message | `jit help` |
| `init` | Initialize a repository | `jit init` |
| `add <path>` | Add file to the staging area | `jit add file.txt` |
| `commit -m <msg>` | Create a new commit | `jit commit -m "message"` |
| `log` | Display the log of the commits | `jit log` |
| `diff` | Computes the differences between files | `jit diff <commit-hash/file-name>` |
| `status` | Shows tracked and untracked files | `jit status` |
| `checkout <ref>` | Switches to a branch or commit | `jit checkout main` |
| `branch <name>` | Creates a branch | `jit branch feature` |
| `merge <branch>` | Merge two branches together | `jit merge feature` |

## 📁 Project Structure

```
ADSProject/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Entry point and CLI implementation
├── README.md               # This file
├── src/                    # Source files
│   ├── argparser.cpp/h     # Command-line parser
│   ├── deque.h             # Deque implementation
│   ├── diff.cpp            # Diff algorithm
│   ├── gitobjects.h        # Git object model
│   ├── hashmap.cpp/h       # HashMap implementation
│   ├── helpers.cpp/h       # Utility functions
│   ├── index.h             # Staging area management
│   ├── objectstore.cpp/h   # Object storage system
│   ├── refs.cpp/h          # Branch/HEAD management
│   └── vector.h            # Vector implementation
├── tests/                  # Unit tests
│   ├── CMakeLists.txt
│   ├── test_hashmap.cpp    # HashMap tests
│   └── test_vector.cpp     # Vector tests
├── build/                  # Build artifacts (generated)
├── dummy/                  # Test directory
└── report/                 # Project documentation
```

This project demonstrates:
1. **Data Structures**: Practical implementation of vectors, hash maps, and deques
2. **Software Engineering**: Modular design, separation of concerns, clean architecture
3. **Algorithms**: Hashing, sorting, diff algorithms, tree traversal
4. **Systems Programming**: File I/O, serialization, content-addressable storage
5. **Version Control**: Understanding Git internals and distributed systems

## License

This project is part of academic coursework at the American University in Cairo (AUC).

---

**Course**: CSCE 2211 - Applied Data Structures  
**Institution**: The American University in Cairo  
**Year**: 2025-2026
