/* ALL General Funtions Definitions */

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

int writen(int fd, char *ptr, int nbytes)
{
   char c;
   int nleft, nwritten;
   nleft = nbytes;
   while(nleft > 0)
   {
      nwritten = write(fd, ptr, nleft);
      if(nwritten <= 0)
         return(nwritten);
      nleft -= nwritten;
      ptr += nwritten;
   }

   c=END_CHAR;
   write(fd, &c, 1);

   return(nbytes-nleft);
}


int readline(int fd, char *ptr)
{
   int n, rc;
   char prevchar;
   char c[2];

   c[1]='\0';
   prevchar='\0';

   strcpy(ptr,"");
   for (n=0; n<255; n++)
   {
      if((rc=read(fd, c, 1))==1)  //-- read system call -- read 1 byte/read
      {
         strcat(ptr,c);

        // printf("\t%d--%c||",*c,*c);	
        // sfprintf(stderr,"%c",*c);
         if(prevchar=='\r' && c[0]=='\n')
         {

            break;
         }
         prevchar=c[0];
      }
      else if(rc == 0)
      {
	if(n==0)
	{
	
	   return(0); // EOF no data read
	}
	else
	{
	
	   break; // some data was read				
	}
     }
     else if(rc<0)
     {

            return(-1);
     }
   }
   return(n);
}
