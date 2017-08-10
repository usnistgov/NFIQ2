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
/* This program will sort the minutiae information from a FMR record and      */
/* produce an output file containg the sorted set.                            */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <biomdimacro.h>
#include <fmr.h>
#include <fmrsort.h>

#define SORT_METHOD_POLAR	1
#define SORT_METHOD_RANDOM	2
#define SORT_METHOD_XY		3
#define SORT_METHOD_YX		4
#define SORT_METHOD_ANGLE	5

#define SORT_ORDER_ASCENDING	1
#define SORT_ORDER_DESCENDING	2

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
static void
usage()
{
	fprintf(stderr, 
	    "usage:\n"
	    "\tfmrsort -i <m1file> -o <outfile> -mp [-r]\n"
	    "\tor\n"
	    "\tfmrsort -i <m1file> -o <outfile> -mr [-r]\n"
	    "\tor\n"
	    "\tfmrsort -i <m1file> -o <outfile> -mx [-r]\n"
	    "\tor\n"
	    "\tfmrsort -i <m1file> -o <outfile> -my [-r]\n"
	    "\tor\n"
	    "\tfmrsort -i <m1file> -o <outfile> -ma [-r]\n"
	    "\twhere:\n"
	    "\t   -i:  Specifies the input file\n"
	    "\t   -o:  Specifies the output file\n"
	    "\t   -mp: Sort using the polar method\n"
	    "\t   -mr: Sort using the random method\n"
	    "\t   -mx: Sort using the Cartesian X-Y method\n"
	    "\t   -my: Sort using the Cartesian Y-X method\n"
	    "\t   -ma: Sort using the Angle method\n"
	    "\t   -r:  Reverse the sort order to descending\n");
}

/*  */
static int sort_method;
static int sort_order = SORT_ORDER_ASCENDING;

/* Keep track of the entire FMR length in a global. */
static unsigned int fmr_length;

/* Global file pointers */
static FILE *in_fp = NULL;	// the FMR input file
static FILE *out_fp = NULL;	// for the output file

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

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	int ch, i_opt, o_opt, m_opt;
	char pm, *out_file;
	struct stat sb;

	i_opt = o_opt = m_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:m:r")) != -1) {
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
				
		    case 'm':
			pm = *(char *)optarg;
			switch (pm) {
			    case 'p':
				sort_method = SORT_METHOD_POLAR;
				break;
			    case 'r':
				sort_method = SORT_METHOD_RANDOM;
				break;
			    case 'x':
				sort_method = SORT_METHOD_XY;
				break;
			    case 'y':
				sort_method = SORT_METHOD_YX;
				break;
			    case 'a':
				sort_method = SORT_METHOD_ANGLE;
				break;
			    default:
				goto err_usage_out;
				break;
			}
			m_opt++;
			break;

		    case 'r':
			sort_order = SORT_ORDER_DESCENDING;
			break;

		    default:
			goto err_usage_out;
			break;
		}
	}

	/* Check the common required options */
	if ((i_opt != 1) || (o_opt != 1) || (m_opt != 1))
		goto err_usage_out;

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

/*
 * Copy one FVMR's header information to another, then sort and copy
 * the minutiae.
 */
#define DUP_FMD							\
	do {							\
		if (new_fmd(src->format_std, &ofmd, m) < 0)	\
			ALLOC_ERR_EXIT("Output FMD");		\
		COPY_FMD(fmds[m], ofmd);			\
		fmr_length += FMD_DATA_LENGTH;			\
		add_fmd_to_fvmr(ofmd, dst);			\
	} while (0)

static int
sort_and_copy_fvmr(FVMR *src, FVMR *dst)
{
	FMD **fmds, *ofmd = NULL;
	int m, mcount;

	COPY_FVMR(src, dst);

	/* XXX We don't handle extended data yet. */
	dst->extended = NULL;
	fmr_length += FEDB_HEADER_LENGTH;

	mcount = get_fmd_count(src);
	if (mcount == 0)
		return (0);

	fmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (fmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(src, fmds) != mcount)
		ERR_OUT("getting FMDs from FVMR");

	switch (sort_method) {
	    case SORT_METHOD_POLAR:
		sort_fmd_by_polar(fmds, mcount, 0, 0, TRUE);
		break;

	    case SORT_METHOD_RANDOM:
		sort_fmd_by_random(fmds, mcount);
		break;

	    case SORT_METHOD_XY:
		sort_fmd_by_xy(fmds, mcount);
		break;

	    case SORT_METHOD_YX:
		sort_fmd_by_yx(fmds, mcount);
		break;

	    case SORT_METHOD_ANGLE:
		sort_fmd_by_angle(fmds, mcount);
		break;
	}

	if (sort_order == SORT_ORDER_ASCENDING)
		for (m = 0; m < mcount; m++)
			DUP_FMD;
	else
		for (m = mcount - 1; m >= 0; m--)
			DUP_FMD;

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

	/* XXX allow other record types */
	if (new_fmr(FMR_STD_ANSI, &ifmr) < 0)
		ALLOC_ERR_OUT("Input FMR");

	if (read_fmr(in_fp, ifmr) != READ_OK)
		ERR_OUT("Could not read FMR from file");

	/* XXX allow other record types */
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
			if (sort_and_copy_fvmr(fvmrs[r], ofvmr) < 0)
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
