all:: objs
	g++ -std=c++11 -o App.exe App.cc Share.o MPCParty.o MPCCircuit.o XorShift.o

objs::
	g++ -fPIC -c -std=c++11 -o MPCCircuit.o MPCCircuit.cc
	g++ -fPIC -c -std=c++11 -o Share.o      Share.cc
	g++ -fPIC -c -std=c++11 -o XorShift.o   XorShift.cc
	g++ -fPIC -c -std=c++11 -o MPCParty.o   MPCParty.cc
	g++ -fPIC -c -std=c++11 -o c_interface.o   c_interface.cc
	
clean::
	rm MPCCircuit.o Share.o XorShift.o MPCParty.o App.exe

lib:
	#ar -cvq libmpc.a MPCCircuit.o Share.o XorShift.o MPCParty.o c_interface.o
	g++ -shared -o libmpc.so MPCCircuit.o Share.o XorShift.o MPCParty.o c_interface.o
