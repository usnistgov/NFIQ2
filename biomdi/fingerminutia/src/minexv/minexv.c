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
/* This program will verify a M1 record contained in a file. The verification */
/* involves using the FMR library to perform the verification as per the      */
/* ANSI INCITS 378-2004 specification in addition to the constraints of the   */
/* suite of MINEX test specifications.                                        */
/* This program assumes that a single FMR is contained in the file, and       */
/* therefore, the record length in the header should match the file size.     */
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

#ifdef MINEX04
#define MAX_REC_LEN	4500
#elif (OMINEX || MINEX2)
#define MAX_REC_LEN	800
#endif
#define MIN_REC_LEN	32
#define MAX_RIDGE_COUNT	15
#define IOERR_EXIT 	-1
#define RECERR_EXIT	-2

int piv_quality = 0;

int
minex_verify(FILE *fp, struct finger_minutiae_record *fmr)
{
	struct stat sb;
	int ret = VALIDATE_OK;
	int lret;
	struct finger_view_minutiae_record *fvmr;
#if (MINEX04 || OMINEX)
	struct finger_minutiae_data *fmd;
#endif
	struct finger_extended_data *fed;
	struct ridge_count_data **rcds;
	int total_cores, total_deltas, total_ridges;

	// Check the header info against file reality
	if (fstat(fileno(fp), &sb) < 0) {
	    ERRP("Could not get stats on input file");
	    return (VALIDATE_ERROR);
	}
	if (fmr->record_length != sb.st_size) {
		ERRP("FMR record length (%d) not equal to file size (%lld)",
		    fmr->record_length, sb.st_size);
		ret = VALIDATE_ERROR;
	}

	if (fmr->record_length_type != FMR_ANSI_SMALL_HEADER_TYPE) {
		ERRP("FMR header length is incorrect");
		ret = VALIDATE_ERROR;
	}

	if(fmr->record_length<MIN_REC_LEN || fmr->record_length>MAX_REC_LEN) {
		ERRP("FMR record length not in range [32,%d]", MAX_REC_LEN);
		ret = VALIDATE_ERROR;
	}

#if (MINEX04 || OMINEX)
	CSR(fmr->product_identifier_owner, 0, "Product ID Owner");
	CSR(fmr->product_identifier_type, 0, "Product ID Type");
#endif

	CSR(fmr->num_views, 1, "Number of Finger Views");

	CSR(fmr->scanner_id, 0, "Capture Equipment ID");

	CSR(fmr->compliance, 0, "Capture Equipment Compliance");

	CSR(fmr->x_resolution, 197, "X-Resolution");

	CSR(fmr->y_resolution, 197, "Y-Resolution");

	// Check the Finger View record
	fvmr = TAILQ_FIRST(&fmr->finger_views);
	if (fvmr == NULL) {
		ERRP("There are no finger views");
		return (VALIDATE_ERROR);
	}
#if (MINEX04)
	if (fvmr->impression_type > 3) {
#elif (OMINEX || MINEX2)
	if (fvmr->impression_type != 0 && fvmr->impression_type != 2) {
#endif
		ERRP("Impression type is invalid");
		ret = VALIDATE_ERROR;
	}


	if (fvmr->number_of_minutiae > 128) {
		ERRP("Number of minutiae is invalid");
		ret = VALIDATE_ERROR;
	}

	if (piv_quality) {
		if ((fvmr->finger_quality != 20) &&
		    (fvmr->finger_quality != 40) &&
		    (fvmr->finger_quality != 60) &&
		    (fvmr->finger_quality != 80) &&
		    (fvmr->finger_quality != 100)) {
			ERRP("Finger quality is invalid");
			ret = VALIDATE_ERROR;
		}
	} else {
		if ((fvmr->finger_quality != 1) &&
		    (fvmr->finger_quality != 25) &&
		    (fvmr->finger_quality != 50) &&
		    (fvmr->finger_quality != 75) &&
		    (fvmr->finger_quality != 100)) {
			ERRP("Finger quality is invalid");
			ret = VALIDATE_ERROR;
		}
	}

#if (MINEX04 || OMINEX)
	// Check each minutia record
	TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
		CSR(fmd->quality, 0, "Minutia Quality");
		// Check of type is done in libfmr
	}
#endif

	// Check the extended data attached to the FVMR
	if (fvmr->extended == NULL) {
		INFOP("There is no extended data on the FVMR");
		return (ret); 	// All remaining checks involved extended data
	}

	// Check each extended data record
	// First we check for Core and Delta information
	total_cores = get_core_count(fvmr);
	total_deltas = get_delta_count(fvmr);
	INFOP("Core count is %u, Delta count is %u", total_cores, total_deltas);

	// Next, check for Ridge Count data, and if present, is it proper type?
	lret = VALIDATE_OK;
	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		// MINEX04 requires Ridge Count method be Eight-neighbor
		if (fed->type_id == FED_RIDGE_COUNT)
			if (fed->rcdb->method != RCE_EIGHT_NEIGHBOR)
				lret = VALIDATE_ERROR;
	}

	total_ridges = get_rcd_count(fvmr);
	INFOP("Ridge record count is %u", total_ridges);
	if (lret != VALIDATE_OK) {
	    ERRP("Ridge Count extended data not eight-neighbor");
	    ret = VALIDATE_ERROR;
	}

	// Check the ridge count being in range 0-15
	if (total_ridges > 0) {
		int i;
		rcds = (struct ridge_count_data **) malloc(
			total_ridges * sizeof(struct ridge_count_data **));
		if (rcds == NULL) {
			ERRP("Allocatung Ridge Count data");
			return(VALIDATE_ERROR);
		}
		if (get_rcds(fvmr, rcds) != total_ridges) {
			ERRP("Rretrieving ridge count data");
			return(VALIDATE_ERROR);
		}
		for (i = 0; i < total_ridges; i++) {
			if (rcds[i]->count > MAX_RIDGE_COUNT) {
			    ERRP("Ridge count %d invalid for index %d->%d\n",
				rcds[i]->count,
				rcds[i]->index_one, rcds[i]->index_two);
			}
		}
	}

	// Check for the core/delta without ridge counts
	if (fvmr->number_of_minutiae > 0)
		if ((total_cores != 0 || total_deltas != 0) && 
		    (total_ridges == 0)) {
			ERRP("Have core/delta info without ridge counts");
			ret = VALIDATE_ERROR;
		}

	// Next, check for Unknown data
	lret = VALIDATE_OK;
	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if ((fed->type_id != FED_RIDGE_COUNT) &&
		    (fed->type_id != FED_CORE_AND_DELTA))
			lret = VALIDATE_ERROR;
	}
	if (lret != VALIDATE_OK) {
		ERRP("Unknown type of extended data");
		ret = VALIDATE_ERROR;
	}

	return (ret);
}

int
main(int argc, char *argv[])
{
#if (MINEX04)
	char *usage = "usage: minexv [-p] <datafile>";
#elif (OMINEX)
	char *usage = "usage: minexov <datafile>";
#else
	char *usage = "usage: minex2v <datafile>";
#endif
	FILE *fp;
	struct finger_minutiae_record *fmr;
#if (MINEX04)
	int ch;
#endif
	int exit_code = EXIT_SUCCESS;

#if (MINEX04)
	if ((argc != 2) && (argc != 3)) {
#else
	if (argc != 2) {
#endif
		fprintf(stderr, "%s\n", usage);
		exit(EXIT_FAILURE);
	}

#if (MINEX04)
	while ((ch = getopt(argc, argv, "p")) != -1) {
		switch (ch) {
			case 'p':
				piv_quality = 1;
				break;

			default:
				fprintf(stderr, "%s\n", usage);
				exit(EXIT_FAILURE);
				break;
		}
	}
#endif

#if (OMINEX || MINEX2)
	piv_quality=1;
#endif

	fp = fopen(argv[optind], "rb");
	if (fp == NULL) {
		ERRP("Open of %s failed:: %s", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (new_fmr(FMR_STD_ANSI, &fmr) < 0) {
		fprintf(stderr, "could not allocate FMR\n");
		exit(EXIT_FAILURE);
	}

	if (read_fmr(fp, fmr) == READ_OK) {

		printf("-------------------------------\n");
		printf("ANSI/INCITS 378-2004 Validation\n");
		printf("-------------------------------\n");
		if (validate_fmr(fmr) != VALIDATE_OK) {
			fprintf(stdout, 
			    "Finger Minutiae Record is invalid.\n");
			exit_code = EXIT_FAILURE;
		} else {
			fprintf(stdout, 
			    "Finger Minutiae Record is valid.\n");
		}

		printf("----------------\n");
		printf("MINEX Validation\n");
		printf("----------------\n");
		if (minex_verify(fp, fmr) == VALIDATE_OK) {
			fprintf(stdout, "Passes MINEX04 criteria.\n");
		} else {
			fprintf(stdout, "Does not pass MINEX04 criteria.\n");
			exit_code = EXIT_FAILURE;
		}

		// Free the entire FMR
		free_fmr(fmr);

		if (new_fmr(FMR_STD_ANSI, &fmr) < 0) {
			fprintf(stderr, "could not allocate FMR\n");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "Could not read FMR file.\n");
		exit (EXIT_FAILURE);
	}
	exit(exit_code);
}
