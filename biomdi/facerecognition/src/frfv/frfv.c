/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */
/******************************************************************************/
/* This program uses the ANSI/INCITS Face Recognition Format library to       */
/* validate the contents of a file containing FRF records.                    */
/*                                                                            */
/* Return values:                                                             */
/*    0 - File contents are valid                                             */
/*    1 - File contents are invalid                                           */
/*   -1 - Other error occurred                                                */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <frf.h>

void
usage(char *name) {
	printf("usage: %s <datafile>\n", name);
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat sb;
	struct facial_block *fb;
	int ret;
	unsigned long long total_length;

	if (argc != 2)
		usage(argv[0]);

	fp = fopen(argv[1], "rb");
	if (fp == NULL)
		OPEN_ERR_EXIT(argv[optind]);

	if (fstat(fileno(fp), &sb) < 0) {
		fprintf(stderr, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	if (new_fb(&fb) < 0)
		ALLOC_ERR_EXIT("Could not allocate Facial Block\n");

	total_length = 0;
	ret = READ_ERROR;
	while (total_length < sb.st_size) {
		ret = read_fb(fp, fb);
		if (ret != READ_OK)
			break;
		total_length += fb->record_length;

		// Validate the Facial Block
		if (validate_fb(fb) != VALIDATE_OK)
			exit (EXIT_FAILURE);

		free_fb(fb);

		if (new_fb(&fb) < 0)
			ALLOC_ERR_EXIT("Could not allocate Facial Block\n");
	}
	if (ret != READ_OK)
		exit (EXIT_FAILURE);

	exit (EXIT_SUCCESS);
}
