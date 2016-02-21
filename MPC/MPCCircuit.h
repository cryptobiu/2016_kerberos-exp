/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Copyritht (c) 2015-2016 Kazuma Ohara, NEC                            %
 * Update On 2016.02.05                                                 %
 * This file is part of the multi-party computation by NEC              %
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.        %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % MPCCircuit class                                                   %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * This class provides multi-party computation which takes array of sliced-shares as inputs. 
 *
 * The constructor of this class loads the information of the MPC circuit into own member variable from the description of the circuit. 
 * We assume that the file on the circuit description includes following information. 
 * - the total number of rounds
 * - the total number of gates (e.g., XOR, AND)
 * - the maximum number of internal register which store the intermediate values on MPC computation.
 * - the maximum number of sliced-shares which is sent in a round. 
 * - the number of input sliced-shares
 * - the number of output sliced-shares
 * - the indices of the register corresponding input sliced-share
 * - the indices of the register corresponding output sliced-share
 * - the description of the gates of the circuit
 *
 * MPCCircuit::run function provides MPC computation for the above circuit.
 * For each round, this function evaluates the circuit according to the gate description, and then sends the adequate values for AND-gate operation via the object of MPCParty class. 
 */

#ifndef MPCCIRCUIT_H
#define MPCCIRCUIT_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <random>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "Share.h"
#include "MPCParty.h"
#include "XorShift.h"
#define CHRnd 0
#define XORGate 1
#define ANDGate 2
//#define ONLY_COMM //if this is active, only communication part is compiled. 
#define WRITE_TIME
//#define DEBUG

struct Gate{ //structure of gate unit of circuit used in MPCCircuit class defined below.
  /* Field: 
   * - leftInputIndex: index of left input register
   * - rightInputIdex: index of right input register
   * - outputIndex: index of output register
   * - gateType: gate type which wants to be evaluated;
   *             1:XOR, 2:AND
   *             0 (defined CHRnd): a special symbol that indicates end of the current round.
   *             When CHRnd is read, communication of this round is invoked. 
   * - sendDataIndex:
   *           Our MPC requiers sending data for AND computation. The value which is sent is stored in sending buffer named sendDataVector.  
   *           When the evaluated value of this gate is sent, sending data corresponding outputIndex is stored in the sendDataVector; 
   *           sendDataIndex indicates the position of the sending data on sendDataVector. 
   * - isSend: the flag that indicates whether the result of evaluation is send.
   *           isSend=1 -> send / isSend=0 -> not send
   */
  int leftInputIndex; 
  int rightInputIndex; 
  int outputIndex; 
  int gateType; 
  int sendDataIndex;
  bool isSend;
};

class MPCCircuit {
 private:
  int numberOfRounds_; //total number of rounds of the circuit
  int numberOfGates_; //total number of gates of the circuit
  int numberOfInputs_;
  int numberOfOutputs_;
  int maxNumOfInternVal_; //maximum number of memorable internal value
  //The number of Wires in each rounds is smaller than the total number of wires in the circuit. The secret shared values corrsponding to each wire in each round is stored in this intermediate state registers. As we reused these registers for each round and prepare these registers for the maxnumber of required registers for all rounds at the begininng.

  std::vector<Gate> gate_; //array of Gate structure composing the circuit.
  std::vector<int> inputWireIndices_;
  std::vector<int> outputWireIndices_;

  MPCParty *party_;   //the party who communicate with other parties on behalf of this circuit. 
  int sliceLen_;      //length of sliced-shares
  int nSlice_;        //number of sliced-shares
  int nSendData_;     //maximum number of the body of (3-out-of-3) sliced-share in a round, which is to be sent for MPC-AND operation. 
  SlicedShare sendDataVector_; //buffer of sending data; sizeof(sendDataVector) = sliceLen_*nSendData;
  SlicedShare readDataVector_; //buffer of receiving data; sizeof(sendDataVector) = sliceLen_*nSendData;

 public:
  MPCCircuit(char *circuitFile, MPCParty& party, int sliceLen, int nSlice);//constructor
  ~MPCCircuit();//destructor
  void run(SlicedShare &inputShare, SlicedShare &outputShare);//main routine of MPC
};
  
#endif //MPCCIRCUIT_H
