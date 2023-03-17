#include "sys_def.h"

// template <class T>
// class data_buffer {

//     public:

//         data_buffer(int size);
//         ~data_buffer();

//         void reset();

//         void write(int write_addr, T write_data);
//         void read(int read_addr, T read_data);
//         void show_data(int addr);

//         int write_addr, read_addr;
//         T write_data, read_data;

//         T *buffer;
//         int buffer_size;

// };

class addr_regfile {

    public:

        addr_regfile(int size);
        ~addr_regfile();

        void reset();

        void show_data(int addr);

        int *buffer;
        int buffer_size;

};

class SPM {

    public:

        SPM(int size);
        ~SPM();

        void reset();

        void show_data(int addr);

        int *buffer;
        int buffer_size;


};

class ctrl_instr_buffer {

    public:

        ctrl_instr_buffer(int size);
        ~ctrl_instr_buffer();

        void show_data(int addr);

        unsigned long **buffer;
        int buffer_size;

};

class comp_instr_buffer {

    public:

        comp_instr_buffer(int size);
        ~comp_instr_buffer();

        void show_data(int addr);

        unsigned long** buffer;
        int buffer_size;

};