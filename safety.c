#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define SYNC_SIZE 16
#define SHARED_SIZE 786432

// This file is a safety file. Always run this before launching the ray tracer!
// It initializes the files for memory mapping between Java and C, and causes
//   them to exist if they don't already.
//
// SyncFile contains information about what buttons the user is pushing,
//   causing movement on the screen. The ray tracer implements a classic
//   double buffering technique: the threads will write to one buffer, while
//   the other buffer is being displayed to the screen. Writing to the buffer
//   takes the entire frame time (~27-28 ms), writing to the screen takes
//   about 4 ms. Total latency between 27 and 35 ms.
//
//   Double buffer is implemented using Shared files 1 and 2. While the
//   backend write to one, the front end displays the other.

int main(int argc, char **argv) {
    int fd = fileno(fopen("syncFile.txt", "w+"));

    unsigned char buf[SYNC_SIZE];
    bzero(buf, SYNC_SIZE);
    write(fd, buf, SYNC_SIZE);
    close(fd);

    fd = fileno(fopen("sharedFile1.txt", "w+"));
    unsigned char buf2[SHARED_SIZE];
    bzero(buf2, SHARED_SIZE);
    write(fd, buf2, SHARED_SIZE);
    close(fd);

    fd = fileno(fopen("sharedFile2.txt", "w+"));
    write(fd, buf2, SHARED_SIZE);
    close(fd);

    exit(0);
}
