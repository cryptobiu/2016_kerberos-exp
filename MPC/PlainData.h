#ifndef PLAINDATA_H
#define PLAINDATA_H

class PlainData {
 public:
  unsigned int* data_; //body of the data
  int dataLen_;       //length of each data
  int nData_;        //number of the data

  PlainData(void);
  void Init(int dataLen, int nData);
  ~PlainData;
};

#endif //PLAINDATA_H
