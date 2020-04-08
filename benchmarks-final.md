# Benchmarks

## Insert

### 10 милиона insert заявки с 3 инт колони и 1 стринг колона / рандом стойности

generated with: [10m-insert-benchmark.py](10m-insert-benchmark.py)  
```
Command being timed: "./build/release/prostgres -s"
User time (seconds): 55.29
System time (seconds): 146.82
Percent of CPU this job got: 98%
Elapsed (wall clock) time (h:mm:ss or m:ss): 3:24.34
Average shared text size (kbytes): 0
Average unshared data size (kbytes): 0
Average stack size (kbytes): 0
Average total size (kbytes): 0
Maximum resident set size (kbytes): 3952
Average resident set size (kbytes): 0
Major (requiring I/O) page faults: 0
Minor (reclaiming a frame) page faults: 161
Voluntary context switches: 432
Involuntary context switches: 923
Swaps: 0
File system inputs: 0
File system outputs: 7332848
Socket messages sent: 0
Socket messages received: 0
Signals delivered: 0
Page size (bytes): 4096
Exit status: 0
```
## Select

### 10 хиляди рандом селекта
generated with: [10m-select-benchmark.py](10m-select-benchmark.py)  
```
Command being timed: "./build/release/prostgres -s"
User time (seconds): 321.93
System time (seconds): 1367.16
Percent of CPU this job got: 99%
Elapsed (wall clock) time (h:mm:ss or m:ss): 28:09.23
Average shared text size (kbytes): 0
Average unshared data size (kbytes): 0
Average stack size (kbytes): 0
Average total size (kbytes): 0
Maximum resident set size (kbytes): 3920
Average resident set size (kbytes): 0
Major (requiring I/O) page faults: 0
Minor (reclaiming a frame) page faults: 159
Voluntary context switches: 1
Involuntary context switches: 5244
Swaps: 0
File system inputs: 0
File system outputs: 0
Socket messages sent: 0
Socket messages received: 0
Signals delivered: 0
Page size (bytes): 4096
Exit status: 0
```