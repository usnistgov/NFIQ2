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
/* This program will transform certain values from an INCITS M1 record to     */
/* ISO compact form, then back again. The purpose of this transformation is   */
/* to simulate a fingerprint minutiae record that is converted from M1        */
/* format to ISO compact, as when an FMR is placed on a smart card, then      */
/* converted back for matching, etc.                                          */
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

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
static void
usage()
{
	fprintf(stderr, 
	    "usage:\n"
	    "\tfmrisocompact -i <m1file> -o <outfile>\n"
	    "\twhere:\n"
	    "\t   -i:  Specifies the input M1 file\n"
	    "\t   -o:  Specifies the output M1 file\n");
}

/* Global file pointers */
static FILE *in_fp;	// the FMR (378-2004) input file
static FILE *out_fp;	// for the output file

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
static void
close_files()
{
	if (in_fp != NULL)
		(void)fclose(in_fp);
	in_fp = NULL;
	if (out_fp != NULL)
		(void)fclose(out_fp);
	out_fp = NULL;
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	int ch, i_opt, o_opt;
	char *out_file;
	struct stat sb;

	i_opt = o_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:")) != -1) {
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
				
		    default:
			goto err_usage_out;
		}
	}

	if ((i_opt != 1) || (o_opt != 1))
		goto err_usage_out;

	return;

err_usage_out:
	usage();
err_out:
	close_files();

	/* If we created the output file, remove it. */
	if (o_opt == 1)
		(void)unlink(out_file);
	exit(EXIT_FAILURE);
}

/*
 * Copy one FVMR's header information to another, then copy the minutiae,
 * altering the angle value according to the ISO specification.
 */
static int
modify_fvmr_theta(FVMR *fvmr)
{
	FMD **fmds = NULL;
	int m, mcount;
	double isotheta;
	int theta;

	mcount = get_fmd_count(fvmr);
	if (mcount == 0)
		return (0);

	fmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (fmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(fvmr, fmds) != mcount)
		ERR_OUT("getting FMDs from FVMR");

	for (m = 0; m < mcount; m++) {
		/* The ISO minutia record has 6 bits for the angle, so
		 * we have 64 possible values to represent 360 degrees.
		 */
		theta = 2 * (int)fmds[m]->angle;
		isotheta = round((64.0 / 360.0) * (double)theta);

		/* Now we convert back to 378 format */
		fmds[m]->angle =
		    (unsigned char)round(((360.0 / 64.0) * isotheta) / 2.0);
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
	FMR *ifmr = NULL;
	FVMR **fvmrs = NULL;
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

	// Get all of the finger view records
	rcount = get_fvmr_count(ifmr);
	if (rcount > 0) {
		fvmrs = (FVMR **) malloc(rcount * sizeof(FVMR *));
		if (fvmrs == NULL)
			ALLOC_ERR_OUT("FVMR Array");
		if (get_fvmrs(ifmr, fvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");

		for (r = 0; r < rcount; r++) {
			if (modify_fvmr_theta(fvmrs[r]) < 0)
				ERR_OUT("Modifying minutiae");
		}

	} else {
		if (rcount == 0)
			ERR_OUT("there are no FVMRs in the input FMR");
		else
			ERR_OUT("retrieving FVMRs from input FMR");
	}

	(void)write_fmr(out_fp, ifmr);
	free_fmr(ifmr);

	close_files();

	exit(EXIT_SUCCESS);

err_out:
	if (ifmr != NULL)
		free_fmr(ifmr);

	close_files();

	exit(EXIT_FAILURE);
}
