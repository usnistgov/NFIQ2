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
/* This program uses the Finger Minutiae Record library to print              */
/* the contents of a file containing minutiae records.                        */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <fmr.h>

static int in_type;	// Standard type of the input file


/******************************************************************************/
/* Map the string given for the record format type into an integer.           */
/* Return -1 on if no match.                                                  */
/******************************************************************************/
static int
stdstr_to_type(char *stdstr)
{
	if (strcmp(stdstr, "ANSI") == 0)
		return (FMR_STD_ANSI);
	if (strcmp(stdstr, "ISO") == 0)
		return (FMR_STD_ISO);
	if (strcmp(stdstr, "ISONC") == 0)
		return (FMR_STD_ISO_NORMAL_CARD);
	if (strcmp(stdstr, "ISOCC") == 0)
		return (FMR_STD_ISO_COMPACT_CARD);
	if (strcmp(stdstr, "ANSI07") == 0)
		return (FMR_STD_ANSI07);
	return (-1);
}

static void
usage()
{
	fprintf(stderr, "usage: prfmr [-v] [-ti <type] <datafile>\n"
		"\t -v Validate the record\n"
		"\t -ti <type> is one of ISO | ISONC | ISOCC | ANSI | ANSI07\n");
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat sb;
	struct finger_minutiae_record *fmr;
	int v_opt = 0;
	int ti_opt = 0;
	int ch;
	int ret;
	unsigned int total_length;
	char pm;

	if ((argc < 2) || (argc > 5))
		usage();

	/* Default to read in an ANSI record */
	in_type = FMR_STD_ANSI;
	while ((ch = getopt(argc, argv, "vt:")) != -1) {
		switch (ch) {
			case 'v':
				v_opt = 1;
				break;
			case 't':
				pm = *(char *)optarg;
				switch (pm) {
				    case 'i':
					in_type = stdstr_to_type(argv[optind]);
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
			default:
				usage();
				break;	/* not reached */
		}
	}
				
	if (ti_opt > 1)
		usage();

	fp = fopen(argv[optind], "rb");
	if (fp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[optind], strerror(errno));
		exit (EXIT_FAILURE);
	}

	if (stat(argv[optind], &sb) < 0) {
		fprintf(stdout, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	if (new_fmr(in_type, &fmr) < 0) {
		fprintf(stderr, "could not allocate FMR\n");
		exit (EXIT_FAILURE);
	}

	total_length = 0;
	ret = READ_ERROR;
	while (total_length < sb.st_size) {
		ret = read_fmr(fp, fmr);
		if (ret != READ_OK)
				break;
		/* We don't want to stop making progress when reading */
		if (fmr->record_length == 0) {
			fprintf(stderr, "FMR Record length is 0.\n");
			exit (EXIT_FAILURE);
		}
		
		total_length += fmr->record_length;

		// Validate the FMR
		if (v_opt) {
			if (validate_fmr(fmr) != VALIDATE_OK) {
				fprintf(stdout, 
				    "Finger Minutiae Record is invalid.\n");
				exit (EXIT_FAILURE);
			} else {
				fprintf(stdout, 
				    "Finger Minutiae Record is valid.\n");
			}
		}
		print_fmr(stdout, fmr);
		//free_fmr(fmr);
	}
	if (ret != READ_OK) {
		fprintf(stderr, "Could not read entire record; Contents:\n");
		print_fmr(stderr, fmr);
		//free_fmr(fmr);
		exit (EXIT_FAILURE);
	}

	if (v_opt) {
		// Check the record length info against file reality
		if (sb.st_size != total_length) {
			fprintf(stdout, "WARNING: "
			    "File size does not match FMR record length(s).\n");
		} 
	}

	exit (EXIT_SUCCESS);
}
