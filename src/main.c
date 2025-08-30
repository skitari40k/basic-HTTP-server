#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <regex.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "url_decode.h"
#include "get_ext.h"
#include "get_mime.h"

#define PORT 8080
#define BUF_SIZE 104857600
#define BACKLOG 10

void 
build_http_response(char *filename, char *file_ext, char *response, size_t *response_len)
{ 
  // if implicit filename -> 403
  if(strstr(filename, ".."))
  {
    snprintf(response, BUF_SIZE,
             "HTTP/1.1 403 Forbidden\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n"
             "403 Forbidden");
    *response_len = strlen(response);
    
    return;
  }
 
  // if file doesn't exist -> 404
  int f = open(filename, O_RDONLY);
  if(f == -1)
  {
    snprintf(response, BUF_SIZE,
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n"
             "404 Not Found");
    *response_len = strlen(response);
    
    return;
  }

  struct stat file_stat;
  fstat(f, &file_stat);
  off_t file_size = file_stat.st_size;

  const char *mime = get_mime(file_ext);
  snprintf(response, BUF_SIZE, 
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "\r\n",
           mime, file_size);
  
  (*response_len) = strlen(response);

  ssize_t bytes_r;
  while ((bytes_r = read(f, response + *response_len, BUF_SIZE - *response_len)) > 0)
  {
    (*response_len) += bytes_r;
  }
  close(f);
}

void
*handle_client(void *arg)
{
  int client_fd = *((int *) arg);
  char *buf = (char *)malloc(BUF_SIZE * sizeof(char));

  ssize_t bytes_rec = recv(client_fd, buf, BUF_SIZE, 0);
  if(bytes_rec > 0)
  {
    // check for get-req
    regex_t reg;
    regcomp(&reg, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
    regmatch_t matches[2];

    if(regexec(&reg, buf, 2, matches, 0) == 0)
    {
      // get filename & extract URL 
      buf[matches[1].rm_eo] = '\0';
      const char *encoded_file_name = buf + matches[1].rm_so;
      char *file_name = malloc(strlen(encoded_file_name) * 3 + 1);
      if(file_name == NULL)
      {
        perror("[ERROR] No file found");
        close(client_fd);
        free(buf);
      }

      urldecode(file_name, encoded_file_name);

      char file_ext[32];
      strcpy(file_ext, get_ext(file_name));

      char *response = (char *)malloc(BUF_SIZE * 2 * sizeof(char));
      size_t response_len;
      build_http_response(file_name, file_ext, response, &response_len);

      send(client_fd, response, response_len, 0);

      free(file_name);

    }
    regfree(&reg);
  }

  close(client_fd);
}

int 
main(void)
{
  int sfd;
  struct sockaddr_in address; 
  size_t addr_len = sizeof(address);

  
  if((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[ERROR] At creating socket");
  }


  // initialize socket, bind to socket

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if(bind(sfd, (struct sockaddr *)&address, addr_len) == -1)
  {
    perror("[ERROR] at init port");
  }

  // start listening socket

  if(listen(sfd, BACKLOG) != 0)
  {
    perror("[ERROR] at listening");
  }

  // listen to new clients

  while(1) 
  {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int *client_fd = malloc(sizeof(int));

    *client_fd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len); 

    if(*client_fd == -1)
    {
      free(client_fd);
      perror("[ERROR] At accepting new connection"); 
      continue;
    }

     pthread_t thread_id;
     pthread_create(&thread_id, NULL, handle_client, (void *) client_fd); 
     pthread_detach(thread_id);
  }

  close(sfd); // don't forget to close ;d
}
