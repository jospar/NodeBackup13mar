// C program to demonstrate  
// socket programming in finding ip address  
// and port number of connected client 
// on Server Side 
#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MYPORT 12050
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

int main() 
{

char *IPbuffer;
      struct hostent *host_entry;
            char hostbuffer[256];
	          int hostname;


int s;
   for(s=0;s<120;s++)
   {
      printf("*");
   }

   printf("\n\t\t\t WELCOME USER !!!"); 
   printf("\n\t\t\t TCP SERVER\n"); 

   for(s=0;s<120;s++)
   {
      printf("*");
   }
 


static int request=0;
int check=0;
   // Two buffers for message communication 
   char buffer1[256], buffer2[256]; 
   int server = socket(AF_INET, SOCK_STREAM, 0); 
   if (server < 0) 
      printf("Error in server creating\n"); 
   else
      printf("\n==>TCP SERVER CREATED\n"); 

   struct sockaddr_in my_addr, peer_addr; 
   my_addr.sin_family = AF_INET; 
   my_addr.sin_addr.s_addr = INADDR_ANY; 

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
//   printf("Host IP: %s\n", IPbuffer);
   my_addr.sin_addr.s_addr = inet_addr(IPbuffer);




   //mera 
   //my_addr.sin_addr.s_addr = inet_addr("10.32.40.213"); 

   my_addr.sin_port =MYPORT;
//   printf("port %d",my_addr.sin_port);




   for(s=0;s<120;s++)
   {
      printf("*");
   }


   printf("\n\t\t IP ADDRESS OF TCP SERVER : %s ",IPbuffer); 
   printf("\n\n\t\t PORT OF TCP SERVER : %d\n",my_addr.sin_port);

   for(s=0;s<120;s++)
   {
      printf("*");
   }




   if (bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr)) == 0) 
      printf("\n\n==>Binded Correctly\n\n"); 
   else
      printf("\n\n==>Unable to bind\n\n"); 

   if (listen(server, 3) == 0) 
      printf("****Listening ...\n"); 
   else
      printf("****Unable to listen\n"); 

   socklen_t addr_size; 
   addr_size = sizeof(struct sockaddr_in); 
printf("paresh after listen");
   // Ip character array will store the ip address of client 
   char *ip1; 

   // while loop is iterated infinitely to  
   // accept infinite connection one by one 
      //while (1) 
	//{ 
	int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size); 
	printf("Connection Established\n"); 
	//char ip1[INET_ADDRSTRLEN]; 
	//inet_ntop(AF_INET, &(peer_addr.sin_addr), ip1 INET_ADDRSTRLEN); 

   // "ntohs(peer_addr.sin_port)" function is  
   // for finding port number of client 
   //printf("connection established with IP : %s and PORT : %d\n",ip1, ntohs(peer_addr.sin_port)); 
 

//only send and recv
while(1)
{
 check =  recv(acc, buffer2, 256, 0);
  //int readVal  = read( acc , buffer2, 256); 
   if (check > 0 )
      request++;
   printf("\n Client : %s\n", buffer2); 
   printf("\n request number = %d \n",request);

   //strcpy(buffer1, "Hello"); 
   int n=0;
   n=send(acc, buffer2, 256, 0); 
   if(n>0)
   printf("SENT"); 
   else
   printf("NOT SENT"); 
//close(acc); 
}

return 0; 
} 
