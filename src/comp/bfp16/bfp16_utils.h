/*
 Copyright 2016-2020 Intel Corporation
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/
#pragma once

inline int __attribute__((__always_inline__))
ccl_bfp16_is_enabled()
{
#ifdef CCL_BFP16_COMPILER
    int is_avx512f_enabled = 0, is_avx512bf_enabled = 0;

    uint32_t reg[4];

    /* baseline AVX512 capabilities used for CCL/BFP16 implementation */
    /* CPUID.(EAX=07H, ECX=0):EBX.AVX512F  [bit 16] */
    /* CPUID.(EAX=07H, ECX=0):EBX.AVX512BW [bit 30] */
    /* CPUID.(EAX=07H, ECX=0):EBX.AVX512VL [bit 31] */
    __asm__ __volatile__ ("cpuid" :
                          "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3])  :
                          "a" (7), "c" (0));
    is_avx512f_enabled = (( reg[1] & (1 << 16) ) >> 16) &
                         (( reg[1] & (1 << 30) ) >> 30) &
                         (( reg[1] & (1 << 31) ) >> 31);

    /* capabilities for optimized BFP16/FP32 conversions */
    /* CPUID.(EAX=07H, ECX=1):EAX[bit 05] */
    __asm__ __volatile__ ("cpuid" :
                          "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3]) :
                          "a" (7), "c" (1));
    is_avx512bf_enabled = ( reg[0] & (1 << 5) ) >> 5; /* not used currently */

    return (is_avx512f_enabled) ? 1 : 0;
#else
    return 0;
#endif
}
