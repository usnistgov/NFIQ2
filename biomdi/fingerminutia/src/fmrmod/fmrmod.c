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
/* This program will modify the minutiae information from a 378 M1 record and */
/* produce an output file containing the modified minutiae.                   */
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
#include <strings.h>
#include <unistd.h>

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
	    "\tfmrmod -i <infile> -o <outfile> -q <qual>\n"
	    "\twhere:\n"
	    "\t   -i:  Specifies the input M1 file\n"
	    "\t   -o:  Specifies the output M1 file\n"
	    "\t   -q:  Specifies the quality value to write into the minutiae\n"
	);
}

/*  */
/* Global file pointers */
static FILE *in_fp = NULL;
static FILE *out_fp = NULL;

static FMD fmd_mods;	/* Contains the values of the modified fields */

#define FMD_MOD_TYPE	0x01
#define FMD_MOD_X	0x02
#define FMD_MOD_Y	0x04
#define FMD_MOD_ANGLE	0x08
#define FMD_MOD_QUALITY	0x10

static uint8_t fmd_mod_flags;

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
	int ch, i_opt, o_opt, q_opt;
	char *out_file;
	struct stat sb;
	long qval;

	i_opt = o_opt = q_opt = 0;
	fmd_mod_flags = 0;
	while ((ch = getopt(argc, argv, "i:o:q:")) != -1) {
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
				
		    case 'q':
			qval = strtol(optarg, NULL, 10);
			if (qval == 0 && errno == EINVAL)
				ERR_OUT("Quality value must be numeric");
			if (qval < 0)
				ERR_OUT("Quality value must be greater than 0");
			fmd_mods.quality = (unsigned char)qval;
			fmd_mod_flags |= FMD_MOD_QUALITY;
			q_opt++;
			break;

		    default:
			goto err_usage_out;
			break;
		}
	}

	/* Check the common required options */
	if ((i_opt != 1) || (o_opt != 1) || (q_opt != 1))
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
 * Copy one FVMR's header information to another, then copy and 
 * modify the minutiae.
 */
static int
copy_and_modify_fvmr(FVMR *src, FVMR *dst)
{
	FMD **fmds, *ofmd = NULL;
	int m, mcount;

	COPY_FVMR(src, dst);

	/* For simplicity, just move the extended data block from src to dst */
	dst->extended = src->extended;
	src->extended = NULL;
	if (dst->extended != NULL)
		dst->extended->fvmr = dst;

	mcount = get_fmd_count(src);
	if (mcount == 0)
		return (0);

	fmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (fmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(src, fmds) != mcount)
		ERR_OUT("getting FMDs from FVMR");

	for (m = 0; m < mcount; m++) {  
		if (new_fmd(FMR_STD_ANSI, &ofmd, m) < 0)
				ALLOC_ERR_EXIT("Output FMD");
		COPY_FMD(fmds[m], ofmd);
		if (fmd_mod_flags & FMD_MOD_QUALITY)
			ofmd->quality = fmd_mods.quality;
		add_fmd_to_fvmr(ofmd, dst);
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

	if (new_fmr(FMR_STD_ANSI, &ifmr) < 0)
		ALLOC_ERR_OUT("Input FMR");

	if (read_fmr(in_fp, ifmr) != READ_OK) {
		fprintf(stderr, "Could not read FMR from file.\n");
		goto err_out;
	}

	if (new_fmr(FMR_STD_ANSI, &ofmr) < 0)
		ALLOC_ERR_OUT("Output FMR");
	COPY_FMR(ifmr, ofmr);

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
			if (copy_and_modify_fvmr(fvmrs[r], ofvmr) < 0)
				ERR_OUT("Selecting minutiae");
			add_fvmr_to_fmr(ofvmr, ofmr);
		}
		free(fvmrs);

	} else {
		if (rcount == 0)
			ERR_OUT("There are no FVMRs in the input FMR");
		else
			ERR_OUT("Retrieving FVMRs from input FMR");
	}

	free_fmr(ifmr);
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
