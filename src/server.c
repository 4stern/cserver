#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "libs/functions.h"
#include "libs/ini.h"
#include "libs/ini.c"

typedef struct
{
    int port;
    const char* name;
} configuration;

static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("server", "port")) {
        pconfig->port = atoi(value);

    } else if (MATCH("http-header", "name")) {
        pconfig->name = strdup(value);

    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

int main()
{
    configuration config = { 5002, "CServer/0.1 alpha" };

    if (ini_parse("config.ini", handler, &config) < 0) {

    }
    printf("Config loaded from 'config.ini': name=%s, port=%i\n",
        config.name, config.port);


    int port = config.port;
    char *httpHeaderServer;
    strcpy(httpHeaderServer,"Server: ");
    strcat(httpHeaderServer,config.name);
    strcat(httpHeaderServer,"\r\n");
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
