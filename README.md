GETstats Library and Test Program
=================================
=================================


The main public function in the GETstats library collects statistics for HTTP GET requests.
The test program uses this function to gather statistics from GET requests to http://google.com/

To compile the library and test program:
make clean
make

Usage Example:
export LD_LIBRARY_PATH=$PWD  #Not needed if you install libGETstats.so in the default library path
./test_get_stats -n 30 -H "blah : qwerty" 2>/dev/null

which produces this output on stdout:
SKTEST;216.58.208.142;302;0.060586;0.080874;0.102089;0.102196


The output format is:
SKTEST;<IP address of HTTP server>;<HTTP response code>;<median of CURLINFO_NAMELOOKUP_TIME>;<median of CURLINFO_CONNECT_TIME>;<median of CURLINFO_STARTTRANSFER_TIME>;<median of CURLINFO_TOTAL_TIME>


Detailed output (and errors) are written to stderr.

Note: Only successful GETs are used to calculate medians. The number of successful GETs is written to stderr if the library is compiled with VERBOSE_GETstats defined.


TO DOs
======
- Check the format of command line HTTP header arguments.
- Add more test cases to run_the_test.sh. Eg use tc to delay and drop packets.
- Measure code coverage (eg with GCov)
- Make the source Windows compatible and add a VisualStudio project
