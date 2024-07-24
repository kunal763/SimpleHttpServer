#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
void handle_client(int client_socket)
{
  char buffer[4096];
  std::string request;
  int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if (bytes_received > 0)
  {
    buffer[bytes_received] = '\0'; // Null-terminate the buffer
    request = buffer;

    // Find the first line of the request (request line)
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

        std::string path_cmd="";
        
        if(path.length()>6)path_cmd = path.substr(1, 5);
        
        std::string not_found_msg = "HTTP/1.1 404 Not Found\r\n\r\n";
        std::string pass_message = "HTTP/1.1 200 OK\r\n\r\n";
        if (path.substr(1,5) == "echo/")
        {
          std::string client_str = path.substr(6);

          std::string server_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(client_str.length());
          server_response += "\r\n\r\n" + client_str;
          send(client_socket, server_response.c_str(), server_response.length(), 0);
        }
        else if(path.find("user-agent/")!=std::string::npos){
          size_t ua_pos = request.find("User-Agent:");
          if (ua_pos != std::string::npos) {
            size_t ua_end = request.find("\r\n", ua_pos);
            std::string user_agent = request.substr(ua_pos + 11, ua_end - ua_pos - 11);
            std::string server_resp="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "+std::to_string(user_agent.length());
            server_resp+="\r\n\r\n"+user_agent;
            send(client_socket,server_resp.c_str(),server_resp.length(),0);
            
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

int main(int argc, char **argv)
{
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  // std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  //
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }
  //
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
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

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  int client = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
  std::cout << "Client connected\n";

  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
  handle_client(client);

  // send(client,message.c_str(),message.length(),0);

  close(server_fd);

  return 0;
}
