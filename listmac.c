#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main(void)
{
  char buf[8192] = {0};
  struct ifconf ifc = {0};
  struct ifreq *ifr = NULL;
  int sck = 0;
  int nInterfaces = 0;
  int i = 0;
  char ip[INET6_ADDRSTRLEN] = {0};
  char macp[19];
  struct ifreq *item;
  struct sockaddr *addr;

  /* Get a socket handle. */
  sck = socket(PF_INET, SOCK_DGRAM, 0);
  if(sck < 0) 
  {
    perror("socket");
    return 1;
  }

  /* Query available interfaces. */
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if(ioctl(sck, SIOCGIFCONF, &ifc) < 0) 
  {
    perror("ioctl(SIOCGIFCONF)");
    return 1;
  }

  /* Iterate through the list of interfaces. */
  ifr = ifc.ifc_req;
  nInterfaces = ifc.ifc_len / sizeof(struct ifreq);

  for(i = 0; i < nInterfaces; i++) 
  {
    item = &ifr[i];

    addr = &(item->ifr_addr);

    /* Get the IP address*/
    if(ioctl(sck, SIOCGIFADDR, item) < 0) 
    {
      perror("ioctl(OSIOCGIFADDR)");
    }

    if (inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), ip, sizeof ip) == NULL) //vracia adresu interf
        {
           perror("inet_ntop");
           continue;
        }

    /* Get the MAC address */
    if(ioctl(sck, SIOCGIFHWADDR, item) < 0) {
      perror("ioctl(SIOCGIFHWADDR)");
      return 1;
    }

    /* display result */

    sprintf(macp, " %02x:%02x:%02x:%02x:%02x:%02x", 
    (unsigned char)item->ifr_hwaddr.sa_data[0],
    (unsigned char)item->ifr_hwaddr.sa_data[1],
    (unsigned char)item->ifr_hwaddr.sa_data[2],
    (unsigned char)item->ifr_hwaddr.sa_data[3],
    (unsigned char)item->ifr_hwaddr.sa_data[4],
    (unsigned char)item->ifr_hwaddr.sa_data[5]);

    printf("%s %s \n", ip, macp);

  }

  return 0;
}

