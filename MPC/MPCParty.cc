#include "MPCParty.h"

using namespace std;

MPCParty::MPCParty(int partyID){
  partyID_ = partyID;
  int SOCKET_TIMEOUT_SEC = 1;  //currently not adjustable
  int SOCKET_TIMEOUT_USEC = 0; //currently not adjutable
  FILE* fp;
  if ((fp = fopen("parties.conf","r")) == NULL) {
    cout<<"File parties.conf doesn't exist"<<endl;
  }
  cout<<"[main] file open"<<endl;
  
  char buf[100];
  int serverPorts[3];
  string serverAddr[3];
  for (int i=0; i<3; i++) {
    fgets(buf, sizeof(buf), fp);
    serverPorts[i] = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    serverAddr[i]  = buf;
  }
  fclose(fp);
  rightPort_ = serverPorts[partyID_-1]; 
  int leftClientID = partyID_ % 3;
  leftPort_     = serverPorts[leftClientID];  
  leftPartyAddress_ = serverAddr[leftClientID]; 
  cout<<"rightPort="    <<rightPort_<<endl;
  cout<<"leftPort="    <<leftPort_<<endl;
  cout<<"leftPartyAddress="<<leftPartyAddress_;

  //Create sockets
  socketForListen_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socketForListen_<0) {
    cout<<"[CreateSocket] server socket failed"<<endl;
    perror("CreateSocket");
  }
  else {
    cout<<"[CreateSocket] Create Server Socket"<<endl;
  }

  leftSocket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (leftSocket_<0) {
    cout<<"[CreateSocket] client socket failed"<<endl;
    perror("CreateSocket");
  }
  else {
    cout<<"[CreateSocket] Create Client Socket"<<endl;
  }
  
  //Get Server and Client Information
  sockaddr_in rightParty;
  rightParty.sin_family      = AF_INET;
  rightParty.sin_port        = htons(rightPort_);
  rightParty.sin_addr.s_addr = INADDR_ANY; //all IP address can be connected

  sockaddr_in leftParty;
  leftParty.sin_family      = AF_INET;
  leftParty.sin_port        = htons(leftPort_);
  leftParty.sin_addr.s_addr = inet_addr(leftPartyAddress_.c_str());
  
  struct timeval timeout;
  
  //Change socket options for the server
  int yes=1;
  setsockopt(socketForListen_,SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
  
  //bind address to server socket
  int statBind=bind(socketForListen_, (struct sockaddr *)&rightParty, sizeof(rightParty));
  if (statBind!=0) {
    cout<<"[Init] Bind Failed:"<<statBind<<endl;
    perror("Bind");
  }
  
  int statListen=listen(socketForListen_,5);  //second argument is the maximum number of client waiting connection
  if(statListen!=0) {
    cout<<"[Init] Listen Failed:"<<statListen<<endl;
    perror("Listen");
  }
  
  int ConResult=-1;
  int AccResult=-1;
  fd_set readfds; //file descriptor set
  
  //establish connections
  while ((ConResult<0) || (AccResult<=0)) {
    if (ConResult<0) {
      ConResult=connect(leftSocket_,(struct sockaddr *)&leftParty, sizeof(leftParty)); //try to connect to server which receives data from this server
    }
    if (AccResult<=0) {
      FD_ZERO(&readfds);            //reset file descriptor set
      FD_SET(socketForListen_,&readfds); //add server socket to file descriptor set
      timeout.tv_sec  = SOCKET_TIMEOUT_SEC;   //2
      timeout.tv_usec = SOCKET_TIMEOUT_USEC;  //0
      AccResult=select(socketForListen_+1, &readfds, NULL, NULL, &timeout); 
      //Note: when the socket in readfds receive the request of connection, select function return the number of request. 
      if (AccResult>0) {
	unsigned int len=sizeof(rightParty);
	rightSocket_ = accept(socketForListen_, (struct sockaddr *)&rightParty, &len); //wait connection from the server which sends data to this server
	AccResult = 1;
      }
    }
  }
}

MPCParty::~MPCParty() {
  close(socketForListen_);
  close(rightSocket_);
  close(leftSocket_);
}

int MPCParty::Write(SlicedShare &slShare, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
    nWrite = write(leftSocket_,slShare.data_,slShare.sliceLen_*slShare.nSlice_*sizeof(unsigned int));
  }
  else if (IDdiff == 2) {
    nWrite = write(rightSocket_,slShare.data_,slShare.sliceLen_*slShare.nSlice_*sizeof(unsigned int));
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}

int MPCParty::Read(SlicedShare &slShare, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

  if (IDdiff == 1) {
    int frame = slShare.sliceLen_*slShare.nSlice_*sizeof(unsigned int);
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)slShare.data_+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 2) {
    int frame = slShare.sliceLen_*slShare.nSlice_*sizeof(unsigned int);
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)slShare.data_+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return loaded; 
}

int MPCParty::Write(unsigned long* data, int nData, int topartyID){
  int nWrite=0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }
  if (IDdiff == 1) {
    nWrite = write(leftSocket_,data,nData*sizeof(unsigned long));
  }
  if (IDdiff == 2) {
    nWrite = write(rightSocket_,data,nData*sizeof(unsigned long));
  }
  if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}


int MPCParty::Write(unsigned long &data, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }
  if (IDdiff == 1) {
    nWrite = write(leftSocket_,&data,sizeof(unsigned long));
  }
  if (IDdiff == 2) {
    nWrite = write(rightSocket_,&data,sizeof(unsigned long));
  }
  if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}

int MPCParty::Read(unsigned long* data, int nData, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int nRead  = 0;
  int loaded = 0;

  if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }
  if (IDdiff == 1) {
    int frame = sizeof(unsigned long)*nData;
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  if (IDdiff == 2) {
    int frame = sizeof(unsigned long)*nData;
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return loaded; 
}

int MPCParty::Read(unsigned long &data, int frompartyID){
  int nRead  = 0;
  int IDdiff = (frompartyID - partyID_+3) % 3;

  if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }
  if (IDdiff == 1) {
    nRead = read(leftSocket_,&data,sizeof(unsigned int));
  }
  if (IDdiff == 2) {
    nRead = read(rightSocket_,&data,sizeof(unsigned int));
  }
  if(nRead<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return nRead; 
}

int MPCParty::getServerID(void) {
  return partyID_;
}
