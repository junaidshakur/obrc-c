# obrc-c
trying to solve one billion row challenge in c to learn c :)
https://github.com/gunnarmorling/1brc

TODO:
- Currently all measurement are aggregated in one, not based on station, so need to implement station based aggregtion
- - Need to implment hashset
- Some lines are missing due to chunks are being started from middle of line so just skipping them to read stable data line.

  
Compilation Steps: (will create a makefile later) 
gcc obrc.c -o obrc -pthread

RUN:
./obrc or time ./obrc

CPU:  **intel core i7 10510U**

Line: 1 billions

Initial version of working file taking **1m10.142s**

After using short for measuremnt instead of double execution time reduce to **0m26.920s**

Some optimzation techniques used in this version are
- multithreading
- assign chunks to each thread based on total size


Will try further improvmement

- Try memory mapped file
- parse measurement as short/int instead of double
- after that will see if what other advace optimzation can be done to reduece time.
