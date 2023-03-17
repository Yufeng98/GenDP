#include "data_buffer.h"
#include "comp_decoder.h"
#include "regfile.h"
#include "compute_unit_32.h"
#include "crossbar.h"

class pe {

    public:

        pe(int id);
        ~pe();

        int id;
        int PC[2], comp_PC;
        int src_dest[2][2];

        void run(int simd);

        int decode(unsigned long instruction, int* PC, int src_dest[], int* op, int simd);
        int load(int pos, int reg_immBar_flag, int rs1, int rs2, int simd);
        void store(int pos, int reg_immBar_flag, int rs1, int rs2, int data, int simd);
        void ctrl_instr_load_from_ddr(int addr, unsigned long data[]);
        int get_gr_10();
        void reset();

        void show_comp_reg();

        // ld/st data
        int load_data, store_data;
        unsigned long store_instruction[COMP_INSTR_BUFFER_GROUP_SIZE], load_instruction[COMP_INSTR_BUFFER_GROUP_SIZE];

    private:

        unsigned long instruction[2];
        // ld/st control signal
        int comp_reg_load, comp_reg_store, addr_reg_load, addr_reg_store, SPM_load, SPM_store;
        int comp_instr_load, comp_instr_store;
        // ld/st addr
        int comp_reg_load_addr, comp_reg_store_addr, addr_reg_load_addr, addr_reg_store_addr, SPM_load_addr, SPM_store_addr;
        int comp_instr_load_addr, comp_instr_store_addr;
        
        // TODO: Put conponents below to private later
        comp_instr_buffer *comp_instr_buffer_unit = new comp_instr_buffer(COMP_INSTR_BUFFER_GROUP_NUM);
        ctrl_instr_buffer *ctrl_instr_buffer_unit = new ctrl_instr_buffer(CTRL_INSTR_BUFFER_NUM);
        SPM *SPM_unit = new SPM(SPM_ADDR_NUM);
        addr_regfile *addr_regfile_unit = new addr_regfile(ADDR_REGISTER_NUM);
        comp_decoder comp_decoder_unit;
        regfile *regfile_unit = new regfile();
        compute_unit_32 cu_32;
        crossbar crossbar_unit;


};