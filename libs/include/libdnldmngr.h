#ifndef _LIB_DNLDMNGR_H
#define _LIB_DNLDMNGR_H 1
#define MAXSIZE 1024
#define END_CHAR '\n'
#define END_CHARS "\r\n\0"


   //int ftpdownload(char *, char *, int, char *, long, long);
   void *httpdownloadThread(void *);
   void *ftpdownloadThread(void *);
   //void *ftpdownloadThread(void *);
   int httpdnldmngr ( int , int, URL ,  thread_data ** , char *);
   int ftpdnldmngr ( int , int, URL ,  thread_data ** , char *);

   int writen(int, char *, int) ;
   int readline(int , char *);
   int StatCheck(char *);
   char ftpuser[30];
   char ftppass[15];
#endif
