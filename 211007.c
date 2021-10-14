// .bashrc
// sudo gedit .bashrc

// export VARIABLE = VALUE1
// source .bashrc

// /etc/profile
// sudo gedit /etc/profile
// source /etc/profile

#include <unistd.h>

int getopt(int argc, char* const argv[], const char *optstring);

//optstring[] = "a:b:c";

extern char *optarg;
extern int optind, opterr, optopt;

//-a 10
// optarg[0] = 1;
// optarg[1] = 0;

// GNU_SOURCE

#include <getopt.h>
int getopt_long (argc, argv, optstring, const struct option *longopts,  int *longindex);

option1 = 1000;

struct option{
    const char* name;
    int flag_arg;
    int *flag;
    int val;
}


flag == NULL => return val;
flag != NULL => return 0;

static struct option
{
    []
};

//
int getopt_long_only (argc, argv, optstring, *longopt, *longindex);