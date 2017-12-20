#ifndef __GETstats_H
#define __GETstats_H

/*
 * Collect various statistics for HTTP GET requests
 */

#define MAX_URL_LENGTH 512
#define MAX_HTTP_HEADER_LENGTH 512

typedef struct GETres {
	 char* http_server_addr;
	 long http_response_code;
	 double name_lookup_time;
	 double connect_time;
	 double start_transfer_time;
	 double total_time;
	 unsigned int successful_gets;
 } GETresults;
 
 
typedef enum {
  GETstats_OK = 0,
  GETstats_INVALID_URL,
  GETstats_OTHER_ERROR
} GETstats_result;
 
 
GETstats_result GETstats(const char* url, unsigned number_of_gets, const char** headers, unsigned num_headers, GETresults* results);


#endif /* __GETstats_H */
