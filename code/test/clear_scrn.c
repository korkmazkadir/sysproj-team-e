#include "nachos_stdio.h"

int main() {
    _printf("Clearn screen is working\n");
    _printf("\033[H\033[J");
    _printf("End of clear screen\n");
}

