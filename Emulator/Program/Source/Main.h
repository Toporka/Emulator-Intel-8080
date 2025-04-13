#pragma once
#include "time.h"
#include "Processor8080.h"
#include "Display.h"

#define TIC (1000.0 / 60.0)

unsigned char* ReadFile(Processor8080* processor, char* filePath);
void PushAddress(Processor8080* processor, uint16_t addres);
void Generate_Interrupt(Processor8080* processor, uint16_t addres);
void Emulate_Controller(Processor8080* processor);
void Emulate_Shift_Register(Processor8080* processor);
void Emulate(Processor8080* processor, long cycle);