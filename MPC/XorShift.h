/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Copyritht (c) 2015-2016 Kazuma Ohara, NEC                            %
 * Update On 2016.02.05                                                 %
 * This file is part of the multi-party computation by NEC              %
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.        %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * % XorShift class                                            %
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * This class provides XorShift algorithm for pseudo random number generator. 
 * (We choose the algorithm tentatively for the experiment.)
 * 
 * In our MPC protocols, this is used for generating zero-sharing (shares of 0) in the AND-gate evaluation, and generating random input share for experiment. 
 * 
 */

#ifndef XORSHIFT_H
#define XORSHIFT_H

#include <climits>
#include <random>

class xorshift{
private:
    unsigned x=123456789u,y=362436069u,z=521288629u,w;
    unsigned random();
public:
    unsigned operator()();
    xorshift();
    xorshift(unsigned s);
};

#endif //XORSHIFT_H
