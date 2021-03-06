#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>


union memptr_t {
	void *any;
	uint32_t *u32;
};


void print_help(char* name) {
	fprintf(stderr, "\nUsage:\t%s { [-B -l length] address | address data }\n"
					"\t-B      : enable binary data output (e.g. for dumping memory to file)\n"
					"\tlength  : length of data in bytes to act upon, starting at address\n"
					"\taddress : memory address to act upon\n"
					"\tdata    : data word to be written\n\n",
					name);
}


void* map_memory(off_t addr, size_t len) {

	int fd;
	void *mem;

	fd = open("/dev/mem", O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Failed to open /dev/mem : %s\n", strerror(errno));
		return NULL;
	}

	mem = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(mem == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap memory at 0x%08lx : %s\n", addr, strerror(errno));
		return NULL;
	}

	return mem;
}


void print_mem(union memptr_t mem, size_t len, off_t paddr) {
	const int words = 4;
	len = len / 4;

	for(size_t i = 0; i < len; ++i) {
		switch(i & (words - 1)) {
		case 0:
			printf("0x%08jx: %08x", paddr + words * i, mem.u32[i]);
			break;
		case 3:
			printf(" %08x\n", mem.u32[i]);
			break;
		default:
			printf(" %08x", mem.u32[i]);
		}
	}

	if(len & (words - 1))
		printf("\n");
}


int write_mem(union memptr_t mem, size_t len, const char* fname) {
	FILE *fd;
	
	fd = fopen(fname, "w"); 
	if(!fd) {
		return -1;
	}

	fwrite(mem.any, len, 1, fd);
	
	fclose(fd);

	return 0;
}


int main(int argc, char **argv) {
	int err = 1;
	int c;
	char* length = NULL;
	char* address = NULL;
	char* value = NULL;
	size_t len = 4;
	off_t addr;
	off_t off;
	int pagesize = getpagesize();
	bool write = false;
	char *fname = NULL;
	uint32_t val;
	union memptr_t map;

	/* handling the length option */
	while ((c = getopt (argc, argv, "B:l:")) != -1) {
		switch(c) {
			case 'l':
				length = optarg;
				break;
			case 'B':
				fname = optarg;
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

	if(address) {
		addr = strtoul(address, NULL, 16);
		if(!addr) {
			fprintf(stderr, "invalid address\n");
			goto fail;
		}
	}

	if(value) {
		write = true;
		val = strtoul(value, NULL, 16);
	}

	off = addr & (pagesize - 1);
	map.any = map_memory(addr & ~(pagesize - 1), len + off);
	if(!map.any) {
		fprintf(stderr, "error mapping address\n");
		goto fail;
	}

	if(write) {
		printf("Writing 0x%x to 0x%jx\n", val, (intmax_t) addr);
		*map.u32 = val;
	}

	if(fname) {
		printf("Writing %zu bytes from 0x%jx to %s...\n", len, (intmax_t) addr, fname);

		err = write_mem(map, len, fname);
		if(err) {
			fprintf(stderr, "Unable to open file.\n");
			goto fail;
		}
	}
	else {
		if(write)
			printf("Readback:\n");
		print_mem(map, len, addr);
	}

	return 0;

fail:
	print_help(argv[0]);
	return err;
}
