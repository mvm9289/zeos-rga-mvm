#include <unistd.h>

int main(int argc, char **argv) {
    int i;
    char c;
    
    read(0, &c, 1);

    for (i = 0 ; i < 5; i++)
        write(1, "hola\n", 5);

    while (1);
}
