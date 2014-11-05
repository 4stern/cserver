/* Datei functions.h */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void getRequestedFileName(char* empfangen, char* dateiname)
{
    char *position1, *position2;
    int laenge;

    position1 = strstr(empfangen, "GET");
    position2 = strstr(empfangen, "HTTP");
    laenge = position2 - position1 - 6;

    strncpy(dateiname, position1+5, laenge);
    dateiname[laenge]=0;
}

int fileExist (char *filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return -1;
}

char * fileGetContent(char *filename){
    long length;
    char * buffer = 0;
    FILE * f = fopen (filename, "r");

    if (f){
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length+1);
        if (buffer){
            fread (buffer, 1, length, f);
        }
        buffer[length] = 0;
      fclose (f);
    }
    return buffer;
}

#endif