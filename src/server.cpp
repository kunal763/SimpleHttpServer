#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void handle_client(int client_socket, const std::string &client_ip, const std::string &directory)
{
  char buffer[4096];
  std::string request;
  int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if (bytes_received > 0)
  {
    buffer[bytes_received] = '\0'; // Null-terminate the buffer
    request = buffer;

    size_t pos = request.find("\r\n");
    if (pos != std::string::npos)
    {
      std::string request_line = request.substr(0, pos);

      // Split the request line by spaces
      size_t method_end = request_line.find(' ');
      size_t path_end = request_line.find(' ', method_end + 1);

      if (method_end != std::string::npos && path_end != std::string::npos)
      {
        std::string method = request_line.substr(0, method_end);
        std::string path = request_line.substr(method_end + 1, path_end - method_end - 1);
        std::string not_found_msg = "HTTP/1.1 404 Not Found\r\n\r\n";
        std::string pass_message = "HTTP/1.1 200 OK\r\n\r\n";
        if (method == "GET")
        {
          if (path.substr(0, 6) == "/echo/")
          {
            std::string client_str = path.substr(6);

            std::string server_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(client_str.length());
            server_response += "\r\n\r\n" + client_str;
            send(client_socket, server_response.c_str(), server_response.length(), 0);
          }
          else if (path.find("/user-agent") != std::string::npos)
          {
            size_t ua_pos = request.find("User-Agent: ");
            if (ua_pos != std::string::npos)
            {
              size_t ua_end = request.find("\r\n", ua_pos);
              std::string user_agent = request.substr(ua_pos + 12, ua_end - ua_pos - 12);
              std::string server_resp = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(user_agent.length());
              server_resp += "\r\n\r\n" + user_agent;
              send(client_socket, server_resp.c_str(), server_resp.length(), 0);
            }
          }
          else if (path.find("/files/") != std::string::npos)
          {
            std::string file_name = path.substr(7);
            std::string file_path = directory + file_name;
            if (fs::exists(file_path))
            {
              auto file_size = fs::file_size(file_path);
              // I am now reading the file
              std::ifstream file(file_path, std::ios::binary);
              if (file)
              {
                std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                std::string resp = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " + std::to_string(file_size) + "\r\n\r\n";
                resp += file_contents;
                std::cout << "File exists: " << file_path << std::endl;
                send(client_socket, resp.c_str(), resp.length(), 0);
              }
            }
            else
            {
              send(client_socket, not_found_msg.c_str(), not_found_msg.length(), 0);
            }
          }
          else if (path != "/")
            send(client_socket, not_found_msg.c_str(), not_found_msg.length(), 0);
          else
            send(client_socket, pass_message.c_str(), pass_message.length(), 0);
        }
        if (method=="POST")
        {
            if (path.find("/files/") != std::string::npos)
          {
            std::string file_name = path.substr(7);
            std::string file_path = directory + file_name;
            std::cout<<"this is post method\n";
            size_t pos_of_content=request.find("application/octet-stream\r\n\r\n")+28;
            std::string content=request.substr(pos_of_content);
            std::string resp="HTTP/1.1 201 Created\r\n\r\n";
            std::ofstream file(file_path);
            if(file){
              file<<content;
              send(client_socket,resp.c_str(),resp.length(),0);
              file.close();
            }
            

          }
        }
        
        // Send a basic HTTP response
      }
    }
  }

  close(client_socket);
}

int main(int argc, char **argv)
{
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string directory;

  // Parse command-line arguments
  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "--directory" && i + 1 < argc)
    {
      directory = argv[i + 1];
    }
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  int reuse = 1;
  std::cout << "Waiting for a client to connect...\n";
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 10;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  std::cout << "Server is listening on port 4221\n";

  while (true)
  {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0)
    {
      std::cerr << "Failed to accept connection\n";
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::string client_ip_str = client_ip;
    std::cout << "Accepted connection from " << client_ip_str << std::endl;

    // Create a new thread to handle the client connection and detach it
    std::thread(handle_client, client_socket, client_ip_str, directory).detach();
  }

  close(server_fd);
  return 0;
}
