#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256

int main(){

    struct sockaddr_in sin, clientAddr;
    char buff[MAX_LINE];
    int len;
    socklen_t clientAddrLen;
    int sock;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        std::perror("simples-talk: socket");
        std::exit(1);
    }

    std::cout << "**Socket created" << std::endl;

    if((bind(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
        std::perror("simplex-talk: bind");
        std::exit(1);
    }

    std::cout << "**Socket bound to local addres" << std::endl;

    listen(sock, MAX_PENDING);

    while(1){
        while(len = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *) &clientAddr, &clientAddrLen)){
            fputs(buff, stdout);
        }
    }

    std::cout << "**End of data" << std::endl;

    return 0;
}