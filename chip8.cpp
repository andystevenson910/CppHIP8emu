#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include "8BitStack.cpp"

class chip8{
    //private:
    public:
        // I like the old logic style of c where its numeric so ill probably stick to that
        bool display[32][64] = {};
        std::uint8_t dataRegister[16] = {}; //maybe input pointers to each on V0 - VF?
       
        //12 bits wide the first nibble is unused maybe enforce that later
        std::uint16_t addressRegister = 0;

        Stack stack;

        std::uint8_t delayTimer = 0;
        std::uint8_t soundTimer = 0;

        std::uint16_t programCounter = 0;


        // 0x200 is where memory starts
        std::uint8_t memory[4096]={};

    //public:

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
            std::size_t fileSize = program.tellg();
            program.seekg(0, std::ios::beg);

            if (fileSize > 0x1000 - 0x200){
                std::cerr << "File too big to load into memory" << std::endl;
                return false;
            }

            std::uint8_t byte;

            for(int i = 0; i < fileSize ; i++){
                program.read(reinterpret_cast<char*>(&byte), sizeof(byte));
                memory[i + 0x200] = byte;
            }

            // Close the file
            program.close();
            return true;
        }

        bool initialize(){
            programCounter = 0x200;
            return true;
            //Start to emulation loop

        };

        void emulationLoop(){
            //TODO IMPLEMENT THE FOLLOWING FUNCTIONS INTO THIS FUNCTION
            //call function to do instruction and advance pc
            //update timers
            int i = 0;
        };

        std::uint16_t fetchInstruction(){
            return (memory[programCounter]<<8 |  memory[programCounter+1]);
        };

        bool enactInstruction(std::uint16_t instruction){
            if (instruction == 0x0E00) { // 0E00/CLS: Clear the display
                memset(display, 0, sizeof(display[0][0]) * 64 * 32)
            } else if (instruction == 0x00EE) { // 00EE/RET: Returns subroutine (pops subroutine stack)
                stack.pop();
            } else if ((instruction & 0xF000) == 0x1000) { // 1NNN/JP addr: Jump to address
                instruction = instruction & 0x0FFF; // cutting off the leading 1 and assigning it to the PC
            } else if ((instruction & 0xF000) == 0x2000) { // 2NNN/CALL addr: Jump to address
                stack.push(programCounter);
                programCounter = instruction & 0x0FFF; // cutting off the leading 2 and assigning it to the PC
            } else if ((instruction & 0xF000) == 0x3000) { // 3xkk/SE Vx, byte: Skip next instruction if kk equal Vx
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] == (instruction & 0x00FF)){
                    programCounter++;
                }
            } else if ((instruction & 0xF000) == 0x4000) { // 4xkk/SNE Vx, byte: Skip next instruction if kk not equal Vx
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] != (instruction & 0x00FF)){
                    programCounter++;
                }
            } else if ((instruction & 0xF000) == 0x5000) { // 5xy0/SE Vx, Vy: Skip next instruction if Vx and Vy are equal
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] == (dataRegister[(instruction & 0x00F0)>>4])){
                    programCounter++;
                }
            } else if ((instruction & 0xF000) == 0x6000) { // 6xkk/LD Vx, byte: Put kk into register Vx
                dataRegister[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
                
            } else if ((instruction & 0xF000) == 0x7000) { // 7xkk/ADD Vx, byte: ADD kk into register Vx (NOTE: THERE IS NO CARRY FLAG SETTING)
                std::uint16_t sum = dataRegister[(instruction & 0x0F00) >> 8] + instruction & 0x00FF;
                dataRegister[(instruction & 0x0F00) >> 8] = sum;
                
            } else {
                // Default case
            }
        }


};