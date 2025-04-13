#include "Main.h"

unsigned char* ReadFile(Processor8080* processor, char* filePath)
{
    // Открытие файл по переданному пути
    FILE *file = fopen(filePath, "rb");    
    if (file == NULL)    
    {    
        printf("ERROR: Не удаётся открыть файл: %s\n", filePath);    
        exit(1);    
    }

    // Получение размеров файла
    fseek(file, 0L, SEEK_END);    
    int fileSize = ftell(file);    
    fseek(file, 0L, SEEK_SET);

    // Выделение памяти для массива
    processor->Memory = malloc(0x10000);

    // Чтение файла и запись данных в массив
    fread(processor->Memory, fileSize, 1, file);
    fclose(file);
}

void PushAddress(Processor8080* processor, uint16_t addres) {
	processor->Memory[processor->StackPointer - 1] = (addres & 0xFF00) >> 8;
	processor->Memory[processor->StackPointer - 2] = (addres & 0xFF);
	processor->StackPointer -= 2;
}

void Generate_Interrupt(Processor8080* processor, uint16_t addres) {
	PushAddress(processor, processor->ProgrammCounter);
	processor->ProgrammCounter = addres; 
	processor->int_enable = 0;
}

void Emulate_Controller(Processor8080* processor) {
	SDL_Event e;
	while(SDL_PollEvent(&e) != 0) 
    {
		if(e.type == SDL_QUIT)
        {
			exit(0);
		} 
        else if(e.type == SDL_KEYDOWN) 
        {
			switch(e.key.keysym.sym) 
            {
				case 'c': // Add coin
					processor->p1_input_port |= 0x1;
					break;
				case '1': // Player 1
					processor->p1_input_port |= 0x4;
					break;
				case '2': // Player 2
					processor->p1_input_port |= 0x2;
					break;
				case 'w': // Shoot 1
					processor->p1_input_port |= 0x10;
					break;
				case 'a': // Move left 1 
					processor->p1_input_port |= 0x20;
					break;
				case 'd': // Move right 1
					processor->p1_input_port |= 0x40;
					break;
				case 't': // Shoot 2
					processor->p2_input_port |= 0x10;
					break;
				case 'f': // Move right 2
					processor->p2_input_port |= 0x20;
					break;
				case 'h': // Move left 2
					processor->p2_input_port |= 0x40;
					break;
			}
		} 
        else if(e.type == SDL_KEYUP) 
        {
			switch(e.key.keysym.sym) 
            {
				case 'c':
					processor->p1_input_port &= 0;
					break;
				case '1':
					processor->p1_input_port &= ~(0x4);
					break;
				case '2':
					processor->p1_input_port &= ~(0x2);
					break;
				case 'w':
					processor->p1_input_port &= ~(0x10);
					break;
				case 'a':
					processor->p1_input_port &= ~(0x20);
					break;
				case 'd':
					processor->p1_input_port &= ~(0x40);
					break;
				case 't':
					processor->p2_input_port &= ~(0x10);
					break;
				case 'f':
					processor->p2_input_port &= ~(0x20);
					break;
				case 'h':
					processor->p2_input_port &= ~(0x40);
					break;
			}	
		}
	}
}

void Emulate_Shift_Register(Processor8080* processor) {
	unsigned char* code = &processor->Memory[processor->ProgrammCounter];
	static uint16_t shift_register;
	static int shift_offset;
	
	if(*code == 0xDB) 
    {
		switch(code[1]){
			case 0:
				processor->A = 0xf;
				break;
			case 1: //Read input player 1
				processor->A = processor->p1_input_port;
				break;
			case 2: //Read input player 2
				processor->A = processor->p2_input_port;
				break;
			case 3: //Shift and return result
				processor->A = (shift_register >> (8 - shift_offset)) & 0xFF;
				break;
		}
	} 
    else if(*code == 0xD3) 
    {
		switch(code[1]){
			case 2: 
				shift_offset = processor->A;
				break;
			case 4:
				shift_register = (processor->A << 8) | (shift_register >> 8);
				break;
		}
	}
}

void Emulate(Processor8080* processor, long cycle)
{
	processor->CyclesCounter = 0;
	
    while (processor->CyclesCounter < cycle)
    {
		// Получение адресса инструкции
		unsigned char *opcode = &processor->Memory[processor->ProgrammCounter];

        // Изменение счётчика циклов
        processor->CyclesCounter += cycles[*opcode];

        Emulate_Shift_Register(processor);	

		// Выполнение инструкции по её адрессу
		instructions[*opcode](processor, opcode);
    }
}

int main(int argc, char** argv)
{
	Processor8080* processor = calloc(1, sizeof(Processor8080));
    
    ReadFile(processor, argv[1]);
	init_sdl();

    uint32_t last_tic = SDL_GetTicks();

    while(1) {

		if((SDL_GetTicks() - last_tic) >= TIC) {
			last_tic = SDL_GetTicks();	

			Emulate(processor, (2000 * TIC)/2);

			if(processor->int_enable) 
			{
				Generate_Interrupt(processor, 0x0008);
			}

			Emulate(processor, (2000 * TIC)/2);
			render_video_memory(processor->Memory);
			Emulate_Controller(processor);

			if(processor->int_enable) 
			{
				Generate_Interrupt(processor, 0x0010);
			}
		}
	}
}