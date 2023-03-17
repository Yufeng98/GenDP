#include "sys_def.h"

class crossbar {

    public:

        crossbar();
        ~crossbar();

        void set(int* in, int* sel, int* out);

        void execute();

    private:

        int *in, *sel, *out;

};