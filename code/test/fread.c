
#include "nachos_stdio.h"

int main() {
    _printf("Reading from file\n");

    char buffer[MAX_STRING_SIZE];
    OpenFileId file = Open("test.txt");

    if (file < 0) {
        _printf("Can not open file. Error code %d\n", file);
        Exit(-1);
    }

    int size = Read(buffer, MAX_STRING_SIZE, file);

    //To make null terminated string
    buffer[size] = '\0';
    _printf("Size of read %d. Result : %s", size, buffer);

    Close(file);

    _printf("End of read\n");
}

