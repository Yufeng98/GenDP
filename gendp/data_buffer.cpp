#include "data_buffer.h"

// template <typename T>
// data_buffer<T>::data_buffer(int size) {

//     buffer = (T*)malloc(size * sizeof(T));
//     buffer_size = size;
//     reset();

// }

// template <typename T>
// data_buffer<T>::~data_buffer() {
//     free(buffer);
// }

// template <typename T>
// void data_buffer<T>::reset() {

//     int i;

//     for (i = 0; i < buffer_size; i++)
//         buffer[i] = 0;
// }

// template <typename T>
// void data_buffer<T>::write(int write_addr, T write_data) {

//     if (write_addr >= 0 && write_addr < buffer_size)
//         buffer[write_addr] = write_data;
//     else fprintf(stderr, "data_buffer write addr error.\n");

// }

// template <typename T>
// void data_buffer<T>::read(int read_addr, T read_data) {

//     if (read_addr >= 0 && read_addr < buffer_size)
//         read_data = buffer[read_addr];
//     else fprintf(stderr, "data_buffer read addr error.\n");

// }

addr_regfile::addr_regfile(int size) {
    buffer = (int*)malloc(size * sizeof(int));
    buffer_size = size;
    reset();
}

addr_regfile::~addr_regfile() {
    free(buffer);
}

void addr_regfile::reset() {
    int i;
    for (i = 0; i < buffer_size; i++)
        buffer[i] = 0;
}

void addr_regfile::show_data(int addr) {
    if (addr >= 0 && addr < buffer_size) {
        printf("addr_regfile[%d] = %d\n", addr, buffer[addr]);
    } else fprintf(stderr, "addr_regfile show data addr error.\n");
}

SPM::SPM(int size) {
    buffer = (int*)malloc(size * sizeof(int));
    buffer_size = size;
    reset();
}

SPM::~SPM() {
    free(buffer);
}

void SPM::reset() {
    int i;
    for (i = 0; i < buffer_size; i++)
        buffer[i] = 0;
}

void SPM::show_data(int addr) {
    if (addr >= 0 && addr < buffer_size) {
        printf("SPM[%d] = %d\n", addr, buffer[addr]);
    } else fprintf(stderr, "SPM show data addr error.\n");
}

ctrl_instr_buffer::ctrl_instr_buffer(int size) {
    int i;
    buffer = (unsigned long**)malloc(size * sizeof(unsigned long*));
    for (i = 0; i < size; i++) {
        buffer[i] = (unsigned long*)malloc(CTRL_INSTR_BUFFER_GROUP_SIZE * sizeof(unsigned long));
        buffer[i][0] = 0xf;
        buffer[i][1] = 0xf;
    }
        
    buffer_size = size;
}

ctrl_instr_buffer::~ctrl_instr_buffer() {
    int i;
    for (i = 0; i < buffer_size; i++)
        free(buffer[i]);
    free(buffer);
}

void ctrl_instr_buffer::show_data(int addr) {
    if (addr >= 0 && addr < COMP_INSTR_BUFFER_GROUP_NUM) {
        printf("ctrl_instr_buffer[%d][0] = %lx\t", addr, buffer[addr][0]);
        printf("ctrl_instr_buffer[%d][1] = %lx\n", addr, buffer[addr][1]);
    } else fprintf(stderr, "ctrl_instr_buffer show data addr error.\n");
}

comp_instr_buffer::comp_instr_buffer(int size) {
    int i;
    buffer = (unsigned long**)malloc(size * sizeof(unsigned long*));
    for (i = 0; i < size; i++) {
        buffer[i] = (unsigned long*)malloc(COMP_INSTR_BUFFER_GROUP_SIZE * sizeof(unsigned long));
        buffer[i][0] = 0x20f7800000000;
        buffer[i][1] = 0x20f7800000000;
    }
        
    buffer_size = size;
}

comp_instr_buffer::~comp_instr_buffer() {
    int i;
    for (i = 0; i < buffer_size; i++)
        free(buffer[i]);
    free(buffer);
}

void comp_instr_buffer::show_data(int addr) {
    if (addr >= 0 && addr < COMP_INSTR_BUFFER_GROUP_NUM) {
        printf("comp_instr_buffer[%d][0] = %lx\t", addr, buffer[addr][0]);
        printf("comp_instr_buffer[%d][1] = %lx\n", addr, buffer[addr][1]);
    } else fprintf(stderr, "comp_instr_buffer show data addr error.\n");
}
