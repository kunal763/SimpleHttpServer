#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

void handle_client(int client_socket, const std::string& client_ip) {
    char buffer[4096];
    std::string request;
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Null-terminate the buffer
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
        else if (path != "/")
          send(client_socket, not_found_msg.c_str(), not_found_msg.length(), 0);
        else
          send(client_socket, pass_message.c_str(), pass_message.length(), 0);
        // Send a basic HTTP response
      }
    }
  }

  close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4221);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind to port 4221" << std::endl;
        return -1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port 4221" << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_size);
        if (client_socket == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::string client_ip_str = client_ip;

        std::cout << "Accepted connection from " << client_ip_str << std::endl;

        // Create a new thread to handle the client connection
        std::thread(handle_client, client_socket, client_ip_str).detach();
    }

    close(server_socket);
    return 0;
}
