#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

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
  timev.tv_sec = 1;
  timev.tv_usec = 0;
  
  float min_rtt = 100000000.0;
  float max_rtt = 0.0;
  float sum_rtt = 0.0;
  float curr_rtt = 0.0;
  int received;

  char message[150] = "";
  unsigned int return_len;
  char response[2000] = "";
  char timestamp[50];
  char sequencenum[2];
  
  for(int i = 1; i < 11; i++){

    for(int i = 0; i < strlen(response); i++){
      response[i] = '\0';
    }

    for(int i = 0; i < strlen(message); i++){
      message[i] = '\0';
    }

    strcat(message, "PING ");
    sprintf(sequencenum, "%d", i);
    strcat(message, sequencenum);
    strcat(message, " ");

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime (&rawtime);

    // timestamp
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    strcat(message, timestamp);
    clock_t clk;
    clk = clock();
    
    sendto( socket_id, message, strlen(message), 0,
	    (struct sockaddr *) &server_address, sizeof(struct sockaddr_in) );
    
    timev.tv_sec = 1;
    timev.tv_usec = 0;
    if(setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev)) < 0) {
      perror("Timer error");
    }
    
    recvfrom( socket_id, response, 2000, 0,
	      (struct sockaddr *) &return_address, &return_len );

    if(strcmp(response, "") != 0){
      clk = clock() - clk;
      curr_rtt = ((float)clk)/CLOCKS_PER_SEC * 1000.0;
      sum_rtt += curr_rtt;
      if(curr_rtt < min_rtt) min_rtt = curr_rtt;
      if(curr_rtt > max_rtt) max_rtt = curr_rtt;
      received++;
      printf("PING received from %s: ", server_ip);
      printf("seq#=%s ", sequencenum);
      printf("time=%.3f ms\n", curr_rtt);
    }
    else{
      printf("Request timeout for icmp_seq %d\n", i);
    }
    
  }

  float percent = (10 - received)/10.0 * 100;
  printf("--- ping statistics ---\n");
  printf("10 packets transmitted, %d received, %.1f%% packet loss", received, percent);

  if(received == 0){
    printf(" rtt min/avg/max = undefined undefined undefined ms\n");
  }
  else{
    printf(" rtt min/avg/max = %.3f %.3f %.3f\n", min_rtt, sum_rtt/received, max_rtt);
  }
  //fprintf(stderr, "%s\n", message);

  close(socket_id);

  return 0;
}
