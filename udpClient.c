// C program to demonstrate socket programming
// as well as explicitly assigning a port number
// on Client Side
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include <unistd.h>
#include <errno.h>
#define MAXLINE 1024
// Returns hostname for the local computer


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


//get data from user
/*
   const char * getUdpIp()
   {
   char string[256];
//scanf( "%s" , string );

printf("\n Enter the IP Address of UDP SERVER ");
scanf( "%s" , string );
//scanf("%s",&udpIpBuffer);
printf("\n You entered the IP Address of UDP SERVER :%s",string);

return string;
}

 */

int main()
{
   //printing on screen


int s;
   for(s=0;s<120;s++)
   {
      printf("*");
   }

   printf("\n\t\t WELCOME USER!!\n\t THIS IS UDP PACKET GENERATOR CLIENT\n");

   for(s=0;s<120;s++)
   {
      printf("*");
   } 


   char *IPbuffer;
   char udpIpBuffer[256];
   struct hostent *host_entry;
   char hostbuffer[256];
   int hostname;
   char buffer[MAXLINE];
   int serverPort;

   //printf("\n Enter the IP Address of UDP SERVER ");
   printf("\n>PLEASE ENTER THE IP ADDRESS OF TRANSPORT PROTOCOL GATEWAY: ");
   scanf( "%s" , udpIpBuffer);
   //scanf("%s",&udpIpBuffer);
   //printf("\n You entered the IP Address of UDP SERVER :%s",udpIpBuffer);


   //port 

   //printf("\n Enter the PORT of UDP SERVER ");
   printf("\n>PLEASE ENTER THE PORT NUMBER OF TRANSPORT PROTOCOL GATEWAY: "); 
   scanf( "%d" , &serverPort);
   //printf("\n You entered the PORT of UDP SERVER :%d",serverPort);


   printf("\n\t ==>IP ADDRESS OF TRANSPORT PROTOCOL GATEWAY : %s ",udpIpBuffer); 
   printf("\n\t ==>PORT OF TRANSPORT PROTOCOL GATEWAY : %d ",serverPort);



   // *udpIpBuffer =getUdpIp();
   // Two buffer are for message communication
   char buffer1[256], buffer2[256];
   struct sockaddr_in my_addr, my_addr1;
   int client = socket(AF_INET, SOCK_DGRAM, 0);
   if (client < 0)
      printf("Error in client creating\n");
   else
      printf("\n\t ==>TRANSPORT PROTOCOL GATEWAY CONNECTED \n\n");

   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   my_addr.sin_port = serverPort;
   // This ip address will change according to the machine
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
   //printf("Host IP: %s\n", IPbuffer);
   //myfunction
   //const char* szSomeString = myFunction(IPbuffer);
   //printf("MY FUNCTION%s",szSomeString);



   //printf("SERVER IP ADDRESS IS :%s",udpIpBuffer);
   my_addr.sin_addr.s_addr = inet_addr(udpIpBuffer);
   // Explicitly assigning port number 12010 by
   // binding client with that port
   my_addr1.sin_family = AF_INET;
   my_addr1.sin_addr.s_addr = INADDR_ANY;
   my_addr1.sin_port = 4168;
   //printf("port %d",my_addr1.sin_port);

   // This ip address will change according to the machine
   my_addr1.sin_addr.s_addr = inet_addr(IPbuffer);

   //my code

   //connect

   // connect to server 
   if(connect(client, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) 
   { 
      printf("\n Error : Connect Failed \n"); 
      exit(0); 
   } 


   //conn end 



   int n, len; 
   char *hello = "Hello from client";
   char sctpip[256];
   char sctpport[256];
   char  sctpmsg[256];
   char  proto[256];

   char message[1000];
   // len=strlen(hello);
   //  printf("length is %d",len);
   // strcat(message);
   //bzero (hello,len+1);

   //strcat(message,"+");
   // printf("\n You entered :%s",message);
   //sctp details
   bzero(message,1000);

   for(s=0;s<120;s++)
   {
      printf("*");
   }


   printf("\n NOTE: DEFAULT TRANSPORT PROTOCOL BETWEEN PACKET GENERATOR AND TRANSPORT PROTOCOL GATEWAY IS SET TO UDP\n");
   printf("\n NOTE: 15000 MESSAGES WILL BE TRANSMITTED TO TRANSPORT PROTOCOL GATEWAY OVER UDP\n");

   for(s=0;s<120;s++)
   {
      printf("*");
   }


   printf("\n>PLEASE ENTER THE IP ADDRESS OF DESTINATION SERVER: ");
   //printf("\n Enter the IP Address of TCP SERVER ");
   scanf( "%s" ,sctpip);

   strcat(sctpip,"+");

//   printf("\n You entered the IP Address of TCP SERVER :%s",sctpip);
   strcat(message,sctpip);
   //scanf("%s",&udpIpBuffer);
   //  printf("\n You entered the IP Address of SCTP SERVER :%s",),
  // printf("\n You entered :%s",message);

   //port 


   printf("\n>PLEASE ENTER THE PORT NUMBER OF DESTINATION SERVER: "); 
   //printf("\n Enter the PORT of TCP SERVER ");
   scanf( "%s" , sctpport);
 //  printf("\n You entered the PORT of TCP SERVER :%s",sctpport);
   strcat(sctpport,"+");
   strcat(message,sctpport);
   //printf("\n You entered :%s",message);

   //proto
   printf("\n>PLEASE ENTER THE PROTOCOL TYPE OF DESTINATION SERVER: ");
   scanf("%s",proto);



   printf("\n>PLEASE ENTER THE MESSAGE FOR DESTINATION SERVER: ");
   //printf("\n Enter the message for TCP SERVER ");
   scanf("%s",sctpmsg);
   strcat(message,sctpmsg);

   //scanf("%s",&udpIpBuffer);
  // printf("\n You entered the message TCP SERVER :%s",sctpmsg);



   for(s=0;s<120;s++)
   {
      printf("*");
   }

  
   printf("\n MESSAGE AND TRANSMISSION DETAILS OF DESTINATION SERVER PACKETS:"); 
   printf("\n\t\t IP ADDRESS OF DESTINATION SERVER : %s ",sctpip); 
   printf("\n\t\t PORT OF DESTINATION SERVER : %s", sctpport);
   printf("\n\t\t PROTOCOL TYPE OF DESTINATION SERVER: %s", proto); 
   printf("\n\t\t MESSAGE FOR DESTINATION SERVER : %s \n", sctpmsg);
 
   for(s=0;s<120;s++)
   {
      printf("*");
   }


sleep(1);

   /*

   //   char *hello = "Hello";
   //   char *hello;


   // const char *szBuffer;   


   sendto(client, (const char *)hello, strlen(hello), 
   MSG_CONFIRM, (const struct sockaddr *) &my_addr,  
   sizeof(my_addr)); 
   printf("\nHello message sent.\n");

    */
   //message sending________________
   int loop = 1;
   int messageReceived = 0;
   int messageSent = 0;
#define loopNum 15000
   while(loop <= (loopNum +1))
   {

   if(messageSent < loopNum)
   {
      char message2send[1000];
      char loopCount[200];
      sprintf(loopCount," packet %d",loop);
      strcpy(message2send,message);
     strcat(message2send,loopCount);
    
      sendto(client, (const char *)message2send, strlen(message2send),
	 MSG_CONFIRM, (const struct sockaddr *) &my_addr,
	 sizeof(my_addr));
     printf("\n message sent %s.\n",message2send);
     messageSent++;
   } 


   //-----------------------

// printf("\nreceived.\n");
 n = recvfrom(client, (char *)buffer, MAXLINE,  
	 MSG_WAITALL, (struct sockaddr *) &my_addr, 
	 &len); 
//printf("n is %d",n);
   buffer[n] = '\0'; 
//   printf("Server : %s\n", buffer); 
//    printf("\n received from sctp server %s length %d\n", buffer,n);

   if(n >0)
   {
    ++messageReceived;
    printf("\n received from tcp server %s length %d\n", buffer,n);
   }
   else
     break;

   loop++;
//   strcpy(loopCount,"");
}

//   close(client); 
   return 0; 



}

