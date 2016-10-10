#include <stdio.h>

int bigendian() {
    int i = 1;
    char *low = (char*) &i;
 
    return *low ? 0 : 1;
}

void endianness_demo() {
    char data[] = "\0\0\0\0\0";
    *(int *) data = 0x31323334;    // ASCII for "1234"
 
    // On a little-endian system, this will print "4321"
    printf("1234 is stored in memory as %s\n", data);
 
    *(int *) data = 0x12345678;
    // On a little-endian system, this will print "0x78563412"
    printf("0x12345678 is stored in memory as 0x%x%x%x%x\n\n", data[0], data[1], data[2], data[3]);
}
 
int main (int argn, char* argc[]) {
    endianness_demo();
 
    // Run the test function and print results.
    if(bigendian())
        printf("System is big-endian\n");
    else
        printf("System is little-endian\n");
 
    return 0;
}