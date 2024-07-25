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
   sudo pacman -S zlib              # For Arch-Linux
   sudo apt-get install zlib1g-dev  # For Debian-based systems
   brew install zlib                # For macOS
2.**Cloning the repository**:
   ```sh
   git clonehttps://github.com/kunal763/SimpleHttpServer.git
   cd SimpleHttpServer
   ```
3.Compile The Server
   ```sh
   g++ -std=c++17 -o http_server src/server.cpp -lz
```

## Usage

### Starting the Server

Run the compiled server executable. You can specify the directory where the files are stored using the `--directory` flag.

   ```sh
.   /http_server --directory /path/to/files
   ```
### Example Usage

1. **Start the Server**:

   ```sh
   ./http_server --directory /var/www/html
   ```
   This command starts the HTTP server and sets `/var/www/html` as the directory where the server will look for files to serve.
   
2. **Access the Server**:

   Open a web browser or use a tool like curl to make a request to the server:
   ```sh
   curl http://localhost:4221/index.html
   ```
   Replace `index.html` with the path to any file you want to access in the specified directory. The server will return the requested file,
