=======================================================================
------------------------------README-----------------------------------
=======================================================================

There are two files
-> server-mp.c
-> multi-client.c

Compiling the Server and Client:
-----------------------------------------------------------------------
Execute the following instructions:
1. gcc server-mp. -o server -w (The -o flag is for changing the name of the executable and the -w flag is for eliminating the warnings) 

2. gcc multi_client.c -o client -pthread -w (The -o flag is for changing the name of the executable and the -w flag is for eliminating the warnings. Also the -pthread is for using the pthreads)

Starting the Server:
------------------------------------------------------------------------
./server <port-name>
And then the server starts. There is no need for human intervention again on the server side


Starting the Client:
------------------------------------------------------------------------
./client <server-ip/hostname> <port-name> <number-of-threads> <time-to-run> <sleep-time-betwwen-requests-from-a-thread> <random/fixed>


