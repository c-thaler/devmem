#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>


void print_help(char* name) {
	fprintf(stderr, "\nUsage:\t%s { [-B -l length] address | address data }\n"
					"\t-B      : enable binary data output (e.g. for dumping memory to file)\n"
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
	char* value = NULL;
	unsigned long len = 4;
	off_t addr;
	bool write = false;
	bool binary = false;
	uint32_t val;


	/* handling the length option */
	while ((c = getopt (argc, argv, "Bl:")) != -1) {
		switch(c) {
			case 'l':
				length = optarg;
				break;
			case 'B':
				binary = true;
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
		address = argv[optind++];
	}
	else {
		fprintf(stderr, "%s: Address argument is required.\n", argv[0]);
		goto fail;
	}

	if(optind < argc) {
		value = argv[optind++];
	}

	if(optind < argc) {
		fprintf(stderr, "%s: Too many arguments.\n", argv[0]);
		goto fail;
	}

	/* parsing the parameters */
	if(length) {
		len = strtoul(length, NULL, 10);
		if(!len) {
			fprintf(stderr, "length must be a value > 0\n");
			goto fail;
		}
	}

	printf("Reading %lu bytes\n", len);

	if(address) {
		addr = strtoul(address, NULL, 16);
//		if(!addr) {
//			fprintf(stderr, "invalid address\n");
//			goto fail;
//		}
	}

	if(value) {
		write = true;
		val = strtoul(value, NULL, 16);
	}

	printf("Target address=0x%08lx (errno=%d)\n", addr, errno);

	if(write) {
		printf("Writing value=%x\n", val);
	}

	return 0;

fail:
	print_help(argv[0]);
	return err;
}
