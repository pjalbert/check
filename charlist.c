#include <stdio.h>
#include <stdlib.h>

int main() {
	int i;

	for (i = 0; i < 256; i++)
		printf("%d %c \n", i, (unsigned char)i);

	return 1;
}
