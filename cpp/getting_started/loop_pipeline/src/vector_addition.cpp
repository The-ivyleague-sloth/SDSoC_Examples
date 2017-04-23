/*********** 
# Copyright (c) 2017, Xilinx, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.
#
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY 
# DIRECT, INDIRECT,INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO,PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# 
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
************/

/**********************************************************************************
   Loop Pipelining

   Pipelining is a form of parallelism that allows the FPGA to reuse the
   hardware for the next instruction before the current instruction has finished
   processing. This maximizes the utilization of the FPGA fabric and allows the
   processing of multiple elements of data at the same time.

   The sds++ compiler automatically performs this optimization on loops so no
   user intervention is required for most cases. The outer most loops will be
   pipelined and inner loops will be unrolled if possible.

   In this example we will demonstrate ways to improve the throughput of a
   vector addition hardware function using the HLS PIPELINE attribute.
************************************************************************************/

#define N 128
#include "vector_addition.h"

const int DATA_SIZE = 1<<10;

// This hardware function is optimized to access only one global variable in a pipelined
// loop. This will improve the II and increase throughput of the hardware function.
void vadd_pipelined_accel(int a[DATA_SIZE],
                          int b[DATA_SIZE],
                          int c[DATA_SIZE],
                          const int len)
{
    int result[N];
    int iterations = len / N;

    // Default behavior of sds++ will pipeline the outer loop. Since we have
    // multiple inner loops, the pipelining will fail. We can instead pipeline
    // the inner loops using the HLS PIPELINE attribute to guide the
    // compiler.
    for(int i = 0; i < iterations; i++) {

        // Pipelining loops that access only one variable is the ideal way to
        // increase the DDR memory bandwidth.
        read_a:
        for (int x=0; x<N; ++x) {
        #pragma HLS PIPELINE
            result[x] = a[i*N+x];
        }
        read_b:
        for (int x=0; x<N; ++x) {
        #pragma HLS PIPELINE
            result[x] += b[i*N+x];
        }
        write:
        for (int x=0; x<N; ++x) {
        #pragma HLS PIPELINE
            c[i*N+x] = result[x];
        }
    }
}
