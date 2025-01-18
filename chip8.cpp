#include <cstdint>
#include <cstack>

class chip8{
    private:
        bool display[32][64] = {};
        uint8_t dataRegister[16] = {};
        //maybe input pointers to each on V0 - VF?

        //12 bits wide the first nibble is unused maybe enforce that later
        uint16_t addressRegister = 0;

        uint8_t stack[64] = {};

        uint8_t stackPointer = 0;

        uint8_t delayTimer = 0;
        uint8_t soundTimer = 0;

        uint16_t programCounter = 0;


        // 0x200 is where memory starts
        uint8_t memory[4096]={}

}