#include "crossbar.h"

crossbar::crossbar() {

    in = (int*)malloc(CROSSBAR_IN_NUM * sizeof(int));
    sel = (int*)malloc(CROSSBAR_OUT_NUM * sizeof(int));
    out = (int*)malloc(CROSSBAR_OUT_NUM * sizeof(int));
    
}

crossbar::~crossbar() {
    free(in);
    free(sel);
    free(out);
}

void crossbar::set(int* _in, int* _sel, int* _out) {

    in = _in;
    sel = _sel;
    out = _out;

}

void crossbar::execute() {

    int i;

    for (i = 0; i < CROSSBAR_OUT_NUM; i++)
        out[i] = in[sel[i]];

}