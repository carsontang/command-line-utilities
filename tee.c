// tee
// Usage: tee a.out
// This command reads its standard input until end-of-file and writes
// to standard output and to the file named in its command-line argument.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[])
{
	int output_filename_index;

	if ( !(argc == 2 || argc == 3) ) {
		fprintf(stderr, "Usage 1: tee out.txt\n");
		fprintf(stderr, "Usage 2: tee -a out.txt\n");
		exit(EXIT_FAILURE);
	}

	// If -a (append) option was specified,
	// filename is in argv[2]
	// otherwise it's in argv[1]
	if (argc == 2) {
		output_filename_index = 1;
	} else {
		if (strcmp(argv[1], "-a") != 0) {
			fprintf(stderr, "Option doesn't exist: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		output_filename_index = 2;
	}

	int outputFd;
	char buf[BUF_SIZE];
	ssize_t numRead;

	// Open output file
	int openFlags = O_CREAT | O_WRONLY;
	if (strcmp(argv[1], "-a") == 0) {
		openFlags |= O_APPEND;
	} else {
		openFlags |= O_TRUNC;
	}

	mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	/* rw-rw-rw */

	outputFd = open(argv[output_filename_index], openFlags, filePerms);
	if (outputFd == -1) {
		fprintf(stderr, "Open failed for %s\n", argv[1]);
		perror("open");
		exit(EXIT_FAILURE);
	} 

	// Read from standard input
	// and write to output file
	// and write to standard output
	while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
		if (write(outputFd, buf, numRead) != numRead) {
			fprintf(stderr, "Could not write the entire buffer.\n");
			perror("write to output file");
			exit(EXIT_FAILURE);
		}
		if (write(STDOUT_FILENO, buf, numRead) != numRead) {
			fprintf(stderr, "Could not write the entire buffer.\n");
			perror("write to standard output");
			exit(EXIT_FAILURE);
		}
	}

	if (numRead == -1) {
		fprintf(stderr, "Could not read from standard input.");
		perror("read from standard input");
	}

	if (close(outputFd) == -1) {
		fprintf(stderr, "Close failed for %s\n", argv[1]);
		perror("close");
		exit(EXIT_FAILURE);		
	}

	exit(EXIT_SUCCESS);
}