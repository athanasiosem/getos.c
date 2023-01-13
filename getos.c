// getos.c
// Tries to fingerprint the operating system of a remote host using the TTL reply of ping.
// Requires ping and traceroute installed.
// The results are valid only if default TTL value has not been changed.
// Athanasios Emmanouilidis - 2023 - MIT License. Code to find closest element by Smitha Dinesh Semwal.
// Use at your own risk.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for access()

void print_usage_instructions()
{
    printf("Tries to guess the operating system of the IP given using the TTL reply of ping.\n");
    printf("The results are valid only if default TTL value has not been changed.\n\n");
    printf("Requires ping and traceroute installed.\n");
    printf("Use at your own risk. ");
    printf("Run it like this: ./getos www.google.com\n");
}

void print_ascii_art()
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
}

int getClosest(int val1, int val2, int target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

// Returns element closest to target in arr[]
int findClosest(int arr[], int n, int target)
{
    // Corner cases
    // left-side case
    if (target <= arr[0])
        return arr[0];
    // right-side case
    if (target >= arr[n - 1])
        return arr[n - 1];

    // Doing binary search
    int i = 0, j = n, mid = 0;
    while (i < j)
    {
        mid = (i + j) / 2;

        if (arr[mid] == target)
            return arr[mid];

        /* If target is less than array element,
            then search in left */
        if (target < arr[mid])
        {

            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > arr[mid - 1])
                return getClosest(arr[mid - 1],
                                  arr[mid], target);
            j = mid;
        }
        /* Repeat for left half */

        // If target is greater than mid
        else
        {
            if (mid < n - 1 && target < arr[mid + 1])
                return getClosest(arr[mid],
                                  arr[mid + 1], target);
            // update i
            i = mid + 1;
        }
    }

    // Only single element left after search
    return arr[mid];
}

int get_ping_ttl(char *host)
{
int fd[2];
pid_t pid;
char pingTtl[4];
char pingCommand[200];

if (pipe(fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
}

if ((pid = fork()) == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
}

if (pid == 0) {
    // child process
    close(fd[0]);    // close the read end of the pipe
    dup2(fd[1], 1);  // redirect stdout to write end of the pipe
    snprintf(pingCommand, sizeof(pingCommand), "%s%s%s", "ping -c1 ", host, " | grep ttl | sed 's/.*ttl=\\([[:digit:]]*\\).*/\\1/' ");
    execl("/bin/sh", "sh", "-c", pingCommand, (char *)NULL);
} else {
    // parent process
    close(fd[1]);    // close the write end of the pipe
    read(fd[0], pingTtl, 4);
    close(fd[0]);
    wait(NULL);
}

return atoi(pingTtl);
}

int get_ping_ttl_old(char *host)
{
    char pingTtl[4];
    char pingCommand[200];
    
    snprintf(pingCommand, sizeof(pingCommand), "%s%s%s", "ping -c1 ", host, " | grep ttl | sed 's/.*ttl=\\([[:digit:]]*\\).*/\\1/' ");

    FILE *pingcmd;
    if (NULL == (pingcmd = popen(pingCommand, "r")))
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(pingTtl, 4, pingcmd);
    pclose(pingcmd);

    return atoi(pingTtl);
}

int get_traceroute_ttl(char *host)
{
    char tracerouteTtl[4];
    char tracerouteCommand[200];

    snprintf(tracerouteCommand, sizeof(tracerouteCommand), "%s%s%s", "traceroute ", host, " | tail -1 | awk '{print $1}' ");

    FILE *traceroutecmd;
    if (NULL == (traceroutecmd = popen(tracerouteCommand, "r")))
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(tracerouteTtl, 4, traceroutecmd);
    pclose(traceroutecmd);

    return atoi(tracerouteTtl -1);
}

int main(int argc, char *argv[])
{

    if (access("/bin/ping", F_OK) == -1 || access("/usr/sbin/traceroute", F_OK) == -1)
    {
        printf("Error: ping or traceroute not found. Please ensure that these dependencies are installed.\n");
        return 1;
    }

    print_ascii_art();

    if (argc != 2)
    {
        print_usage_instructions();
        return 0;
    }

    printf("Script is running. Please wait for results.\n");

    enum os
    {
        Windows_Vista_7_Server2008_10_11,
        Windows_95_98_ME,
        Unix_Linux_FreeBSD_MacOSX,
        Solaris_AIX_Cisco
    }; 

    enum os operatingSystem;

    const char *operatingSystemNames[] = {"Windows Vista / Windows 7 / Windows Server 2008 / Windows 10 / Windows 11", "Windows 95 / Windows 98 / Windows ME", "Unix / Linux / FreeBSD / MacOSX", "Solaris / AIX / Cisco"};
    int ttlValuesArray[] = {32, 64, 128, 255};

    int pingTtl = get_ping_ttl(argv[1]);
    int tracerouteTtl = get_traceroute_ttl(argv[1]);
    int finalTtl = pingTtl + tracerouteTtl;
    int closestTtl = (findClosest(ttlValuesArray, sizeof(ttlValuesArray) / sizeof(ttlValuesArray[0]), finalTtl));

    if (closestTtl == 32)
        operatingSystem = Windows_95_98_ME;
    else if (closestTtl == 64)
        operatingSystem = Unix_Linux_FreeBSD_MacOSX;
    else if (closestTtl == 128)
        operatingSystem = Windows_Vista_7_Server2008_10_11;
    else if (closestTtl == 255)
        operatingSystem = Solaris_AIX_Cisco;

    if (pingTtl != 0)
        printf("Script finished. TTL=%i. %s is probably running %s.\n", finalTtl, argv[1], operatingSystem[operatingSystemNames]);
    else
        printf("An error occured. The machine is probably blocking our pings.\n");

    return 0;
}
