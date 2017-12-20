/*
 * Collect various statistics for HTTP GET requests
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "GETstats.h"


static     char** http_server_addr = NULL;
static     long* http_response_code = NULL;
static     double* name_lookup_time = NULL;
static     double* connect_time = NULL;
static     double* start_transfer_time = NULL;
static     double* total_time = NULL;


/* comparison function for qsort */
static int compare_doubles(const void * a, const void * b)
{
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;  
}


static void free_arrays(void){

    if (http_server_addr) free(http_server_addr);
    if (http_response_code) free(http_response_code);
    if (name_lookup_time) free(name_lookup_time);
    if (connect_time) free(connect_time);
    if (start_transfer_time) free(start_transfer_time);
    if (total_time) free(total_time);

}


static double median(double* values, unsigned numvals){

    unsigned odd;
    unsigned mid;

    if (numvals == 1)
        return values[0];

    if (numvals == 2)
        return (values[0] + values[1])/2.0 ;

    qsort(values, numvals, sizeof(double), compare_doubles);
    mid = numvals / 2;
    odd = numvals % 2;

    if (odd)
        return values[mid];
        
        
    return (values[mid] + values[mid-1])/2;
}




GETstats_result GETstats(const char* url,
                         unsigned number_of_gets, const char** headers,
                         unsigned num_headers, GETresults* results){

    CURL *curl;
    CURLcode res;
    unsigned int successful_gets=0;

    /* Check inputs */
    /* Check the URL - just checking max length for now */
    if (sizeof(url) > MAX_URL_LENGTH){
        fprintf(stderr,"ERROR: url too long\n");
        return GETstats_INVALID_URL;
    }
    if (number_of_gets == 0){
        fprintf(stderr,"ERROR: number of gets is zero\n");
        return GETstats_OTHER_ERROR;
    }
    if ( headers == NULL && num_headers > 0){
        fprintf(stderr,"ERROR: headers is NULL and num_headers is %d\n",num_headers);
        return GETstats_OTHER_ERROR;
    }

    
    /* Malloc arrays for result of each GET */
    http_server_addr = calloc(number_of_gets, sizeof(char*));
    if ( http_server_addr == NULL ){
        fprintf(stderr,"ERROR: could not allocate http_server_addr[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }
    http_response_code = calloc(number_of_gets, sizeof(long));
    if ( http_response_code == NULL ){
        fprintf(stderr,"ERROR: could not allocate http_response_code[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }
    name_lookup_time = calloc(number_of_gets, sizeof(double));
    if ( name_lookup_time == NULL ){
        fprintf(stderr,"ERROR: could not allocate name_lookup_time[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }
    connect_time = calloc(number_of_gets, sizeof(double));
    if ( connect_time == NULL ){
        fprintf(stderr,"ERROR: could not allocate connect_time[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }
    start_transfer_time = calloc(number_of_gets, sizeof(double));
    if ( start_transfer_time == NULL ){
        fprintf(stderr,"ERROR: could not allocate start_transfer_time[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }
    total_time = calloc(number_of_gets, sizeof(double));
    if ( total_time == NULL ){
        fprintf(stderr,"ERROR: could not allocate total_time[]\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }



    /* do number_of_gets GETs */
    for (int i=0; i < number_of_gets; i++){

        char* ip = NULL;
        long  xhttp_response_code;
        double xname_lookup_time;
        double xconnect_time;
        double xstart_transfer_time;
        double xtotal_time;


        /* initialize libcurl */ 
        curl = curl_easy_init();
        if(curl == NULL){
            fprintf(stderr,"ERROR: curl_easy_init() failed\n");
            continue;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)stderr);
        if(res != CURLE_OK){
            fprintf(stderr,"ERROR: curl_easy_setopt() failed for CURLOPT_WRITEDATA\n");
        }
#ifdef VERBOSE_GETstats
        res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        if(res != CURLE_OK){
            fprintf(stderr,"ERROR: curl_easy_setopt() failed for CURLOPT_VERBOSE\n");
        }
#endif
        res = curl_easy_setopt(curl, CURLOPT_URL, url);
        if(res != CURLE_OK){
            fprintf(stderr,"ERROR: curl_easy_setopt() failed for CURLOPT_URL\n");
            curl_easy_cleanup(curl);
            continue;
        }
        res = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        if(res != CURLE_OK){
            fprintf(stderr,"ERROR: curl_easy_setopt() failed to enable CURLOPT_HTTPGET\n");
            curl_easy_cleanup(curl);
            continue;
        }
        if (num_headers > 0){
            struct curl_slist *list = NULL;
            for (int j=0;j < num_headers;j++){
                list = curl_slist_append(list, headers[j]);
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);            
            if(res != CURLE_OK){
                fprintf(stderr,"ERROR: curl_easy_setopt() failed for CURLOPT_URL\n");
                curl_easy_cleanup(curl);
                continue;
            }
        }



        /* do the GET */
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }


        /* collect the stats */
        if (curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip) != CURLE_OK || ip == NULL){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_PRIMARY_IP: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }
        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &xhttp_response_code);
        if (res != CURLE_OK ){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_RESPONSE_CODE: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }
                res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &xname_lookup_time);
        if ( res != CURLE_OK ){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_NAMELOOKUP_TIME: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }
        res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &xconnect_time);
        if ( res != CURLE_OK ){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_CONNECT_TIME: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }
        res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &xstart_transfer_time);
        if ( res != CURLE_OK ){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_STARTTRANSFER_TIME: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }
        res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &xtotal_time);
        if ( res != CURLE_OK ){
            fprintf(stderr, "curl_easy_getinfo() failed for CURLINFO_TOTAL_TIME: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            continue;
        }

        /* copy results to array */
        http_server_addr[successful_gets] = (char*) malloc( (strlen(ip)+1)*sizeof(char));
        if ( http_server_addr[successful_gets] == NULL ){
            fprintf(stderr, "ERROR: could not allocate http_server_addr[%d]\n", successful_gets);
            curl_easy_cleanup(curl);
            continue;
        }

        memcpy(http_server_addr[successful_gets], ip, strlen(ip)+1);
        http_response_code[successful_gets] = xhttp_response_code;
        name_lookup_time[successful_gets] = xname_lookup_time;
        connect_time[successful_gets] = xconnect_time;
        start_transfer_time[successful_gets] = xstart_transfer_time;
        total_time[successful_gets] = xtotal_time;

        /* the GET succeeded, and we retrieved all the stats */
        successful_gets++;
        curl_easy_cleanup(curl);
    }


    results->successful_gets = successful_gets;

    if (successful_gets == 0){
        fprintf(stderr,"ERROR: No successul GETs\n");
        free_arrays();
        return GETstats_OTHER_ERROR;
    }

    /* just using the first values and discarding the rest */
    results->http_server_addr = http_server_addr[0];
    results->http_response_code = http_response_code[0];


    if (successful_gets == 1){
        /* don't need to calculate medians  */
        results->name_lookup_time = name_lookup_time[0];
        results->connect_time = connect_time[0];
        results->start_transfer_time = start_transfer_time[0];
        results->total_time = total_time[0];
    }
    else {
        results->name_lookup_time = median(name_lookup_time, successful_gets);
        results->connect_time = median(connect_time, successful_gets);
        results->start_transfer_time = median(start_transfer_time, successful_gets);
        results->total_time = median(total_time, successful_gets);
    }

#ifdef VERBOSE_GETstats
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"http_server_addr[%d]=%s\n",i,http_server_addr[i]);
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"http_response_code[%d]=%ld\n",i,http_response_code[i]);
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"name_lookup_time[%d]=%f\n",i,name_lookup_time[i]);
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"connect_time[%d]=%f\n",i,connect_time[i]);
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"start_transfer_time[%d]=%f\n",i,start_transfer_time[i]);
    for (int i=0;i < successful_gets;i++)
		fprintf(stderr,"total_time[%d]=%f\n",i,total_time[i]);

    fprintf(stderr,"GETstats: successful_gets=%d\n",successful_gets);
#endif
    free_arrays();
    return GETstats_OK;
    
    
}
