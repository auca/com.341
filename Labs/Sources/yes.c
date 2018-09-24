#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc > 1) {
        while (true) {
            for (int i = 1; i < argc; ++i) {
                write(1, argv[i], strlen(argv[i]));
                if (i < argc - 1) {
                    write(1, " ", 1);
                } else {
                    write(1, "\n", 1);
                }
            }
        }
    } else {
        while (true) {
            write(1, "y\n", 2);
        }
    }

    return 0;
}

