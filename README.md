# java_hprof_to_txt
converts the binary java heap dump (*.hprof) into text
written in C++

# Supported JDKs
Currently I'm writing and testing for JDK8 on Windows and Mac.

# Motivation

I attempted to open a 20GB hprof file and it took over 3.5 hours to finish processing in MAT and VisualVM. 
Even worse, it was unusablely slow.
Just clicking on an object instance in VisualVM takes forever to compute the forward and back references.
As a result, this is the my first attempt at parsing the hprof file in C++.
The primary goal for this intial project is to dump all the sizes, edges, and ignore the data.
A secondary goal is to denomalize the types.
Another secondary goal is to accept the hprof file from standard in so that intermediate pipes like pv can be used.
The plan is to clone this project into a second project that converts hprof into csv.
The final step is to create a REPL that efficiently operates on the graph memory structure.

# Specification

    Byte Number Description
    /---\ 
      0     N byte null terminated string (including the null char)
      1     Will say: JAVA PROFILE 1.0.1
     ...    or        JAVA PROFILE 1.0.2
      18    null terminating character
    \---/
    /---\
      20   4 Bytes describing how many bytes are used for identifiers
      21   is this usually 4 ie 00 00 00 04 ?
      22
      23
    \---/
    /---\
      24  highest 4 bytes of the long millseconds since epoch
      25   
      26
      27
    \---/
    /---\
      28  lowest 4 bytes of the long millseconds since epoch
      29   
      30
      31
    \---/

    /-----\
     /---\
        0     byte describing the type of the record
     \---/
     /---\
        1     microseconds since timestamp in header
        2
        3
        4
     \---/
     /---\
        1     this number, N, represents the number of bytes to follow
        2
        3
        4
     \---/
     /---\
       0
      ...
      N-1
     \---/
    



The most recent version is available at:

* https://blogs.oracle.com/kto/resource/manual.html#mozTocId848088
* https://java.net/downloads/heap-snapshot/hprof-binary-format.html
