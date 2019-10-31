#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>

void get_time(){
  struct timeval tv_tod;
  gettimeofday(&tv_tod, 0);
  printf("%d\n", tv_tod.tv_usec);
}

int main(int argc, char * argv[]) {

  if(argc != 3){
    printf("Enter IP address and Port #\n");
    exit(1);
  }  

  struct sockaddr_in server_address, return_address;

  // get IP and port# from args
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

  struct timeval timev;
  /*timev.tv_sec = 1;
  timev.tv_usec = 0;
  if(setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev)) < 0) {
    perror("Timer error");
  }
  */
  float min_rtt;
  float max_rtt = 0;
  float sum_rtt = 0;
  int received;

  char message[150] = "hello";
  unsigned int return_len;
  char response[2000] = "";
  
  for(int i = 1; i < 11; i++){

    for(int i = 0; i < strlen(response); i++){
      response[i] = '\0';
    }
    
    sendto( socket_id, message, strlen(message), 0,
	    (struct sockaddr *) &server_address, sizeof(struct sockaddr_in) );
    
    timev.tv_sec = 1;
    timev.tv_usec = 0;
    if(setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev)) < 0) {
      perror("Timer error");
    }
    
    recvfrom( socket_id, response, 2000, 0,
	      (struct sockaddr *) &return_address, &return_len );

    if(strcmp(response, "") != 0) printf("%s\n", response);
    else printf("LOST\n");
    
  }
  
  //fprintf(stderr, "%s\n", message);

  close(socket_id);

  return 0;
}
