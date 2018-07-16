#include "parse_parm.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int is_daemon = 1;

int init_opt(int argc, char *argv[])
{
    int opt = -1;
    char *ptr = "dt:";

    while((opt = getopt(argc, argv, ptr)) != -1) {
        switch(opt) {
            case 'd':
                is_daemon = 0;
                break;
            case 't':
                printf("param:[%s]\n", optarg);
                break;
            default:
                printf("usage:\n\t-d\t\tenter debug mode\n\t\
                        -t\t\tthis is test\n");
                return -1;
        }
    }
    return 0;

}

