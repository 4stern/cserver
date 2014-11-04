/* Datei functions.h */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>

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


#endif
