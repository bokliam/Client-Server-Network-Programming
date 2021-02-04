/*************************************************************************
  Liam Bok
  Project #11 - Client
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
#include <iostream>

using namespace std;

/* Port number */
#define PORT 49821

/* Buffer size */
#define BUFSIZE 256

/* Flag for quit */
bool quit = false;

/* Flag for local file */
bool exists = false;


/*------------------------------------------------------------------
Name: main

Purpose: Simulate client for network programming with sockets
Receive: argc Argument count; argv Array of arguments
Return: 0 on success
------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  /* Make sure both parameters are passed */
  if (argc != 3)
  {
    printf("Error: Please supply domain name followed by port number and "\
        "try again.");
    exit(1);
  }

  /* Get host name and port */
  char* hostname = argv[1];
  int port = atoi(argv[2]);

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sd < 0)
  {
    perror("Socket");
    exit(2);
  }

  struct hostent* server = gethostbyname(hostname);
  
  if (server == NULL)
  { 
    perror("Error: host not found\n");
    exit(3);
  }

  /* Connect to server */
  struct sockaddr_in saddr;
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  bcopy(server->h_addr, &saddr.sin_addr.s_addr, server->h_length);
  saddr.sin_port = htons(port);

  int stat = connect(sd, (struct sockaddr*) &saddr, sizeof(saddr));
  
  if (stat < 0)
  {
    perror("Connect");
    exit(4);
  }
  
  printf("Connected to server\n");

  /* Process user input */
  while (quit != true)
  {
    /* Get input from user */
    printf("Please enter a command: ");

    char input[256];
    cin.getline(input, 256);
    
    /* Display input line */
    printf("%s\n", input);
    
    char* cmd = strtok(input, " ");
    
    /* Quit command entered */
    if ( strcmp(cmd, "quit") == 0)
    {
      const char* quitCommand = "quit";
      char buffer[BUFSIZE];
      strncpy(buffer, quitCommand, strlen(quitCommand));

      /* Send quit message to server */
      send(sd, buffer, strlen(buffer), 0);

      quit = true;
    }

    /* Get command entered */
    else if ( strcmp(cmd, "get") == 0)
    {
      /* Get file names from input */
      char* remoteFile = strtok(NULL, " ");
      char* localFile = strtok(NULL, " ");

      char buffer[BUFSIZE];
      strncpy(buffer, remoteFile, strlen(remoteFile));

      /* Send requested filename to server */
      int ssend = send(sd, buffer, strlen(buffer), 0);

      if (ssend < 0)
      {
        perror("Send");
        exit(5);
      }

      /* Check if local file exists */
      if (fopen(localFile, "r") != NULL)
      {
        exists = true;
      }


      /* Receive file contents from server */
      bzero(buffer, BUFSIZE);
      int srecv = 0;

      /* File exists, transfer data to it */
      if (exists == true)
      {
        int dest = open(localFile, O_TRUNC|O_WRONLY, S_IRWXU);
        while ( (srecv = recv(sd, buffer, BUFSIZE, 0)) != 0)
        {
          write(dest, buffer, BUFSIZE);
        }
        /* Close file */
        close(dest);
      }

      /* File does not exist, create and transfer data to it */
      else
      {
        int dest = open(localFile, O_CREAT|O_WRONLY, S_IRWXU);
        while ( (srecv = recv(sd, buffer, BUFSIZE, 0)) != 0)
        {
          write(dest, buffer, BUFSIZE);
        }
        /* Close file */
        close(dest);
      }

      close(sd);
      
    }

    /* Invalid command */
    else
    {
      printf("Error: Please enter a valid command and try again.\n");
    }
  
  
  }
  /* Close connection */
  close(sd);
}




