#include <cstdint>
#include <stdexcept>

class Stack {
    public:
        std::uint8_t pop(void){
            if (index == 0) {
                std::underflow_error("Stack underflow: cannot pop off empty stack");
            }
            index--;
            return stack[index + 1];
        }
        void push(uint8_t value){
            if (index >= 64) {
                std::overflow_error("Stack overflow: cannot push onto full stack");
            }
            index++;
            stack[index] = value;
        }

        std::uint8_t size(){
            return index;
        }
    private:
        std::uint8_t stack[64] = {};
        std::uint8_t index = 0;

};