/*************************************************************************
  Liam Bok
  Project #11 - Server 
*************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>

/* Port number */
#define PORT 49825

/* Buffer size */
#define BUFSIZE 256

/* File exists flag */
bool exists = false;
bool quit = false;

/*-----------------------------------------------------------------
Name: main

Purpose: Simulate server in network programming with sockets
Return: 0 on success
-----------------------------------------------------------------*/
int main()
{
  char hostname[256];  
  gethostname(hostname, 256);

  /* Print host name */
  struct hostent* h;
  h = gethostbyname(hostname);
  printf("%s %d\n", h->h_name, PORT);

  /* Create socket */
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sd < 0)
  {
    perror("Socket");
    exit(1);
  }

  /* Bind socket */
  struct sockaddr_in sd_addr;
  bzero(&sd_addr, sizeof(sd_addr));

  sd_addr.sin_family = AF_INET;
  sd_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  sd_addr.sin_port = htons((unsigned short) PORT);

  int sstat = bind(sd, (struct sockaddr*) &sd_addr, sizeof(sd_addr));
  
  if (sstat < 0)
  {
    perror("Bind");
    exit(2);
  }

  /* Listen */
  
  int lstat = listen(sd, 5);
  if (lstat < 0)
  {
    perror("Listen");
    exit(3);
  }


  while (quit != true)
  {

    /* Accept socket connection */
    struct sockaddr_in saddr;
    unsigned int slen = sizeof(saddr);

    int sd_comm = accept(sd, (struct sockaddr*) &saddr, &slen);
    
    if (sd_comm < 0)
    {
      perror("Accept");
      exit(4);
    } 

    /* Receive requested filename */
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE);
    int srecv = recv(sd_comm, buffer, BUFSIZE, 0);
    
    if (srecv < 0)
    {
      perror("Receive");
      exit(5);
    }

    char fileName[256];
    strncpy(fileName, buffer, strlen(buffer));
  
    if ( strcmp(fileName, "quit") == 0)
    {
      close(sd_comm);
      exit(6);
    }

    /* Byte count for data transfer */
    int total = 0;
 
    int source = open(fileName, O_RDONLY, S_IRWXU);

    /* Get size of requested file */
    struct stat st;
    stat(fileName, &st);
    int size = st.st_size;

    /* If file exists, send data */
    if (source != -1)
    {
      while (total != size)
      {
        bzero(buffer, BUFSIZE);
        int reader = read(source, buffer, BUFSIZE);
        total += reader;

        int ssend = send(sd_comm, buffer, sizeof(buffer), 0);
        if (ssend < 0)
        {
          perror("Send");
          exit(7);
        }
    

      }
      close(sd_comm);
      exit(8);
    }

    /* Requested file does not exits */
    else
    {
      printf("File does not exist\n");
      close(sd_comm);
      exit(9);
    } 
  }

  return 0;
}




