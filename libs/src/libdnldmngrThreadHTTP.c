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



void *httpdownloadThread( void *Thread_url1)
{
   extern int h_errno;
   thread_data Thread_url = *((thread_data *)Thread_url1);

  //((thread_data *)(thread_url1))->
  //(*thread_url1)->

   URL localURL = Thread_url.url;
   char * hosturl;
   unsigned long start_pos = Thread_url.start_pt;
   unsigned long end_pos = Thread_url.end_pt;
   int flag_Rec_found = 0 ;
   long f_position ;

   char * start_byte;
   char * end_byte ;
   int numdigits=0;

   char c;
   char * host = localURL.hostname;

   int port = localURL.portnumber;
   char * directory = localURL.directory;
   char * file = localURL.filename;

   int sockfd;
   char traceTID[20] = "THREAD--";
   char *templine;

   int n , i ;
   int count;
   int lentl;

   FILE *f_cont , *f_stat;
   int no_ofThread , Thread_index;
   stat_downloaded * dnld_status;
   long size_stat_downloaded = sizeof(stat_downloaded);


   pthread_t tid = pthread_self();
   struct sockaddr_in serv_addr;
   struct in_addr inet_address;
   struct hostent * host_details;
   char sendline[MAXSIZE] , *recvline;

   // for Error trapping

   char *tempresp, * ptr_Response;

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
	  fprintf(stderr, "File - fstatus does not exists\n");
	  return ERR_http_thread ;
     }



     for (i=1;i<=no_ofThread ;i++)
       if(tid== thread_id[i-1]) {
       	     char * id = (char *)malloc( 2 *sizeof(char));
       	     sprintf(id , "%d", i);
	     strcat(traceTID, id);
	     break;
       }

     Thread_index = i ;



   printf("\nEntered %s\n",traceTID);

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
		return ERR_http_thread ;

         case NO_ADDRESS:
         	fprintf(stderr,"prodname: No IP adress associated with host\n");
		return ERR_http_thread ;
         case NO_RECOVERY:
         	fprintf(stderr,"prodname: Unrecoverable NameServer Error\n");
		return ERR_http_thread ;
      }

   }
   inet_address= *(struct in_addr *)(host_details->h_addr_list[0]);
   bzero((char *) &serv_addr , sizeof(serv_addr) );
   serv_addr.sin_family=AF_INET;
   serv_addr.sin_addr.s_addr=inet_address.s_addr;
   serv_addr.sin_port=htons(port);

   if((sockfd=socket(AF_INET, SOCK_STREAM,0))<0)
   {
   	fprintf(stderr,"prodname: stream Socket can't be open\n");
	return ERR_http_thread ;
   }
   if((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))<0)
   {
   	fprintf(stderr, "prodname: Can't connect to server");
	return ERR_http_thread ;
   }

   // Making HTTP Requesting for  the content of desired Range


   strcpy(sendline,"GET ");
   strcat(sendline,directory );
   strcat(sendline,file);
   strcat(sendline," HTTP/1.1\nHost:");
   strcat(sendline,host);
   strcat(sendline,"\n");
   strcat(sendline,"Range: bytes=");
   strcat(sendline,start_byte);
   strcat(sendline,"-");
   strcat(sendline,end_byte);
   strcat(sendline,"\r\n");
   //printf("\n Request Sent: %s\n", sendline);

   n=strlen(sendline);
   if(writen(sockfd, sendline,n)!=n)
   {
   	fprintf(stderr, "prodname: error writing to socket\n");
    	return ERR_http_thread ;
   }


   // Read from socket and write it to std. o/p


   recvline = (char *)malloc(255 * sizeof(char));
   strcpy(recvline,"");
   tempresp = (char *)malloc(sizeof(char));
   strcpy(tempresp,"");




    // Escaping the Header

    do{

      	n = readline(sockfd, recvline);

      	if(n < 0)
      	{

       		fprintf(stderr,"prodname: Error Reading Incoming Data \n");
          	return(0);

      	}
      	{
      		unsigned long lenr = strlen(recvline);
      		unsigned long lent = strlen(tempresp);
      		tempresp=(char *)realloc(tempresp , (lenr + lent + 1) * sizeof(char));
		strcat(tempresp,recvline);

      	}


      	if(strcmp(recvline,"\r\n")==0)
      	  	break;
    }while(n);


      //Trapping the Error-----

    if((ptr_Response = strtok(tempresp, " "))!=NULL){

	   char c , prev ;
	   int count;
	   count=0;
	   prev=' ' ;

	   ptr_Response = strtok(NULL , " ");

	   if(strcmp(ptr_Response,"206") &&  strcmp(ptr_Response,"200")) /*Added 200 response code for IIS 5*/
	   {
	   	printf("Prodname ::%s ERROR IN REQUEST ::-->\n", traceTID );
	   		for(c = ' '; ; )
		   	{

		    		printf("%c",c);
	      			if((c=='\r'))
		        	    printf("\r\n");
			            //if((c=='\n'))
		            	    c = *tempresp++;
	        	    	    if((prev=='\r') && (c=='\n'))
	            		     	count++;

		            	    if(count==5)
		            	     // exit(1);
		            	    prev = c;
		        }
		        //return ERR_http_thread ;

	   }
	   else
	   {

	   	printf("%s - DOWNLOADING  Content\n",traceTID);

	   }

    }

   // Reading the contents

   templine = (char *)malloc(2 * sizeof(char));
   strcpy(templine,"");


   // Writing to file
  // printf("Testing -- file in thread-%s\n",file);

   if((f_cont = fopen(file, "r+"))!=NULL)
   {

   	fseek(f_cont,start_pos,SEEK_SET);
   }
   else
   {
   	fprintf(stderr,"prodname: Error in opening --file \n");
       	return ERR_http_thread ;
   }


   printf(" COUNT -  POS--%ld----%ld\n",start_pos, end_pos);
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
					       return ERR_http_thread ;
					}


               }


       }
       else if(n==0) // actually '-1' indicates end of file
       {
       	      if(count==0)
       	      {
       	      	fprintf(stderr,"prodname: No data is Read \n");
		return ERR_http_thread ;
       	      }
       }
       else if(n<0)
       {
		fprintf(stderr, "prodname: Error Reading Incoming Data\n");
	        return ERR_http_thread ;
       }

       lentl = strlen(templine);
       templine = (char *)realloc(templine, (lentl + 2)*sizeof(char) );
   }


   fclose(f_stat);
   fclose(f_cont);
   //printf("%s :::%s\n",traceTID,templine);

   return NULL;

}


