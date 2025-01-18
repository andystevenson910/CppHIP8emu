#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

class chip8{
    private:
        // I like the old logic style of c where its numeric so ill probably stick to that
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
        uint8_t memory[4096]={};

    public:
        bool loadProgram(std::string filePath){
            //TODO: implement
            // load file
            // check that it loaded correctly
            // loop through it byte by byte placing it starting at 200


            std::ifstream program(filePath, std::ios::binary);
            if (!program) {
                std::cerr << "Failed to open file: " << filePath << std::endl;
                return false;
            }

            //determine file length

            program.seekg(0, std::ios::end);
            size_t fileSize = program.tellg();
            program.seekg(0, std::ios::beg);

            if (fileSize > 0x1000 - 0x200){
                std::cerr << "File too big to load into memory" << std::endl;
                return false;
            }

            uint8_t byte;

            for(int i = 0; i < fileSize ; i++){
                program.read(reinterpret_cast<char*>(&byte), sizeof(byte));
                memory[i + 0x200] = byte;
            }

            // Close the file
            program.close();
                return true;
            }
}