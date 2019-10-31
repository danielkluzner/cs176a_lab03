#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char * argv[]) {

  if(argc != 3){
    printf("Enter IP address and Port #\n");
    exit(1);
  }  

  struct sockaddr_in server_address, return_address;

  char* start = argv[1];
  char server_ip[16];
  for(int i = 0; i < 16; i++){
    server_ip[i] = *(start + i);
    if(*(start + i) == '\0') break;
  }
  int port = atoi(argv[2]);

  int socket_id = socket(AF_INET, SOCK_DGRAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr( server_ip );
  server_address.sin_port = htons(port);

  char input[150] = "hello";
  sendto( socket_id, input, strlen(input), 0,
	  (struct sockaddr *) &server_address, sizeof(struct sockaddr_in) );

  unsigned int return_len;
  char message[2000] = "";
  recvfrom( socket_id, message, 2000, 0,
	    (struct sockaddr *) &return_address, &return_len );

  fprintf(stderr, "%s\n", message);

  close(socket_id);

  return 0;
}
