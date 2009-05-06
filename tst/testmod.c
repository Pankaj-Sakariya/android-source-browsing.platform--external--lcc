extern void printf(const char* s,...);

void testUnsigned(unsigned int a, unsigned int b) {
	printf("%u / %% %u = %u, %u\n", a, b, a / b, a % b);
}

void testSigned(int a, int b) {
	printf("%d / %% %d = %d, %d\n", a, b, a / b, a % b);
}

void main(int argc, char** argv) {
    {
		int i;
		printf("signed:\n");
		for (i = -6; i < 6; i++) {
			testSigned(i, 3);
		}
	}
	{
		unsigned int u;
    	unsigned int big = ((unsigned int) 1) << ((unsigned int) 31);
		printf("unsigned:\n");
		for (u = 0; u < 6; u++) {
			testUnsigned(u, 3);
		}
    	testUnsigned(big, 2);
   	}
}
