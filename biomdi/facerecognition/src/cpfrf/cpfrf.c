/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility  whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <frf.h>

int main(int argc, char *argv[])
{
	char *usage = "usage: cpfrf <infile> <outfile>" ;
	FILE *infp, *outfp;
	struct facial_block *fb;
	int rc = 0;

	if (argc != 3) {
		printf("%s\n", usage);
		exit(EXIT_FAILURE);
	}

	infp = fopen(argv[1], "r");
	if (infp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	outfp = fopen(argv[2], "w+");
	if (outfp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (new_fb(&fb) < 0) {
		fprintf(stderr, "could not allocate input Facial Block\n");
		exit(EXIT_FAILURE);
	}
	while (read_fb(infp, fb) == FRF_READ_OK) {

		// Validate the block
		if (validate_fb(fb) != FRF_VALIDATE_OK) {
			fprintf(stdout, "Facial Block is invalid.\n");
		} else {
			fprintf(stdout, "Facial Block is valid.\n");
		}

		// Write the block to the output file
		if (write_fb(outfp, fb) == FRF_WRITE_ERROR) {
			fprintf(stderr, "error writing output file\n");
			exit(EXIT_FAILURE);
		}

		// Free the entire block
		free_fb(fb);

		if (new_fb(&fb) < 0) {
			fprintf(stderr, "could not allocate Facial Block\n");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stdout, "File %s written successfully\n", argv[2]);

	exit(EXIT_SUCCESS);
}
