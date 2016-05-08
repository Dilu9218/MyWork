#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

/*to run----> 
gcc webserver.c -o webserver
./webserver  */

#define MAX_BUF 2048

typedef struct {
 char *ext;
 char *filetype;
} exten;

//file types to be handled
exten extensions[] ={ {"gif", "image/gif" },{"jpg", "image/jpg" }, {"png", "image/png" }, {"ico", "image/ico" },
 {"htm", "text/html" }, {"html","text/html" }, {"php", "text/html" }, {"pdf","application/pdf"}, {0,0} };

void handleRequest(char request[50]){
 char *ptr;
// Check for a valid browser request
 ptr = strstr(request, " HTTP/");
 if (ptr == NULL) {
  printf("NOT HTTP !\n");
 } else {
  *ptr = 0;
  ptr = NULL;

  if (strncmp(request, "GET ", 4) == 0) {
   printf("valid GET");
   ptr = request + 4;
  }
  if (ptr == NULL) {
   printf("Unknown Request ! \n");
  }
 }
}
//---------------------------------------------------------------------------------------------------------------------------
void handleFiles(int newsockfd, char request[50]){
    char requestFilename[80];   /* request with filename to send */
    int fd;                    /* file descriptor for file to send */
    struct stat stat_buf;      /* argument to fstat */
    off_t offset = 0;          /* file offset */
    int rc;
    char path[MAX_BUF];
    //char header[100];    

    rc = recv(newsockfd, requestFilename, sizeof(requestFilename), 0);
    if (rc == -1) {
      fprintf(stderr, "recv failed: %s\n", strerror(errno));
      exit(1);
    }
    char *filename= strtok(requestFilename, " ");
    filename=strtok(NULL, " ");
    strsep(&filename,"/");
    printf("%s",filename);
    fprintf(stderr, "received request to send file %s\n", filename);
    char* s ;
    s= strchr(filename, '.');
    printf("content of s= %x ,%s\n", *s,filename);
    int i;
    for (i = 0; extensions[i].ext != NULL; i++) {
      if (strcmp(s + 1, extensions[i].ext) == 0) {
	//for php
	if(strcmp(extensions[i].ext, "php") == 0) {
         printf("\tExecuting PHP file.\n");
         //close(fd); 
	 FILE *filep;
	 int MAXSIZE = 0xFFF;
	 filep= popen("php -f /home/dilushi/4thyear1stSem/networks/webserver/browser.php", "r");
       	 int fno = fileno(filep);
	 if(!filep){
          fprintf(stderr, "Could not open pipe for output.\n");
          }
	 if(filep==NULL){
          printf("Could not open pipe for output.\n");
          }
	 char str[100000];
	 int nbyte= 100000;

         char *type = "text/html";
         char header[100];
         sprintf(header, "HTTP/1.1 200 OK\n Content-Type: %s; charset=utf-8  \n\n", type);
         write(newsockfd, header, strlen(header));
	 //send(newsockfd, header, strlen(header), 0);
	 
	 while (fgets (str, 99999, filep)!=NULL ){
		int bytes= write(newsockfd, str, strlen(str));
	 }
	 pclose(filep);
 
}
	else{
	char *type;
	type= extensions[i].filetype;
       	fd = open(filename, O_RDONLY); /*  open the file to be sent */
       	printf("Opening \"%s\"\n", filename);
       		if (fd == -1) {
			char *msg;	
			char header[100];
			sprintf(header, "HTTP/1.1 404 FILE NOT FOUND\n Content-Type: %s; charset=utf-8  \n\n", type);
			write(newsockfd, header, strlen(header));
			//msg= "<html><head><title>404 Not Found</head></title>";
			//int len = strlen(msg);
			//send(newsockfd, msg, len, 0);
			/*if (send(newsockfd, msg, len, 0) == -1) {
			  printf("Error in send\n");
			}*/
        		exit(1);
       		}	
	
    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);
    char header[100];
    sprintf(header, "HTTP/1.1 200 OK\n Content-Type: %s; charset=utf-8  \n\n", type);
    write(newsockfd, header, strlen(header));
    /* copy file using sendfile */
    offset = 0;
    rc = sendfile (newsockfd, fd, &offset, stat_buf.st_size);
    printf("file sending\n");
    if (rc == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      exit(1);
    }
    if (rc != stat_buf.st_size) {
      fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n", rc, (int)stat_buf.st_size);
      exit(1);
    }
    printf("file sent\n");
    /* close descriptor for file that was sent */
    close(fd);
 }
}
}
   }

//----------------------------------------------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen, pid;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* dilushi: sockfd is integer returned after socket()*/
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
/*dilushi: bzero fills struct with 0s*/
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
/*dilushi: socket and ip address binding,here it is localhost*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   } 
   printf("Server Connected. Waiting for client requests");

   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
  while (1) {
	   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
	   if (newsockfd < 0) {
	      perror("ERROR on accept");
	      exit(1);
	   }
   
   /* Connection is established, start communicating */

	 pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);	

/*fill buffer with 0s,reading client message from socket to buffer*/
	     bzero(buffer,256);
	     char str[50];
	     strcpy(str,buffer);
             //handleRequest(str);
	     handleFiles(newsockfd,str);
  
 	    exit(0);
         }
	/*if not child process, the parent process closes newsockfd*/
         else close(newsockfd);
   }
   return 0;
}

