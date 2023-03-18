#ifndef HOST_KERNEL_H
#define HOST_KERNEL_H

#include "host_data.h"
#include "compute_unit_32.h"
#include "comp_decoder.h"

void host_chain_kernel(std::vector<call_t> &arg, std::vector<return_t> &ret, int numThreads, int setting);

void execute_instrution(int id, long instruction, comp_decoder* decoder_unit, compute_unit_32* cu, int* regfile);


#endif // HOST_KERNEL_H
