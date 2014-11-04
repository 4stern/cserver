#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "libs/functions.h"

int main()
{
    int server_socket, client_socket;
    int anzahl, laenge;
    int port = 5001;
    struct sockaddr_in serverinfo, clientinfo;
    char client_ip[INET_ADDRSTRLEN];
    char empfangen[1000];
    char *position1, *position2;
    char dateiname[100];
    char text_http_ok[] = "HTTP/1.0 200 OK\r\n\r\n";
    char text_html_anfang[] = "<html><body>";
    char text_html_ende[] = "</body></html>";

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    serverinfo.sin_family = AF_INET;
    serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);
    serverinfo.sin_port = htons(port);
    laenge = sizeof(serverinfo);
    
    bind(server_socket, (struct sockaddr *)&serverinfo, laenge);
    listen(server_socket, 3);

    while(1)
    {
        printf("\nServer running at port %i", port);
        fflush(stdout);

        client_socket = accept(server_socket, (struct sockaddr *)&clientinfo, &laenge);
        inet_ntop(AF_INET, &clientinfo.sin_addr.s_addr,client_ip, INET_ADDRSTRLEN);

        printf("\nVerbindung mit %s", client_ip);

        anzahl = read(client_socket, empfangen, sizeof(empfangen));
        empfangen[anzahl]=0;
        
        write(client_socket,text_http_ok, strlen(text_http_ok));
        write(client_socket,text_html_anfang, strlen(text_html_anfang));
        write(client_socket,text_html_ende, strlen(text_html_ende));
        close(client_socket);
    }
}
