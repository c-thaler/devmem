#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


void print_help(char* name) {
	fprintf(stderr, "\nUsage:\t%s { [-l length] address | address data }\n"
					"\tlength  : length of data in bytes to act upon, starting at address\n"
					"\taddress : memory address to act upon\n"
					"\tdata    : data word to be written\n\n",
					name);
}


int main(int argc, char **argv) {
	int err = 1;
	int c;
	int i;
	char* length = NULL;
	char* address = NULL;
	char* access = NULL;
	char* value = NULL;
	unsigned long len = 4;


	/* handling the length option */
	while ((c = getopt (argc, argv, "l:")) != -1) {
		switch(c) {
			case 'l':
				length = optarg;
				break;
			case '?':
				if(optopt == 'l') {
					fprintf(stderr, "Missing length value.\n");
				}
				goto fail;
				break;
			default:
				abort();
		}
	}


	/* handling the non-option parameters */
	if(optind < argc) {
		address = optind++;
	}
	else {
		fprintf(stderr, "%s: Address argument is required.\n", argv[0]);
		goto fail;
	}

	if(optind < argc) {
		access = optind++;
	}

	if(optind < argc) {
		value = optind++;
	}

	if(optind < argc) {
		fprintf(stderr, "%s: Too many arguments.\n", argv[0]);
		goto fail;
	}


	/* parsing the parameters */
	if(length) {
		len = strtoul(length, NULL, 10);
		if(!len)
			fprintf(stderr, "length must be a value > 0\n");
	}

	printf("Reading %lu bytes\n", len);

	return 0;

fail:
	print_help(argv[0]);
	return err;
}
