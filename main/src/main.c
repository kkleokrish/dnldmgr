#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#include "netdnld.h"
#include "dnldmngr.h"
#include "libdnldmngr.h"

int main(int argc, char *argv[])
{
   extern int  h_errno;
   char * HostName;
   int PortNumber;
   int sockfd , i;
   //--char *ptr_Response;
   //--char *ptr_filelen;
   //--char *tempresp , *temp;
   //--char **token_filelen;
   //--char *ptr_status;
   //--char **token_status;


   //--unsigned long filelen;




   //--thread_data *dnld_Thread1_data;
   //--thread_data *dnld_Thread2_data;

   thread_data *dnld_Thread_data = NULL;

   URL dnldURL;
   int ThreadNo_or_ErrCode;
   int ErrCode;

  /* int ErrStat; */



   struct sockaddr_in serv_addr;
   struct in_addr inet_address;
   struct hostent *host_details;
   //--char sendline[MAXSIZE], *recvline, *Recvline;

   char *recvline;

   //--FILE * f_cont;
   //--char * s;
   //--char * file;

   /* FTP variables */
   //--char c =' ';
   //--char * str_tok;
   //--char *portpm1;
   //--char *portpm2;

   //--char bk=' ';

   //--char *filesize ,*code_Retn, *fileline ;
   //--unsigned long file_len , serv_port;
   //--int i, count;

  /*  Resuming downloading   */

   //--FILE * f_stat;
   //char  Fstatus[20] = "Status";
   //--stat_downloaded * dnld_status;
   //--int size_stat_downloaded = sizeof(stat_downloaded);
   char * hosturl;


   if (argc<2) /*In Case The Program is invoked without any arguments*/
   {
      fprintf(stderr,"prodname: Usage : prodname  [<protocol://>]<hostname><Path-To-File> [<port number>]\n");
      exit(1);
   }
   	
   HostName=argv[1]; /*The First Argument Following the Program Name is PortNumber*/
   PortNumber=0;
   if (argc >= 3)    /*If more than three arguments are provided (including the Program Name) the third argument is the PortNumber*/
   {
      PortNumber=atoi(argv[2]);
      if (PortNumber<=0)
      {
         fprintf(stderr,"prodname: Invalid Port Number\n");
         exit(1);
      }
   }


//   int url_len = strlen(HostName);
  // printf("urllen-%d ", url_len);

   hosturl =  (char *)malloc( strlen(HostName) * sizeof(char)  + 1);
   strcpy(hosturl,HostName);
   //printf("hosturl--1-- %s\n",hosturl);

   ErrCode=splitURL(HostName, PortNumber, &dnldURL);
   if(!ErrCode)
   {
      printf("\n================================================\n");
      printf("Protocol            : %s\n", dnldURL.protocol);
      printf("HostName            : %s\n", dnldURL.hostname);
      printf("Port Number         : %d\n", dnldURL.portnumber);
      printf("Directory           : %s\n", dnldURL.directory);
      printf("File                : %s\n", dnldURL.filename);
      printf("\n=============================dfsd==================dg=\n");



      /* Getting  The initial details */


      printf("\n%s",dnldURL.hostname);
      while((host_details=gethostbyname(dnldURL.hostname))==NULL)
      {
         switch (h_errno)
         {
            case HOST_NOT_FOUND:
         	fprintf(stderr,"prodname: Unknown host\n");
         	return(0);
             case NO_ADDRESS:
         	fprintf(stderr,"prodname: No IP adress associated with host\n");
         	return (0);
             case NO_RECOVERY:
         	fprintf(stderr,"prodname: Unrecoverable NameServer Error\n");
         	return(0);
         }

      }

      inet_address= *((struct in_addr *) (host_details->h_addr_list[0]));

      bzero((char *) &serv_addr, sizeof(serv_addr) );

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = inet_address.s_addr;
      serv_addr.sin_port = htons(dnldURL.portnumber);
      //printf("Port Got- %u\n",serv_addr.sin_port);
      //printf("Port Got- %u\n",dnldURL.portnumber);


     printf("-----------Enter The No of Thread:--------\n");
     scanf("%d", &no_ofThread);


     if((sockfd=socket(AF_INET, SOCK_STREAM,0))<0)
      {
      	   fprintf(stderr,"prodname: Can't open Stream Socket \n");
      	   return(0);

      }

      if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0)
      {
           fprintf(stderr, "prodname: can't connect to Server \n");
           return(0);

      }
      printf("connected\n");

      recvline= (char *)malloc(255 * sizeof(char));
      strcpy(recvline, "");

      if((!strcmp(dnldURL.protocol,"http:")) || (!strcmp(dnldURL.protocol,"HTTP:")))
      {
	
      	    printf("before httpdnldmngr\n");
	    ThreadNo_or_ErrCode = httpdnldmngr ( sockfd , no_ofThread , dnldURL , &dnld_Thread_data , hosturl );
	    if(ThreadNo_or_ErrCode !=ERR_http_mod)
	    {
	    	//Code : Implementing Threads
	    	
	    	
	    	int flag_join_return=0;
	    	no_ofThread = ThreadNo_or_ErrCode ;
	    	
	    	
	    	join_Thread_return = (void *)malloc(no_ofThread * sizeof(int));
	    			
	        printf("--- Calling Threads In Main  ----\n");
	
		for(i=0; i< no_ofThread ; i++)
		{
			printf(" HTTP--%d --start-pt-- %ld\n",  i, (*(dnld_Thread_data + i)).start_pt);

			printf(" HTTP--%d --end-pt-- %ld\n",  i, (*(dnld_Thread_data + i)).end_pt);
			
		
 		     	if((pthread_create(thread_id +i, NULL, httpdownloadThread, (void *)(dnld_Thread_data + i)))!=0)
 		     	{
				 		     	
 		     	        fprintf(stderr, "prodname: HTTP -- Thread Create Error\n");
 		     	        exit(1);
 		     	}
		}
		
		
	
	     	for(i=0; i< no_ofThread ; i++)
		{
	        	if((pthread_join(thread_id[i], (void **)&join_Thread_return[i] ))!=0)
	        	{
       	     			   fprintf(stderr, "prodname: HTTP -- join error: \n");
			           exit (1);
	        	}
		        //pthread_join(helper1, NULL);
		}
	

	     	for(i=0; i< no_ofThread ; i++)
		{				

                        flag_join_return = (join_Thread_return[i] == NULL) ? 1 : 0 ;
			
		}		
		
		if(flag_join_return)
		{			
			printf(" HTTP *********  The Download is Completed Successfully  ************\n");		
		}
		else
		{
			printf(" HTTP *********  The Download is not  Completed ************\n");		
		
		}
		
	    }
	    else
	    {
	      printf(" HTTP -- Error occured\n");
	      exit(1);
	
	    }
	

      }
      else if((!strcmp(dnldURL.protocol,"ftp:")) || (!strcmp(dnldURL.protocol,"FTP:")))
      {
      	
            printf("ftpdnldmngr \n");
	    ThreadNo_or_ErrCode = ftpdnldmngr ( sockfd , no_ofThread , dnldURL , &dnld_Thread_data , hosturl );
	    if(ThreadNo_or_ErrCode !=ERR_ftp_mod)
	    {
	    	//Code : Implementing Threads
		    	
	    	if(ThreadNo_or_ErrCode == No_T_ftpdload_complete)
	    	{
		    	printf(" *** Download is completed  With single Segment*** \n");
		    	printf("--since Segmentation is not supported by the Server \n");
	    	
	    	}
	    	else
	    	{
		    	int flag_join_return=0;
		    	no_ofThread = ThreadNo_or_ErrCode ;
	    	
	    	
	    		join_Thread_return = (void *)malloc(no_ofThread * sizeof(int));
	    			
		        printf("--- FTP Calling Threads In Main  ----\n");
	
			for(i=0; i< no_ofThread ; i++)
			{
				printf("FTP--%d --start-pt-- %ld\n",  i, (*(dnld_Thread_data + i)).start_pt);

				printf("FTP--%d --end-pt-- %ld\n",  i, (*(dnld_Thread_data + i)).end_pt);
			
		
 			     	if((pthread_create(thread_id +i, NULL, ftpdownloadThread, (void *)(dnld_Thread_data + i)))!=0)
 			     	{
				 		     	
 		     		        fprintf(stderr, "prodname: FTP Thread Create Error\n");
 		     	        	exit(1);
	 		     	}
			}
		
		
	
	     		for(i=0; i< no_ofThread ; i++)
			{	
		        	if((pthread_join(thread_id[i], (void **)&join_Thread_return[i] ))!=0)
	        		{
       	     				   fprintf(stderr, "prodname:FTP join error: \n");
			        	   exit (1);
		        	}
			        //pthread_join(helper1, NULL);
			}
	

		     	for(i=0; i< no_ofThread ; i++)
			{				
				printf("return flag_join_return %d \n", join_Thread_return[i]);

                	        flag_join_return = (join_Thread_return[i] == NULL) ? 1 : 0 ;
			
			}		
		
			if(flag_join_return)
			{			
				printf(" ********* FTP --  The Download is Completed Successfully  ************\n");		
			}
			else
			{
				printf(" ********* FTP -- The Download is not  Completed ************\n");		
		
			}
	    	
	    	} // No_T_ftpdload_complete	
		
	    }
	    else
	    {
	      printf("FTP -- Error occured\n");
	      exit(1);
	
	    }

      }//  if ftp

	
   }//split
   else
   {
         exit(1);
   }
   exit(0);
}    /*end of main*/



int splitURL(char *url, int PortNumber, URL *surl)
{
   int i=0;
   char * temptoken;
   int No_of_Tokens;
   char **Tokens;

   boolean Protocol_Specified, Port_Specified, FileName_Specified;

   No_of_Tokens=0;

   /*copy the hostname string to a temporary variable as strtok alter the pointer*/
	printf("\nLength %d",strlen(url));
   temptoken=(char *) malloc((strlen(url) + 1) * sizeof(char));
   strcpy(temptoken,url);

   Tokens=(char **)malloc(sizeof(char *));
   /*This is To find whether a protocol is specified or not.
   If // occurs in the hostname and hostname doesn't start with it then protocol is specified*/
   if(strstr(temptoken,"//") && (strstr(temptoken,"//")!=temptoken))
   {
      Protocol_Specified=true;
   }
   else
   {
      Protocol_Specified=false;
   }

   if(strstr(temptoken,"//")==(temptoken + strlen(temptoken) -2 ))
   {
      fprintf(stderr,"prodname: Invalid URL\n");
      return ERR_IURL;
   }

   if(rindex(temptoken, '/')==NULL)
   {
      fprintf(stderr,"prodname: Invalid URL\n");
      return ERR_IURL;
   }

   /*  Reading Protocol name */
   if((*Tokens=strtok(temptoken,"//")))
   {
         No_of_Tokens++;
   }


   Tokens=(char **)realloc(Tokens, (No_of_Tokens + 1) * sizeof(char *));

   while((*(Tokens + No_of_Tokens)=strtok(NULL,"/")))
   {
      No_of_Tokens++;
      Tokens=(char **)realloc(Tokens, (No_of_Tokens + 1) * sizeof(char *));
   }

	printf("\nNo. of Tokens %d",No_of_Tokens);
   if (Protocol_Specified && No_of_Tokens < 2)
   {
      fprintf(stderr,"prodname: Invalid URL");
      return ERR_IURL;
   }
   if (!Protocol_Specified && No_of_Tokens < 1)
   {
      fprintf(stderr,"prodname: Invalid URL");
      return ERR_IURL;
   }
   if (Protocol_Specified && No_of_Tokens < 3)
   {
      fprintf(stderr,"prodname: You Must Specify A FileName within the directory to download\n");
      return ERR_FN;
   }
   if (!Protocol_Specified && No_of_Tokens < 2)
   {
      fprintf(stderr,"prodname: You Must Specify A FileName within the directory to download\n");
      return ERR_FN;
   }
   if(rindex(url, '/')!=(url + strlen(url) - 1))
      FileName_Specified=true;
   else
   {
      fprintf(stderr,"prodname: You Must Specify a FileName Within the Directory to Download\n");
      return ERR_FN;
   }

   /*If Protocol is specified check whether it is http or ftp. if not throw error.
     If protocol is not specified assume ftp.
     If Port Number is not given then assume 80 for http and 21 for ftp.
   */
   if (Protocol_Specified)
   {
    /* ratan--> *tokens  base addr of array of string tokens */
      if(strcmp(*Tokens,"http:")==0)
      {
         surl->protocol=(char *) malloc(6 * sizeof(char));
         strcpy(surl->protocol,"http:");
         if(PortNumber==0)
         {
            Port_Specified=false;
            PortNumber=80;
         }
         else
         {
            Port_Specified=true;
         }
      }
      else if(strcmp(*Tokens,"ftp:")==0)
      {
         surl->protocol=(char *) malloc(5 * sizeof(char));
         strcpy(surl->protocol,"ftp:");
         if(PortNumber==0)
         {
            Port_Specified=false;
            PortNumber=21;
         }
         else
         {
            Port_Specified=true;
         }
      }
      else
      {
         fprintf(stderr, "prodname: Invalid Protocol\n");
         return ERR_IPROTO;
      }
      i++;
   }
   else
   {
      printf("No protocol is specified by you. So assuming ftp\n");
      if(PortNumber==0)
      {
         Port_Specified=false;
         PortNumber=21;
      }
      else
      {
         Port_Specified=true;
      }
   }

   if(Port_Specified)
   {

   }
   else
   {
      printf("No Port Number is Specified By You. Assuming Default Port %d\n",PortNumber);
   }
   surl->portnumber=PortNumber;
   /*  ratan--> token+1  pointer to hostname ..after protocol */

   surl->hostname=(char *)malloc((strlen(*(Tokens + i)) + 1) * sizeof(char));
   strcpy(surl->hostname, *(Tokens + i));

   i++;

   //surl->directory=(char *)malloc(strlen(*(Tokens + i)) * sizeof(char));
   surl->directory=(char *)malloc(sizeof(char));

   strcpy(surl->directory,"");
   while(i<No_of_Tokens-1)
   {
      surl->directory=(char *)realloc(surl->directory,( strlen(surl->directory) + strlen(*(Tokens + i))+2) * sizeof(char));
      strcat(surl->directory, "/");
      strcat(surl->directory, *(Tokens + i));

      i++;

   }
   surl->directory = (char *)realloc(surl->directory,((strlen(surl->directory))+2)*sizeof(char) );
   strcat(surl->directory,"/");

   printf("AFTER HOST\n");


   surl->filename=(char *)malloc((strlen(*(Tokens + i)) + 1) * sizeof(char));
   strcpy(surl->filename, *(Tokens + i));


   free(temptoken);
   free(Tokens);
   return NO_ERR;

}

