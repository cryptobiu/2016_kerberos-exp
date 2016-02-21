/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Copyritht (c) 2015-2016 Kazuma Ohara, NEC                            %
 * Update On 2016.02.05                                                 %
 * This file is part of the multi-party computation by NEC              %
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.        %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % Share/SlicedShare class                                            %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * This file includes two classes: Share and SlicedShare. 
 * Share indicates secret shared value and SlicedShare indicates the bit-sliced value of shares. 
 * 
 * Both Share and SlicedShare include data and two parameters. 
 * Two parameters represent the number of (sliced-)share and length of (sliced-)share. 
 * These parameters can be set by Init function. 
 * 
 * sharegen/tripletgen functions are for experiment. 
 * These functions generates random shares and zero-sharing (shares of 0) by pseudo random generator. (We provisionally use XorShift algorithm (defined in XorShift.h) as pseudo random generator.) 
 */

#ifndef SHARE_H
#define SHARE_H
#include "XorShift.h"

typedef unsigned int block; //the unit of evaluation and communication

class Share {
 public:
  block* data_; //body of share
  int nData_;   //number of data
  int dataLen_; //length of data

  Share(void);
  ~Share(void);
  void Init(int nData, int dataLen);
  static void sharegen(block *v,block *x, block *y, block *z, block *a, block *b, block *c, int s); //generator of random share
  static void tripletgen(block *alpha, block *beta, block *gamma, int s); //generator of zero-sharing
};

class SlicedShare {
 public:
  block* data_;  //body of sliced-share
  int sliceLen_; //length of slice
  int nSlice_;   //number of slice
  
  SlicedShare(void);
  SlicedShare(int sliceLen, int nSlice);
  ~SlicedShare(void);
  void Init(int sliceLen, int nSlice);
  static void sharegen(block *v,block *x, block *y, block *z, block *a, block *b, block *c, int s); //generator of random share
  static void tripletgen(block *alpha, block *beta, block *gamma, int s); //generator of zero-sharing

  //following functions are only prototype declaration now. 
  void bitSlice(Share share, SlicedShare slshare);
  void bitDeSlice(SlicedShare slshare, Share share);
};

#endif //SHARE_H
