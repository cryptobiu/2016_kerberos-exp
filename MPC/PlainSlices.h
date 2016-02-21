#ifndef PLAINSLICES_H
#define PLAINSLICES_H

class PlainSlices {
 public:
  unsigned int* data_; //the body of data
  int dataLen_;        //the length of each data (depends on the function)
  int nData_;          //the number of data (depends on the instruction set)
  int nBlocks_;        //the number of bit-sliced data units
  
  PlainSlices(PlainData &pldata);
  void addSlice(PlainSlices &plslice, PlainSlices &addslice);
  void deslice(PlainData &pldata);
  void concatinate(PlainData &pldata);
  void deconcatinate(PlainData &pldata);
  ~PlainSlices();
};

#endif //PLAINSLICES_H
