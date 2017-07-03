#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
	int c;
	int i;
	char* length = NULL;
	char* address = NULL;
	char* access = NULL;
	char* value = NULL;
	unsigned long len = 1;

	while ((c = getopt (argc, argv, "l:")) != -1) {
		switch(c) {
			case 'l':
				length = optarg;
				break;
			case '?':
				if(optopt == 'l') {
					fprintf(stderr, "Missing length value.\n");
				}
				return 1;
				break;
			default:
				abort();
		}
	}


	if(optind < argc) {
		address = optind++;
	}
	else {
		fprintf(stderr, "%s: Address argument is required.\n", argv[0]);
		return 1;
	}

	if(optind < argc) {
		access = optind++;
	}

	if(optind < argc) {
		value = optind++;
	}

	if(optind < argc) {
		fprintf(stderr, "%s: Too many arguments.\n", argv[0]);
		return 1;
	}


	if(length) {
		len = strtoul(length, NULL, 10);
		if(!len)
			fprintf(stderr, "length must be a value > 0\n");
	}

	printf("Reading %lu bytes\n", len);
}
