/*
 * Test the GETstats library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "GETstats.h"


#define DEFAULT_SAMPLES 10
#define MIN_SAMPLES 1
#define MAX_SAMPLES 100
#define MAX_HEADERS 20


static void print_usage(){
    printf("USAGE:\n");
    printf("[[-H \"Header-name-0: Header-value-0\"]...[-H \"Header-name-10: Header-value-10\"]] [-n <number of samples>] [-v (enables verbose output)]\n");
    printf("The number of samples defaults to 10, if not specified. The minimum is 1 and the maximum is 100.\n");
    printf("The maximum number of user defined HTTP headers is 20.\n");
}


bool is_pos_int(const char* number, int len)
{
   bool res = true;
   for (int i=0;i < len; i++)
       if ( number[i] < '0' || number[i] > '9' ){
           res = false;
           break;
       }

    return res;
}



int main(int argc, char *argv[])
{
    GETresults* res=NULL;
    GETstats_result result;
    char *http_headers[MAX_HEADERS];
    int num_headers=0;
    int samples=DEFAULT_SAMPLES;
    bool verbose=false;
    

    /* check the command line arguments */
    for (int i=1; i <argc; i++) {
        if (strncmp(argv[i],"-n", 3) == 0 )
        {
            ++i;
            if (i < argc && is_pos_int(argv[i], strlen(argv[i])) == true){
                samples = atoi(argv[i]);
                if (samples >= MIN_SAMPLES && samples <= MAX_SAMPLES)
                    continue;
            }
            fprintf(stderr,"ERROR: Bad command line\n");
            print_usage();
            return 1;
        }
        else if (strncmp(argv[i],"-H", 3) == 0 && num_headers < MAX_HEADERS)
        {
            ++i;
            if (i < argc ){
                /* TODO: Check http header string */
                http_headers[num_headers] = argv[i];
                num_headers++;
            }
        }
        else if (strncmp(argv[i],"-v", 3) == 0 )
        {
                verbose=true;
        }
        else if (strncmp(argv[i],"--help", 7) == 0 )
        {
                print_usage();
                return 0;
        }
        else 
        {
                fprintf(stderr,"ERROR: Bad command line\n");
                print_usage();
                return 1;
        }
    }



    result = GETstats("http://google.com/", samples, (const char **) http_headers, num_headers, verbose, &res);

    if (result != GETstats_OK ){
        printf ("FAILED: returned %d\n",result);
        return 1;
    }

    GETstats_print_result(res);
    GETstats_cleanup(res);
    
    return 0;
}



