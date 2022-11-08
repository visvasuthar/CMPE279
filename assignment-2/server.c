// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pwd.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{

  if(argc == 1){
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    struct passwd* pwd_ptr;


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("forking child process\n");
    pid_t process_id = fork();
    pwd_ptr = getpwnam("nobody");
    if( process_id < 0 ){
      printf("error in forking child\n");
      return -1;
    }else if( process_id == 0 ){
      printf("forked\n");
      printf("inside child process\n");
      if (pwd_ptr == NULL) {
            printf("error finding uid for user nobody: %d\n", (long)pwd_ptr->pw_uid);
            return -1;
      }
      int dropStatus = setuid(pwd_ptr->pw_uid );

      if(dropStatus < 0){
        printf("error dropping privileges and dropStatus = %d\n", dropStatus);
        return -1;
      }
      printf("child process dropped privileges\n" );

      char fd_arg [10];
      char port_arg [10];
      snprintf(fd_arg,10, "%d", server_fd);
      snprintf(port_arg,10,"%d",PORT);

      char *args[]={fd_arg, port_arg, NULL};

      execv("./server",args);

      return 0;
    }
    else{
      wait(NULL);
      printf("child process finished\n");
      printf("parent terminating\n");
    }
  }
  else{
    int new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    int server_fd = atoi(argv[0]);
    int port = atoi(argv[1]);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                 (socklen_t*)&addrlen))<0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
  }

}