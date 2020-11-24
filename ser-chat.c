/*Hector Acevedo, Arturo Casillas, Aldo Sebastian*/

/* -------------------------------------------------------------------------- */
/* chat server                                                                */
/*                                                                            */
/* server program that works with datagram-type sockets by receiving the mes- */
/* sages of a set of clients, displaying them, and returning the message sent */
/* by each client to the rest, until the last client sends an "exit" command  */
/*                                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* libraries needed for the program execution                                 */
/* -------------------------------------------------------------------------- */
#include <netinet/in.h>                /* TCP/IP library                      */
#include <arpa/inet.h>                 /* Newer functionality for  the TCP/IP */
                                       /* library                             */
#include <sys/socket.h>                /* sockets library                     */
#include <sys/types.h>                 /* shared data types                   */
#include <stdio.h>                     /* standard input/output               */
#include <unistd.h>                    /* unix standard functions             */
#include <string.h>                    /* text handling functions             */
#include <time.h> //time handling

/* -------------------------------------------------------------------------- */
/* global definitions                                                         */
/* -------------------------------------------------------------------------- */
#define  BUFFERSIZE 1024               /* buffer size                         */
#define  MAX_MEMBERS 10                /* maximum number of members in room   */

/* -------------------------------------------------------------------------- */
/* global variables and structures                                            */
/* -------------------------------------------------------------------------- */
struct data
  {
    int  data_type;                              /* type of data sent         */
    int  chat_id;                                /* chat id sent by server    */
    char data_text[BUFFERSIZE-(sizeof(int)*2)];  /* data sent                 */
  };

struct member
  {
    int    chat_id;                           /* chat id                      */
    char   alias[BUFFERSIZE-(sizeof(int)*2)]; /* member alias                 */
    struct sockaddr_in address;               /* address of the member        */    
  };

/* -------------------------------------------------------------------------- */
/* main ()                                                                    */
/*                                                                            */
/* main function of the system                                                */
/* -------------------------------------------------------------------------- */
main()
  {
    struct sockaddr_in sock_read;      /* structure for the read socket       */
    struct sockaddr_in sock_write;     /* structure for the write socket      */
    struct data message;               /* message to sendto the server        */
    char   text1[BUFFERSIZE];          /* reading buffer                      */
    char   *client_address;            /* address of the sending client       */
    int    participants;               /* number of participats in the chat   */
    int    i,j;                        /* counters                            */
    int    sfd;                        /* socket descriptor                   */
    int    read_char;                  /* read characters                     */
    socklen_t  length;                 /* size of the read socket             */
    struct member list[MAX_MEMBERS];   /* list of members in room             */
    time_t hb[MAX_MEMBERS]; //list of times, last heartbeat received
    time_t time_hb; //time of heartbeat 

    /* ---------------------------------------------------------------------- */
    /* structure of the socket that will read what comes from the client      */
    /* ---------------------------------------------------------------------- */
    sock_read.sin_family = AF_INET;    
    sock_read.sin_port   = 10011; 
    inet_aton("200.13.89.15",(struct in_addr *)&sock_read.sin_addr);
    memset(sock_read.sin_zero, 0, 8);

    /* ---------------------------------------------------------------------- */
    /* Instrucctions for publishing the socket                                */
    /* ---------------------------------------------------------------------- */
    sfd = socket(AF_INET,SOCK_DGRAM,0);
    bind(sfd,(struct sockaddr *)&(sock_read),sizeof(sock_read));

    /* ---------------------------------------------------------------------- */
    /* Inicialization of several variables                                    */
    /* ---------------------------------------------------------------------- */
    length = sizeof(struct sockaddr);
    message.data_text[0] = '\0';
    participants = 0;
    for (i=0; i<MAX_MEMBERS; ++i)
      list[i].chat_id = -1;

    /* ---------------------------------------------------------------------- */
    /* The socket is  read and the messages are  answered until the word exit */
    /* is received by the last member of the chat                             */
    /* ---------------------------------------------------------------------- */

    while (strcmp(message.data_text,"shutdown") != 0)
      {

        /* first we read the message sent from any client                     */
        read_char = recvfrom(sfd,(struct data *)&(message),sizeof(struct data),0,(struct sockaddr *)&(sock_write),&(length));
        printf("Type:[%d], Part:[%d], Mess:[%s]\n",message.data_type, message.chat_id, message.data_text);
        
        /* ------------------------------------------------------------------ */
        /* Now we display the address of the client that sent the message     */
        /* ------------------------------------------------------------------ */
        client_address = inet_ntoa(sock_write.sin_addr);
        printf("Server: From Client with Address-[%s], Port-[%d], AF-[%d].\n", client_address, sock_write.sin_port, sock_write.sin_family);

        printf("Participants when msg is received: %d\n", participants);        
        /* if data_type == 0, it means that the client is logging in          */
        if (message.data_type == 0)    /* Add new member to chat room         */
          {
          	/* We look for the first available  empty space in the list array */
          	/* which will have a value of -1 in its chat_id field             */
            i=0;
            while ((list[i].chat_id != -1) && (i < MAX_MEMBERS)) ++i;
            if (i >= MAX_MEMBERS)
              i=-1;                    /* i = -1 means client rejected        */
            else
              {
                list[i].chat_id = i;
                strcpy(list[i].alias, message.data_text);
                memcpy((struct sockaddr_in *)&(list[i].address), (struct sockaddr_in *)&(sock_write), sizeof(struct sockaddr_in));  
                ++ participants;              
              }
            printf("Participants in Room:[%d]\n", participants);
            printf("Value of i:[%d]\n", i);
            sendto(sfd,(void *)&(i),sizeof(int),0,(struct sockaddr *)&(sock_write),sizeof(struct sockaddr));      
            
            
            /* We notify the entrance to the chat of the new participant      */
            /* Variable i still keeps the number of the last member added     */
            sprintf(text1,"Participante [%s] se ha unido al grupo.\n",message.data_text);
            for (j=0; j < MAX_MEMBERS; ++j)
              if ((j != i) && (list[j].chat_id != -1))
                sendto(sfd,text1,strlen(text1),0,(struct sockaddr *)&(list[j].address),sizeof(struct sockaddr_in));

            time(&time_hb);
            hb[i] = time_hb;      
          }

        /* if data_type == 1, it means that this is a message                 */
        if (message.data_type == 1)
          {
            /* if the message received is the word "exit", it  means that the */
            /* client is leaving the room, so we report that to the other cli */
            /* ents, and change the value of its id in the list to -1         */
            if (strcmp(message.data_text,"exit") == 0)
              {
                sprintf(text1,"Client [%s] is leaving the chat room.",list[message.chat_id].alias);
                list[message.chat_id].chat_id = -1;
                -- participants;
              }
            else
              {
                sprintf(text1,"[%s]:[%s]",list[message.chat_id].alias, message.data_text);
              }
            for (i=0; i < MAX_MEMBERS; ++i)
              if ((i != message.chat_id) && (list[i].chat_id != -1))
                sendto(sfd,text1,strlen(text1),0,(struct sockaddr *)&(list[i].address),sizeof(struct sockaddr_in));      
          }
        /* If data_type == 2, it means this is a heartbeat signal */
        if (message.data_type == 2){
          double time_diff;
          time(&time_hb); //get time
          for(i=0; i < participants; i++){
            if(i == message.chat_id){
              hb[i] = time_hb;
            }
            time_diff = difftime(time_hb, hb[i]);
            if(time_diff > 30){
              sprintf(text1,"Client [%s] disconnected due to inactivity.",list[i].alias);
              printf("Participant [%s] disconnected", list[i].alias);
              list[i].chat_id = -1;
              -- participants;
              for (j=0; j < MAX_MEMBERS; ++j)
              if ((j != i) && (list[j].chat_id != -1))
                sendto(sfd,text1,strlen(text1),0,(struct sockaddr *)&(list[j].address),sizeof(struct sockaddr_in));
            }
          }
        }
      }
    shutdown(sfd, SHUT_RDWR);
    close(sfd);
    return(0);
  }


