# HTTP Server with Gzip Compression

This project is a simple HTTP server implemented in C++ that supports concurrent connections and gzip compression. It includes functionality to handle client requests, compress data using gzip, and convert compressed data to a hexadecimal string.

## Features

- **Concurrent Connections**: Uses multithreading to handle multiple client connections simultaneously.
- **Gzip Compression**: Compresses response data using the gzip format.
- **Hexadecimal Encoding**: Converts compressed data to a hexadecimal string for easy readability and debugging.
- **File Handling**: Checks if files exist in the specified directory and reads their contents.

## Requirements

- C++17 or later
- zlib library
- gzstream library (for gzip compression)
- A POSIX-compliant operating system (e.g., Linux, macOS)

## Installation

1. **Install `zlib`**:
   ```sh
   sudo apt-get install zlib1g-dev  # For Debian-based systems
   brew install zlib                # For macOS
