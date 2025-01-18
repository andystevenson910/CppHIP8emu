#include <cstdint>
class chip8{
    private:
        bool display[32][64] = {};
        uint8_t dataRegister[16] = {};
        //maybe input pointers to each on V0 - VF?

        //12 bits wide the first nibble is unused maybe enforce that later
        uint16_t addressRegister = 0;


}