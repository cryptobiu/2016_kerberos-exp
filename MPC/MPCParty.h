/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Copyritht (c) 2015-2016 Kazuma Ohara, NEC                            %
 * Update On 2016.02.05                                                 %
 * This file is part of the multi-party computation by NEC              %
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.        %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % MPCParty class                                                     %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * This class provides communication among the entities of MPC protocols. 
 * 
 * The constructor of MPCParty loads the information of parties(i.e., IP addresses and port numbers) from config file named "parties.conf". We assume that this program is for 3-party computation and each party is distinguished by server ID. 
 * The communications are established regardless of the order of invocation. 
 * 
 * In our MPC protocols, each party sends (receives) data to (from) only one of the other parties. 
 * That is, server 1 sends data to only server 2, and receives data from only server 3. Similarly, server 2 sends data to server 3 and receives data from server 1 (server 3 sends data to server 1 and receives from server 2).
 * The information written in parties.conf corresponds to the ID = 1,2,3 from top to bottom. 
 */

#ifndef MPCPARTY_H
#define MPCPARTY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>   //sockaddr_in
#include <arpa/inet.h>    //inet_addr
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "Share.h"

using namespace std;

class MPCParty{
 private:
  int partyID_;   //server ID
  int socketForListen_; //socket as a server
  int rightSocket_; //socket of the client
  int rightPort_;      //port number as the server

  //ServerID of the Rservaddr is serverID+1;
  int leftSocket_;
  int leftPort_;
  string leftPartyAddress_;

 public:
  MPCParty(int serverID);
  ~MPCParty();
  int Write(SlicedShare &slShare, int topartyID);  //Write function for sliced share-vector
  int Read(SlicedShare &slShare, int frompartyID); //Read function for sliced share-vector
  int getServerID(void);                           //accessor for serverID_;

  //following functions are used for peformance measurement. 
  int Write(unsigned long* data, int nData, int topartyID);  //for clock value
  int Write(unsigned long &data, int topartyID);             //for clock value
  int Read(unsigned long* data, int nData, int frompartyID); //for clock value
  int Read(unsigned long &data, int frompartyID);            //for clock value
};

#endif //MPCPARTY_H
