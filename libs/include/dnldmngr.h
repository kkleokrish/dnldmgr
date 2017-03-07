#ifndef _DNLDMNGR_H
#define _DNLDMNGR_H 1
#define true 1
#define false 0

#define ENDCHARS "\r\n"
#define NO_ERR 0
#define ERR_IURL 1
#define ERR_FN 2
#define ERR_IPROTO 3
#define ERR_status 4
#define ERR_Thread 5
#define ERR_http_mod 999
#define ERR_ftp_mod  555
#define No_T_ftpdload_complete 1000
#define ERR_http_thread  ((void *) 8)
#define ERR_ftp_thread   ((void *) 9)


typedef unsigned char boolean;
pthread_t helper1, helper2;

pthread_t *thread_id;
int no_ofThread ;
int *join_Thread_return ;

struct tagURL
{
   char *protocol;
   char *hostname;
   int portnumber;
   char *directory;
   char *filename;
};
typedef struct tagURL URL;

struct struct_thread
{
   URL url;
   char * HostUrl;
   //char * dest;
   //unsigned long headlen;
   //unsigned long contentlen;
   unsigned long start_pt;
   unsigned long end_pt;

};
typedef struct struct_thread thread_data ;

struct tagdownloaded
{
   long startpt;
   long endpt;

};
typedef struct tagdownloaded downloaded ;

/*struct tagstat_downloaded
{
   int no_of_thread ;
   char *url ;
   downloaded **data ;

};
*/

struct tagstat_downloaded
{
   int no_of_thread ;
   char url[70] ;
   downloaded data[5];

};

typedef struct tagstat_downloaded stat_downloaded ;

int splitURL(char *, int, URL *);
#endif

