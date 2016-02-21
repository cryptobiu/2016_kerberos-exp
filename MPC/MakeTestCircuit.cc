#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int main(void) {
  FILE* fp;
  int MAX_SIZE_BUF = 256;
  char buf[MAX_SIZE_BUF];

  fp = fopen("TestCircuit.txt","w");
  
  cout<<"Number of Rounds?: ";
  fgets(buf, sizeof(buf), stdin);
  fprintf(fp,"%d\n", atoi(buf));//number of rounds
  int numberOfRounds = atoi(buf);
  cout<<"Number of AND Gates per round? :";
  fgets(buf,sizeof(buf), stdin);
  int numberOfGates = 3*atoi(buf)*numberOfRounds;
  fprintf(fp, "%d\n", numberOfGates);//number of gates
  int numberOfANDGates = atoi(buf);
  int numberOfWires = 3*numberOfANDGates;
  int numberOfSendData = numberOfANDGates;
  int numberOfInputs = 2*numberOfANDGates;
  int numberOfOutputs = 2*numberOfANDGates;
  fprintf(fp, "%d\n", numberOfWires); //number of wires
  fprintf(fp, "%d\n", numberOfSendData);
  fprintf(fp, "%d\n", numberOfInputs); //numberOfInputs
  fprintf(fp, "%d\n", numberOfOutputs); //numberOfOutputs
  int ctrin=0;
  //input wire indices
  for (int i=0; i<numberOfInputs; i++) {
    fprintf(fp, "%d\n", ctrin++);
  }
  //output wire indices
  int ctrout=0;
  for (int i=0; i<numberOfInputs; i++) {
    fprintf(fp, "%d\n", ctrout++);
  }

  //gate
  int ctrwire = 0;
  int ctrmidwire=numberOfInputs;
  
  for (int i=0; i<numberOfRounds; i++){
    for (int j=0; j<numberOfANDGates; j++) {
      fprintf(fp, "%d %d %d %d %d\n", 2*j, 2*j+1, numberOfANDGates+j, 1, -1);
      fprintf(fp, "%d %d %d %d %d\n", 2*j, 2*j+1, 2*j, 1, -1);
      fprintf(fp, "%d %d %d %d %d\n", 2*j, numberOfANDGates+j, 2*j+1, 2, j);
    }
    fprintf(fp, "%d %d %d %d %d\n",0,0,0,0);
  }

  cout<<"Number of Input Wires = "<<numberOfInputs<<endl;
  
}
