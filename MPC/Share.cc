#include "Share.h"

void SlicedShare::sharegen(block *v,block *x, block *y, block *z, block *a, block *b, block *c, int s){
  xorshift xs(s);

  // generate random value v 
  *v = xs()%UINT_MAX;
  // generate random x,y,z such that x^y^z =0
  *x = xs()%UINT_MAX;
  *y = xs()%UINT_MAX;
  *z = *x^*y;
  //compute masked value v
  *a = *z^*v;
  *b = *x^*v;
  *c = *y^*v;
}

void SlicedShare::tripletgen(block *alpha, block *beta, block *gamma, int s){
  xorshift xs(s);
  // generate random alpha,beta,gamma such that alpha^beta^gamma = 0

  *alpha = xs()%UINT_MAX;
  *beta = xs()%UINT_MAX;
  *gamma = *alpha^*beta;
}

SlicedShare::SlicedShare(void) {
  data_ = NULL;
  sliceLen_ = 0;
  nSlice_ = 0;
}

void SlicedShare::Init(int sliceLen, int nSlice) {
  sliceLen_ = sliceLen;
  nSlice_ = nSlice;
  data_ = new block[sliceLen_*nSlice_];
}

SlicedShare::~SlicedShare(void) {
  delete[] data_;
}
