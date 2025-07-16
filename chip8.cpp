#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include "8BitStack.cpp"




class chip8{
    //private:
    public:

        static uint32_t prngState = 0;
        //EMULATED
        bool display[32][64] = {};
        std::uint8_t dataRegister[16] = {}; //V0-VF
       
        //12 bits wide the first nibble is unused maybe enforce that later
        std::uint16_t addressRegister = 0; // also refered to as 'I'

        Stack stack;

        std::uint8_t delayTimer = 0;
        std::uint8_t soundTimer = 0;

        std::uint16_t programCounter = 0;


        // 0x200 is where memory starts
        std::uint8_t memory[4096]={};

    //public:
        uint8_t chip8Rand() {
            prngState = prngState * 1664525 + 1013904223;
            return static_cast<uint8_t>(prngState >> 24);
        }

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
            prngState = 1; //TODO: set using epoch for first seed before deployment
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
                programCounter = instruction & 0x0FFF - 0x2; // cutting off the leading 2 and assigning it to the PC

            } else if ((instruction & 0xF000) == 0x3000) { // 3xkk/SE Vx, byte: Skip next instruction if kk equal Vx
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] == (instruction & 0x00FF)){
                    programCounter = programCounter + 0x2;
                }
            } else if ((instruction & 0xF000) == 0x4000) { // 4xkk/SNE Vx, byte: Skip next instruction if kk not equal Vx
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] != (instruction & 0x00FF)){
                    programCounter = programCounter + 0x2;
                }
            } else if ((instruction & 0xF000) == 0x5000) { // 5xy0/SE Vx, Vy: Skip next instruction if Vx and Vy are equal
                //DEBUG: this might be a problem a) it might not check intended and a more important b)might need to increment the pc twice
                if (dataRegister[(instruction & 0x0F00)>>8] == (dataRegister[(instruction & 0x00F0)>>4])){
                    programCounter = programCounter + 0x2;
                }
            } else if ((instruction & 0xF000) == 0x6000) { // 6xkk/LD Vx, byte: Put kk into register Vx
                dataRegister[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;    
            } else if ((instruction & 0xF000) == 0x7000) { // 7xkk/ADD Vx, byte: ADD kk into register Vx (NOTE: THERE IS NO CARRY FLAG SETTING)
                std::uint16_t sum = dataRegister[(instruction & 0x0F00) >> 8] + instruction & 0x00FF;
                dataRegister[(instruction & 0x0F00) >> 8] = sum;
                
            } 

            // 8 Vx Vy operations VVVV

            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0000)) {  // 8xy0/LD Vx, Vy: Set Vx = Vy: Store the value of register Vy in register Vx.
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x00F0) >> 4];
            } 
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0001)) {  // 8xy1/OR Vx, Vy: Set Vx = Vx OR Vy. Bitwise OR Vx and Vy stored in Vx.
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x0F00) >> 8] | dataRegister[(instruction & 0x00F0) >> 4]; //TODO: Check this one
            } 
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0002)) {  // 8xy2/AND Vx, Vy: Set Vx = Vx AND Vy. Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corresponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0. 
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x0F00) >> 8] & dataRegister[(instruction & 0x00F0) >> 4];
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0003)) {  // 8xy3/XOR Vx, V: Set Vx = Vx XOR Vy.
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x0F00) >> 8] ^ dataRegister[(instruction & 0x00F0) >> 4];
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0004)) {  //8xy4/ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
                std::uint16_t sum = dataRegister[(instruction & 0x0F00) >> 8] + dataRegister[(instruction & 0x00F0) >> 4];
                dataRegister[(instruction & 0x0F00) >> 8] = sum;
                dataRegister[0xF] = (sum >> 8) & 0x1;
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0005)) {  //8xy5/SUB Vx, Vy: SetVx=Vx-Vy,set VF= !borrow.
                std::uint16_t difference = dataRegister[(instruction & 0x0F00) >> 8] - dataRegister[(instruction & 0x00F0) >> 4];
                dataRegister[0xF] = dataRegister[(instruction & 0x0F00) >> 8] < dataRegister[(instruction & 0x00F0) >> 4] ? 0 : 1;
                dataRegister[(instruction & 0x0F00) >> 8] = difference;  
                //PROPOSED AI BITWISE VF set: dataRegister[0xF] = ((~diff) >> 8) & 1;
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0006)) {  //8xy6/SHR Vx, Vy: set vX to vY and shift vX one bit to the right, set vF to the bit shifted out
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x00F0) >> 4] >> 1;
                dataRegister[0xF] = dataRegister[(instruction & 0x00F0) >> 4] & 0x1;
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x0007)) {  //8xy7/SUBN Vx, Vy: SetVx=Vx-Vy,set VF= !borrow.
                std::uint16_t difference = dataRegister[(instruction & 0x00F0) >> 4] - dataRegister[(instruction & 0x0F00) >> 8];
                dataRegister[0xF] = dataRegister[(instruction & 0x0F00) >> 8] > dataRegister[(instruction & 0x00F0) >> 4] ? 0 : 1;
                dataRegister[(instruction & 0x0F00) >> 8] = difference;  
                //PROPOSED AI BITWISE VF set: dataRegister[0xF] = ((~diff) >> 8) & 1;
            }
            else if (((instruction & 0xF000) == 0x8000) && ((instruction & 0x000F) == 0x000E)) { //8xyE/SHL Vx, Vy: set vX to vY and shift vX one bit to the left, set vF to the bit shifted out
                dataRegister[(instruction & 0x0F00) >> 8] = dataRegister[(instruction & 0x00F0) >> 4] << 1;
                dataRegister[0xF] = dataRegister[(instruction & 0x00F0) >> 4] >> 7;
            } 
            
            // 8 Vx Vy operations ^^^^

            else if ((instruction & 0xF000) == 0x9000) { //9xy0/SNE Vx, Vy: Skip next instruction if Vx != Vy. The values of Vx and Vy are compared, and if they're not equal then the PC is incremented an extra time.                
                if (dataRegister[(instruction & 0x0F00)>>8] != dataRegister[(instruction & 0x00F0)>>4]){
                    programCounter = programCounter + 0x2;
                }
            } 
            else if ((instruction & 0xF000) == 0xA000) { //Annn/LD I, addr: Set I = nnn. The value of register I is set to nnn.
                addressRegister = instruction & 0x0FFF;
            } 
            else if ((instruction & 0xF000) == 0xB000) { //Bnnn/JP V0, addr: Jump to instructoin nnn + V0
                programCounter = (instruction & 0x0FFF) + dataRegister[0] - 0x2;
            }
            else if ((instruction & 0xF000) == 0xC000) { //Cxkk/RND Vx, byte: Random number 0 to 255 anded with kk and assigned to Vx
                dataRegister[(instruction & 0x0F00) >> 8] = (instruction & 0x00FF) & chip8Rand();
            }
            else if ((instruction & 0xF000) == 0xD000) { //Dxyn/DRW Vx, Vy, nibble: //TODO: Review this
                xStart = instruction & 0x0F00 >> 8;
                yStart = instruction & 0x00F0 >> 4;

                dataRegister[0xF] = 0;
                for (std::uint8_t i = 0; i < (instruction & 0x000F); i++ ){
                    std::uint8_t sprite = memory[addressRegister+i];
                    for (std::uint8_t j = 0; j < 8; j++){
                        yCoor = yStart+i % 32;
                        xCoor = xStart+j % 64;
                        if ((display[yCoor][xCoor] == 1) & ((sprite >> bitIndex) & 1)){
                            dataRegister[0xF] = 1;
                        }
                        display[yCoor][xCoor] = display[yCoor][xCoor] ^ ((sprite >> bitIndex) & 1);
                    }
                }
            }
            else if ((instruction & 0xF0FF) == 0xE09E) { // EX9E: Skip if key Vx is pressed
                uint8_t key = dataRegister[(instruction & 0x0F00) >> 8]; // Key to check (0x0-0xF)
                if (keypad[key]) {              // If key is pressed...
                    programCounter += 0x2;        // Skip next instruction
                }
            }
            else if ((instruction & 0xF0FF) == 0xE0A1) { // EXA1: Skip if key Vx is not pressed.
                uint8_t key = dataRegister[(instruction & 0x0F00) >> 8]; // Key to check (0x0-0xF)
                if (!keypad[key]) {              // If key is not pressed...
                    programCounter += 0x2;        // Skip next instruction
                }
            }
            else if ((instruction & 0xF0FF) == 0xF007) { //Fx07 - LD Vx, DT: Set Vx = delay timer value. 
                dataRegister[(instruction & 0x0F00) >> 8] = delayTimer;
            }
            else if ((instruction & 0xF0FF) == 0xF00A) { //Fx0A - LD Vx, K: Wait for a key press, store the value of the key in Vx. 
                dataRegister[(instruction & 0x0F00) >> 8] = delayTimer;
            }
            else {
                // Default case
            }
        }


};