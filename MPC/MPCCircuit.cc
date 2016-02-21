#include "MPCCircuit.h"
#include <immintrin.h>
#define NO_VEC_OPT


//ABARAK added to support chrono measurement
#include <chrono>
#include <ctime>
#define NDEBUG
//#define NO_VEC_OPT
#define ABARAK_NO_CLK_OUTPUT
#include <cassert>
using mpc_chrono = std::chrono::time_point<std::chrono::high_resolution_clock>;
//ABARAK END


MPCCircuit::MPCCircuit(char *filename, MPCParty& party, int sliceLen, int nSlice) {
  party_    = &party;
  sliceLen_ = sliceLen;
  nSlice_   = nSlice;

  FILE *fp;
  int ret;
  int MAX_BUF_SIZE = 65536;

  char buf[MAX_BUF_SIZE];

  //open file
  if ((fp = fopen(filename, "r")) == NULL) {
    cout<<"file open error"<<endl;
    exit(EXIT_FAILURE);
  }

  //read the number of rounds
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &numberOfRounds_);
#ifdef DEBUG
  cout<<"Round:"<<numberOfRounds_<<endl;
#endif//DEBUG

  //read the number of gates
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &numberOfGates_);
#ifdef DEBUG
  cout<<"Number of gates:"<<numberOfGates_<<endl;
#endif //DEBUG

  //read the number of wires
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &maxNumOfInternVal_);
#ifdef DEBUG
  cout<<"Number of wires:"<<maxNumOfInternVal_<<endl;
#endif //DEBUG

  //read the maximum number of sending sliced-shares per round
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &nSendData_);
#ifdef DEBUG
  cout<<"Number of sending data:"<<nSendData_<<endl;
#endif //DEBUG

  //read the number of inputs;
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &numberOfInputs_);
#ifdef DEBUG
  cout<<"Number of inputs:"<<numberOfInputs_<<endl;
#endif //DEBUG


  //read the number of outputs;
  fgets(buf, sizeof(buf), fp);
  sscanf(buf,"%d", &numberOfOutputs_);
#ifdef DEBUG
  cout<<"Number of outputs:"<<numberOfOutputs_<<endl;
#endif //DEBUG

  //read the indices of input wires
  inputWireIndices_.resize(numberOfInputs_);
  for (int i=0; i<numberOfInputs_; i++) {
    fgets(buf, sizeof(buf), fp);
    sscanf(buf,"%d", &inputWireIndices_[i]);
#ifdef DEBUG
    cout<<"Number of input wire "<<i<<":"<<inputWireIndices_[i]<<endl;
#endif //DEBUG
  }

  //read the indices of output wires
  outputWireIndices_.resize(numberOfOutputs_);
  for (int i=0; i<numberOfOutputs_; i++) {
    fgets(buf, sizeof(buf), fp);
    sscanf(buf,"%d", &outputWireIndices_[i]);
#ifdef DEBUG
    cout<<"Number of output wire "<<i<<":"<<outputWireIndices_[i]<<endl;
#endif //DEBUG
  }

  //read the gates;
  int rnd = 0;
  int bufAddr;
  int ctrAddr = 0;
  gate_.resize(numberOfGates_+numberOfRounds_);
  for (int i=0; i<(numberOfGates_+numberOfRounds_); i++) {
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%d %d %d %d %d", &gate_[i].leftInputIndex, &gate_[i].rightInputIndex, &gate_[i].outputIndex, &gate_[i].gateType, &bufAddr);
    if (bufAddr != -1) {
      gate_[i].sendDataIndex = bufAddr;
      gate_[i].isSend        = true;
    }
    else if (bufAddr == -1) {
      gate_[i].isSend = false;
    }

    if (gate_[i].gateType == CHRnd) { rnd++; }
#ifdef DEBUG
    cout<<"Gate "<<i<<": "<<"("<<gate_[i].leftInputIndex<<","<<gate_[i].rightInputIndex<<","<<gate_[i].outputIndex<<","<<gate_[i].gateType<<")"<<endl;
#endif //DEBUG
  }
  fclose(fp);

  sendDataVector_.Init(sliceLen_, nSendData_);
  readDataVector_.Init(sliceLen_, nSendData_);
}


MPCCircuit::~MPCCircuit() {
  cout<<"Circuit Evaluation has been finished. "<<endl;
}

static void XOR(block in1[], block in2[], block out[], int sliceLen){

#ifdef NO_VEC_OPT
for (int i=0; i<sliceLen; i++){
    out[i] = in1[i] ^ in2[i];
 }
#else

    const unsigned int u = 0xFFFFFFFF;
    __m256 in1_vec, in2_vec, out_vec;
    __m256i ALL_ONES_256 = _mm256_set_epi32(u,u,u,u,u,u,u,u);
    __m256i *output_as_int_vec;

    float *fin1 = (float *)in1;
    float *fin2 = (float *)in2;
    float *fout = (float *)out;


    for (int i=0; i<sliceLen; i+=8){
	in1_vec = _mm256_load_ps(fin1 + i);
        in2_vec = _mm256_load_ps(fin2 + i);
        out_vec = _mm256_xor_ps(in1_vec, in2_vec);
        output_as_int_vec = (__m256i* ) &out_vec;
	_mm256_maskstore_epi32((int *)fout,ALL_ONES_256,(*output_as_int_vec));

    }

#endif
}

static void local_AND(block in1[], block in2[], block in3[], block out[], int sliceLen) {

#ifdef NO_VEC_OPT
for (int i=0; i<2*sliceLen; i++){
    out[i] = in1[i] & in2[i];
 }
#else
    const unsigned int u = 0xFFFFFFFF;
    __m256 in1_vec, in2_vec, out_vec;
    __m256i ALL_ONES_256 = _mm256_set_epi32(u,u,u,u,u,u,u,u);
    __m256i *output_as_int_vec;

    float *fin1 = (float *)in1;
    float *fin2 = (float *)in2;
    float *fout = (float *)out;


    for (int i=0; i<2*sliceLen; i+=8){
	in1_vec = _mm256_load_ps(fin1 + i);
        in2_vec = _mm256_load_ps(fin2 + i);
        out_vec = _mm256_xor_ps(in1_vec, in2_vec);
        output_as_int_vec = (__m256i* ) &out_vec;
	_mm256_maskstore_epi32((int *)fout + i,ALL_ONES_256,(*output_as_int_vec));

    }

#endif

 for (int i =0; i<sliceLen; i++){
   out[i+sliceLen] = out[i]^out[i+sliceLen]^in3[i];
 }


}

void MPCCircuit::run(SlicedShare &inputShare, SlicedShare &outputShare) {
  int rnd = 0; //round pointer
  int i   = 0; //gate pointer
  unsigned int alpha,beta,gamma;
  int s   =111;
  cout<<"[MPC.run] START"<<endl;
  block** value_ = new block*[maxNumOfInternVal_];
  block triplets_[sliceLen_];



  //ABARAK Start
  auto start_time = std::chrono::high_resolution_clock::now();
  auto total_comm_time = (start_time - start_time);


  unsigned long clk0;
  unsigned long clk1[numberOfRounds_];
  unsigned long clk2[numberOfRounds_];
  unsigned long clk3[numberOfRounds_];
  unsigned long clk4[numberOfRounds_];
  unsigned long clk5[numberOfRounds_];
  unsigned long clk6[numberOfRounds_];
  unsigned long clk7[numberOfRounds_];

  for(int i=0; i<maxNumOfInternVal_; i++){
    value_[i] = new block[2*sliceLen_];
  }

#ifdef DEBUG
  cout<<"Memory for storing intermal state for each wires are initialized. "<<endl;
#endif //DEBUG

  for(int i =0; i < numberOfInputs_;i++){
    int idx  = inputWireIndices_[i];
    for(int j=0;j<2*sliceLen_;j++){
      value_[idx][j] = inputShare.data_[i*sliceLen_+j];
    }
  }

#ifdef DEBUG
  cout<<"[MPC.run] Shares are stored."<<endl;
#endif //DEBUG

  clk0 = __rdtsc();
  int sendWire[nSendData_];

  while (rnd < numberOfRounds_) {
#ifndef ONLY_COMM
    int sendCtr = 0; //number of send data

    while (gate_[i].gateType != CHRnd) {
      int a = gate_[i].leftInputIndex;
      int b = gate_[i].rightInputIndex;
      int c = gate_[i].outputIndex;
      if (gate_[i].gateType == XORGate) {

	XOR(value_[a], value_[b], value_[c], sliceLen_);//[XOR Gate] Only local computation

      } // endif: XOR
      else if (gate_[i].gateType == ANDGate){
	//generate zero-sharing

        //ABARAK move party id out of loop
	int partyID = party_->getServerID();

	for(int j=0;j<sliceLen_;j++){
	  if (partyID == 1) {
	    SlicedShare::tripletgen(&triplets_[j],&beta,&gamma,s+i+j);
	  }
	  else if (partyID == 2) {
	    SlicedShare::tripletgen(&alpha,&triplets_[j],&gamma,s+i+j);
	  }
	  else if (partyID == 3) {
	    SlicedShare::tripletgen(&alpha,&beta,&triplets_[j],s+i+j);
	  }
	}



	//[AND Gate] Step(1): local computation
	local_AND(value_[a], value_[b], triplets_,value_[c], sliceLen_);

      } //end if: AND

      //[AND Gate] Step(2)-1: prepare communication value

      if (gate_[i].isSend) {
	for (int ic=0; ic<sliceLen_; ic++) {
	  int addr = gate_[i].sendDataIndex;
	  sendDataVector_.data_[sliceLen_*addr+ic] = value_[c][ic+sliceLen_];
	  sendWire[sendCtr] = c;
	}
	sendCtr++;
      }

      i++; //see next gate
    } // end while(gate)


    i++; //go to next gate of (0,0,0,CHR,0)
#endif //ONLY_COMM

    //[AND Gate] Step(2)-2: Communication

    //ABARAK
    auto round_start_comm_step_time = std::chrono::high_resolution_clock::now();

    if (sendCtr != 0) {//If sendDataVector_ is empty in the final round, exit;
      if ((party_->getServerID()) == 1) {
	clk1[rnd]  = __rdtsc();
	int nWrite = party_->Write(sendDataVector_, 2);
	cout<<"[Server A]nWrite: "<<nWrite<<endl;
	clk2[rnd]  = __rdtsc();
	int nRead  = party_->Read(readDataVector_, 3);
	cout<<"[Server A]nRead:  "<<nRead<<endl;
	clk3[rnd]  = __rdtsc();
      }
      else if ((party_->getServerID()) == 2) {
	int nRead = party_->Read(readDataVector_, 1);
	cout<<"[Server B]nRead:  "<<nRead<<endl;
	clk4[rnd]  = __rdtsc();
	int nWrite = party_->Write(sendDataVector_, 3);
	cout<<"[Server B]nWrite: "<<nWrite<<endl;
	clk5[rnd]  = __rdtsc();
      }
      else if ((party_->getServerID()) == 3) {
	int nRead = party_->Read(readDataVector_, 2);
	cout<<"[Server C]nRead:  "<<nRead<<endl;
	clk6[rnd]  = __rdtsc();
	int nWrite = party_->Write(sendDataVector_, 1);
	cout<<"[Server C]nWrite: "<<nWrite<<endl;
	clk7[rnd]  = __rdtsc();
      }
    } //end of communication

//ABARAK
    auto round_end_comm_step_time = std::chrono::high_resolution_clock::now();
    auto round_comm_time = round_end_comm_step_time - round_start_comm_step_time;
    total_comm_time = total_comm_time + round_comm_time;

//ABARAK END



    //[AND Gate] Step(3) define sharing
#ifndef ONLY_COMM
    for (int i=0; i<nSendData_; i++){ // number of AND per each gate
      for (int j=0; j<sliceLen_; j++){
	value_[sendWire[i]][j] = readDataVector_.data_[i*sliceLen_+j] ^ value_[sendWire[i]][j+sliceLen_];
      }
    }
#endif //ONLY_COMM

    rnd++;
  }// end while(round)

//ABARAK
  auto end_time = std::chrono::high_resolution_clock::now();
  cout << "ABARAK\n";
  cout << "TOTAL RUN TIME\n";
  cout << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << ":";
  cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << ":";

  cout << "\nTOTAL COMM_TIME TIME\n";
  cout << chrono::duration_cast<chrono::seconds>(total_comm_time).count() << ":";
  cout << chrono::duration_cast<chrono::microseconds>(total_comm_time).count() << ":";
  cout << "\nABARAK\n";
//ABARAK end

  //send clock value to server 1
  if ((party_->getServerID()) == 1) {
    unsigned long clk0_b, clk0_c;
    party_->Read(clk4, numberOfRounds_,2);
    party_->Read(clk5, numberOfRounds_,2);
    party_->Read(clk0_b, 2);
    party_->Read(clk6, numberOfRounds_,3);
    party_->Read(clk7, numberOfRounds_,3);
    party_->Read(clk0_c,3);
    unsigned long clkdiff_a_b = clk0_b-clk0;
    unsigned long clkdiff_b_c = clk0_c-clk0_b;
    unsigned long clkdiff_c_a = clk0_c-clk0;
    cout<<"clk0_a:"      <<clk0<<endl;
    cout<<"clk0_b:"      <<clk0_b<<endl;
    cout<<"clk0_c:"      <<clk0_c<<endl;
    //cout<<"clkdiff_a_b: "<<clkdiff_a_b<<endl;
    //cout<<"clkdiff_b_c: "<<clkdiff_b_c<<endl;
    //cout<<"clkdiff_c_a: "<<clkdiff_c_a<<endl;
    unsigned long time_1_3 = 0;
    unsigned long time_2_3 = 0;
    unsigned long time_1_2 = 0;
    unsigned long time_4_5 = 0;
    unsigned long time_6_7 = 0;
    unsigned long time_3_1 = 0;
    unsigned long time_1_1 = 0;

    for (int i=0; i<numberOfRounds_; i++) {
      /*
      cout<<"[Round "    <<i+1<<"]"<<endl;
      cout<<"clk 1: "    <<clk1[i]<<endl;
      cout<<"clk 2: "    <<clk2[i]<<endl;
      cout<<"clk 3: "    <<clk3[i]<<endl;
      cout<<"clk 4: "    <<clk4[i]<<endl;
      cout<<"clk 5: "    <<clk5[i]<<endl;
      cout<<"clk 6: "    <<clk6[i]<<endl;
      cout<<"clk 7: "    <<clk7[i]<<endl;
      */
      /*
      cout<<"Interval [3-1]: "<<(clk3[i]-clk1[i])/1899<<endl;
      cout<<"Interval [3-2]: "<<(clk3[i]-clk2[i])/1899<<endl;
      cout<<"Interval [2-1]: "<<(clk2[i]-clk1[i])/1899<<endl;
      cout<<"Interval [5-4]: "<<(clk5[i]-clk4[i])/1899<<endl;
      cout<<"Interval [7-6]: "<<(clk7[i]-clk6[i])/1899<<endl;
      */
      time_1_3 += (clk3[i]-clk1[i])/1899;
      time_2_3 += (clk3[i]-clk2[i])/1899;
      time_1_2 += (clk2[i]-clk1[i])/1899;
      time_4_5 += (clk5[i]-clk4[i])/1899;
      time_6_7 += (clk7[i]-clk6[i])/1899;
      if (i != 0) {
	time_3_1 += (clk1[i]-clk3[i-1])/1899;
	time_1_1 += (clk1[i]-clk1[i-1])/1899;
      }
    }
    ofstream ofs("Experiment.txt", std::ios::out | std::ios::app);
    ofs << "Length of Slice = "<< sliceLen_ << endl;
    ofs << "Number of Slice = "<< nSlice_ << endl;
    ofs << "Time of Interval 1-3 : " << time_1_3/(numberOfRounds_) << endl;
    ofs << "Time of Interval 2-3 : " << time_2_3/(numberOfRounds_) << endl;
    ofs << "Time of Interval 1-2 : " << time_1_2/(numberOfRounds_) << endl;
    ofs << "Time of Interval 4-5 : " << time_4_5/(numberOfRounds_) << endl;
    ofs << "Time of Interval 6-7 : " << time_6_7/(numberOfRounds_) << endl;
    ofs << "Time of Interval 3-1 : " << time_3_1/(numberOfRounds_-1) << endl;
    ofs << "Time of Interval 1-1 : " << time_1_1/(numberOfRounds_-1) << endl;
  }
  else if ((party_->getServerID()) == 2) {
    party_->Write(clk4, numberOfRounds_,1);
    party_->Write(clk5, numberOfRounds_,1);
    party_->Write(clk0,1);
  }
  else if ((party_->getServerID()) == 3) {
    party_->Write(clk6, numberOfRounds_,1);
    party_->Write(clk7, numberOfRounds_,1);
    party_->Write(clk0,1);
  }

  // prepare output data
  for (int i=0; i<numberOfOutputs_; i++) {
    int idx = outputWireIndices_[i];
    for (int j=0; j<2*sliceLen_; j++) {
      outputShare.data_[i*2*sliceLen_+j] = value_[idx][j];
    }
#ifdef DEBUG
    cout<<"outputShare["<<i<<"]: ";
    for (int j=0; j<2*sliceLen_; j++){
      cout<<outputShare.data_[i*2*sliceLen_+j];
    }
    cout<<endl;
#endif //DEBUG
  }

  cout<<"[MPC.run] END"<<endl;

  for (int i=0; i<maxNumOfInternVal_; i++) {
    delete[] value_[i];
  }
  delete[] value_;
}
