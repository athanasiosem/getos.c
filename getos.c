// getos.c
// Athanasios Emmanouilidis - 2022 - MIT License
// Tries to guess the operating system of the IP given using the TTL reply of ping.
// The results are valid only if default TTL value has not been changed.
// Use at your own risk.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{

    printf("            _                  \n");
    printf("           | |                 \n");
    printf("  __ _  ___| |_ ___  ___   ___ \n");
    printf(" / _` |/ _ \\ __/ _ \\/ __| / __|\n");
    printf("| (_| |  __/ || (_) \\__ \\| (__ \n");
    printf(" \\__, |\\___|\\__\\___/|___(_)___|\n");
    printf("  __/ |                        \n");
    printf(" |___/                         \n");
    printf("-=== getos.c by Athanasios Emmanouilidis ===-\n\n");

    if (argc == 1) 
    {
        printf("Tries to guess the operating system of the IP given using the TTL reply of ping.\n");
        printf("The results are valid only if default TTL value has not been changed.\n\n");
        printf("Use at your own risk. ");
        printf("Run it like this: ./getos www.google.com\n");
        return 0;
    }

    printf("Script is running. Please wait for results.\n");

    enum os { Windows, Windows9598ME, UnixLinux, SolarisAIXCisco, Unknown };
    const char *operatingSystemNames[] = { "Windows", "Windows 95/98/ME","Unix/Linux", "Solaris/AIX/Cisco", "Unknown" };
    
    char tracerouteCommand[200];
    char pingCommand[200];
    char tracerouteTtl[4];
    char ttl[4];
        
    snprintf(tracerouteCommand, sizeof(tracerouteCommand), "%s%s%s", "traceroute ", argv[1], " | tail -1 | awk '{print $1}' ");
    snprintf(pingCommand, sizeof(pingCommand), "%s%s%s", "ping -c1 ", argv[1], " | grep ttl | sed 's/.*ttl=\\([[:digit:]]*\\).*/\\1/' ");

    enum os operatingSystem;

    FILE *traceroutecmd;
    if (NULL == (traceroutecmd = popen(tracerouteCommand, "r"))) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(tracerouteTtl, 4, traceroutecmd);

    FILE *pingcmd;
    if (NULL == (pingcmd = popen(pingCommand, "r"))) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(ttl, 4, pingcmd);

    int tracerouteTtlInteger = atoi(tracerouteTtl);
    int ttlInteger = atoi(ttl);

    tracerouteTtlInteger = tracerouteTtlInteger - 1;

    int finalTtl = tracerouteTtlInteger + ttlInteger;

    if (finalTtl == 32) operatingSystem = Windows9598ME;
    else if (finalTtl == 64) operatingSystem = UnixLinux;
    else if (finalTtl == 128) operatingSystem = Windows;
    else if (finalTtl == 255) operatingSystem = SolarisAIXCisco;
    else operatingSystem = Unknown;

    int rValue = strcmp(ttl,"");

    if (rValue != 0)
        printf("Script finished. TTL=%i. %s is probably running %s.\n", finalTtl, argv[1], operatingSystem[operatingSystemNames]);
    else
        printf("Some error happened.\n");

    pclose(traceroutecmd);
    pclose(pingcmd);

    return 0;
}
