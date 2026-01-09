#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
struct str { char* data; size_t size; };

int main();


int main() {
    struct str x = (struct str) { "Hello World", 11 };
}
