// getos.c
// Athanasios Emmanouilidis - 2022
// Tries to guess the operating system of the IP given using the TTL reply of ping.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{

    if (argc == 1) 
    {
        printf("-=== getos.c by Athanasios Emmanouilidis ===-\n");
        printf("Tries to guess the operating system of the IP given using the TTL reply of ping.\n");
        printf("Run it like this: ./getos www.google.com\n");
        return 1;
    }

    enum os { Windows, Linux, Unknown };
    const char *operatingSystemNames[] = { "Windows", "Linux", "Unknown" };
    
    char pingCommand[200];
    char ttl[4];
        
    snprintf(pingCommand, sizeof(pingCommand), "%s%s%s", "ping -c1 ", argv[1], " | grep ttl | sed 's/.*ttl=\\([[:digit:]]*\\).*/\\1/' ");

    enum os operatingSystem;

    FILE *cmd;
    if (NULL == (cmd = popen(pingCommand, "r"))) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(ttl, 4, cmd);

    int ttlInteger = atoi(ttl);

    if (ttlInteger == 64) operatingSystem = Linux;
    else if (ttlInteger == 128) operatingSystem = Windows;
    else operatingSystem = Unknown;

    int rValue = strcmp(ttl,"");

    if (rValue != 0)
        printf("TTL=%i. This machine is probably running %s.\n", ttlInteger, operatingSystem[operatingSystemNames]);
    else
        printf("Some error happened.\n");

    pclose(cmd);

    return 0;
}
