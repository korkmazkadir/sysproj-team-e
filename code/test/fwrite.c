
#include "nachos_stdio.h"



int main() {

    _printf("\nWrite file is working\n");

    char str[] = "Kadir Korkmaz\nHello There :)\n";

    OpenFileId file = Open("test.txt");

    if (file < 0) {
        _printf("Can not open file. Error code %d\n", file);
        Exit(-1);
    }
    
    _printf("Size of the message : %d", sizeof (str));

    Write(str, sizeof (str), file);
    Close(file);

    _printf("\nEnd of write file\n");
    return 0;
}

