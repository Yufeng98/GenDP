#include "sys_def.h"

class regfile {

    public:

        regfile();
        ~regfile();

        void reset();

        void write(int* write_addr, int* write_data, int n);
        void read(int* read_addr, int* read_data);
        void show_data(int addr);
        
        int *write_addr, *write_data;
        
        int *read_addr, *read_data;
        
    private:

        int *register_file;

};