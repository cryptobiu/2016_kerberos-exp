 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % README.txt                                                         %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Update: 2016/02/12
Author: Kazuma Ohara, NEC

===============================================================
This folder includes the programs of our MPC protocol. 
We assume that the all input/output shares of the MPC is bit-sliced. 
* Contents
 - App.cc
 - MPCParty.cc/MPCParty.h
 - MPCCircuit.cc/MPCCircuit.h
 - Share.cc/Share.h
 - XorShift.cc/XorShift.h
 - parties.conf
 - test.conf
 - Makefile

***.h file is a header file, including definitions of member variables and prototype functions. 
***.cc file is the implementation corresponds to ***.h file. 
The brief description of each file is written in the comments of ***.h file. 

parties.conf and test.conf are config files for setting parameters. 

* parties.conf
parties conf specifies the IP addresses and the port number of each MPC party, which is read by MPCParty.cc. 
Default parties.conf is for the environment that 3 parties run in the same machine. 
(localhost(127.0.0.1) as IP address) 
When you want to use the program, you should edit this file for your own environment. 
In this file, the following informations are written in the following order.  
 - the port number of party 1 (default: 127.0.0.1)
 - the IP address of party 1  (default: 10080)
 - the port number of party 2 (default: 127.0.0.1)
 - the IP address of party 2  (default: 20080)
 - the port number of party 3 (default: 127.0.0.1)
 - the IP address of party 3  (default: 30080)

* test.conf
test.conf specifies the name of the circuit and parameters for the test environment, which is read by App.cc. 
  this file, the following informations are written in the following order. 
 - The name of the circuit file which is evaluated. (default: TestCircuit.txt)
 - the length of each sliced-shares
 - the number of input sliced-shares

* App.cc
  It is main programs called by each MPC server. 
  First argument is the ID of the server calling the application. 
  (usage example)
	[server 1] $ ./App.out 1
	[server 2] $ ./App.out 2
	[server 3] $ ./App.out 3
	The target circuit and parameters are read from "test.conf". 
	The information of the IP address and the port numbers of parties are read from "parties.conf". 

* MakeTestCircuit.cc
 - It generates a description of test-circuit named "TestCircuit.txt". 
   This is only for the experiments. 
 - The parameters are the number of round and the number of AND gates per each round. 
   You can run MakeTestCircuit.out and enter the parameters on the command line. 
   The circuit described in TestCircuit.txt evaluates 
      AND Gates * (number of AND gates and each round) 
      XOR Gates * (number of AND gates and each round) * 2
   for each round. 
   e.g.) When (number of AND gates per each round)=100, (the number of round)=200
         the number of AND gate is 100 and the number of XOR gates is 200 per each round. 
         Total (100 round) numbers of AND/XOR gates are 10000/20000. 
         (The total scale of the circuit is close to 128-bit AES circuit.)
 (usage example)
 	$ g++ MakeTestCircuit.cc -std=c++11 -o MakeTestCircuit.out
	$ ./MakeTestCircuit.out
	 - The number of Rounds?: 
	     (enter the number from keyboard)
	 - The number of AND Gates per round?: 
	     (enter the number from keyboard)
