/* time_server.c - main */

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <time.h> 
    #include <stdbool.h>


    /*------------------------------------------------------------------------
     * main - Peer-by-Peer Applications
     *------------------------------------------------------------------------
     */
    #define BUFLEN		256

    int
    main(int argc, char *argv[])
    {
        struct  sockaddr_in fsin;	/* the from address of a client	*/
        char	buf[100],list[1000];		/* "input" buffer; any size > 0	*/
        char    *pts;
        int	sock;			/* server socket		*/
        time_t	now;			/* current time			*/
        int	alen;			/* from-address length		*/
        struct  sockaddr_in sin; /* an Internet endpoint address         */
        int     s, type, tag;        /* socket descriptor and socket type    */
        int 	port=3000;
        struct pdu{
            char type;
            char peer_name[10];
            char content_name[10];
            char address[100];
            unsigned short port;
            char data[100];
        };
        struct content_server{
            char peer_name[10];
            char content_name[10];
            char address[100];
            unsigned short port;
        }; 
        struct content_server arr_content_servers[10]={"A","B","C",1};
        struct content_server desired_content_server;



        int n, r, bytes_to_read, index = 0;
        int words = 0,byteSize, bytes_written;
        char c, *foo;
        char fileName[BUFLEN];

        switch(argc){
            case 1:
                break;
            case 2:
                port = atoi(argv[1]);
                break;
            default:
                fprintf(stderr, "Usage: %s [port]\n", argv[0]);
                exit(1);
        }

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);

        /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
            fprintf(stderr, "can't creat socket\n");

        /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
            fprintf(stderr, "can't bind to %d port\n",port);
        listen(s, 5);
        alen = sizeof(fsin);
        
        
        bool remove_content(struct pdu given) //make 1 and 0 instead of bool
        {
            for(int k=0;k<10;k++)
            {
                if (strcmp(arr_content_servers[k].peer_name, given.peer_name) == 0 && strcmp(arr_content_servers[k].content_name, given.content_name) == 0)
                {
                    memset(arr_content_servers[k].peer_name, 0, sizeof arr_content_servers[k].peer_name);
                    memset(arr_content_servers[k].content_name, 0, sizeof arr_content_servers[k].content_name);
                    arr_content_servers[k].port = 0;
                    return true;
                }
            }
            
            return false;
        }


        void dequeue(int tag) // not traditional a queue idk what to call it
        {
           	
            arr_content_servers[index] = arr_content_servers[tag];
            index++;         
	    memset(arr_content_servers[tag].peer_name, 0, sizeof arr_content_servers[tag].peer_name);
            memset(arr_content_servers[tag].content_name, 0, sizeof arr_content_servers[tag].content_name);
            arr_content_servers[tag].port = 0;		             
            
        }


        int find_content_server(struct pdu given)
        {  
            
            for(int n=0;n<10;n++)
            {	
            
                if (strcmp(arr_content_servers[n].content_name, given.content_name)==0)
                {
                    desired_content_server = arr_content_servers[n];
                    tag = n;
                    
                    return 1;
                }
            }
            return 0;
        }
        
        int find_matching_peer_name_content_server(struct pdu given)
        {
        
         
            for(int r=0;r<10;r++)
            {	
                if (strcmp(arr_content_servers[r].peer_name, given.peer_name)==0 && strcmp(arr_content_servers[r].content_name, given.content_name)==0)
                {
                    printf("Peer: %s already registered this content: %s\n",arr_content_servers[r].peer_name, arr_content_servers[r].content_name);
                    return 1;
                }
            }
            return 0;
        }
        
        while (1) {
            struct pdu spdu,rpdu;


            //server obtains fileanme
            if (recvfrom(s, &rpdu, sizeof(rpdu), 0,
                         (struct sockaddr *)&fsin, &alen) < 0)
                fprintf(stderr, "recvfrom error\n");

            FILE *f;
            struct content_server *bp;

            if(rpdu.type == 'R')
            {
		printf("Regestriation Begins\n");
                if(find_matching_peer_name_content_server	(rpdu))
                {	
                 spdu.type='E';
                    sprintf(spdu.data,"This peer is already registerd");
                    sendto(s, (struct pdu*)&spdu, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                }
                else
                {
                    
                    strcpy(arr_content_servers[index].peer_name, rpdu.peer_name);
                    strcpy(arr_content_servers[index].content_name, rpdu.content_name);
                    strcpy(arr_content_servers[index].address, rpdu.address);        
                    arr_content_servers[index].port = rpdu.port;
                          
                    index++;
                    
                    

                    spdu.type='A'; //Acknowledgement //Peer create open TCP socket client side
                    sendto(s, (struct pdu*)&spdu, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                }
            }

            if (rpdu.type == 'S') //Search Request
            {
            	printf("Search Request Begin\n");
            	
            	
                if(!find_content_server(rpdu))
                {
                    printf("Error is happening \n");   
                    spdu.type='E';
                    sprintf(spdu.data,"A peer with this content name can not be found");
                    sendto(s, (struct pdu*)&spdu, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                }
                else
                {
                    printf("Send Port to Client\n");
                    spdu.type = 'S';
                    strcpy(spdu.address, desired_content_server.address);
                    strcpy(spdu.content_name, desired_content_server.content_name);
                    strcpy(spdu.peer_name, desired_content_server.peer_name);     
                    
                    spdu.port = desired_content_server.port;
                    
                    printf("Desired Content Server %d \n",desired_content_server.port);
                    
                    dequeue(tag);
                               
                        sendto(s, (struct pdu*)&spdu, 1000, 0,
                            (struct sockaddr *)&fsin, sizeof(fsin));

                }
            }

            if(rpdu.type == 'O')
            {
            	 spdu.type='O';
                bp=arr_content_servers;
                            
                    sendto(s, (struct content_server*)&arr_content_servers, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                     
                    
            }

            if (rpdu.type == 'T')
            {
            	printf("Removing Content from List for Peer   \n ");
                if (remove_content(rpdu))
                {
                
                    printf("Acknowledge the peer is deleted \n");
                    spdu.type='A'; //Acknowledgement //Peer has been removed from content server list
                    sendto(s, (struct pdu*)&spdu, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                }
                else
                {
                    spdu.type='E';
                    sendto(s, (struct pdu*)&spdu, 1000, 0,
                           (struct sockaddr *)&fsin, sizeof(fsin));
                }
            }

        }
    }
