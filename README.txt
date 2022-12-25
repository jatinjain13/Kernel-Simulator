November 10, 2022

By: Jatin Jain - 101184197 (jatinjain@cmail.carleton.ca)

This application is composed of a C file (main.c), an executable generated from the C file (main.exe),
a set of test cases contained in a folders for the schedulers (batch files and csvs) and for memory management

-------------------INSTALLATION and COMPILING-------------------------
For Windows 10/11
Clicking on the batch file will run the executable.
The format for the batch files is the following:

ExeFile InputCSV OutputCSV SchedulingType(0:FCFS, 1:Priority, 2: RR) Memory(0/1 - N/Y) Partition1Size Partition2Size Partition3Size Partition4Size

To have FCFS with memory management in the first scenario you would put the following
Assignment2_Jatin_Jain test_case_4.csv output4.csv 0 1 100 500 250 150 100

----------------------INPUT--------------------
The input CSV must contain one process per line, with the following format

PID ArrivalTime TotalCPUTime I/OFreqency I/ODurration Priority MemoryRequirement

---------------Assumptions this program undergoes------------
All procceses are entered in the input file are ordered off of arrival time.
For memory there are 4 partitions, no more, no less.
For memory there is fragmentation and only a first fit algorithm.
For memory if two process have the same arrival time since there is no priority
 and only one process can run at a time, the second process will take an addition tick to enter the ready queue.

-------------Additional Contents--------------
Part1.doc contains answers to questions from part 1
Scheduling_Test_Cases_Analysis.doc contains analysis of each test case for scheduling only (11 cases each use 3 scheduling algorithms)
Memory_Test_Cases_Analysis.doc contains analysis of each test case for memory only (10 cases each run 2 scenarios)
Gantt.png contains Gantt chart if not readable from doc
Other contents may be exclusive to a pelles C project and not needed to run through Batch files
