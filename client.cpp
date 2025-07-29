#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <iostream>
using namespace std;

static void die(const char *msg){
    int err =errno;
    fprintf(stderr,"[%d]" "%s\n",err,msg );
    abort();
}

int main(){
    int fd= socket(AF_INET,SOCK_STREAM,0); // ipV4 ,TCP,0 fill
// fd stands for file descriptor (an integer).
// If successful, it will hold a number representing the socket.
// If it fails, it returns -1.
    if(fd<0){
        die("socket()");
    }
    struct sockaddr_in addr={};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int rv = connect (fd,(const struct sockaddr *)& addr ,sizeof(addr));
    if(rv){
        die("connect");
    } 
cout<<"enter the  message:";
char msg[64];
cin>>msg;
write(fd,msg,strlen(msg));

char rbuf[64]={};

ssize_t n= read(fd,rbuf,sizeof(rbuf)-1);
if(n<0){
    die("read");
}
printf("server says:%s\n",rbuf);// READ BUFFER
close(fd);


}