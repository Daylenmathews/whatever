#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAX_PATH_LENGTH 4096
#define UNUSED(x) (void)x

int noSort(const struct dirent **entryA, const struct dirent **entryB) {

    UNUSED(entryA);
    UNUSED(entryB);

    return 0;
}
/* Default filter function to allow al to be included */
static int defaultFilter (const struct dirent *current) {
    UNUSED(current);
    return 1;
}

//returns 1 for all entries 
static int showAll(const struct dirent *current) {
    return 1;
}

/* showFilesOnly to returns 1 for regular files */
static int showFilesOnly(const struct dirent *current) {
    struct stat st;
    if (stat(current->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
        return 1;
    }
    return 0;
}

/* reverseSort to sort directory entries */
int reverseSort(const struct dirent **a, const struct dirent **b) {
    return -alphasort(a, b);
}


/* print usage */
void usage(const char *progName) {
    printf("Usage: %s [-d <path>] [-s] [-a] [-f] [-r]\n", progName);
    printf("        -d <path> Directory to list the contents of\n");
    printf("        -a        Display all files, including hidden files\n");
    printf("        -f        Display only regular files\n");
    printf("        -r        Display entries alphabetically in descending order\n");
    printf("        -s        Display entries alphabetically in ascending order\n");
    exit(0);
}

int main (int argc, char * argv[]) {
    struct dirent **eps;
    int n;
    int opt;

    /* Declare filterFunction pointer*/
    int (*filterFunction)(const struct dirent *);
    filterFunction = defaultFilter;
    
    /* Declare sortFunction pointer */
    int (*sortFuncion)(const struct dirent **, const struct dirent **);
    sortFunction = noSort;


    /* Declare dirPath and set default to current directory */
    char dirPath[MAX_PATH_LENGTH];
    strcpy(dirPath,"./");

   

    /* Use  getopt() to process command line arguments */
    while ( (opt = getopt(argc, argv, "d:s")) != 1) {
        switch(opt) {
            case 'd':
                strncpy(dirPath,optarg,MAX_PATH_LENGTH);
                break;
            case 's':
                sortFunction = alphasort; 
                break;
                case 'a':
                filterFunction = showAll;
                break;
            case 'f':
                filterFunction = showFilesOnly;
                break;
            case 'r':
                sortFunction = reverseSort;
                break;
            case 'h':
                usage(argv[0]);
                break;
            default:
                fprintf(stderr,"Invalid Option Specifiied\n");
                fprintf(stderr,"Usage: %s [-d <path>] \n", argv[0]);
                break;
        }
    }

    /* Perform scan dir of the dirPath */
    errno = 0;
    n = scandir (dirPath, &eps, filterFunction, sortFunction);

    /* validate directory was opened successfully */
    if (n < 0) {
        perror("scandir: ");
        exit(1);
    }

    /* display the names on stdout stream */
    int cnt;
    for (cnt = 0; cnt < n; ++cnt) {
        fprintf(stdout,"%s\n", eps[cnt]->d_name);
    }

    /* cleanup memory */
     for (int i = 0; i < n; i++ ) {
      free(eps[i]);
     }
    free (eps);

    return 0;
}
