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
/* This program uses the ANSI/INCITS Finger Image-Based Record library to     */
/* validate the contents of a file containing finger image records.           */
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

#include <fir.h>
#include <biomdimacro.h>

static void
usage(char *name)
{
	fprintf(stderr, "usage: %s [-ti <type>] <datafile>\n"
			"\t -ti <type> is one of ISO | ANSI\n", name);
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat sb;
	struct finger_image_record *fir;
	int ret;
	unsigned long long total_length;
	int in_type;
	int ch;
	char pm;

	if ((argc != 2) && (argc != 4))
		usage(argv[0]);

	in_type = FIR_STD_ANSI;	/* Default input type */
	while ((ch = getopt(argc, argv, "vt:")) != -1) {
		switch (ch) {
			case 't':
				pm = *(char *)optarg;
				switch (pm) {
					case 'i':
						in_type = fir_stdstr_to_type(
						    argv[optind]);
						if (in_type < 0)
							usage(argv[0]);
						optind++;
						break;
					default:
						usage(argv[0]);
						break;  /* not reached */
                                }
                                break;
			default:
				usage(argv[0]);
				break;  /* not reached */
		}
	}
	fp = fopen(argv[optind], "rb");
	if (fp == NULL)
		OPEN_ERR_EXIT(argv[optind]);

	if (fstat(fileno(fp), &sb) < 0) {
		fprintf(stderr, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	if (new_fir(in_type, &fir) < 0)
		ALLOC_ERR_EXIT("Could not allocate FIR\n");

	total_length = 0;
	ret = READ_ERROR;	/* In case the file is empty */
	while (total_length < sb.st_size) {
		ret = read_fir(fp, fir);
		if (ret != READ_OK)
			break;
		total_length += fir->record_length;

		// Validate the FIR
		if (validate_fir(fir) != VALIDATE_OK)
			exit (EXIT_FAILURE);

		free_fir(fir);

		if (new_fir(in_type, &fir) < 0)
			ALLOC_ERR_EXIT("Could not allocate FIR\n");
	}
	if (ret != READ_OK)
		exit (EXIT_FAILURE);

	exit (EXIT_SUCCESS);
}
