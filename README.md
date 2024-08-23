# obrc-c
trying to solve one billion row challenge in c to learn c :)
https://github.com/gunnarmorling/1brc

Compilation Steps: (will create a makefile later) 
gcc obrc.c -o obrc -pthread

RUN:
./obrc or time ./obrc


Initial version of working file to process 1 billion records on **intel core i7 10510U** took avg **1m10.142s**

Some optimzation techniques used in this version are
- multithreading
- assign chunks to each thread based on total size


Will try further improvmement

- Try memory mapped file
- parse measurement as short/int instead of double
- after that will see if what other advace optimzation can be done to reduece time.
