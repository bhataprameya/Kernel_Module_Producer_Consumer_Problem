# Kernel_Module_Producer_Consumer_Problem
Producer consumer problem implemented in Kernel Module


Command to execute:

1)make- compiles the kernalcode and the user level code to generates .ko file file for kernal level code and user_code.out file for user level code

2)
		i) sudo insmod kernel_status.ko		-Inserts the character device with default buffer size 10.

			OR

		ii) sudo insmod kernel_status.ko N=X 		-Inserts the character device with user defined buffer size X, where X is a integer value.

3)./producer.out /dev/linepipe		 - Runs the user level producer code 

4) ./consumer.out /dev/linepipe		-Runs user level consumer code

5)sudo rmmod kernel_status - removes the kernal device

Note: Step 3 and 4 can be executed in any combination any no of times.

Project Report:

This project  contains a characer device when reads and writes messages in FIFO order to the user level program.

- There are 2 semaphore and a mutex lock used in total here.
- The buffer size is set to 10 messages. i.e only 10 messages can be stored in this FIFO
- whenever the buffer is full  writing process is blocked till there is a empty space.
- whenever the buffer is empty readding process is blocked till there is a messages in the kernel memory
- there can be any no of read and write process running concerently.
- any process can be terminated by pressing Ctrl+C
- A ragged array is used in this project to store all the messages in character device.
- readcount and writecount are the 2 counters used to read and write messages to the ragged array. 
