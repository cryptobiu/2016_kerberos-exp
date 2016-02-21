#include "XorShift.h"

static constexpr unsigned min() { return 0u; } //min. of random value

static constexpr unsigned max() { return UINT_MAX; }//max. of random value

unsigned xorshift::operator()() { return random(); }

xorshift::xorshift(){
    std::random_device rd; 
    w=rd();
    for(int i=0;i<10;i++){
      xorshift::random();
    }
}

xorshift::xorshift(unsigned s) { w=s; }

unsigned xorshift::random(){
    unsigned t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
