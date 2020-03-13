/////////////////////////////////////////////////////////////////////////////////////////////
////                                                                                     ////
////                     ============   ==========     ==========                        ////
////                         //        //      //     //                                 ////
////                        //        //======//     //  	                         ////
////                       //        //             //============                       ////
////                      //        //             //       //  ||                       ////
////			 //        //             //       //   ||                       //// 
////			//        //             //       //    ||                       ////
////			                         =========                               ////
/////////////////////////////////////////////////////////////////////////////////////////////


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
#include <time.h>
#include "sctp.h"
#include<pthread.h>

// Some global definitions to begin with..!!
#define MAX_BUFFER 1024
#define MY_PORT_NUM 62324 
#define MAXLINE 1024

char* tokenIP;
char* tokenPort;
char* tokenMsg;

char *IPbuffer;
char buffer[1024];
int connSock;
int sctp_sock = 0;
int sock=0;

void * sendOverSctp(void * args);
void * recvOverSctp(void * args);


void * sendToCLI(void * args);
struct sockaddr_in my_addr, peer_addr, udp_srvr_addr;
int server = 0;
pthread_mutex_t lock;      

////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                //
//                               Lets now create our list and queues...!!                         //
////////////////////////////////////////////////////////////////////////////////////////////////////


struct list_head {
   struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
   struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
   (ptr)->next = (ptr); (ptr)->prev = (ptr); \
   } while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
   next->prev = new;
   new->next = next;
   new->prev = prev;
   prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
   __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
   __list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
   next->prev = prev;
   prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
   __list_del(entry->prev, entry->next);
   entry->next = (void *) 0;
   entry->prev = (void *) 0;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
   __list_del(entry->prev, entry->next);
   INIT_LIST_HEAD(entry); 
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head)
{
   __list_del(list->prev, list->next);
   list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *list,
				  struct list_head *head)
{
   __list_del(list->prev, list->next);
   list_add_tail(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(struct list_head *head)
{
   return head->next == head;
}

static inline void __list_splice(struct list_head *list,
				 struct list_head *head)
{
   struct list_head *first = list->next;
   struct list_head *last = list->prev;
   struct list_head *at = head->next;

   first->prev = head;
   head->next = first;

   last->next = at;
   at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
   if (!list_empty(list))
	__list_splice(list, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
				    struct list_head *head)
{
   if (!list_empty(list)) {
	__list_splice(list, head);
	INIT_LIST_HEAD(list);
   }
}

/**
 * list_entry - get the struct for this entry
 * @ptr:the &struct list_head pointer.
 * @type:the type of the struct this is embedded in.
 * @member:the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each-iterate over a list
 * @pos:the &struct list_head to use as a loop counter.
 * @head:the head for your list.
 */
#define list_for_each(pos, head) \
   for (pos = (head)->next; pos != (head); \
	pos = pos->next)
/**
 * list_for_each_prev-iterate over a list backwards
 * @pos:the &struct list_head to use as a loop counter.
 * @head:the head for your list.
 */
#define list_for_each_prev(pos, head) \
   for (pos = (head)->prev; pos != (head); \
        pos = pos->prev)

/**
 * list_for_each_safe-iterate over a list safe against removal of list entry
 * @pos:the &struct list_head to use as a loop counter.
 * @n:another &struct list_head to use as temporary storage
 * @head:the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
	 pos = n, n = pos->next)

/**
 * list_for_each_entry-iterate over list of given type
 * @pos:the type * to use as a loop counter.
 * @head:the head for your list.
 * @member:the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)\
    for (pos = list_entry((head)->next, typeof(*pos), member);\
    &pos->member != (head); \
	 pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:the type * to use as a loop counter.
 * @n:another type * to use as temporary storage
 * @head:the head for your list.
 * @member:the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)\
   for (pos = list_entry((head)->next, typeof(*pos), member),\
		   n = list_entry(pos->member.next, typeof(*pos), member);\
   &pos->member != (head); \
	 pos = n, n = list_entry(n->member.next, typeof(*n), member))


/* by default, QueueElement is int
 Usage: #define QueueElement <TYPE> */
#ifndef QueueElement
#define QueueElement void*
#endif

typedef struct{
   int capacity;
   int size;
   QueueElement e;
   struct list_head list;
} Queue;

Queue * initQueue(int max);
QueueElement front(Queue *Q);	
QueueElement tail(Queue* Q);
void* dequeue(Queue *Q);
void enqueue(Queue *Q, QueueElement element);


/* Create a Queue */
Queue * initQueue(int max)
{
    Queue *Q;
    Q = (Queue *)malloc(sizeof(Queue));
    Q->size = 0;
    Q->capacity = max;
    /* initialize the list head. Kernel list method */
    INIT_LIST_HEAD(&Q->list);
    return Q;
}

void * dequeue(Queue *Q)
{
    Queue* tmp;
    if(Q->size==0){
//        printf("Queue is Empty.\n");
        return NULL;
    }else{
        pthread_mutex_lock(&lock); 
        Q->size--;
        tmp = list_entry(Q->list.next, Queue, list);
        list_del(Q->list.next);
        printf("\n dequeue data is %s \n",(char *)tmp->e);
//      free(tmp);
        pthread_mutex_unlock(&lock); 
        return (tmp->e);
    }
}

QueueElement front(Queue *Q)
{
    Queue* first_element;
    struct list_head * first;
    if(Q->size==0){
	printf("Queue is Empty\n");
	exit(0);
    }
    /* find the first element first */
    first = Q->list.next;
    /* reconstruct the first structure */
    first_element = list_entry(first, Queue, list);
    return first_element->e; 
}

QueueElement tail(Queue *Q)
{
    Queue* last_element;
    struct list_head * last;
    if(Q->size==0){
	printf("Queue is Empty.\n");
	exit(0);
    }
    /* find the last element first */
    last = Q->list.prev;
    /* reconstruct the last structure */
    last_element = list_entry(last, Queue, list);
    return last_element->e; 
}

void enqueue(Queue *Q, QueueElement element)
{
    Queue* newQ;
    if(Q->size == Q->capacity){
	printf("Queue is Full. No element added.\n");
    }
    else{
        pthread_mutex_lock(&lock); 
	Q->size++;
	newQ = (Queue*) malloc(sizeof(Queue));
	newQ->e = element;
	printf("adding element %s in queue \n",(char*)newQ->e);
	/* add to the list tail */
	list_add_tail(&(newQ->list), &(Q->list));
        pthread_mutex_unlock(&lock); 
    }
}


int max = 20;
Queue *udpClientIncQueue;
Queue *udpClientOutQueue;

// Done with list and queue creation.
/////////////////////////////////////////////////////////////////////////////////////////////////////

// Some methods that will help us to determine our server hostname and server host IP.

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


void getIp()
{
   struct hostent *host_entry;
   char hostbuffer[256];
   int hostname;

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
   printf("IP ADDRESS OF NODE IS %s\n",IPbuffer);
}

    ////////////////////////////////////////////////////////////////////
   //                                                                //
  //            Tokenize, a.k.a parse incoming buffer to determine  //
 //            server IP, port and message details                 //
////////////////////////////////////////////////////////////////////
void tokenize(char* str)
{
   int i=0;

   const char s[2] = "+";
   char *token;
   token = strtok(str,"+");

   while( token != NULL)
   {
      i++;
      switch(i)
      {
         case 1 : tokenIP=token;
		  printf("\ntoken IP is %s\n",tokenIP);
		  break;

	 case 2 : tokenPort=token;
		  printf("\ntoken Port is %s\n",tokenPort);
		  break;

	 case 3 : tokenMsg=token;
		  printf("\ntoken Msg is %s\n",tokenMsg);
		  enqueue(udpClientIncQueue,(void *)tokenMsg);
		  break;
      }
      token = strtok(NULL,"+");
   }
}

     ////////////////////////////////////////////////////////////////////
    //                                                                //    
   //            Tokenize, a.k.a parse incoming buffer to determine  //    
  //            message details. Use this method when you already   //   
 //            server ip port details and all you need is message  // 
////////////////////////////////////////////////////////////////////

char * tokenizeAndGetData(char* str)
{
   int i=0;
   const char s[2] = "+";
   char *token;
   token = strtok(str,"+");
   while( token != NULL)
   {
      i++;
      switch(i)
      {
         case 1 : 
                    break;
         case 2 :
                    break;

         case 3 :   return token;
      }
      token = strtok(NULL,"+");
   }
}

   //////////////////////////////////////////////////////////////////////////
  //                                                                      //
 //             UDP protocol related methods.                            //
//             connet, send and received methods.                       //
/////////////////////////////////////////////////////////////////////////

int connectUDP(char * buff)
{
   printf("HELLO !! THIS IS UDP CLIENT!!\n");
   //getIp
   getIp();

   //token call
   tokenize(buff);
   struct sockaddr_in my_addr;
   int connSock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sctp_sock < 0)
      printf("Error in client creating\n");
   else
      printf("\nclient Created\n");

   udp_srvr_addr.sin_family = AF_INET;
   udp_srvr_addr.sin_addr.s_addr = INADDR_ANY;
   udp_srvr_addr.sin_port = atoi(tokenPort);
   udp_srvr_addr.sin_addr.s_addr = inet_addr(tokenIP);

   printf("SERVER IP ADDRESS IS :%s",tokenIP);

   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   my_addr.sin_port = 6000;
   my_addr.sin_addr.s_addr = inet_addr(IPbuffer);

   // connect to server 
   if(connect(connSock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
   {
      printf("\n Error : Connect Failed \n");
      exit(0);
   }
   else
      printf("Connected UDP server \n");
   return connSock;
}

//sendOverUdp
void sendOverUdp(int connSock)
{
   printf("\nSENDING DATA !!\n");
   printf (" Data : %s\n\n", (char *) tokenMsg);
   //send(connSock, (const char *)tokenMsg, strlen(tokenMsg),0);
   int res;
   res= sendto(connSock, (const char *)tokenMsg, strlen(tokenMsg),
	 MSG_CONFIRM, (const struct sockaddr *) &udp_srvr_addr,
	 sizeof(udp_srvr_addr));

   if(res>0)
      printf("MESSAGE SENT%d\n",connSock);
   else
      printf("MESSAGE SENDING FAILED\n");


}

//recvOverUdp
void recvOverUdp(int connSock)
{
   int n;
   printf("RECEIVING DATA !! %d\n",connSock);
//   bzero(buffer,MAX_BUFFER+1);  

//   n=recvfrom(connSock, (void *)buffer, MAX_BUFFER+1,
//            MSG_WAITALL, (struct sockaddr *) &udp_srvr_addr,
//	      (socklen_t *) &len);
   if(n == -1)
      printf("RECEIVING DATA ERROR");
//   buffer[n] = '\0';
//   printf("DATA : %s\n", buffer);
}

    ////////////////////////////////////////////////////////////////////
   //                                                                //
  //            TCP protocol related methods.                       //
 //            connet, send and received methods.                  //
////////////////////////////////////////////////////////////////////

//connect TCP
int connectTCP(char * buff)
{
   printf("HELLO !! THIS IS TCP CLIENT!!\n");
   //getIp
   getIp();

   //token call
   tokenize(buff);
   int port=atoi(tokenPort);

   printf("Port is %d and port+1 %d",port,port+1);
   struct sockaddr_in my_addr, srv_addr;
   connSock = socket(AF_INET, SOCK_STREAM, 0);
   if (connSock < 0)
      printf("Error in client creating\n");
   else
      printf("Client Created on connSock %d\n",connSock);

   bzero ((void *) &srv_addr, sizeof (srv_addr));
   srv_addr.sin_family = AF_INET;
   srv_addr.sin_addr.s_addr = INADDR_ANY;
   srv_addr.sin_port =port;
   srv_addr.sin_addr.s_addr = inet_addr(tokenIP);

   //   printf("server IP %d and Port is %d",srv_addr.sin_addr.s_addr,srv_addr.sin_port);   

   // binding client with that port
   bzero ((void *) &my_addr, sizeof (my_addr));
   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   my_addr.sin_port =  htons(MY_PORT_NUM);
   my_addr.sin_addr.s_addr = inet_addr(IPbuffer);

   //  printf("my IP %d and Port is %d",my_addr.sin_addr.s_addr,my_addr.sin_port);

   if (bind(connSock, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) == 0)
      printf("Binded Correctly\n");
   else
      printf("Unable to bind\n");

   socklen_t addr_size = sizeof srv_addr;
   int con = connect(connSock, (struct sockaddr*) &srv_addr, sizeof srv_addr);
   if (con == 0)
      printf("Connected\n");
   else
      printf("Error in Connection\n");
   return connSock;
}



//sendOverTCP
void sendOverTcp(int connSock)
{
   printf("SENDING DATA !!\n");
   printf (" Data : %s\n\n", (char *) tokenMsg);
  // send(connSock,tokenMsg, 256, 0);



 void * my_tok2;
 while(1)
   {
      while((my_tok2 = dequeue(udpClientIncQueue)) != NULL)
      {
         char * my_tok3 = (char *) my_tok2;
         int ret = send(sock,tokenMsg, 256, 0);
         
         if(ret == -1 )
         {
            perror("tcp_send()");
         }
         else
         {
            printf("Successfully sent %d bytes data to server %s \n", ret,my_tok3);
         }

         printf("going for next loop \n");
      }
   }
   printf("done with loop \n");
   pthread_exit(0);



}

//recvOverTcp()
void recvOverTcp(int connSock)
{

   printf("RECEIVING DATA !!\n");
   //bzero (buffer, MAX_BUFFER + 1);
   //recv(sock, buffer, 256, 0); 
   //printf (" Data : %s\n", (char *) buffer);

   int * recv_soc = &sock;
   printf("RECEIVING DATA over tcp assoc !!%d!\n", *recv_soc);
   int  in=0,flags;
   char buffer[MAX_BUFFER+1];
   while(1)
   {
      bzero (buffer, MAX_BUFFER + 1);
      in = recv(sock, buffer, 1024, 0); 
      printf("in %d",in);
      if( in == -1)
      {
         printf("Error in tcp_recv\n");  //ONLY FOR DEMO
         perror("tcp_recv()");           //ONLY FOR DEMO
         close(recv_soc);
      }
      else
      {
         //Add '\0' in case of text data
         buffer[in] = '\0';
         printf (" Data : %s length %d \n", (char *) buffer, in);
         enqueue(udpClientOutQueue,(char *)buffer);

         int n=sendto(server, (const char *)buffer, strlen(buffer),
             MSG_CONFIRM, (const struct sockaddr *) &peer_addr,
             sizeof(peer_addr));
         if(n==-1)
         {
            char *ip = inet_ntoa(peer_addr.sin_addr);
            printf("ERROR IN SEND %d peer addr %s\n",n ,ip);
         }
         else
         {
            printf("MESSAGE SENT !%s! \n",buffer);
         }
      }
   }
  pthread_exit(0);
}

    ////////////////////////////////////////////////////////////////////
   //                                                                //
  //            SCTP protocol related methods.                      //
 //            connet, send and received methods.                  //
////////////////////////////////////////////////////////////////////

//connect SCTP 

int connectSCTP(char * buff)
{
   struct sockaddr_in servaddr;
   struct sockaddr_in myaddr;
   int ret = 0;
   printf("HELLO !! THIS IS SCTP connect\n");

   //getIp
   getIp();

   connSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);

   if (connSock == -1)
   {
      printf("Socket creation failed\n");
      perror("socket()");
      exit(1);
   }

   bzero ((void *) &servaddr, sizeof (servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(tokenPort));
   servaddr.sin_addr.s_addr = inet_addr (tokenIP);

   bzero ((void *) &myaddr, sizeof (myaddr));
   myaddr.sin_family = AF_INET;
   myaddr.sin_port = htons(MY_PORT_NUM);
   myaddr.sin_addr.s_addr = inet_addr (IPbuffer);
   printf("\n\nHost IP: %s and Port is\n\n", IPbuffer);

   int retBind = bind (connSock, (struct sockaddr *) &myaddr, sizeof (myaddr));

   if(retBind == -1 )
   {
//      printf("Bind failed \n");
//      perror("bind()");
//      close(connSock);
//      exit(1);
   }

   //connect
   ret = connect (connSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

   if (ret == -1)
   {
      printf("Connection failed\n");
      perror("connect()");
      close(connSock);
    //  exit(1);
   }

   printf("\nconnected on socket %d %s \n",connSock,tokenMsg);

   return connSock;
}


void * sendOverSctp(void * args)
{
   printf("SENDING DATA  in new thread !%s! \n",tokenMsg);
   char * my_tok = (char *) args;
   struct sctp_sndrcvinfo sndrcvinfo;
   int ret;
   void * my_tok2;
   while(1)
   {
      while((my_tok2 = dequeue(udpClientIncQueue)) != NULL)
      {
         char * my_tok3 = (char *) my_tok2;
         ret = sctp_sendmsg (sctp_sock, (void *)my_tok3, (size_t) strlen(my_tok3),
                         NULL, 0, 0, 0, 0, 0, 0);
         if(ret == -1 )
         {
            perror("sctp_sendmsg()");
         }
         else
         {
            printf("Successfully sent %d bytes data to server %s \n", ret,my_tok3);
	 }

         printf("going for next loop \n");
      }
   }
   printf("done with loop \n");
   pthread_exit(0);
}

void * recvOverSctp(void * connSock)
{
   struct sctp_sndrcvinfo sndrcvinfo;
   int * recv_sctp_soc = &sctp_sock;
   printf("RECEIVING DATA over sctp assoc !!%d!\n", *recv_sctp_soc);
   int  in,flags;
   char buffer[MAX_BUFFER+1];
   while(1)
   {
      bzero (buffer, MAX_BUFFER + 1);
      in = sctp_recvmsg (*recv_sctp_soc, buffer, sizeof (buffer),
	 (struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);

      if( in == -1)
      {
//         printf("Error in sctp_recvmsg\n");  ONLY FOR DEMO
//         perror("sctp_recvmsg()");           ONLY FOR DEMO
         //  close(connSock);
      }
      else
      {
         //Add '\0' in case of text data
         buffer[in] = '\0';
         printf (" Data : %s length %d \n", (char *) buffer, in);
//      enqueue(udpClientOutQueue,(char *)buffer);

         int n=sendto(server, (const char *)buffer, strlen(buffer),
             MSG_CONFIRM, (const struct sockaddr *) &peer_addr,
             sizeof(peer_addr));
         if(n==-1)
         {
            char *ip = inet_ntoa(peer_addr.sin_addr);
            printf("ERROR IN SEND %d peer addr %s\n",n ,ip);
         }
         else
         {
            printf("MESSAGE SENT !%s! \n",buffer);
         }
      }
   }
  pthread_exit(0);
}

void * sendToCLI(void * udpClientOutQueue)
{
   printf("send to udp client thread created \n");
   int n = 0;
   Queue * myudpClientOutQueue = (Queue *)udpClientOutQueue; 
   void * outMsg;
   while(1)
   {
      while((outMsg = dequeue(myudpClientOutQueue)) != NULL)
      {
         char *outMsg2 = (char *)outMsg;
         printf("send to udp client thread message just received !%s! server %d\n",outMsg2,server);
   
         n=sendto(server, (const char *)outMsg, strlen(outMsg),
                  MSG_CONFIRM, (const struct sockaddr *) &peer_addr,
                  sizeof(peer_addr));
         if(n==-1)
         {
            char *ip = inet_ntoa(peer_addr.sin_addr);
            printf("ERROR IN SEND %d peer addr %s\n",n ,ip);
         }
         else
         {
            printf("MESSAGE SENT !%s! \n",outMsg2);
         }
      }
   }
   pthread_exit(0);
}

int main()
{

   if (pthread_mutex_init(&lock, NULL) != 0) 
   { 
      printf("\n mutex init has failed\n"); 
      return 1; 
   } 

   int len, n;
   static int loop = 0;

   //SERVER CREATION
   server = socket(AF_INET, SOCK_DGRAM, 0);
   if (server == 0)
   {  
      printf("Error in server creating\n");
      return 1;
   }   
   else
      printf("Server Created %d\n",server);

   char buffer[MAX_BUFFER+1];
   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;

   getIp();
   my_addr.sin_port = 12000;

   if (bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr)) == 0)
      printf("Binded Correctly\n");
   else
   {
      printf("Unable to bind\n");
      return 1;
   }

   len = sizeof(peer_addr);
   udpClientIncQueue = initQueue(max);
   udpClientOutQueue = initQueue(max);

   while(1)
   {
      printf("\n Going for receive \n");
      n = recvfrom(server, (char *)buffer, MAXLINE,
    	           MSG_WAITALL, ( struct sockaddr *) &peer_addr,
	           &len);
      char *ip = inet_ntoa(peer_addr.sin_addr);
      buffer[n] = '\0';
      printf("Client : %s ip %s ip %d port %d\n", buffer,ip,len, (int) ntohs(peer_addr.sin_port));

//    enqueue(udpClientIncQueue,(void *)buffer);   
      tokenize(buffer);

          //SWITCH FOR PROTOCOLTYPES
         //===================================
        //Connect
       // int sock=connectSCTP(buffer);
      // RGV making soc global.
    
  if(loop == 0)
      {
         //sctp_sock = connectSCTP(buffer); 
           sock=connectTCP(buffer);
         //int sock=connectUDP(buffer);
          // sendOverTcp(sock);
           //sleep(3);
           //recvOverTcp(sock);
   
         pthread_t sctp_client_sender;

         pthread_t sctp_client_receiver;

         pthread_t udp_client_sender;
         int ne = 0;
         if(( ne = pthread_create(&sctp_client_sender, NULL, sendOverTcp,tokenMsg)))
            printf("Error creating pthread tcp sender %d\n", ne);
         else
            printf("tcp_client_sender thread created socket %d \n",sock);


         int ne1 = 0;
         if(( ne1 = pthread_create(&sctp_client_receiver, NULL, recvOverTcp,&sock)))
         {
            printf("Error creating pthread tcp_client_receiver %d\n", ne1);
         }
         else
            printf("tcp_client_receiver thread created socket %d \n",sock);


         //   int ne2 =  0;
         //   if(( ne2 = pthread_create(&udp_client_sender, NULL, sendToCLI,udpClientOutQueue)))
         //   {
         //      printf("Error creating pthread udp_client_sender %d \n", ne2);
         //   }
         //   else
         //      printf("udp_client_sender thread created socket %d \n",server);
         //   pthread_join(udp_client_sender, NULL);


      }
      loop++;
   } 
   printf("\nEND OF APP\n!!");
}
