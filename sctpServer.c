//SCTPServer.C To compile - gcc sctpserver.c - o server - lsctp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "sctp.h"
#include<string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include<sys/un.h>
#include<netdb.h>
#include<netinet/in.h>

#include<arpa/inet.h>



#define MAX_BUFFER 1024
#define MY_PORT_NUM 29118 /* This can be changed to suit the need and should be same in server and client */
static int messageSent = 0;
static int messageReceived = 0;
//dynamic

void checkHostName(int hostname)
{
   if (hostname == -1)
   {
      perror("gethostname");
      exit(1);
   }
}
// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
   if (hostentry == NULL)
   {
      perror("gethostbyname");
      exit(1);
   }
}

// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
   if (NULL == IPbuffer)
   {
      perror("inet_ntoa");
      exit(1);
   }
}



int main ()
{
   printf("HELLO !! THIS IS SCTP SERVER!!\n\n");

   char *IPbuffer;
   struct hostent *host_entry;
   char hostbuffer[256];
   int hostname;

   int listenSock, connSock, ret, in, flags, i;
   struct sockaddr_in servaddr;
   struct sctp_initmsg initmsg;
   struct sctp_event_subscribe events;
   struct sctp_sndrcvinfo sndrcvinfo;

   int datalen = 0;
   //char buffer[MAX_BUFFER + 1];

   listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
   if(listenSock == -1)
   {
      printf("Failed to create socket\n");
      perror("socket()");
      exit(1);
   }

   // To retrieve hostname
   hostname = gethostname(hostbuffer, sizeof(hostbuffer));
   checkHostName(hostname);
   // To retrieve host information
   host_entry = gethostbyname(hostbuffer);
   checkHostEntry(host_entry);
   // To retrieve host information
   host_entry = gethostbyname(hostbuffer);
   checkHostEntry(host_entry);
   // To convert an Internet network
   // address into ASCII string
   IPbuffer = inet_ntoa(*((struct in_addr*)
	    host_entry->h_addr_list[0]));
   //we got the ip
   printf("Host IP: %s.\n\n", IPbuffer);
   servaddr.sin_addr.s_addr = inet_addr(IPbuffer);




   bzero ((void *) &servaddr, sizeof (servaddr));
   servaddr.sin_family = AF_INET;
   //servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
   servaddr.sin_port = htons(MY_PORT_NUM);
   ret = bind (listenSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

   if(ret == -1 )
   {
      printf("Bind failed \n");
      perror("bind()");
      close(listenSock);
      exit(1);
   }

   /* Specify that a maximum of 5 streams will be available per socket */
   memset (&initmsg, 0, sizeof (initmsg));
   initmsg.sinit_num_ostreams = 5;
   initmsg.sinit_max_instreams = 5;
   initmsg.sinit_max_attempts = 4;

   // events
   struct sctp_event_subscribe subscribe;                                                     




   // registers for SCTP notifications
   // first we reset the subscribe structure
   memset(&subscribe, 0, sizeof(subscribe));

   // setting the events we want to be notified about to 1
   subscribe.sctp_data_io_event = 1;

   subscribe.sctp_association_event = 1;




   ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG,
	 &initmsg, sizeof (initmsg));

   if(ret == -1 )
   {
      printf("setsockopt() failed \n");
      perror("setsockopt()");
      close(listenSock);
      exit(1);
   }


//set events
/*
int  reteve = setsockopt (listenSock, IPPROTO_SCTP, SCTP_EVENTS,
          &subscribe, sizeof (subscribe));

	     if(reteve == -1 )
	        {
		      printf("setsockopt() failed \n");
		            perror("setsockopt()");
			          close(listenSock);
				        exit(1);
					   }


  */
  ret = listen (listenSock, 5);
   if(ret == -1 )
   {
      printf("listen() failed \n");
      perror("listen()");
      close(listenSock);
      exit(1);
   }

   //   while (1)
   // {

   //     char buffer[MAX_BUFFER + 1];
   int len;

   //Clear the buffer
   //bzero (buffer, MAX_BUFFER + 1);

   printf ("Awaiting a new connection\n");

   
   connSock = accept (listenSock, (struct sockaddr *) NULL, (int *) NULL);
   if (connSock == -1)
   {
      printf("accept() failed\n");
      perror("accept()");
      close(connSock);
      //	 continue;
   }
  
   else
    printf ("New client connected....\n");
   while(1)
   {
   //receive while
   int j;
   printf("in for loop\n");
   char buffer[MAX_BUFFER + 1];

   bzero (buffer, MAX_BUFFER + 1);
   in = sctp_recvmsg (connSock, buffer, sizeof (buffer),
	 (struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);

   if( in == -1)
   {
      printf("Error in sctp_recvmsg\n");
      perror("sctp_recvmsg()");
      break;
       //	    close(connSock);
      //   continue;
   }
   else
   {
     messageReceived++;
      //Add '\0' in case of text data
      buffer[in] = '\0';

      printf (" Length of Data received: %d data %s packet count\n", in,(char *) buffer);
//      printf (" Data : %s\n", (char *) buffer);
      // bzero (buffer, MAX_BUFFER + 1);
      //continue;
   }
   //  close (connSock);
   

//   printf("in sctp_sendmsg\n");
   //send
   datalen=strlen(buffer);


//   printf("sent %d bytes data %s\n", datalen,buffer);
   ret = sctp_sendmsg (connSock, (void *) buffer, (size_t) datalen,
   NULL, 0, 0, 0, 0, 0, 0);
//   printf("Successfully sent %d bytes data to client data %s\n", ret, buffer);
   if(ret == -1 )
   {
   printf("Error in sctp_sendmsg\n");
   perror("sctp_sendmsg()");
   break;
   }
   else
    {
     messageSent++;
     printf("Successfully sent %d bytes data to client data %s\n", ret,(char *) buffer);
    }
   }

//   return 0;
}


