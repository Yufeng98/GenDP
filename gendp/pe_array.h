#include "pe.h"
#include "FIFO.h"

typedef struct thread_data {
    int master;
    unsigned long instruction;
    int* PC;
    bool halt;
} thread_data;

class pe_array {

    public:

        pe_array(int input_size, int output_size);
        ~pe_array();
        
        int main_addressing_register[MAIN_ADDR_REGISTER_NUM];
        unsigned long main_instruction_buffer[CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
        unsigned long compute_instruction_buffer[COMP_INSTR_BUFFER_GROUP_NUM][COMP_INSTR_BUFFER_GROUP_SIZE];
        
        int main_PC;
        int input_buffer_size, output_buffer_size;

        void run(int cycle_limit, int simd, int setting, int main_instruction_setting);
        void show_gr();
        void show_compute_instruction_buffer();
        void poa_show_output_buffer(int len_y, int len_x, FILE* fp);
        void bsw_show_output_buffer(FILE* fp);
        void chain_show_output_buffer(int n, FILE* fp);
        void phmm_show_output_buffer(FILE* fp);

        void input_buffer_write_from_ddr(int addr, int* data);
        void input_buffer_write_from_ddr_unsigned(int addr, unsigned int* data);
        void compute_instruction_buffer_write_from_ddr(int addr, unsigned long data[]);
        void main_instruction_buffer_write_from_ddr(int addr, unsigned long data[]);
        void pe_instruction_buffer_write_from_ddr(int addr, unsigned long data[], int id);

        void buffer_reset(int* buffer, int num);

        int decode(unsigned long instruction, int* PC, int simd, int setting);
        int decode_output(unsigned long instruction, int* PC, int simd, int setting);

        int load(int source_pos, int reg_immBar_flag, int rs1, int rs2, int simd);
        void store(int dest_pos, int reg_immBar_flag, int rs1, int rs2, int data, int simd);
        unsigned long PE_instruction[2];
        int load_data, store_data, from_fifo;

        int *input_buffer, *output_buffer;
        FIFO fifo_unit[FIFO_GROUP_NUM][FIFO_GROUP_SIZE];
        pe *pe_unit[PE_NUM];

};
