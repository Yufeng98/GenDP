#include "sys_def.h"
#include <queue>

class FIFO {

    public:

        FIFO();
        ~FIFO();

        int pop();

        void push(int data);

        int size();
        
        void show();

        void clear();

    private:

        std::queue<int> fifo_array;

};