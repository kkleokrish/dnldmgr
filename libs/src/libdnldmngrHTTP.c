/* Main function for HTTP download  */

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

int httpdnldmngr ( int http_sockfd , int http_no_ofThread , URL http_dnldURL ,  thread_data **http_dnld_Thread_data , char * Http_hosturl )
 {
	char *ptr_Response;
   	char *ptr_filelen;
   	char *tempresp , *temp;
	char **token_filelen;
	char *ptr_status;
	char **token_status;
	int n;
	long filelen ;
	
	char sendline[MAXSIZE], recvline[255] ;
	// *Recvline;

	FILE * f_cont;
	char * s;
	char * file;
	
	thread_data *dnld_Thread1_data;
	//thread_data *dnld_Thread2_data;
	
	FILE * f_stat;
  	//char  Fstatus[20] = "Status";
	stat_downloaded * dnld_status;
	int size_stat_downloaded = sizeof(stat_downloaded);
	
	//thread_data *http_dnld_Thread_data = NULL;	

	
	strcpy(sendline,"HEAD ");
	strcat(sendline,http_dnldURL.directory );
	strcat(sendline,http_dnldURL.filename);
	strcat(sendline,"  HTTP/1.1\nHost:");
	strcat(sendline,http_dnldURL.hostname);
	strcat(sendline,"\r\n");
	printf("request sent---> %s\n***\n",sendline);
	n=strlen(sendline);
	
	if(writen(http_sockfd, sendline, n )!=n)
	{
	   	fprintf(stderr, "prodname: Error in writing Socket\n" );
   		return ERR_http_mod ;

	}
	
	tempresp=(char *)malloc(2 * sizeof(char));
	strcpy(tempresp,"");
	

	
	do
	{
   		
		
   		n = readline(http_sockfd, recvline);
 		

   		if(n < 0)
		{       	
	      	       fprintf(stderr,"prodname: Error Reading Incoming Data \n");
		       return ERR_http_mod ;
		
   		}
   	
	   	{
   		 	int lenrl, lentr;
		  	lenrl= strlen(recvline);
   	  		lentr= strlen(tempresp);
   		  	tempresp=(char *)realloc(tempresp, (lenrl + lentr + 1) * sizeof(char));
	   	}
   	
		strcat(tempresp, recvline);
		if(!strcmp(recvline,"\r\n"))
	        break;
		
	}while(n);
	
	temp = (char *) malloc((strlen(tempresp)+1) * sizeof(char));
	strcpy(temp, tempresp);
	
	if((ptr_Response = strtok(temp, " "))!=NULL)
	{   	      	
   	 	char c , prev ;
   	 	int count;	
   	 	count=0;
   	 	prev=' ' ;
   	 	ptr_Response = strtok(NULL , " ");
   	 	
   	 	if(strcmp(ptr_Response,"200"))
	        {
         		printf("Prodname :: ERROR IN REQUEST ::-->\n");
	         	for(c = ' '; ; )
        	 	{
         		            	
         		     printf("%c",c);         	
	         	     if((c=='\r'))
	            	     printf("\r\n");
		
	         	     c = *temp++;
        	 	     if((prev=='\r') && (c=='\n'))
         		     	count++;
         	     	
	         	     if(count==5)	
        	 	
        	 	       return ERR_http_mod;
         		     prev = c;            	           	
	         	}
         	
        	}
        	else
        	{
	         	 printf("DOWNLOADING  File Length\n");	
			 printf("%s\n",tempresp);	
	        } 	

	}


	
	if((ptr_filelen = strstr(tempresp,"Content-Length"))!=NULL)
	{	
		
		  printf("len of the file : %s", ptr_filelen );	

	}

	
	token_filelen=(char **)malloc(sizeof(char*));

	if((*token_filelen = strtok(ptr_filelen,":"))!=NULL)
	{
		*token_filelen = strtok(NULL,":");
  		printf("filelen ---%s \n",*token_filelen);      		

	}
	
	

	
	filelen=atoi(*token_filelen);
	free(token_filelen);
 	
 	s=(char *)calloc(filelen, sizeof (char));

 	
	if((ptr_status=strstr(tempresp,"HTTP/1.1"))!=NULL)
	{   	
		// printf("Status  fisrt--%s\n", ptr_status);	
	}


        printf("filelen : %ld \n",filelen);
	token_status=(char **)malloc(sizeof(char*));
	
	
	if((*token_status = strtok(ptr_status," "))!=NULL)
	{

		*token_status = strtok(NULL," ");
	  	printf("status-- inside %s \n", *token_status);

	}
     	// Checking download status to Resume
     	 //   fflush(stdin);
         //   getchar();
	if((f_stat = fopen("Fstatus","ab+"))!=NULL)
	{  	     	
  		int i, no_str, End_pt , End_remainder;
		no_str = 0;
	 	rewind(f_stat);
    		printf("size %d\n",size_stat_downloaded);

	  	dnld_status = (stat_downloaded *)malloc(size_stat_downloaded );		
  	
  		for(;;)
	  	{
		     	while(fread(dnld_status, size_stat_downloaded, 1, f_stat )==1)
		        {
                            	printf("\n URL----> %s --2 %s\n", dnld_status->url, Http_hosturl);
	        	
				if(!strcmp(dnld_status->url, Http_hosturl))
				{
			
	   				printf("\nhostname found\n ");				
					dnld_Thread1_data = (thread_data *)malloc (sizeof(thread_data)); 	
		                    	dnld_Thread1_data->url = http_dnldURL ;
	        	            	dnld_Thread1_data->HostUrl = (char *)malloc (strlen(Http_hosturl) + 1);
       	   				strcpy(dnld_Thread1_data->HostUrl , Http_hosturl);
		                    	no_str = 0;
								
					http_no_ofThread = dnld_status->no_of_thread ;				
					for(i=0; i<http_no_ofThread ;  i++)
					{
				      	
					      	no_str ++;
				      		//dnld_Thread1_data->start_pt = *(downloaded *)( ((stat_downloaded * )dnld_status)->data + i)-> startpt;
		     	      			//dnld_Thread1_data->end_pt = *(downloaded *)( ((stat_downloaded * )dnld_status)->data + i)-> endpt;
	     	      			
					      	dnld_Thread1_data->start_pt = dnld_status->data[i].startpt;
	     		      			dnld_Thread1_data->end_pt = dnld_status->data[i].endpt;
  		      				//printf("start Pt--%d-- %ld\n",i, dnld_Thread1_data->start_pt);
  	      					//printf("end Pt--%d-- %ld\n",i, dnld_Thread1_data->end_pt);	     	      			
  	      			
  	      			
  	      					*http_dnld_Thread_data = (thread_data * )realloc( *http_dnld_Thread_data , no_str * sizeof(thread_data ) );
			      			*(*(http_dnld_Thread_data) + no_str - 1) = *(dnld_Thread1_data);
		      			
				
					        printf("\n Came to for ---- %ld \n",(*(*(http_dnld_Thread_data) + no_str - 1)).start_pt);
					        printf(" Came to for -- %ld \n", (*(*(http_dnld_Thread_data) + (no_str - 1))).end_pt);
						printf(" Came to for -- %s \n", (*(*(http_dnld_Thread_data) + (no_str - 1))).HostUrl);
	
		      			
           														
					}
					thread_id  = (pthread_t *)malloc (sizeof(pthread_t) * http_no_ofThread);
					fclose(f_stat);
					goto WRITE;
				
			
			
			  	} // inner if
				  //else
				  //{
				  //printf("HERE NOT FOUND  -- back to while\n");
				  //}
	
		         } //while
	
		         //  Writing new Eecords
		         printf("Not Matched / No Records \n");
                          	 			         		
		         no_str = 0;
	        	 dnld_status = (stat_downloaded *)malloc( size_stat_downloaded );
                         	                 	
		         dnld_status->no_of_thread = http_no_ofThread  ;	
		         End_pt = filelen/http_no_ofThread ;
		         End_remainder = filelen % http_no_ofThread  ;

		         //dnld_status->url = (char *)malloc((strlen(HostName) + 1) * sizeof(char));		
			 strcpy(dnld_status->url, Http_hosturl);
			 printf("Http_hosturl--3-- %s---%s\n",dnld_status->url, Http_hosturl);
		
	  		 dnld_Thread1_data = (thread_data *)malloc (sizeof(thread_data));
		         dnld_Thread1_data->url = http_dnldURL ;
		       	        	
		       	 dnld_Thread1_data->HostUrl = (char *)malloc (strlen(Http_hosturl)+1 );
		       	 strcpy(dnld_Thread1_data->HostUrl , Http_hosturl);

			      	
		   		
			 for( i=0;i<http_no_ofThread ; i++)
			 {		   		
				 no_str++;
		        	 printf("i %d\n",i);

				 dnld_status->data[i].startpt = i * End_pt + i ;
				 if( i == (http_no_ofThread -1))
			         	dnld_status->data[i].endpt =filelen;						      		
			  	 else
		          	 	dnld_status->data[i].endpt =  dnld_status->data[i].startpt + End_pt;		
	
			         printf("it's in else -ST  - %ld ----- \n",dnld_status->data[i].startpt);
		                 printf("it's in else EN - %ld ----- \n",dnld_status->data[i].endpt );	
	    	      		
		    	   	 dnld_Thread1_data->start_pt = dnld_status->data[i].startpt ;
		     	   	 dnld_Thread1_data->end_pt = dnld_status->data[i].endpt ;
		     	
  	   		   	 *http_dnld_Thread_data = (thread_data * )realloc( *http_dnld_Thread_data , no_str * sizeof(thread_data ) );
			 	 *(*(http_dnld_Thread_data) + no_str - 1) = *(dnld_Thread1_data);
			 	
             		         printf(" Came not found ---- %ld \n",(*(*(http_dnld_Thread_data) + no_str - 1)).start_pt);
				 printf(" Came  not found -- %ld \n", (*(*(http_dnld_Thread_data) + (no_str - 1))).end_pt);
				 printf(" Came  not found -- %s \n", (*(*(http_dnld_Thread_data) + (no_str - 1))).HostUrl);
		
		 	 }
	
		  	 thread_id  = (pthread_t *)malloc (sizeof(pthread_t) * http_no_ofThread);
			 printf("going to write\n") ;
	
		  	 // dnld_status->data = dnld_pts_all;
		  	
		  	 fclose(f_stat);
		
		    	 if((f_stat = fopen("Fstatus","ab+"))!=NULL)
		         {
		               //fseek(f_stat,0,SEEK_END);
			       fwrite(dnld_status, size_stat_downloaded, 1, f_stat);
			       printf("written to file\n");
			       fclose(f_stat);
		     	       //break;		
	     	    	
			  }
			  else
			  {	
			       fprintf(stderr,"prodname: Error Opening Fstatus File for writing \n");	
			       return ERR_http_mod ;
			  }			
			
			  // Alocating Memory for File
			  printf("In file alloc\n");
			  {
				 //int lendir = strlen(dnld_Thread1_data->dest);
				 int lendir = strlen(http_dnldURL.filename);
				 file = (char *)malloc((lendir + 1) * sizeof(char));
				 //strcpy(file,dnld_Thread1_data->dest);
				 strcpy(file, http_dnldURL.filename);

			  }


			  if((f_cont=fopen(file,"w+"))!=NULL){
			
				  fwrite(s, sizeof(char), filelen, f_cont);
				  free(s);
				  printf("File IF allocated\n");
			
			  }
			  else
			  {
			
			       fprintf(stderr,"prodname: Error Opening File for writing \n");	
			       return ERR_http_mod ;
			
			
			  }

			  rewind(f_cont);
			  fclose(f_cont);
			  //fclose(f_stat); 				
			  goto WRITE;
			
			  	
 		}//for

 }
 else
 {
 	fprintf(stderr,"prodname: Error Opening Fstatus File\n");
 	return ERR_http_mod;
	
 }	
  	
 WRITE:

  printf("before initialising thread data");

 // return NO_ERR;
 return http_no_ofThread ;



 }

