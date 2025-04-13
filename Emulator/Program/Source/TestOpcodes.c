#include <stdint.h>
#include <stdio.h>

typedef struct CounterOpcodes
{
    unsigned char *opcode;
    int counter;
} CounterOpcodes;

int sizeArray = 256;
CounterOpcodes array[256] = {};

CounterOpcodes changeArray(unsigned char* opcode)
{
    CounterOpcodes *newArray = array;

    for (int i = 0; i < sizeArray; i++)
    {
        if (newArray[i].opcode != NULL && *newArray[i].opcode == *opcode)
        {
            newArray[i].counter++;
            return *newArray;
        }
    }
    
    for (int i = 0; i < sizeArray; i++)
    {
        if (newArray[i].opcode == NULL)
        {
            newArray[i].opcode = opcode;
            newArray[i].counter = 1;

            return *newArray;
        }
    }
}

void writeArrayToFile()
{
    FILE *file = fopen("Opcodes.txt", "w");
    if (file == NULL)
    {
        perror("Ошибка при открытии файла");
        return;
    }

    for (int i = 0; i < sizeArray; i++)
    {
        if (array[i].opcode != 0)
        {
            fprintf(file, "Opcode Address: %02x, Counter: %d\n", 
                *array[i].opcode, array[i].counter);
        }
    }

    fclose(file);
}