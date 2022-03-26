/* A client */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>

#define	PORTNUM	8001
#define	BLENGTH	64
#define	ALENGTH	32

static void
loop(int s)
{
  char buffer[BLENGTH];
  char name[ALENGTH] = "Jimmy";
  
  /* Send name to server */
  send(s, name, ALENGTH, 0);

  for (;;) {

    /* Receive prompt */
    if (recv(s, (void *)buffer, BLENGTH, 0) != BLENGTH) {
      break;
    }

    /* Display prompt */
    fputs(buffer, stdout);

    /* Read user response */
    fgets(buffer, BLENGTH, stdin);

    /* Send user response */
    send(s, (void *)buffer, BLENGTH, 0);
  }
}

int
main(void)
{
  struct sockaddr_in server;
  struct hostent *host;
  int s;

  /* Create an Internet family, stream socket */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* Server listening on localhost interface */
  if ((host = gethostbyname("localhost")) == NULL) {
    perror("gethostbyname()");
    exit(EXIT_FAILURE);
  }

  /* Fill in socket address */
  memset((char *)&server, '\0', sizeof (server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORTNUM);
  memcpy((char *)&server.sin_addr, host->h_addr_list[0], host->h_length);

  /* Connect to server */
  if (connect(s, (struct sockaddr *)&server, sizeof (server)) < 0) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  /* Talk to server */
  loop(s);

  /* Close the socket */
  close(s);

  return (0);
}
