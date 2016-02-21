#ifndef SHAREDSLICES_H
#define SHAREDSLICES_H

class SharedSlices {
 public:
  unsigned int* data_;
  int sliceLen_;
  int nSlice_;
  
  SharedSlices(void);
  SharedSlices(PlainSlices &plslices);
  void Init(int sliceLen, int nSlice);
  ~SharedSlices();

  //The following functions are tentatively defined in this class
  static void sharegen(block *v,block *x, block *y, block *z, block *a, block *b, block *c, int s); //generator of random share
  static void tripletgen(block *alpha, block *beta, block *gamma, int s); //generator of zero-sharing
};


#endif //SHAREDSLICES_H
