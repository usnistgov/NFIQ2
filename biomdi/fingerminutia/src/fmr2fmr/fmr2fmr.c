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
/* This program will transform a finger minutia record in one format to       */
/* another, ANSI to ISO compact for for example. The type of input and        */
/* output files are given on the command line.                                */
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
#include <fmr2fmr.h>

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
static void
usage()
{
	fprintf(stderr, 
	    "usage:\n"
	    "\tfmr2fmr -i <infile> -ti <type> -o <outfile> -to <type>\n"
	    "\t\t[-rx <res> -ry <res>]\n"
	    "\twhere:\n"
	    "\t   -i:  Specifies the input FMR file\n"
	    "\t   -ti: Specifies the input file type\n"
	    "\t   -o:  Specifies the output FMR file\n"
	    "\t   -to: Specifies the output file type\n"
	    "\t   -rx: Specifies the X resolution for ISO card formats\n"
	    "\t   -ry: Specifies the Y resolution for ISO card formats\n"
	    "\t   <type> is one of ISO | ISONC | ISOCC | ANSI | ANSI07\n");
}

/* Global file pointers */
static FILE *in_fp;	// the FMR input file
static FILE *out_fp;	// for the output file
static char *out_file;

static int in_type;	// Standard type of the input file
static int out_type;	// Standard type of the output file
static unsigned short iso_xres, iso_yres;	// X/Y resolution for ISO NC/CC

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
/* Map the string given for the record format type into an integer.           */
/* Return -1 on if no match.                                                  */
/******************************************************************************/
static int
stdstr_to_type(char *stdstr)
{
	if (strcmp(stdstr, "ANSI") == 0)
		return (FMR_STD_ANSI);
	if (strcmp(stdstr, "ANSI07") == 0)
		return (FMR_STD_ANSI07);
	if (strcmp(stdstr, "ISO") == 0)
		return (FMR_STD_ISO);
	if (strcmp(stdstr, "ISONC") == 0)
		return (FMR_STD_ISO_NORMAL_CARD);
	if (strcmp(stdstr, "ISOCC") == 0)
		return (FMR_STD_ISO_COMPACT_CARD);
	return (-1);
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	int ch, i_opt, o_opt, ti_opt, to_opt, rx_opt, ry_opt;
	char pm;
	struct stat sb;

	i_opt = o_opt = ti_opt = to_opt = rx_opt = ry_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:t:r:")) != -1) {
		/* Make sure we don't fall off the end of argv */
		if (optind > argc)
			goto err_usage_out;
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
				
		    case 't':
			if (argv[optind] == NULL)
				goto err_usage_out;
			pm = *(char *)optarg;
			switch (pm) {
			    case 'i':
				in_type = stdstr_to_type(argv[optind]);
				if (in_type < 0)
					goto err_usage_out;
				optind++;
				ti_opt++;
				break;
			    case 'o':
				out_type = stdstr_to_type(argv[optind]);
				if (out_type < 0)
					goto err_usage_out;
				optind++;
				to_opt++;
				break;
			}
			break;

		    case 'r':
			if (argv[optind] == NULL)
				goto err_usage_out;
			pm = *(char *)optarg;
			switch (pm) {
			    case 'x':
				iso_xres = strtol(argv[optind], NULL, 10);
			        if (iso_xres == 0 && errno == EINVAL)
					goto err_usage_out;
				optind++;
				rx_opt++;
				break;
			    case 'y':
				iso_yres = strtol(argv[optind], NULL, 10);
			        if (iso_yres == 0 && errno == EINVAL)
					goto err_usage_out;
				optind++;
				ry_opt++;
				break;
			}
			break;

		    default:
			goto err_usage_out;
		}
	}

	if ((i_opt != 1) || (o_opt != 1) || (ti_opt != 1) || (to_opt != 1))
		goto err_usage_out;

	/* ISO card formats have no input resolution, so require the
	 * program to be called with the X and Y resolution parameters.
	 */
	if ((in_type == FMR_STD_ISO_NORMAL_CARD) ||
	    (in_type == FMR_STD_ISO_COMPACT_CARD))
		if ((rx_opt != 1) || (ry_opt != 1))
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
 * Copy an FMR without any conversion. This procedure goes through the process
 * of reading the FMR and copying it using the FMR library. The reason for
 * this is to verify that the FMR is valid.
 */
static int
copy_without_conversion(FMR *ifmr, FMR *ofmr, int fmr_type)
{
	FVMR *ofvmr;
	FVMR **ifvmrs = NULL;
	FMD **ifmds = NULL;
	FMD *ofmd;
	int r, rcount, m, mcount;
	int retval;

	retval = -1;			/* Assume failure, for now */

	COPY_FMR(ifmr, ofmr);

	/* Get all of the finger view records */
	rcount = get_fvmr_count(ifmr);
	if (rcount > 0) {
		ifvmrs = (FVMR **) malloc(rcount * sizeof(FVMR *));
		if (ifvmrs == NULL)
			ALLOC_ERR_OUT("FVMR Array");
		if (get_fvmrs(ifmr, ifvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");

		for (r = 0; r < rcount; r++) {
			if (new_fvmr(fmr_type, &ofvmr) < 0)
	                        ALLOC_ERR_RETURN("Output FVMR");

			COPY_FVMR(ifvmrs[r], ofvmr);
			mcount = get_fmd_count(ifvmrs[r]);
			if (mcount != 0) {
				ifmds = (FMD **)malloc(mcount * sizeof(FMD *));
				if (ifmds == NULL)
					ALLOC_ERR_RETURN("FMD array");
				if (get_fmds(ifvmrs[r], ifmds) != mcount)
					ERR_OUT("getting FMDs from FVMR");

				for (m = 0; m < mcount; m++) {
					if (new_fmd(FMR_STD_ISO, &ofmd, m) != 0)
						ALLOC_ERR_RETURN("Output FMD");
					COPY_FMD(ifmds[m], ofmd);
					add_fmd_to_fvmr(ofmd, ofvmr);
				}
			}

			/* Subtract off the length of the extended data block,
			 * if present, because we don't copy extended data yet.
			 */
			if (ifvmrs[r]->extended != NULL) {
				ofmr->record_length -=
				    ifvmrs[r]->extended->block_length;
				
			}
			add_fvmr_to_fmr(ofvmr, ofmr);
			// XXX Copy the FEDB to the output fmr
		}

	} else {
		if (rcount == 0)
			ERR_OUT("there are no FVMRs in the input FMR");
		else
			ERR_OUT("retrieving FVMRs from input FMR");
	}
	retval = 0;

err_out:
	if (ifvmrs != NULL)
		free (ifvmrs);
	if (ifmds != NULL)
		free (ifmds);
	return (retval);
}

/*
 * Copy an FMR with conversion.
 */
static int
copy_with_conversion(FMR *ifmr, FMR *ofmr, int in_type, int out_type)
{
	FVMR *ofvmr;
	FVMR **ifvmrs = NULL;
	int r, rcount;
	unsigned int fmr_len, fvmr_len;
	int rc, retval;
	char *ver;

	retval = -1;			/* Assume failure, for now */

	if (in_type == out_type)
		return (-1);

	COPY_FMR(ifmr, ofmr);
	switch (out_type) {
	case FMR_STD_ANSI:
		fmr_len = FMR_ANSI_SMALL_HEADER_LENGTH;
		ver = FMR_ANSI_SPEC_VERSION;
		break;
	case FMR_STD_ANSI07:
		fmr_len = FMR_ANSI07_HEADER_LENGTH;
		ver = FMR_ANSI07_SPEC_VERSION;
		break;
	case FMR_STD_ISO:
		fmr_len = FMR_ISO_HEADER_LENGTH;
		ver = FMR_ISO_SPEC_VERSION;
		break;
	}

	/* Fix up the output FMR header for those input types that don't
	 * have all the needed information.
	 */
	
	if ((out_type == FMR_STD_ANSI) || (out_type == FMR_STD_ISO) ||
	    (out_type == FMR_STD_ANSI07))
		if ((in_type == FMR_STD_ISO_NORMAL_CARD) ||
		    (in_type == FMR_STD_ISO_COMPACT_CARD)) {
			strncpy(ofmr->format_id, FMR_FORMAT_ID,
			    FMR_FORMAT_ID_LEN);
			strncpy(ofmr->spec_version, ver,
			    FMR_SPEC_VERSION_LEN);
		}

	/* Get all of the finger view records */
	rcount = get_fvmr_count(ifmr);
	if (rcount > 0) {
		ifvmrs = (FVMR **) malloc(rcount * sizeof(FVMR *));
		if (ifvmrs == NULL)
			ALLOC_ERR_OUT("FVMR Array");
		if (get_fvmrs(ifmr, ifvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");

		for (r = 0; r < rcount; r++) {
			if (new_fvmr(out_type, &ofvmr) < 0)
	                        ALLOC_ERR_RETURN("Output FVMR");

			rc = -1;
			switch (in_type) {
			    case FMR_STD_ANSI07:
				/* The coord and resolution info is stored
				 * in the FVMR; copy it to the FMR header,
				 * using the first FVMR as the canonical set.
				 */
				ofmr->x_image_size = ifvmrs[0]->x_image_size;
				ofmr->y_image_size = ifvmrs[0]->y_image_size;
				ofmr->x_resolution = ifvmrs[0]->x_resolution;
				ofmr->y_resolution = ifvmrs[0]->y_resolution;
			    case FMR_STD_ANSI:
				switch (out_type) {
				case FMR_STD_ISO:
				case FMR_STD_ISO_NORMAL_CARD:
					rc = ansi2iso_fvmr(ifvmrs[r], ofvmr,
					    &fvmr_len, ifmr->x_resolution,
					    ifmr->y_resolution);
					break;
				case FMR_STD_ISO_COMPACT_CARD:
					rc = ansi2isocc_fvmr(ifvmrs[r], ofvmr,
					    &fvmr_len, ifmr->x_resolution,
					    ifmr->y_resolution);
					break;
				default:
					ERR_OUT("Invalid output type");
				}
				break;

			    /* XXX Eventually handle ISO->ISO conversions */
			    case FMR_STD_ISO:
				/* For ANSI07, the coord and resolution info
				 * is copied by iso2ansi_fvmr().
				 */
			    case FMR_STD_ISO_NORMAL_CARD:
				rc = iso2ansi_fvmr(ifvmrs[r], ofvmr, &fvmr_len,
				    ifmr->x_resolution, ifmr->y_resolution);
				break;
			    case FMR_STD_ISO_COMPACT_CARD:
				rc = isocc2ansi_fvmr(ifvmrs[r], ofvmr,
				    &fvmr_len, ifmr->x_resolution,
				    ifmr->y_resolution);
				break;
			}
			if (rc != 0)
				ERR_OUT("Modifying FVMR");

			fmr_len += fvmr_len;
			// XXX Copy the FEDB to the output fmr
			ofvmr->extended = NULL;
			add_fvmr_to_fmr(ofvmr, ofmr);

			fmr_len += FEDB_HEADER_LENGTH;
		}

	} else {
		if (rcount == 0)
			ERR_OUT("there are no FVMRs in the input FMR");
		else
			ERR_OUT("retrieving FVMRs from input FMR");
	}

	ofmr->record_length = fmr_len;
	retval = 0;
err_out:
	if (ifvmrs != NULL)
		free (ifvmrs);
	return (retval);
}

int
main(int argc, char *argv[])
{
	FMR *ifmr = NULL, *ofmr = NULL;

	get_options(argc, argv);

	if ((in_type == FMR_STD_ISO) ||
	    (in_type == FMR_STD_ISO_NORMAL_CARD) ||
	    (in_type == FMR_STD_ISO_COMPACT_CARD))
		if ((out_type == FMR_STD_ISO) ||
		    (out_type == FMR_STD_ISO_NORMAL_CARD) ||
		    (out_type == FMR_STD_ISO_COMPACT_CARD))
			ERR_OUT("Unsupported conversion");

	// Allocate the input/output FMR records in memory
	if (new_fmr(in_type, &ifmr) < 0)
		ALLOC_ERR_OUT("Input FMR");
	if (new_fmr(out_type, &ofmr) < 0)
		ALLOC_ERR_OUT("Output FMR");

	// Read the input FMR
	if (read_fmr(in_fp, ifmr) != READ_OK) {
		fprintf(stderr, "Could not read FMR from file.\n");
		goto err_out;
	}

	/* ISO card formats have no input resolution, so set it here
	 * from the input options.
	 */
	if ((in_type == FMR_STD_ISO_NORMAL_CARD) ||
	    (in_type == FMR_STD_ISO_COMPACT_CARD)) {
		ifmr->x_resolution = iso_xres;
		ifmr->y_resolution = iso_yres;
	}
	/* If the input and output file types are the same,
	 * do a straight copy.
	 */
	if (in_type == out_type)
		copy_without_conversion(ifmr, ofmr, in_type);
	else
		copy_with_conversion(ifmr, ofmr, in_type, out_type);

	(void)write_fmr(out_fp, ofmr);
	free_fmr(ifmr);
	free_fmr(ofmr);

	close_files();

	exit(EXIT_SUCCESS);

err_out:
	if (ifmr != NULL)
		free_fmr(ifmr);
	if (ofmr != NULL)
		free_fmr(ofmr);
	/* If we created the output file, remove it. */
	if (out_fp != NULL)
		(void)unlink(out_file);

	close_files();
	exit(EXIT_FAILURE);
}
