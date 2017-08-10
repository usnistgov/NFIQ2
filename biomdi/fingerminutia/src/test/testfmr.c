/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/queue.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdimacro.h>
#include <fmr.h>

// Test program to exercise some of the FMR library functions.

static void
print_fmr_stats(FMR *fmr) {

	int count;
	int i;
	struct finger_view_minutiae_record **fvmrs;

	count = get_fvmr_count(fmr);
	printf("FVMR count is %d\n", count);
	fvmrs = (struct finger_view_minutiae_record **) malloc(
		 count * sizeof(struct finger_view_minutiae_record **));
	if (fvmrs == NULL) {
		fprintf(stderr, "Memory allocation error.\n");
		exit (EXIT_FAILURE);
	}
	get_fvmrs(fmr, fvmrs);
	for (i = 0; i < count; i++) {
		printf("FVMR %d has %d minutiae.\n", 
			i, get_fmd_count(fvmrs[i]));
		printf("FVMR %d has %d core records.\n", 
			i, get_core_count(fvmrs[i]));
		printf("FVMR %d has %d delta records.\n", 
			i, get_delta_count(fvmrs[i]));
		printf("FVMR %d has %d ridge data records.\n", 
			i, get_rcd_count(fvmrs[i]));
	}
}

int main(int argc, char *argv[])
{
	char *usage = "usage: test_fmr <infile> (must be ANSI FMR)";
	FILE *infp;
	FILE *outfp;
	FMR *fmr;
	uint8_t *buf;
	BDB *fmdb;
	struct stat sb;

	if (argc != 2) {
		printf("%s\n", usage);
		exit(EXIT_FAILURE);
	}

	infp = fopen(argv[1], "r");
	if (infp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (new_fmr(FMR_STD_ANSI, &fmr) < 0) {
		fprintf(stderr, "could not allocate input FMR\n");
		exit(EXIT_FAILURE);
	}
	printf("Testing the read functions...\n");
	if (read_fmr(infp, fmr) != READ_OK) {
		fprintf(stderr, "could not read input FMR\n");
		exit(EXIT_FAILURE);
	}
	print_fmr_stats(fmr);

	/* Test the push functions by writing the buffer into a file, then
	 * reading it back in.
	 */
	printf("\nTesting the push functions...\n");

	buf = (uint8_t *)malloc(fmr->record_length);
	if (buf == NULL) {
		fprintf(stderr, "could not allocate buffer\n");
		exit (EXIT_FAILURE);
	}
	fmdb = (BDB *)malloc(sizeof(BDB));
	if (fmdb == NULL) {
		fprintf(stderr, "could not allocate BDB\n");
		exit (EXIT_FAILURE);
	}
	INIT_BDB(fmdb, buf, fmr->record_length);
	if (push_fmr(fmdb, fmr) != WRITE_OK) {
		fprintf(stderr, "could not push FMR\n");
		exit (EXIT_FAILURE);
	}

        outfp = tmpfile();
        if (outfp == NULL) {
                fprintf(stderr, "open of temp file failed: %s\n",
                        strerror(errno));
                exit (EXIT_FAILURE);
        }
	if (fwrite(fmdb->bdb_start, 1, fmdb->bdb_size, outfp) !=
	    fmdb->bdb_size) {
		fprintf(stderr, "write of temp file failed: %s\n",
		    strerror(errno));
		exit (EXIT_FAILURE);
	}
	free_fmr(fmr);
	new_fmr(FMR_STD_ANSI, &fmr);
	rewind(outfp);
	if (read_fmr(outfp, fmr) != READ_OK) {
		fprintf(stderr, "could not read generated output file\n");
		exit(EXIT_FAILURE);
	}
	print_fmr_stats(fmr);
	free_fmr(fmr);
	fclose(outfp);

	/* Test the scan functions by reading the file into a buffer, and
	 * scanning the FMR, then verifying it.
	 */
	printf("\nTesting the scan functions...\n");
	if (fstat(fileno(infp), &sb) < 0) {
		fprintf(stdout, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	free(buf);
	buf = (void *)malloc(sb.st_size);
	rewind(infp);
	if (fread(buf, 1, sb.st_size, infp) != sb.st_size) {
		fprintf(stderr, "could not read file\n");
		exit (EXIT_FAILURE);
	}
	fclose(infp);

	INIT_BDB(fmdb, buf, sb.st_size);

	new_fmr(FMR_STD_ANSI, &fmr);
	if (scan_fmr(fmdb, fmr) != READ_OK) {
		fprintf(stderr, "could not scan FMR\n");
		exit (EXIT_FAILURE);
	}
	print_fmr_stats(fmr);

	free(buf);
	free(fmdb);
	free_fmr(fmr);

	exit (EXIT_SUCCESS);
}
