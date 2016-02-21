/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Copyritht (c) 2015-2016 Kazuma Ohara, NEC                            %
 * Update On 2016.02.05                                                 %
 * This file is part of the multi-party computation by NEC              %
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.        %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % App.cc                                                             %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * This is the main program invoked by entities of MPC protocols. 
 * 
 * This program takes server ID(any one from 1,2,3) as the argument. 
 * 
 * The program of each server works as follows: 
 * - read config file and set the parameters and name of the test circuit. 
 * - generate random share for input of test circuit. 
 * - allocate memory for output of the MPC protocol (we assume that all inputs/outputs of the MPC protocols are sliced-share). 
 * - establishes communication between other two parties by invocation of MPCParty class 
 * - construct MPCCircuit object and execute MPC protocols by invocation MPCCircuit::run. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "Share.h"
#include "XorShift.h"
#include "MPCParty.h"
#include "MPCCircuit.h"
using namespace std;

int main(int argc, char* argv[]) {
  //argument is the number of ID of this server. 
  int serverID;
  if (argc == 2) {
    serverID = atoi(argv[1]);
    cout<<"serverID="<<serverID<<endl;
  }
  else {//argument number is inappropriate
    cout<<"serverID is missing"<<endl;
  }
  
  //set parameters for test environment
  // Config.txt
  // - File name of circuit description
  // - parameters of bit-slice
  string buf;
  std::ifstream ifs("test.conf");
  if(ifs.fail()){
    cout<<"Config file doesn't exist"<<endl;
  }
  getline(ifs, buf);  //read the file name of the circuit
  char* cfilename = new char[buf.size()+1]; 
  std::strcpy(cfilename, buf.c_str());
  getline(ifs, buf);  //read the length of slice (= the number of parallerization)
  int sliceLen = std::stoi(buf); 
  getline(ifs, buf);  //read the number of input sliced-share
  int nSlice = std::stoi(buf); 
  struct timeval start, end; //for measurement of the runnning time of MPCCircuit::run
  int seed=1000;             //for random number generationxs

  // generate an array of input sliced-shares for experiment
  // (assuming all shares are bit-sliced.)
  SlicedShare inputShareVector;
  inputShareVector.Init(2*sliceLen, nSlice);
  block v,x,y,z,a,b,c;
  SlicedShare sec;
  sec.Init(sliceLen, nSlice);

  //Generate random share
  for(int i=0; i <nSlice; i++){
    for (int j=0; j<sliceLen; j++) {
      SlicedShare::sharegen(&v, &x,&y, &z, &a, &b, &c, seed+i*sliceLen+j);
      if (serverID == 1) {
	inputShareVector.data_[i*2*sliceLen+j]          = x;
	inputShareVector.data_[i*2*sliceLen+j+sliceLen] = a;
	sec.data_[i*sliceLen+j]                         = v; //debug
      }
      else if (serverID == 2) {
	inputShareVector.data_[i*2*sliceLen+j]          = y;
	inputShareVector.data_[i*2*sliceLen+j+sliceLen] = b;
	sec.data_[i*sliceLen+j]                         = v; //debug
      }
      else if (serverID == 3) {
	inputShareVector.data_[i*2*sliceLen+j]          = z;
	inputShareVector.data_[i*2*sliceLen+j+sliceLen] = c;
	sec.data_[i*sliceLen+j]                         = v; //debug
      }
    }
  }
  cout<<endl;
  cout<<"Input has been read normally. "<<endl;

  MPCParty party(serverID); //establish connections among MPC servers
  MPCCircuit circuit(cfilename, party, sliceLen, nSlice); //construct circuit object
  SlicedShare outputShareVector; //prepare an array of output sliced-share
  outputShareVector.Init(2*sliceLen, nSlice);
  
  gettimeofday(&start,NULL);
  circuit.run(inputShareVector,outputShareVector); //compute MPC-circuit
  gettimeofday(&end,NULL);

  //reconstruct array of result (for debug)
  SlicedShare readDataVector;
  readDataVector.Init(2*sliceLen, nSlice);
  if (serverID == 1) {
    int nWrite = party.Write(outputShareVector,2);
    int nRead  = party.Read(readDataVector,3);
  }
  else if (serverID == 2) {
    int nRead  = party.Read(readDataVector,1);
    int nWrite = party.Write(outputShareVector,3);
  }
  else if (serverID == 3) {
    int nRead  = party.Read(readDataVector,2);
    int nWrite = party.Write(outputShareVector,1);
  }
  for (int i=0; i<nSlice; i++){
#ifdef DEBUG
    cout<<"Output "<<i<<" = ";
#endif //DEBUG
    for (int j=0; j<sliceLen; j++){
      block outv = readDataVector.data_[i*2*sliceLen+j]^outputShareVector.data_[i*2*sliceLen+j+sliceLen];
#ifdef DEBUG
      cout<<outv<<",";
#endif //DEBUG
    }
#ifdef DEBUG
  cout<<endl;
#endif //DEBUG
  }

#ifdef DEBUG
  for (int i=0; i<nSlice; i++) {
    cout<<"v["<<i<<"] = ";
    for (int j=0; j<sliceLen; j++) {
      //cout<<sec[i][j];
      cout<<sec.data_[i*sliceLen+j];
    }
  cout<<endl;
  }
#endif //DEBUG

  //#ifdef DEBUG //write running time in Result.txt
  if (serverID == 1) {
    cout<<"Running Time of MPC[ms]: "<<1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000<<endl;
    ofstream ofs("Experiment.txt", std::ios::out | std::ios::app);
#ifdef ONLY_COMM
    ofs << "ONLY_COMM:";
#endif //ONLY_COMM
    //ofs << "Number of Parallelization: "<<slieLen<<endl;
    ofs << "Running Time of MPC[ms]: "<<1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000<<endl;
    ofs << endl;
  }
  //#endif //DEBUG

  //delete
  delete[] cfilename;
}
