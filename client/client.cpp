#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#define SERVER_PORT 5432
#define MAX_LINE 256

int main(int argc, char *argv[]){

    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buff[MAX_LINE];
    int sock;
    int len;

    if(argc==2){
        host = argv[1];
    }
    else{
        std::cerr << "usage: simplex-talk host" << std::endl; 
        std::exit(1);
    }

    hp = gethostbyname(host);
    if(!hp){
        std::cerr << "simplex-talk: unknow host " << host << std::endl;
        std::exit(1);
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        std::perror("simplex-talk: socket");
        std::exit(1);
    }

    if ( connect(sock, (struct sockaddr *) &sin, sizeof(sin) ) < 0 ) {
                std::perror("simplex-talk: connect"); 
                std::exit(1);
        }


    std::cout << "**Socket created" << std::endl;
    std::cout << sizeof(buff);
    while(fgets(buff, sizeof(buff), stdin)){
        std::cout << sizeof(buff);
        buff[MAX_LINE-1] = '\0';
        len = sizeof(buff) + 1;
        send(sock, buff, len, 0);
    }

    return 0;
}