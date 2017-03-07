
/* Thread function for ftp Download  */

#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "dnldmngr.h"
#include "libdnldmngr.h"

void *ftpdownloadThread( void *Thread_url1)
{
   extern int h_errno;
   thread_data Thread_url = *((thread_data *)Thread_url1);
  /*
  ((thread_data *)(thread_url1))->
  (*thread_url1)->
  */

   URL localURL = Thread_url.url;

   unsigned long start_pos = Thread_url.start_pt;
   unsigned long end_pos = Thread_url.end_pt;
   /*
   unsigned long Headlen = Thread_url.headlen;
   char *homedir = Thread_url.dest;
   unsigned long end_pos_with_head = end_pos + Headlen ;
   */

   char * start_byte;
   char * end_byte ;

   char c;
   char * host = localURL.hostname;
   /*
   char * protocol = localURL.protocol;
   */
   char * directory = localURL.directory;
   char * file = localURL.filename;
   int port = localURL.portnumber;

   //Latest variables
   int flag_Rec_found = 0 ;
   long f_position ;
   int numdigits=0;
   char traceTID[20] = "THREAD--";
   char * hosturl;

   FILE *f_stat;
   int no_ofThread , Thread_index;
   stat_downloaded * dnld_status;
   long size_stat_downloaded = sizeof(stat_downloaded);




   int sockfd;

   char *templine;

   //char source1[]="FIRST THREAD";
   //char source2[]="SECOND THREAD";
   int i, n;
   int count;
   int lentl;

   FILE *f_cont;

   char bk=' ';
   char *portpm1;
   char *portpm2;
   char * str_tok, *Recvline;
   unsigned long serv_port;


   pthread_t tid = pthread_self();
   struct sockaddr_in serv_addr;
   struct in_addr inet_address;
   struct hostent * host_details;
   char sendline[MAXSIZE] ;


   fflush(stdin);

   //   Reading From  file Fstatus

   hosturl = (char * ) malloc(strlen(Thread_url.HostUrl) + 1 );
   strcpy(hosturl , Thread_url.HostUrl );

   if((f_stat = fopen("Fstatus","rb+"))!=NULL)
     {
     	    	
    	rewind(f_stat);
 	dnld_status = (stat_downloaded *)malloc(size_stat_downloaded );		
	while(fread(dnld_status, size_stat_downloaded, 1, f_stat )==1)
	  {	
	  	printf("\nurl %s", dnld_status->url);
		if(!strcmp(dnld_status->url, hosturl))
		{
                 	printf("\nhostname found \n ");
			no_ofThread = dnld_status->no_of_thread ;
			flag_Rec_found = 1;

			f_position = ftell(f_stat) ;
			break;			
		}
		else
		{
			printf("not found");
		}				              	
	
	  } //while
	
	
	
   }
   else
   {
   	fprintf(stderr,"File - fstatus does not exists\n");
	return ERR_ftp_thread ;	
   }


   for (i=1;i<=no_ofThread ;i++)
   if(tid== thread_id[i-1])
   {
   	char * id = (char *)malloc( 2 *sizeof(char));
    	sprintf(id , "%d", i);       	
	strcat(traceTID, id);
	break;
   }

   Thread_index = i ;

   printf("\nEntered %s\n",traceTID);



   //getchar();
   {
      unsigned long temp=start_pos;

      do
      {
   	 temp/=10;
   	 numdigits++;
      }while (temp);

      start_byte=(char *)malloc((numdigits+1) * sizeof(char));

      temp=end_pos;

      do
      {
   	 temp/=10;
   	 numdigits++;
      }while (temp);
      end_byte=(char *)malloc((numdigits+1) * sizeof(char));

   }

   sprintf(start_byte,"%ld",start_pos);
   sprintf(end_byte,"%ld",end_pos);


   while((host_details=gethostbyname(host))==NULL)
   {
      switch(h_errno)	
      {
         case HOST_NOT_FOUND:
	        fprintf(stderr,"prodname : Unknown Host");
		return ERR_ftp_thread ;	

         case NO_ADDRESS:
         	fprintf(stderr,"prodname: No IP adress associated with host\n");
		return ERR_ftp_thread ;	
         case NO_RECOVERY:
         	fprintf(stderr,"prodname: Unrecoverable NameServer Error\n");
		return ERR_ftp_thread ;	
      }

   }
   inet_address= *(struct in_addr *)(host_details->h_addr_list[0]);
   bzero((char *) &serv_addr , sizeof(serv_addr) );
   serv_addr.sin_family=AF_INET;
   serv_addr.sin_addr.s_addr=inet_address.s_addr;
   serv_addr.sin_port=htons(port);


   if((sockfd=socket(AF_INET, SOCK_STREAM,0))<0)	
   {
   	fprintf(stderr, " prodname: Socket Cant be open \n") ;
	return ERR_ftp_thread ;
   }

   if((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))<0)	
   {
   	fprintf(stderr, "prodname: Cant connect to server\n");
	return ERR_ftp_thread ;	
   }
   bk=' ';
   while(1)
   {
      	
	  if((n=read(sockfd,&c,1)) >=0)
	  {
	  	printf("%c",c);
    	        // strcpy(bk,c);
	        if(c=='\n' && bk=='\r')
	      	   break;
	      	bk=c;
	      	          	         	
	  }
	  else
	  {      	
    		printf("Error in reading\n");
		return ERR_ftp_thread ;	    		
	  }
      	
   }
   strcpy(sendline,"USER ");
   strcat(sendline,ftpuser);	             	      		

   n=strlen(sendline);
   printf("\n %s before sending username\n", traceTID);
   //getchar();
   if(writen(sockfd, sendline, n )!=n)
   {
	fprintf(stderr, "prodname: Error in writing Socket\n" );
	return ERR_ftp_thread ;	    		
	
   }
   bk=' ';
   while(1)
   {
      	
	  if((n=read(sockfd,&c,1)) >=0)
	  {
	  	printf("%c",c);
    	        // strcpy(bk,c);
	        if(c=='\n' && bk=='\r')
	      	   break;
	      	 bk=c;
	      	          	         	
	   }
	   else
	   {
    		fprintf(stderr, "Error in reading\n");
		return ERR_ftp_thread ;	    		      	
	   }
      	
   }
   printf("\n %s user name sent\n",traceTID);
   //getchar();

   strcpy(sendline,"PASS ");
   strcat(sendline,ftppass);
	
   n=strlen(sendline);
   if(writen(sockfd, sendline, n )!=n)
   {
   	fprintf(stderr, "prodname: Error in writing Socket\n" );
	return ERR_ftp_thread ;
	
   }


   while(1)
   {
      	
	  if((n=read(sockfd,&c,1)) >=0)
	  {
      	        printf("%c",c);
	        // strcpy(bk,c);
      	        if(c=='\n' && bk=='\r')
      	           break;
      	        bk=c;
      	          	         	
      	    }
      	    else
      	    {
      	    	printf("Error in reading\n");
		return ERR_ftp_thread ;
            }

   }
   printf("%s Login finished", traceTID);
   //getchar();
   bk=' ';

   /* Changing The Directory if Required */

   if(strcmp(directory,"/"))
   {	             	
	strcpy(sendline,"CWD ");
	strcat(sendline,(directory+1));		
	n=strlen(sendline);
	if(writen(sockfd, sendline, n )!=n)
	{
		fprintf(stderr, "prodname: Error in writing Socket\n" );
		return ERR_ftp_thread ;
	}

	while(1)
	{
      	
		if((n=read(sockfd,&c,1)) >=0)
		{
	 		printf("%c",c);
	   		// strcpy(bk,c);
		    	if(c=='\n' && bk=='\r')
		     		break;
		      		bk=c;  	      	          	         	
		}
		else
		{
	  		fprintf(stderr, "Error in reading\n");
			return ERR_ftp_thread ;	  		
		}
      	
	}
		
   } // Directory

   printf(" %s directory changed", traceTID);
   //getchar();
   /* Entering into the passive mood*/

   strcpy(sendline,"PASV");
   //printf("PASSSSSSSsive\n");

   n=strlen(sendline);
   if(writen(sockfd, sendline, n )!=n)
   {
   	fprintf(stderr, "prodname: Error in writing Socket\n" );
	return ERR_ftp_thread ;	  		
	
   }
    //printf("\nPASV send %s\n", sendline);

   Recvline = (char *) malloc(2 * sizeof(char));
   strcpy(Recvline,"");
   count=0;
   while(1)
   {
   	if((n=read(sockfd,&c,1)) >=0)
   	{
	   	printf("%c",c);
	        *(Recvline + count) = c;
	        *(Recvline + count + 1) = '\0' ;
      		     		
	        if(c=='\n' && bk=='\r')
	          break;
 	        bk=c;
	   	{
		      unsigned long len = strlen(Recvline);
		      Recvline = (char *)realloc(Recvline, (len + 2) * sizeof(char) ); 	      	
	
	        }   	
	        count++;
	      	          	         	
         }
         else
         {
      	        fprintf(stderr, "Error in reading\n");
      	        return ERR_ftp_thread ;	  		
      	 }
   }

   printf("%s\n", Recvline);
   printf("\n %s changed to pasv mode",traceTID);
   //getchar();
   //exit(1);
   if((str_tok = strtok(Recvline,","))!=NULL)
   {
	  for(i=0;i<=4;i++)
	  {
	       str_tok = strtok(NULL,",");
	       if(i==3)
	       	 portpm1=str_tok;
	       else if(i==4)
	       	 portpm2=str_tok;	
	
	  }
	
   }
	
   printf("Token-%s-%s\n",portpm1, portpm2 );

   serv_port = atoi(portpm1)*256 + atoi(portpm2);
   serv_addr.sin_port = htons(serv_port);


  /*******/

   /*  FTP Request for setting FIle position  */

   strcpy(sendline,"REST ");
   strcat(sendline,start_byte);


   n=strlen(sendline);
   if(writen(sockfd, sendline, n )!=n)
   {
	fprintf(stderr, "prodname: Error in writing Socket\n" );
 	return ERR_ftp_thread ;	  			

   }

   while(1)
   {
   	if((n=read(sockfd,&c,1)) >=0)
   	{
		//printf("%c",c);
  		// strcpy(bk,c);
	   	if(c=='\n' && bk=='\r')
	    		break;
	      	bk=c;
	      	          	         	
      	}
      	else
      	{
      		printf("Error in reading\n");
      	        return ERR_ftp_thread ;	  		      		
      	}
   }


   /* Retrieving the file*/	
   strcpy(sendline,"RETR ");
   strcat(sendline,file);

   n=strlen(sendline);
   if(writen(sockfd, sendline, n )!=n)
   {
	fprintf(stderr, "prodname: Error in writing Socket\n" );
 	return ERR_ftp_thread ;	  		
	
   }

   printf(" %s Retrieving the file \n" , traceTID);

   if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
   {
   	fprintf(stderr,"prodname: Can''t open Stream Socket for PASV Connection \n");
    	return ERR_ftp_thread ;	  		
   }

   if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) )<0)
   {      	
   	fprintf(stderr, "prodname: can''t make PASV connection to Server \n");
    	return ERR_ftp_thread ;	  		
   }

   printf(" %s connected_PASV\n",traceTID);

   /* Read from socket and write it to FILE */

   templine = (char *)malloc(2 * sizeof(char));
   strcpy(templine,"");


   /* Writing to file*/

   if((f_cont = fopen(file, "r+"))!=NULL)
   {   	
   	fseek(f_cont,start_pos,SEEK_SET);
   }
   else
   {
       fprintf(stderr,"prodname: Error in opening --file \n");             	
       return ERR_ftp_thread ;	  		
   }


   printf(" %s  COUNT -  POS--%ld----%ld\n",traceTID , start_pos, end_pos);
   for(count=0; count<=end_pos-start_pos; count++)
   {
  	
       n=read(sockfd,&c,1);
       if(n==1)
       {
	       *(templine + count) = c;
	       *(templine + count + 1) = '\0';
	       fwrite(&c, sizeof(char), 1, f_cont);	
	       printf("%s---%d\n",traceTID , count);
	       	
	       	       	
               if( (count) && (count%500==0)  && (flag_Rec_found) )
               {

			//printf("pos- >> %ld >>\n",f_position);
			//printf("--fseek-Return-%d\n",fseek(f_stat, f_position - size_stat_downloaded , SEEK_SET));
			fseek(f_stat, f_position - size_stat_downloaded , SEEK_SET);
					
			if(fread(dnld_status, size_stat_downloaded, 1, f_stat )==1)
			{
				//printf("BEFORE-- %ld\n", dnld_status->data[Thread_index - 1].startpt );

				dnld_status->data[Thread_index - 1].startpt = dnld_status->data[Thread_index -1].startpt + 500 ;
				//printf("AFTER-- %ld\n", dnld_status->data[Thread_index - 1].startpt );					
					
				fseek(f_stat, -size_stat_downloaded, SEEK_CUR);
				fwrite(dnld_status, size_stat_downloaded, 1, f_stat );	
				//printf("---POS >> %ld >>\n",ftell(f_stat));
				//printf("startpt --updated \n");
			}
			else
			{
				fprintf(stderr,"prodname: Error Reading from file \n");							
		      	        return ERR_ftp_thread ;	  						
			}
					
				
                }
   	}		      	
       	else if(n==0)  // actually '-1' indicates end of file
       	{
       	      if(count==0)
       	      {
       	      	fprintf(stderr,"prodname: No data is Read \n");
      	       	return ERR_ftp_thread ;	  		
       	      } 	
       }
       else if(n<0)
       {
	        fprintf(stderr, "prodname: Error Reading Incoming Data\n");	
      	        return ERR_ftp_thread ;	  		
       }
       				
       lentl = strlen(templine);
       templine = (char *)realloc(templine, (lentl + 2)*sizeof(char) );
   }


   fclose(f_stat);
   fclose(f_cont);

  // printf("Before returning null\n");
return NULL;



}

