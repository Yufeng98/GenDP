#include "sys_def.h"

class comp_decoder {

    public:

        comp_decoder();
        ~comp_decoder();

        void execute(long instruction, int* op, int* in_addr, int* out_addr, int* PC);

    private:

};