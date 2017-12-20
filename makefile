CC=gcc
AR=ar
CFLAGS = -g -Wall

test_get_stats : test_get_stats.c libGETstats.a
	    $(CC) $(CFLAGS) test_get_stats.c -L./ -lGETstats -lcurl -o test_get_stats
	    
libGETstats.a : GETstats.c GETstats.h
	    $(CC) $(CFLAGS) -c GETstats.c -DVERBOSE_GETstats -o GETstats.o
	    $(AR) rcs libGETstats.a GETstats.o


clean :
	    rm *.a *.o test_get_stats


