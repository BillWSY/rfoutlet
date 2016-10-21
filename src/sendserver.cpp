#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "RCSwitch.h"

#define UDP_PORT 9876
#define DEFAULT_PIN 0
#define DEFAULT_PULSE_LENGTH 189

char buffer[1024];

int main() {
  int sd = socket(PF_INET, SOCK_DGRAM, 0);
  sockaddr_in addr;
  memset(&addr, sizeof(addr), 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(UDP_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sd, (sockaddr*)&addr, sizeof(addr)) != 0) {
    perror("bind");
    exit(-1);
  }
  if (wiringPiSetup () == -1) {
    return EXIT_FAILURE;
  }
  while (true) {
    unsigned addr_len = sizeof(addr);
    unsigned byte_cnt = 0;
    byte_cnt = recvfrom(
        sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
    if (byte_cnt == 4) {
      int code = 0;
      for (int i = 0; i < 4; ++ i) {
        code = (code << 8) | buffer[i];
      }
      RCSwitch sw;
      sw.setPulseLength(DEFAULT_PULSE_LENGTH);
      sw.enableTransmit(DEFAULT_PIN);
      sw.send(code, 24);
      sendto(sd, buffer, byte_cnt, 0, (struct sockaddr*)&addr, sizeof(addr));
      printf("Sent %d\n", code);
    }
  }
}
