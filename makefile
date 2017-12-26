CC=gcc
AR=ar
CFLAGS = -g -Wall

test_get_stats : test_get_stats.c libGETstats.so
	    $(CC) $(CFLAGS) test_get_stats.c -lGETstats -Wl,-rpath-link=$(PWD) -o test_get_stats -L$(PWD)
	    
libGETstats.so : GETstats.c GETstats.h
	    $(CC) -shared -o libGETstats.so $(CFLAGS) -shared -fPIC  GETstats.c -lcurl


clean :
	    rm *.so test_get_stats


