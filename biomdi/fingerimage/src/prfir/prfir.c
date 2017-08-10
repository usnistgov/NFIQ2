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
/* This program uses the ANSI/INCITS Finger Image Record library to print     */
/* the contents of a file containing finger image records. The file may       */
/* contain more than one record.                                              */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fir.h>
#include <biomdi.h>
#include <biomdimacro.h>

static void
usage()
{
	fprintf(stderr, "usage: prfir [-v] [-ti <type>] <datafile>\n"
			"\t -v Validate the record\n"
			"\t -ti <type> is one of ISO ANSI\n");
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat sb;
	struct finger_image_record *fir;
	int ch;
	int ret;
	int in_type;
	unsigned long long total_length;
	int v_opt = 0;
	int ti_opt = 0;
	char pm;

	if ((argc < 2) || (argc > 5))
		usage();

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
							usage();
						optind++;
						ti_opt++;
						break;
					default:
						usage();
						break;	/* not reached */
				}
				break;
			case 'v' :
				v_opt = 1;
				break;
			default :
				usage();
				break;
		}
	}
				
	if (argv[optind] == NULL)
		usage();

	fp = fopen(argv[optind], "rb");
	if (fp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[optind], strerror(errno));
		exit (EXIT_FAILURE);
	}

	if (new_fir(in_type, &fir) < 0) {
		fprintf(stderr, "could not allocate FIR\n");
		exit (EXIT_FAILURE);
	}

	if (fstat(fileno(fp), &sb) < 0) {
		fprintf(stdout, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	total_length = 0;
	ret = READ_ERROR;
	while (total_length < sb.st_size) {
		ret = read_fir(fp, fir);
		if (ret != READ_OK)
			break;
		total_length += fir->record_length;

		// Validate the FIR
		if (v_opt) {
			if (validate_fir(fir) != VALIDATE_OK) {
				fprintf(stdout, 
				    "Finger Image Record is invalid.\n");
				exit (EXIT_FAILURE);
			} else {
				fprintf(stdout, 
				    "Finger Image Record is valid.\n");
			}
		}

		// Dump the entire FIR
		print_fir(stdout, fir);

		// Free the entire FIR
		free_fir(fir);

		if (new_fir(in_type, &fir) < 0) {
			fprintf(stderr, "could not allocate FIR\n");
			exit (EXIT_FAILURE);
		}
	}
	if (ret != READ_OK) {
		fprintf(stderr, "Could not read entire record; Contents:\n");
		print_fir(stderr, fir);
		free_fir(fir);
		exit (EXIT_FAILURE);
	}

	if (v_opt) {
		// Check the header info against file reality
		if (sb.st_size != total_length) {
			fprintf(stdout, "WARNING: "
			    "File size does not match FIR record length(s).\n");
		} 
	}

	exit (EXIT_SUCCESS);
}
