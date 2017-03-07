/* Main function for FTP download  */

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

int ftpdnldmngr ( int ftp_sockfd , int ftp_no_ofThread , URL ftp_dnldURL ,  thread_data **ftp_dnld_Thread_data , char * ftp_hosturl )
 {
	//char *ptr_Response;
   	//char *ptr_filelen;
   	//char *tempresp , *temp;
	//char **token_filelen;
	//char *ptr_status;
	//char **token_status;
	int n;
	//long filelen ;
	
	char sendline[MAXSIZE];
	//recvline[255] ;
	char *Recvline;

	FILE * f_cont;
	char * s;
	char * file;
	
	
	// FTP variables
	char c =' ';
	char * str_tok;
	char *portpm1;
   	char *portpm2;
   	int sockfd ;
   	struct sockaddr_in serv_addr;

   	char bk=' ';

   	char *filesize ,*code_Retn, *fileline ;
   	unsigned long file_len , serv_port;
   	int count;

	
	thread_data *dnld_Thread1_data;
	//thread_data *dnld_Thread2_data;
	
	FILE * f_stat;
  	//char  Fstatus[20] = "Status";
	stat_downloaded * dnld_status;
	int size_stat_downloaded = sizeof(stat_downloaded);
	
       //if((!strcmp(ftp_dnldURL.protocol,"ftp:")) || (!strcmp(ftp_dnldURL.protocol,"FTP:")))
       //{

	      	printf("Enter The User Name:\n");
	      	scanf("%s", ftpuser);
	      	printf("Enter The PassWord Name:\n");
	      	scanf("%s", ftppass);

       //}
	
       while(1)
       {
		if((n=read(ftp_sockfd,&c,1)) >=0)
		{
			printf("%c",c);
		 	// strcpy(bk,c);
		if(c=='\n' && bk=='\r')
			break;
		 	bk=c;
		      	          	         	
	   	}
	   	else
	   	{
	    		fprintf(stderr, "prodname: Error in Reading \n");
	      		return ERR_ftp_mod;	
	      	
	    	}
       }	

	


        if(!strcmp(ftpuser,""))
        	strcpy(ftpuser,"anonymous");
              	
        if(!strcmp(ftppass,""))
              	strcpy(ftppass,"anonymous@");

	strcpy(sendline,"USER ");
        strcat(sendline,ftpuser);	             	      	


	n=strlen(sendline);
	if(writen(ftp_sockfd, sendline, n )!=n)
	{
		fprintf(stderr, "prodname: Error in writing Socket\n" );
	      	return ERR_ftp_mod;	
	}

	
	while(1)
	{
      	
		if((n=read(ftp_sockfd,&c,1)) >=0)
	        {
	 		printf("%c",c);
      		        // strcpy(bk,c);
	      	        if(c=='\n' && bk=='\r')
	   		   break;
	      	        bk=c;
	      	          	         	
	   	}
	    	else
	        {      	
			fprintf(stderr, "prodname: Error in Reading from Socket\n" );
		      	return ERR_ftp_mod;	

      	  		
	        }
      	
	}

	strcpy(sendline,"PASS ");
	strcat(sendline,ftppass);
	

	n=strlen(sendline);
	if(writen(ftp_sockfd, sendline, n )!=n)
	{
		fprintf(stderr, "prodname: Error in writing Socket\n" );
		return ERR_ftp_mod;
	
	}

	while(1)
	{
      	
		if((n=read(ftp_sockfd,&c,1)) >=0)
		{
			printf("%c",c);
	  		// strcpy(bk,c);
		   	if(c=='\n' && bk=='\r')
		    		break;
		      	bk=c;
	      	          	         	
		}
		else
		{      	
			fprintf(stderr, "prodname: Error in writing Socket\n" );
		      	return ERR_ftp_mod;	
		      		 		
	 	}
	      	
	}
	
      	// Changing The Directory if Required
	if(strcmp(ftp_dnldURL.directory,"/"))
	{
	      		
	             	
		strcpy(sendline,"CWD ");
		strcat(sendline,(ftp_dnldURL.directory+1));
		n=strlen(sendline);
		if(writen(ftp_sockfd, sendline, n )!=n)
		{
	 		fprintf(stderr, "prodname: Error in writing Socket\n" );
		  	return ERR_ftp_mod;	
		}

		while(1)
		{
      	
			if((n=read(ftp_sockfd,&c,1)) >=0){
	  			printf("%c",c);
	     			// strcpy(bk,c);
			      	if(c=='\n' && bk=='\r')
			      		break;
			      	bk=c;
	      	          	         	
			}
			else
			{      	
				fprintf(stderr, "prodname: Error in writing Socket\n" );
	      			return ERR_ftp_mod;	

			}
      	
		}
		
	}// Directory
	

	strcpy(sendline,"SIZE ");
	strcat(sendline,ftp_dnldURL.filename);

	n=strlen(sendline);
	if(writen(ftp_sockfd, sendline, n )!=n)
	{
		fprintf(stderr, "prodname: Error in writing Socket\n" );
		return ERR_ftp_mod;

	}
	
	


	count=0;
	filesize = (char *) malloc(2 * sizeof(char));
	strcpy(filesize,"");

	while(1)
	{
		if((n=read(ftp_sockfd,&c,1)) >=0)
		{
	 		// printf("%c",c);
		  	*(filesize + count) = c;
		      	*(filesize + count + 1) = '\0' ;
	      		// strcpy(bk,c);
      		
		   	if(c=='\n' && bk=='\r')
		      	      break;
		    	bk=c;
		   	{
			      unsigned long len = strlen(filesize);
			      filesize = (char *)realloc(filesize, (len + 2) * sizeof(char) ); 	      	
	
			}   	
		   	count++;
	      	          	         	
      		}
      		else
      		{
      	
			fprintf(stderr, "prodname: Error in writing Socket\n" );
		      	return ERR_ftp_mod;	

	      	}
	}
	printf("%s\n", filesize);
	
	if((str_tok = strtok(filesize," "))!=NULL)
	{
	
		str_tok = strtok(NULL," ");
		file_len = atoi(str_tok);
  		printf("file-len-%ld\n", file_len);

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
		        	printf("\n URL----> %s --2 %s\n", dnld_status->url, ftp_hosturl);
	        	
				if(!strcmp(dnld_status->url, ftp_hosturl))
				{			
	   				printf("\nhostname found\n ");				
					dnld_Thread1_data = (thread_data *)malloc (sizeof(thread_data)); 	
		   			dnld_Thread1_data->url = ftp_dnldURL ;
	       				dnld_Thread1_data->HostUrl = (char *)malloc (strlen(ftp_hosturl) + 1);
	       	   			strcpy(dnld_Thread1_data->HostUrl, ftp_hosturl);
			            	no_str = 0;
									
					ftp_no_ofThread = dnld_status->no_of_thread ;				
					for(i=0; i<ftp_no_ofThread ;  i++)
					{
						no_str ++;
					 	//dnld_Thread1_data->start_pt = *(downloaded *)( ((stat_downloaded * )dnld_status)->data + i)-> startpt;
			    			//dnld_Thread1_data->end_pt = *(downloaded *)( ((stat_downloaded * )dnld_status)->data + i)-> endpt;
		     	      			
						dnld_Thread1_data->start_pt = dnld_status->data[i].startpt;
		    				dnld_Thread1_data->end_pt = dnld_status->data[i].endpt;
	  		     			//printf("start Pt--%d-- %ld\n",i, dnld_Thread1_data->start_pt);
	  	      				//printf("end Pt--%d-- %ld\n",i, dnld_Thread1_data->end_pt);	     	      			
	  	      			
	  	      			
	  	      				*ftp_dnld_Thread_data = (thread_data * )realloc( *ftp_dnld_Thread_data , no_str * sizeof(thread_data ) );
				      		*(*(ftp_dnld_Thread_data) + no_str - 1) = *(dnld_Thread1_data);
		      			
				
						printf("\n Came to for ---- %ld \n",(*(*(ftp_dnld_Thread_data) + no_str - 1)).start_pt);
						printf(" Came to for -- %ld \n", (*(*(ftp_dnld_Thread_data) + (no_str - 1))).end_pt);
						printf(" Came to for -- %s \n", (*(*(ftp_dnld_Thread_data) + (no_str - 1))).HostUrl);
	
		      			
           														
					}
					thread_id  = (pthread_t *)malloc (sizeof(pthread_t) * ftp_no_ofThread);
					fclose(f_stat);
					printf("before initialising thread data");			
			  		return ftp_no_ofThread ;

					
					//goto WRITE;		
			
				 } // inner if
			
			 } //while
	
		         //  Writing new Eecords
		         printf("Not Matched / No Records \n");
		
		         // Alocating Memory for File
			 s=(char *)calloc(file_len, sizeof (char));		
			 printf("In file alloc\n");
			 {
			 	//int lendir = strlen(dnld_Thread1_data->dest);
				int lendir = strlen(ftp_dnldURL.filename);
				file = (char *)malloc((lendir + 1) * sizeof(char));
				//strcpy(file,dnld_Thread1_data->dest);
				strcpy(file, ftp_dnldURL.filename);
			 }

			 if((f_cont=fopen(file,"w+"))!=NULL)
			 {			 			
			 	fwrite(s, sizeof(char), file_len, f_cont);
				free(s);
				printf("File IF allocated\n");
			
			 }
			 else
			 {			
			 	fprintf(stderr,"prodname: Error Opening File for writing \n");	
			       	return ERR_ftp_mod ; 			
			 }

			 rewind(f_cont);
			 fclose(f_cont);		

			 		
		         // if multithreading is not supported
		         		
			 strcpy(sendline,"REST 0");

			 n=strlen(sendline);
	      		 if(writen(ftp_sockfd, sendline, n )!=n)
			 {
		 		fprintf(stderr, "prodname: Error in writing Socket\n" );
	      		  	return ERR_ftp_mod;
	      		 }

			 count=0;
	      		 code_Retn = (char *) malloc(2 * sizeof(char));
	      		 strcpy(code_Retn,"");

	      		 while(1)
	   		 {
	      	   		if((n=read(ftp_sockfd,&c,1)) >=0)
			    	{
	      	   			// printf("%c",c);
			      	   	*(code_Retn + count) = c;
	      			   	*(code_Retn + count + 1) = '\0' ;
		      		   	// strcpy(bk,c);
      		
	      			    	if(c=='\n' && bk=='\r')
				      	      	break;
	      	    			bk=c;
				   	{
				      		unsigned long len = strlen(code_Retn);
	 		        		code_Retn = (char *)realloc(code_Retn, (len + 2) * sizeof(char) ); 	      	
	
					}   	
				   	count++;
	      			          	         	
	      		   	}
      			   	else
      		   		{
      	
		 		fprintf(stderr, "prodname: Error in Reading from Socket\n" );
	      		  	return ERR_ftp_mod;
	      		  	
			      	}
			 }
			 printf("%s\n", code_Retn);
	
		  	 if((str_tok = strtok(code_Retn," "))!=NULL)
		    	 {
			    	printf("Code--%s\n", str_tok);
			 }

	            	 /* IF Resume is not supported by the server ,No Multithreading is used */
	
		    	 if(strcmp(str_tok,"350"))
		    	 {	
	       			strcpy(sendline,"PASV");

			        n=strlen(sendline);
			     	if(writen(ftp_sockfd, sendline, n )!=n)
			      	{
		      	  		fprintf(stderr, "prodname: Error in writing Socket\n" );				
		      	  		return ERR_ftp_mod;
		
			   	}
			    	// printf("\nPASSword send %s\n", sendline);
		
			    	Recvline = (char *) malloc(2 * sizeof(char));
		      	   	strcpy(Recvline,"");
		      	   	count=0;
		      	   	while(1)
			   	{
		     			if((n=read(ftp_sockfd,&c,1)) >=0)
		     			{
				      		printf("%c",c);
					      	*(Recvline + count) = c;
				      	   	*(Recvline + count + 1) = '\0' ;
				      		// strcpy(bk,c);
		      		
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
		      	
				 		fprintf(stderr, "prodname: Error in writing Socket\n" );
	      				  	return ERR_ftp_mod;
				     	}
		
			     	}
		
			    	printf("%s\n", Recvline);
				
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
		
		

			     	strcpy(sendline,"RETR ");
			      	strcat(sendline,ftp_dnldURL.filename);


		      	   	n=strlen(sendline);
			      	if(writen(ftp_sockfd, sendline, n )!=n)
		      	   	{
			      		fprintf(stderr, "prodname: Error in writing Socket\n" );
				      	return ERR_ftp_mod;
			
		    	   	}

		      	   	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
			      	{
		      	   		fprintf(stderr,"prodname: Can't open Stream Socket for PASV Connection \n");
				      	return ERR_ftp_mod;
			      	}

			      	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) )<0)
		      	   	{      	
		      	   		fprintf(stderr, "prodname: can't make PASV connection to Server \n");
				      	return ERR_ftp_mod;
		      	    	}

			        printf("connected_PASV\n");
			
			     	// printf("\nPASSword send %s\n", sendline);

		          	fileline = (char *) malloc(2 * sizeof(char));
		          	strcpy(fileline,"");

       		     	  	f_cont = fopen(file, "r+");
		     	  	fseek(f_cont,0,SEEK_SET);


		     	  	for(count=0;count<=file_len;count++)
	                  	{
      	
		      			if((n=read(sockfd,&c,1)) >=0)
		      			{
				      		// printf("%c",c);
				      	   	*(fileline + count) = c;
				      	   	*(fileline + count + 1) = '\0' ;
	      		
					   	{
					      		unsigned long len = strlen(fileline);
					      		fileline = (char *)realloc(fileline, (len + 2) * sizeof(char) ); 	      	
			
				   		}   	
	
	      	          	         	
				      	}	
				      	else
				      	{
				     		fprintf(stderr, "prodname: Error in writing Socket\n" );
					      	return ERR_ftp_mod;
	
				      	}
			      	
	      	
	
				}

			        printf("%s", fileline);	
		                return No_T_ftpdload_complete; // Download completed
	
	      		  }
	      		  else
	      		  {
		      	  	printf("REST Supported by the Server\n");
		
		
			      	// if multithreading is not supported
                          	 			         		
			      	no_str = 0;
			        dnld_status = (stat_downloaded *)malloc( size_stat_downloaded );
                         	                 	
			        dnld_status->no_of_thread = ftp_no_ofThread  ;	
		         	End_pt = file_len/ftp_no_ofThread ;
		         	End_remainder = file_len % ftp_no_ofThread  ;

		         	//dnld_status->url = (char *)malloc((strlen(HostName) + 1) * sizeof(char));		
			 	strcpy(dnld_status->url, ftp_hosturl);
			 	printf("Http_hosturl--3-- %s---%s\n",dnld_status->url, ftp_hosturl);
		
	  		 	dnld_Thread1_data = (thread_data *)malloc (sizeof(thread_data));
		         	dnld_Thread1_data->url = ftp_dnldURL ;
		       	        	
		       	 	dnld_Thread1_data->HostUrl = (char *)malloc (strlen(ftp_hosturl)+1 );
		       	 	strcpy(dnld_Thread1_data->HostUrl , ftp_hosturl);
		   		
			 	for( i=0;i<ftp_no_ofThread ; i++)
				{		   		
					 no_str++;
			        	 printf("i %d\n",i);

					 dnld_status->data[i].startpt = i * End_pt + i ;
					 if( i == (ftp_no_ofThread -1))
				         	dnld_status->data[i].endpt =file_len;
			  		 else
		          	 		dnld_status->data[i].endpt =  dnld_status->data[i].startpt + End_pt;		
	
				         printf("it's in else -ST  - %ld ----- \n",dnld_status->data[i].startpt);
		                 	 printf("it's in else EN - %ld ----- \n",dnld_status->data[i].endpt );	
	    	      		
			    	   	 dnld_Thread1_data->start_pt = dnld_status->data[i].startpt ;
			     	   	 dnld_Thread1_data->end_pt = dnld_status->data[i].endpt ;
			     	
  	   			   	 *ftp_dnld_Thread_data = (thread_data * )realloc( *ftp_dnld_Thread_data , no_str * sizeof(thread_data ) );
			 		 *(*(ftp_dnld_Thread_data) + no_str - 1) = *(dnld_Thread1_data);
			 	
	             		         printf(" Came not found ---- %ld \n",(*(*(ftp_dnld_Thread_data) + no_str - 1)).start_pt);
					 printf(" Came  not found -- %ld \n", (*(*(ftp_dnld_Thread_data) + (no_str - 1))).end_pt);
					 printf(" Came  not found -- %s \n", (*(*(ftp_dnld_Thread_data) + (no_str - 1))).HostUrl);
		
		 		}
	
			  	thread_id  = (pthread_t *)malloc (sizeof(pthread_t) * ftp_no_ofThread);
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
				  	return ERR_ftp_mod ;
				}			
			
			  //fclose(f_stat); 				
			
			  } //end of  IF REST
			  //goto WRITE;
			  printf("before initialising thread data");			
			  return ftp_no_ofThread ;
			
			  	
 		}//for

 }
 else
 {
 	fprintf(stderr,"prodname: Error Opening Fstatus File\n");
 	return ERR_ftp_mod;
	
 }	
  	
 //WRITE:

//  printf("before initialising thread data");

 // return NO_ERR;
// return ftp_no_ofThread ;

	
	
	
	
	
 }  // ftpdnldmngr
                                                            	
