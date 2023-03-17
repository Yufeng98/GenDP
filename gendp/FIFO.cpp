#include "FIFO.h"

FIFO::FIFO() {};
FIFO::~FIFO() {};

int FIFO::pop() {
    int data = fifo_array.front();
    if (fifo_array.empty()) {
        fprintf(stderr, "pop empty FIFO.\n");
        exit(-1);
    } else {
        fifo_array.pop();
        // fprintf(stderr, "pop %d\n", data);
    }
    return data;
}

void FIFO::push(int data) {
    if (fifo_array.size() == FIFO_ADDR_NUM) fifo_array.pop();
    fifo_array.push(data);
    // fprintf(stderr, "push %d\n", data);
}

int FIFO::size() {
    return fifo_array.size();
}

void FIFO::show() {
    std::queue<int> q(fifo_array);
    while (!q.empty())
    {
        std::cout << std::hex << q.front() << " ";
        q.pop();
    }
    std::cout << std::endl;
}


void FIFO::clear() {
    std::queue<int> empty;
    std::swap(fifo_array, empty);
}