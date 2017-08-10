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
/* This program uses the Iris Image Biometric Data Block library to print     */
/* the contents of a file containing iris image records according to the      */
/* ISO/IEC 19794-6:2005 standard. The record can be optionally validated.     */
/* The file may contain more than one image biometric data block.             */
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

#include <biomdi.h>
#include <biomdimacro.h>
#include <iid_ext.h>
#include <iid.h>

int main(int argc, char *argv[])
{
	char *usage = "usage: priibdb [-v] <datafile>\n"
			"\t -v Validate the record";
	FILE *fp;
	struct stat sb;
	IIBDB *iibdb;
	int vflag = 0;
	int ch;
	int ret;
	unsigned long long total_length;

	if ((argc < 2) || (argc > 3)) {
		printf("%s\n", usage);
		exit (EXIT_FAILURE);
	}

	while ((ch = getopt(argc, argv, "v")) != -1) {
		switch (ch) {
			case 'v' :
				vflag = 1;
				break;
			default :
				printf("%s\n", usage);
				exit (EXIT_FAILURE);
				break;
		}
	}
				
	if (argv[optind] == NULL) {
		printf("%s\n", usage);
		exit (EXIT_FAILURE);
	}

	fp = fopen(argv[optind], "rb");
	if (fp == NULL)
		ERR_EXIT("Open of %s failed: %s\n", argv[optind],
		    strerror(errno));

	if (new_iibdb(&iibdb) < 0)
		ALLOC_ERR_EXIT("Iris Image Biometric Data Block");

	if (fstat(fileno(fp), &sb) < 0)
		ERR_EXIT("Could not get stats on input file");

	total_length = 0;
	ret = READ_ERROR;	/* In case of zero length file */
	while (total_length < sb.st_size) {
		ret = read_iibdb(fp, iibdb);
		if (ret != READ_OK)
			break;
		if (iibdb->record_header.record_length == 0)
			INFOP("Warning, record length is 0");
		else
			total_length += iibdb->record_header.record_length;

		if (vflag) {
			if (validate_iibdb(iibdb) != VALIDATE_OK) {
				INFOP("Iris Image Data Record is invalid");
			} else {
				INFOP("Iris Image Data Record is valid");
			}
		}
		print_iibdb(stdout, iibdb);

		free_iibdb(iibdb);

		if (new_iibdb(&iibdb) < 0)
			ALLOC_ERR_EXIT("Iris Image Biometric Data Block");
	}
	if (ret != READ_OK) {
		fprintf(stderr, "Could not read entire record; Contents:\n");
		print_iibdb(stderr, iibdb);
		free_iibdb(iibdb);
		exit (EXIT_FAILURE);
	}

	if (vflag) {
		// Check the header info against file reality
		if (sb.st_size != total_length) {
			fprintf(stdout, "WARNING: "
			    "File size does not match record length(s).\n");
		} 
	}

	exit (EXIT_SUCCESS);
}
