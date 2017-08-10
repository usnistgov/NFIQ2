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
/* This program will prune the minutiae information from a M1 record and      */
/* produce an output file containg the specified amount of minutiae.          */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <fmr.h>
#include <fmrsort.h>

#include "fmrprune.h"

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
static void
usage()
{
	fprintf(stderr, 
	    "usage:\n"
	    "\tfmrprune -i <m1file> -o <outfile> -n <num> -mp\n"
	    "\tor\n"
	    "\tfmrprune -i <m1file> -o <outfile> -n <num> -mr\n"
	    "\tor\n"
	    "\tfmrprune -i <m1file> -o <outfile> -n <num> -me "
		"-a <val> -b <val>\n"
	    "\tor\n"
	    "\tfmrprune -i <m1file> -o <outfile> -ml "
		"-c <coord> -a <val> -b <val>\n"
	    "\twhere:\n"
	    "\t   -i:  Specifies the input M1 file\n"
	    "\t   -o:  Specifies the output M1 file\n"
	    "\t   -n:  Specifies the number of minutiae\n"
	    "\t   -mp: Prune using the polar method\n"
	    "\t   -me: Prune using the elliptical method\n"
	    "\t\t -a:  Semiminor axis length (i.e. width)\n"
	    "\t\t -b:  Semimajor axis length (i.e. height)\n"
	    "\t   -mr: Prune using the random method\n"
	    "\t   -ml: Prune using the rectangular method\n"
	    "\t\t -c: Upper-left coordinate for the rectangle\n"
	    "\t\t -a -b: width and height values for the rectangle\n");
}

/*  */
static int prune_method;

/* Values of semimajor/semiminor ellipse axes, or rectangle side lengths */
static int a, b;

/* Upper left coordinate of the rectangle */
static int x, y;

/* Keep track of the entire FMR length in a global. */
static unsigned int fmr_length;

/* Global file pointers */
static FILE *in_fp = NULL;	// the FMR (378-2004) input file
static FILE *out_fp = NULL;	// for the output file

static long selected_minutiae_count;

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
static void
close_files()
{
	if (in_fp != NULL)
		(void)fclose(in_fp);
	if (out_fp != NULL)
		(void)fclose(out_fp);
}

#define MAX_COORD_LENGTH	13
static int
get_xy(const char *str)
{
	char *cptr;
	char buf[32];
	if (strlen(str) > MAX_COORD_LENGTH)
		return (-1);
	cptr = strchr(str, ',');
	if (cptr == NULL)
		return (-1);

	*cptr = 0;
	strcpy(buf, str);
	x = strtol(buf, NULL, 10);
	if (x == 0 && errno == EINVAL)
		return (-1);
	if (x < 0)
		return (-1);
	strcpy(buf, cptr + 1);
	y = strtol(buf, NULL, 10);
	if (y == 0 && errno == EINVAL)
		return (-1);
	if (y < 0)
		return (-1);

	return (0);
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	int ch, i_opt, o_opt, n_opt, m_opt, a_opt, b_opt, c_opt;
	char pm, *out_file;
	struct stat sb;

	i_opt = o_opt = n_opt = m_opt = a_opt = b_opt = c_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:n:m:a:b:c:")) != -1) {
		switch (ch) {
		    case 'i':
			if ((in_fp = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			i_opt++;
			break;

		    case 'o':
			if (o_opt == 1)
				goto err_usage_out;
			if (stat(optarg, &sb) == 0) {
		    	    ERR_OUT(
				"File '%s' exists, remove it first.", optarg);
			}
			if ((out_fp = fopen(optarg, "wb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			out_file = optarg;
			o_opt++;
			break;
				
		    case 'n':
			selected_minutiae_count = strtol(optarg, NULL, 10);
			if (selected_minutiae_count == 0 && errno == EINVAL)
				ERR_OUT("Number of minutiae must be numeric");
			if (selected_minutiae_count < 0)
				ERR_OUT("Number of minutiae must be greater than 0");
			n_opt++;
			break;

		    case 'm':
			pm = *(char *)optarg;
			switch (pm) {
			    case 'p':
				prune_method = PRUNE_METHOD_POLAR;
				break;
			    case 'e':
				prune_method = PRUNE_METHOD_ELLIPTICAL;
				break;
			    case 'r':
				prune_method = PRUNE_METHOD_RANDOM;
				break;
			    case 'l':
				prune_method = PRUNE_METHOD_RECTANGULAR;
				break;
			    default:
				goto err_usage_out;
				break;
			}
			m_opt++;
			break;

		    case 'a':
			a = strtol(optarg, NULL, 10);
			if (a == 0 && errno == EINVAL)
				ERR_OUT("Value of a must be numeric");
			if (a <= 0)
				ERR_OUT("Value of a must be > 0");
			a_opt++;
			break;

		    case 'b':
			b = strtol(optarg, NULL, 10);
			if (b == 0 && errno == EINVAL)
				ERR_OUT("Value of b must be numeric");
			if (b <= 0)
				ERR_OUT("Value of b must be > 0");
			b_opt++;
			break;

		    case 'c':
			if (get_xy(optarg) != 0)
				ERR_OUT("Invalid coordinate specification");
			c_opt++;
			break;

		    default:
			goto err_usage_out;
			break;
		}
	}

	/* Check the common required options */
	if ((i_opt != 1) || (o_opt != 1) || (m_opt != 1))
		goto err_usage_out;

	switch(prune_method) {
	    case PRUNE_METHOD_POLAR:
	    case PRUNE_METHOD_RANDOM:
		if (n_opt != 1)
			goto err_usage_out;
		break;
	    case PRUNE_METHOD_ELLIPTICAL:
		if ((n_opt != 1) || (a_opt != 1) || (b_opt != 1))
			goto err_usage_out;
		if (a > b)
			ERR_OUT("Value of a cannot be greater than b");
		break;
	    case PRUNE_METHOD_RECTANGULAR:
		if ((a_opt != 1) || (b_opt != 1) || (c_opt != 1))
			goto err_usage_out;
		break;
	}

	return;

err_usage_out:
	usage();
err_out:
	/* If we created the output file, remove it. */
	if (o_opt == 1)
		(void)unlink(out_file);
	close_files();
	exit(EXIT_FAILURE);
}

static int
compare_fmd_by_index(const void *m1, const void *m2)
{
	FMD *lm1, *lm2;
	lm1 = (FMD *)m1;
	lm2 = (FMD *)m2;
	if (lm1->index == lm2->index)	/* Should not happen */
		return (0);
	else if (lm1->index < lm2->index)
		return (-1);
	else
		return (1);
}

/*
 * Copy one FVMR's header information to another, then selectively copy
 * the minutiae. Parameter mcount is the requested number of minutiae.
 */
static int
copy_and_select_fvmr(FVMR *src, FVMR *dst, int mcount)
{
	FMD **fmds, *ofmd = NULL;
	int m, num;

	/* Copy the header information from the source FVMR */
	bcopy(&src->fvmr_startcopy, &dst->fvmr_startcopy,
            (unsigned) ((uint8_t *)&dst->fvmr_endcopy -
		(uint8_t *)&dst->fvmr_startcopy));
	/* Reset the minutiae count in the output as it will be incremented
	 * based on what minutiae meet the selection criteria.
	 */
	dst->number_of_minutiae = 0;

	dst->extended = NULL;
	fmr_length += FEDB_HEADER_LENGTH;

	num = get_fmd_count(src);
	if (num == 0)
		return (0);

	fmds = (FMD **)malloc(num * sizeof(FMD *));
	if (fmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(src, fmds) != num)
		ERR_OUT("getting FMDs from FVMR");

	switch (prune_method) {
	    case PRUNE_METHOD_POLAR:
		/* If more minutiae are requested than exist, return
		   what exists. Otherwise, sort the minutiae and return
		   the selected amount.  */
		if (mcount > num)
			mcount = num;
		else
			sort_fmd_by_polar(fmds, num, 0, 0, TRUE);
		break;

	    case PRUNE_METHOD_ELLIPTICAL:
		select_fmd_by_elliptical(fmds, &num, a, b);
		if (num < mcount)	// We may have less minutiae than asked
			mcount = num;
		break;

	    case PRUNE_METHOD_RANDOM:
		if (mcount > num)
			mcount = num;
		else
			sort_fmd_by_random(fmds, num);
		break;

	    case PRUNE_METHOD_RECTANGULAR:
		select_fmd_by_rectangular(fmds, &num, x, y, a, b);
		mcount = num;
		break;

	}

	qsort(fmds, mcount, sizeof(FMD *), compare_fmd_by_index);
	for (m = 0; m < mcount; m++) {
		if (new_fmd(FMR_STD_ANSI, &ofmd, m) < 0)
			ALLOC_ERR_EXIT("Output FMD");
		COPY_FMD(fmds[m], ofmd);
		fmr_length += FMD_DATA_LENGTH;
		add_fmd_to_fvmr(ofmd, dst);
		dst->number_of_minutiae++;
	}

	free(fmds);
	return (0);

err_out:
	if (fmds != NULL)
		free(fmds);
	return (-1);
}

int
main(int argc, char *argv[])
{
	FMR *ifmr, *ofmr = NULL;
	FVMR **fvmrs = NULL;
	FVMR *ofvmr = NULL;
	int r, rcount;

	get_options(argc, argv);

	// Allocate the FMR record in memory
	if (new_fmr(FMR_STD_ANSI, &ifmr) < 0)
		ALLOC_ERR_OUT("Input FMR");

	// Read the FMR
	if (read_fmr(in_fp, ifmr) != READ_OK) {
		fprintf(stderr, "Could not read FMR from file.\n");
		goto err_out;
	}

	if (new_fmr(FMR_STD_ANSI, &ofmr) < 0)
		ALLOC_ERR_OUT("Output FMR");
	COPY_FMR(ifmr, ofmr);
	if (ifmr->record_length_type == FMR_ANSI_SMALL_HEADER_TYPE)
		fmr_length = FMR_ANSI_SMALL_HEADER_LENGTH;
	else
		fmr_length = FMR_ANSI_LARGE_HEADER_LENGTH;

	// Get all of the finger view records
	rcount = get_fvmr_count(ifmr);
	if (rcount > 0) {
		fvmrs = (FVMR **) malloc(rcount * sizeof(FVMR *));
		if (fvmrs == NULL)
			ALLOC_ERR_OUT("FVMR Array");
		if (get_fvmrs(ifmr, fvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");

		for (r = 0; r < rcount; r++) {
			if (new_fvmr(FMR_STD_ANSI, &ofvmr) < 0)
				ALLOC_ERR_OUT("Output FVMR");
			if (copy_and_select_fvmr(fvmrs[r], ofvmr,
			    selected_minutiae_count) < 0)
				ERR_OUT("Selecting minutiae");
			add_fvmr_to_fmr(ofvmr, ofmr);
			fmr_length += FVMR_HEADER_LENGTH;
		}
		free(fvmrs);

	} else {
		if (rcount == 0)
			ERR_OUT("there are no FVMRs in the input FMR");
		else
			ERR_OUT("retrieving FVMRs from input FMR");
	}

	free_fmr(ifmr);
	ofmr->record_length = fmr_length;
	(void)write_fmr(out_fp, ofmr);
	free_fmr(ofmr);

	close_files();

	exit(EXIT_SUCCESS);

err_out:
	if (ifmr != NULL)
		free_fmr(ifmr);
	if (ofmr != NULL)
		free_fmr(ofmr);

	if (fvmrs != NULL)
		free(fvmrs);

	close_files();

	exit(EXIT_FAILURE);
}
