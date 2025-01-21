#include "chip8.cpp"
#include <iostream>

int main(){
    chip8 emu;
    memset(emu.display, 1, sizeof(emu.display[0][0]) * 64 * 32);
    for (int y = 0; y < 32; ++y) { // Loop through rows
        for (int x = 0; x < 64; ++x) { // Loop through columns
            std::cout << (emu.display[y][x] ? 'X' : ' ');
        }
        std::cout << std::endl; // Move to the next line after printing a row
    }

    std::cout << "wieners"<<std::endl;
    emu.enactInstruction(0x0E00);
    for (int y = 0; y < 32; ++y) { // Loop through rows
        for (int x = 0; x < 64; ++x) { // Loop through columns
            std::cout << (emu.display[y][x] ? 'X' : ' ');
        }
        std::cout << std::endl; // Move to the next line after printing a row
    }

}