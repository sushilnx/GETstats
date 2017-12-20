CC=gcc
AR=ar

test_get_stats : test_get_stats.c libGETstats.a
	    $(CC) test_get_stats.c -L./ -lGETstats -lcurl -o test_get_stats
	    
libGETstats.a : GETstats.c GETstats.h
	    #$(CC) -c GETstats.c -DVERBOSE_GETstats -o GETstats.o
	    $(CC) -c GETstats.c -o GETstats.o
	    $(AR) rcs libGETstats.a GETstats.o


clean :
	    rm *.a *.o test_get_stats


