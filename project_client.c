/* time_client.c - main */

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>                                                                            
#include <netinet/in.h>
#include <arpa/inet.h>
                                                                                
#include <netdb.h>

#define	BUFSIZE 64
#define 	BUFLEN 100

#define	MSG		"Any Message \n"


/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int
main(int argc, char **argv)
{
	char	*host = "localhost";
	int	port = 3000;
	//char	contents[1000];	/* 32-bit integer to hold time	*/ 
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin,fsin,client,num_fd, sock_addr, content_server; /* an Internet endpoint address		*/
	int	s, s1, s2, sock, n, type, new_sd, client_len;	/* socket descriptor and socket type	*/
	s1 = 0;

	switch (argc) {
	case 1:
		break;
	case 2:
		host = argv[1];
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;                                                               
        sin.sin_port = htons(port);
                                                                                        
    /* Map host name to IP address, allowing for dotted decimal */
        if ( phe = gethostbyname(host) ){
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        }
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		fprintf(stderr, "Can't get host entry \n");
                                                                                
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "Can't create socket \n");
		                                                                            
    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "Can't connect to %s %s \n", host, "Time");

//create struct
	char command; 
	char filename[10],username[10],myIP[16];
	struct pdu{
	char type;
	char peer_name[10];
    	char content_name[10];
    	char address[100];
    	unsigned short port;
	char data[101];
	};
	struct content_server{
            char peer_name[10];
            char content_name[10];
            char address[100];
            unsigned short port;
        }; 
        struct content_server content_list[10];
	int bytes_read,alen,alen1, i;
	unsigned short myPort;
	fd_set rfds, afds;
	
	struct pdu rpdu,spdu,tpdu;
	char cmd [100],buf3[500],buf[BUFLEN];
	FILE *fp, *fp1;
	char *bp;
	int download,num_port,byteSize, bytes_written;
	struct sockaddr_in reg_addr;
	num_port = 0;
	
	unsigned short create_tcp_connection()
	{			
		s1 = socket(AF_INET,SOCK_STREAM, 0);

		/* Create a stream socket	*/	
		if ( s1 == -1) {
			fprintf(stderr, "Can't creat a socket\n");
			exit(1);
		}


		reg_addr.sin_family = AF_INET;
		reg_addr.sin_port = htons(0);
		reg_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		if(bind(s1, (struct sockaddr *)&reg_addr, sizeof(reg_addr)) == -1)
		{
			printf("Can't bind name to to socket");
			exit(1);
		}
		
		alen1 = sizeof(struct sockaddr_in);
		getsockname(s1,(struct sockaddr*)&reg_addr,&alen1);
		
		inet_ntop(AF_INET, &reg_addr.sin_addr, myIP, sizeof(myIP));

		myPort = reg_addr.sin_port;		
		

		
		FD_SET(s1,&afds);
		
		
		if(listen(s1, 10) == 0)
		{
			printf("Listening\n");
		}
		else
		{
			printf("Not Listening\n");
		}
		
		num_port++;
		
		return myPort;
	}

	//ask user for download or exit
	printf("Choose a  user name\n");
	n=read(0,username,100);
	username[n-1]='\0';

	if (10 > sizeof(spdu.peer_name))
	{
		printf("Choose an appropriate user name less than 10 bytes\n");
		exit(1);
	}
	
	

//create forever loop for multiple downloads
while(1){

		printf("Command: \n");
		
		FD_ZERO(&afds);
		FD_SET(s1,&afds);
		FD_SET(0,&afds);
		memcpy(&rfds,&afds,sizeof(rfds));
	
	if(select(FD_SETSIZE,&rfds, NULL, NULL, NULL) < 0)
	{
		exit(1);
	}
	
	if(FD_ISSET(0,&rfds))
	{
		scanf(" %c",&command);		
  		      	                	                    
	if (command == 'S')
	{
			spdu.type='S';
			
			printf("Enter the name of the Content:\n");
			n=read(0,spdu.content_name, 100);
			strcpy(spdu.peer_name,username);
			
			
			spdu.content_name[n-1]='\0'; //make filename char string
			
			
			printf("DOWNLOAD : %s %s \n",spdu.peer_name,spdu.content_name);	
			
			write(s, &spdu, 1000); 
			printf("Sent \n");

			n=read(s, &rpdu, 1000);
			 
			if ( rpdu.type == 'E')
			{
				printf("Error \n");
			}
			else if( rpdu.type == 'S')
			{
				printf("Server sent back S type \n");
				tpdu.type='D';
				strcpy(tpdu.peer_name,rpdu.peer_name);
				strcpy(tpdu.content_name,rpdu.content_name);
				
				memset(&content_server, 0, sizeof(content_server));
        			content_server.sin_family = AF_INET;                                                                
			        content_server.sin_port = rpdu.port;
			        
			        
			        	
			        s2 = socket(AF_INET,SOCK_STREAM, 0);
				
				content_server.sin_addr.s_addr = inet_addr("127.0.0.1");
		                                                                    
    				/* Connect the socket */
        			if (connect(s2, (struct sockaddr *)&content_server, sizeof(content_server)) < 0)
        			{
        				fprintf(stderr, "Can't connect to %s %s \n", host, "Time");
        				exit(1);
				}
				write(s2, &tpdu, 1000);
				
				n=read(s2, &rpdu,200);
				
				printf("DOWNLOAD : T: %c P: %s CN: %s D: %s 	\n",rpdu.type, rpdu.peer_name, rpdu.content_name, rpdu.data);
				
				
				
				if(rpdu.type == 'C'){
				fp1 =fopen(rpdu.content_name,"w");
				
					while(rpdu.type == 'C'){
					
					rpdu.data[100] = '\0';
					
					printf("DATA: %s",rpdu.data);
			
					fprintf(fp1, "%s",rpdu.data);
				
					n = read(s2,&rpdu,200);
				}
				}
					
				
				fclose(fp1);
				close(s2);
				
				// Send registration for new content
				printf("\n");
				printf("Register new content: %s \n",tpdu.content_name);
				printf("\n");
				

				command = 'R';				
			}
	}
	
	if (command == 'R')
	{
			spdu.type='R';	
			spdu.port = create_tcp_connection();
			sprintf(spdu.address,"%d",reg_addr.sin_port);
			
			printf("Enter the name of the Content:\n");
			n=read(0,spdu.content_name, 100);
			
			strcpy(spdu.peer_name,username);
			spdu.content_name[n-1]='\0'; //make filename char string	
			
		 
		
			write(s, &spdu, 1000);
			printf("Regestriation Sent\n"); 
			
					
			n=read(s, &rpdu, 1000);
			
			if( rpdu.type == 'A')
			{
				printf("Successful Regestriation\n");
			}
			else if( rpdu.type == 'E')
			{
				printf("Regestriation Error \n");
			}			
	}
	
	if (command == 'O')
	{
		spdu.type='O';
		write(s, &spdu, 1000); 

		n=read(s, &content_list, 1000);

		printf("List of On-Line Registered Content: \n");
		for(int i=0;i<10;i++)
		{
		if(strlen(content_list[i].peer_name) > 1)
		{
		printf("Peer name: %s, Content Name: %s \n", content_list[i].peer_name, content_list[i].content_name);
		}
		}
	}	
	
	if (command == 'T')
	{
		spdu.type='T';
		
		printf("Enter the name of the Content you would like to de-register:\n");
		n=read(0,spdu.content_name, 100);
		spdu.content_name[n-1]='\0';
		strcpy(spdu.peer_name,username);
		
		
		write(s, &spdu, 1000); 
	
		n=read(s, &rpdu, 1000);

		if (rpdu.type == 'A')
		{
			printf("The content of this peer has been unregistered from the List of Content Servers\n");
		}
		else
		{	
			printf("Peer was not unregistered\n");
		}
	}
	
	}
	
	if(FD_ISSET(s1,&rfds))	
	{
		printf("SOCKET s1: %d \n", s1);
		alen = sizeof(client);
	
		new_sd = accept(s1, (struct sockaddr *)&client,&alen);
		
		if(new_sd == -1)
		{
			printf("DID NOT ACCEPT \n");
		}
		else
		{
			printf("ACCEPTED \n");
		}
		printf("TCP Connection Made \n");

		
	        n=read(new_sd,&rpdu,1000);
                	
             		
	    	if(rpdu.type == 'D'){
	    	
		
		printf("Downloading from somewhere else %s \n", rpdu.content_name);
		fp = fopen(rpdu.content_name, "r");
		
		if(fp == NULL){
			printf("File not found in peer \n");
		}else	
		{
			fseek(fp,0L, SEEK_END);
			byteSize = ftell(fp);	
			rewind(fp);	
			
			printf("Byte Size %d \n", byteSize);
		
			fgets(buf3, byteSize, fp);
			bp = buf3;
			rpdu.type = 'C';
			strcpy(rpdu.peer_name,username);
		
			while(byteSize>0){
			printf("Byte Size %d \n", byteSize);	
			printf("Data Sent: %s \n",rpdu.data);		
			strcpy(rpdu.data,bp);	
			rpdu.type = 'C';
			
			bytes_written = write(new_sd, &rpdu, 200);
			byteSize -=100;
			bp +=100; //go to the next 100 bytes
		
		}
			rpdu.type = 'A';
			write(new_sd, &rpdu, 101);
			close(new_sd);
		}
		
		}

	    }	
	    
	    }
		
	}
