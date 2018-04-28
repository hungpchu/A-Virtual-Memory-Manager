# Designing-a-Virtual-Memory-Manager

## 1 Compile, Run, and Test program:

### 1.1 Compile and Run

	

	a/ To run the program in folder part1:

make
./Part1.out BACKING_STORE.bin addresses.txt 

Then the output will be in the correct2.txt

run ./part1.out BACKING_STORE.bin addresses.txt for having output in correct.txt

Check the differ between 2 files with: diff correct.txt correct2.txt



	b/ To run the program in folder part2:


make
./Part2.out BACKING_STORE.bin addresses.txt fifo

or 

make
./Part2.out BACKING_STORE.bin addresses.txt lru


Then the output will be in the correct2.txt

run ./part2.out BACKING_STORE.bin addresses.txt lru 
or   ./part2.out BACKING_STORE.bin addresses.txt fifo for having output in correct.txt

Check the differ between 2 files with: diff correct.txt correct2.txt




## 2  Document how you have tested your program, report on the testing results and what the
testing results demonstrate.



	I have tested the program from small test case to big test case. Then to find bug, i print a lot to see how the values be.




## 3 Document any problems you had.




	I have so many problems to understand how the whole program work, how page table or physical memory function. However, thanks to the explain of amazing TA Aditya, i have overcame all to finish this PA4 on time. If you are reading this, i just want to say "Thank you so much Aditya"




## 4 Answer to the following “fun” question.




In this assignment, we assume there is only one process running in the system and all the memory belongs to it. However, in a typical computer system there will be multiple processes at any given time. Thus, the physical memory will be shared among all the processes. Briefly discuss (in no more than 150 words) the challenges that you might encounter when extending our model to a multiprogramming system.



Answer: Within the multi system, i think we will have each page table for each process so there will be easier to keep track many of them in the main physical memory.
