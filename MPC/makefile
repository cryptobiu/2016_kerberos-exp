all:: objs
	g++ -std=c++11 -o App.exe App.cc Share.o MPCParty.o MPCCircuit.o XorShift.o UDPMCParty.o

objs::
	g++ -c -std=c++11 -o MPCCircuit.o MPCCircuit.cc
	g++ -c -std=c++11 -o Share.o      Share.cc
	g++ -c -std=c++11 -o XorShift.o   XorShift.cc
	g++ -c -std=c++11 -o MPCParty.o   MPCParty.cc
	g++ -c -std=c++11 -o UDPMCParty.o   UDPMCParty.cc
	
clean::
	rm MPCCircuit.o Share.o XorShift.o MPCParty.o App.exe
