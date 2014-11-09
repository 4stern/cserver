#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "libs/functions.h"

int main()
{
    int port = 5001;
    char httpHeaderServer[] = "Server: CServer (Unix/Linux)\r\n";
    char httpHeaderDoNotTrack[] = "X-Do-Not-Track: 1\r\nDNT: 1\r\n";

    int server_socket, client_socket;
    int anzahl, laenge;
    struct sockaddr_in serverinfo, clientinfo;
    char client_ip[INET_ADDRSTRLEN];
    char empfangen[1000];
    char *position1, *position2;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    serverinfo.sin_family = AF_INET;
    serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);
    serverinfo.sin_port = htons(port);
    laenge = sizeof(serverinfo);
    
    bind(server_socket, (struct sockaddr *)&serverinfo, laenge);
    listen(server_socket, 3);

    printf("\nServer listen ar port %i", port);

    char publicPath[256];
    strcpy (publicPath, "/home/vierstern/git/cserver/public/");
    printf("\npublicPath: %s",publicPath);

    while(1)
    {
        fflush(stdout);
        client_socket = accept(server_socket, (struct sockaddr *)&clientinfo, &laenge);
        inet_ntop(AF_INET, &clientinfo.sin_addr.s_addr,client_ip, INET_ADDRSTRLEN);

        printf("\nRequest from %s", client_ip);

        anzahl = read(client_socket, empfangen, sizeof(empfangen));
        empfangen[anzahl]=0;
        
        char *requestedPathFilename;
        getRequestedFileName(empfangen, requestedPathFilename);

        //concat publicPath with requestedPathFilename
        char currentFilePath[256];
        strcpy(currentFilePath, publicPath);
        strcat (currentFilePath, requestedPathFilename);

        //file exists?
        if( fileExist(currentFilePath) == 1 ){
            
            char *fileContent;
            //long *length;
            fileContent = fileGetContent(currentFilePath);
            printf(": '%s'(%lu) -> 200",requestedPathFilename);

            char header[] = "HTTP/1.0 200 OK\r\n";
            strcat(header, "Content-Type: text/html; charset=utf-8\r\n");
            strcat(header, httpHeaderServer);
            strcat(header, httpHeaderDoNotTrack);
            strcat(header, "\r\n");

            write(client_socket,header, strlen(header));
            write(client_socket,fileContent, strlen(fileContent));

        } else {

            printf(": '%s' -> 404",requestedPathFilename);

            char header[] = "HTTP/1.0 404 Not Found\r\n";
            strcat(header, "Content-Type: text/html; charset=utf-8\r\n");
            strcat(header, httpHeaderServer);
            strcat(header, httpHeaderDoNotTrack);
            strcat(header, "\r\n");

            char body[] = "<html><body>";
            strcat(body, "Not Found");
            strcat(body, "</body></html>");

            write(client_socket, header, strlen(header));
            write(client_socket, body, strlen(body));
        }
        close(client_socket);
    }
}
