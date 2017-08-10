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
/* This program uses the ANSI/INCITS Finger Minutiae Record library to create */
/* a file containing a Finger Minutiae Record based on data contained in      */
/* several text files.                                                        */
/*                                                                            */
/*                                                                            */
/* Parameters to this program:                                                */
/*   -h <header file> : The name of the header file                           */
/*   -f <fvmr file>   : The name of the Finger View Minutia Record file       */
/*   -x <xyt file>    : The name of the Minutia Data file in X-Y-Theta form   */
/*   -o <out file>    : The name of the output file, a raw M1 record          */
/*   -r <rc file>     : The name of the Rdige Count data file (optional)      */
/*   -p               : Print the entire FMR to stdout                        */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <fmr.h>

int total_length = 0;

// Load the M1 header info from a text file and set the fields in the finger 
// minutiae record; note that this function only handles short header lengths
// Header format:
// ID Version Length(dec) CBEFF-ID(hex) Eqpt(hex) X-Size(dec) Y-Size(dec)
// X-Res(dec) Y-Res(dec) Num-Views(dec)
int
load_hdr(FILE *fp, struct finger_minutiae_record *fmr)
{
	unsigned short eqpt;
	unsigned int cbeff_id;

	if (fscanf(fp, "%3c %3c %u %x %hx %hu %hu %hu %hu %hhx",
		fmr->format_id, fmr->spec_version, &fmr->record_length,
		&cbeff_id,
		&eqpt, // The combined equipment fields
		&fmr->x_image_size, &fmr->y_image_size,
		&fmr->x_resolution, &fmr->y_resolution,
		&fmr->num_views) < 0)
			READ_ERR_RETURN("header file");
	
	fmr->scanner_id = eqpt & HDR_SCANNER_ID_MASK;
	fmr->compliance = (eqpt & HDR_COMPLIANCE_MASK) >> HDR_COMPLIANCE_SHIFT;
	fmr->product_identifier_owner = (cbeff_id & HDR_PROD_ID_OWNER_MASK) >>
	    HDR_PROD_ID_OWNER_SHIFT;
	fmr->product_identifier_type = cbeff_id & HDR_PROD_ID_TYPE_MASK;

	// Shift the spec version over by one char and insert the space
	fmr->spec_version[2] = fmr->spec_version[1];
	fmr->spec_version[1] = fmr->spec_version[0];
	fmr->spec_version[3] = 0x00;
	fmr->spec_version[0] = ' ';
	fmr->reserved = 0;

	total_length += FMR_ANSI_SMALL_HEADER_LENGTH;
	return 0;
}

// Load a single finger view from a text file and add to the finger minutiae
// record
int
load_fvmr(FILE *fp, struct finger_minutiae_record *fmr)
{
	struct finger_view_minutiae_record *fvmr;
	unsigned char cval;

	if (new_fvmr(FMR_STD_ANSI, &fvmr) < 0)
		ALLOC_ERR_RETURN("FVMR");

	if (fscanf(fp, "%hhu %hhx %hhu %hhu",
		&fvmr->finger_number, &cval,
		&fvmr->finger_quality, &fvmr->number_of_minutiae) < 0)
			READ_ERR_RETURN("FVMR");
	fvmr->impression_type = cval & FVMR_IMPRESSION_MASK;
	fvmr->view_number = 
		(cval & FVMR_VIEW_NUMBER_MASK) >> FVMR_VIEW_NUMBER_SHIFT;
	add_fvmr_to_fmr(fvmr, fmr);
	total_length += FVMR_HEADER_LENGTH;
	return 0;
}

// Load all the finger minutiae data from a text file and add to the
// finger view 
int
load_fmds(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	int i;
	unsigned int index;
	struct finger_minutiae_data *fmd;

	// File format is, decimal values:
	// index type x y theta quality
	//
	for (i = 0; i < fvmr->number_of_minutiae; i++) {
		if (new_fmd(FMR_STD_ANSI, &fmd, i) < 0)
			ALLOC_ERR_RETURN("FMD for XYT data");

		if (fscanf(fp, "%u %hhu %hu %hu %hhu %hhu",
		    &index, &fmd->type, &fmd->x_coord, &fmd->y_coord,
		    &fmd->angle, &fmd->quality) < 0)
			READ_ERR_RETURN("XYT");
		add_fmd_to_fvmr(fmd, fvmr);
		total_length += FMD_DATA_LENGTH;
	}
	return 0;
}

// Load ridge count data from a text file and add to the finger view
// as an extended data block
// File format is:
// First line: Method Num-of-Ridge-Counts
// Remaining lines: Index1 Index2 Count
//
int
load_rcs(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	struct ridge_count_data *rcd;
	struct finger_extended_data_block *fedb;
	struct finger_extended_data *fed;
	unsigned int length = 0;
	unsigned int i, rcount;

	//XXX replace with a library call when available
	if (fvmr->extended != NULL)
		fedb = fvmr->extended;
	else
		if (new_fedb(FMR_STD_ANSI, &fedb) != 0)
			ALLOC_ERR_RETURN("Extended Data block");

	// Will set length of the FED to 0, for now; will set later
	if (new_fed(FMR_STD_ANSI, &fed, FED_RIDGE_COUNT, 0) != 0)
		ALLOC_ERR_RETURN("Extended Data record");

	if (fscanf(fp, "%hhu %u", &fed->rcdb->method, &rcount) < 0)
		READ_ERR_RETURN("Ridge Count method");
	length = RIDGE_COUNT_HEADER_LENGTH;

	for (i = 0; i < rcount; i++) {
		if (new_rcd(&rcd) != 0)
			ALLOC_ERR_RETURN("Ridge Count record");
		if (fscanf(fp, "%hhu %hhu %hhu",
		    &rcd->index_one, &rcd->index_two, &rcd->count) < 0) {
			if (feof(fp)) {
				break;
			} else {
				READ_ERR_RETURN("Ridge Count record");
			}
		}
		add_rcd_to_rcdb(rcd, fed->rcdb);
		length += RIDGE_COUNT_DATA_LENGTH;
	}

	fed->length = length + FED_HEADER_LENGTH;
	add_fed_to_fedb(fed, fedb);
	fedb->block_length += fed->length;
	total_length += fed->length;

	// XXX replace with library call when available
	if (fvmr->extended == NULL)
		add_fedb_to_fvmr(fedb, fvmr);

	return 0;
}

// Load Core/Delta data from a text file and add to the finger view
// as an extended data block
// File format is:
// First line: Core-Type Num-of-Cores
// Lines 2 .. Number-of-Cores+1 : X-Coord Y-Coord [Core-Angle]
// Line Num-of-Cores+2 : Delta-Type Num-of-Deltas
// Remaining lines: X-Coord Y-Coord [Angle1 Angle2 Angle3]
//
int
load_cds(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	struct finger_extended_data_block *fedb;
	struct finger_extended_data *fed;
	struct core_data *cd;
	struct delta_data *dd;
	int ret;
	int i;
	unsigned int length = 0;

	if (fvmr->extended != NULL)
		fedb = fvmr->extended;
	else
		if (new_fedb(FMR_STD_ANSI, &fedb) != 0)
			ALLOC_ERR_RETURN("Extended Data block");

	// Will set length of the FED to 0, for now; will set later
	if (new_fed(FMR_STD_ANSI, &fed, FED_CORE_AND_DELTA, 0) != 0)
		ALLOC_ERR_RETURN("Extended Data record");

	// Read the Cores first
	if (fscanf(fp, "%hhu %hhu", &fed->cddb->core_type, 
			&fed->cddb->num_cores) < 0)
		READ_ERR_RETURN("Core type and count");

	length = CORE_DATA_HEADER_LENGTH;
	for (i = 0; i < fed->cddb->num_cores; i++) {
		if (new_cd(FMR_STD_ANSI, &cd) != 0)
			ALLOC_ERR_RETURN("Core record");
		// Read the proper number of bytes, based on type of core data
		if (fed->cddb->core_type != CORE_TYPE_NONANGULAR)
			ret = fscanf(fp, "%hu %hu %hhu", 
				&cd->x_coord, &cd->y_coord, &cd->angle);
		else
			ret = fscanf(fp, "%hu %hu", &cd->y_coord, &cd->x_coord);

		if (ret < 0) {
			if (feof(fp)) {
				READ_ERR_RETURN("Core record, premature EOF");
			} else {
				READ_ERR_RETURN("Core record");
			}
		}
		add_cd_to_cddb(cd, fed->cddb);
		if (fed->cddb->core_type != CORE_TYPE_NONANGULAR)
			length += CORE_DATA_MIN_LENGTH + CORE_ANGLE_LENGTH;
		else
			length += CORE_DATA_MIN_LENGTH;
	}

	// Read the Deltas
	if (fscanf(fp, "%hhu %hhu", &fed->cddb->delta_type, 
		&fed->cddb->num_deltas) < 0)
		READ_ERR_RETURN("Delta type and count");

	length += DELTA_DATA_HEADER_LENGTH;
	for (i = 0; i < fed->cddb->num_deltas; i++) {
		if (new_dd(FMR_STD_ANSI, &dd) != 0)
			ALLOC_ERR_RETURN("Delta record");
		// Read the proper number of bytes, based on type of delta data
		if (fed->cddb->delta_type != DELTA_TYPE_NONANGULAR)
			ret = fscanf(fp, "%hu %hu %hhu %hhu %hhu", 
				&dd->x_coord, &dd->y_coord, &dd->angle1,
				&dd->angle2, &dd->angle3);
		else
			ret = fscanf(fp, "%hu %hu", &dd->y_coord, &dd->x_coord);

		if (ret < 0) {
			if (feof(fp)) {
				READ_ERR_RETURN("Delta record, premature EOF");
			} else {
				READ_ERR_RETURN("Delta record");
			}
		}
		add_dd_to_cddb(dd, fed->cddb);
		if (fed->cddb->delta_type != DELTA_TYPE_NONANGULAR)
			length += CORE_DATA_MIN_LENGTH + (3*DELTA_ANGLE_LENGTH);
		else
			length += CORE_DATA_MIN_LENGTH;
	}

	fed->length = length + FED_HEADER_LENGTH;
	add_fed_to_fedb(fed, fedb);
	fedb->block_length += fed->length;
	total_length += fed->length;

	// XXX replace with library call when available
	if (fvmr->extended == NULL)
		add_fedb_to_fvmr(fedb, fvmr);

	return (0);
}

int
main(int argc, char *argv[])
{
	char *usage = "usage:\n\tmkfmr -h <headerfile> -f <fvmrfile> -x <xytfile> -o <datafile>\n\t     [-r <ridgecountfile>] [-c <coredeltafile> [-p]\n\t where -h, -f, -x, and -o are required";

	FILE *out_fp;	// for the output file
	FILE *hdr_fp;	// the header input file
	FILE *fvmr_fp;	// the finger view minutiae record input file
	FILE *xyt_fp;	// the x,y,theta finger minutia data input file
	FILE *rcs_fp;	// the ridge counts input file
	FILE *cds_fp;	// the core/delta data input file
	int h_opt, f_opt, x_opt, r_opt, c_opt, o_opt, p_opt;

	struct finger_minutiae_record *fmr;
	struct finger_view_minutiae_record *fvmr;
	struct stat sb;
	char ch;
	int i;

	h_opt = f_opt = x_opt = r_opt = o_opt = p_opt = c_opt = 0;
	while ((ch = getopt(argc, argv, "h:f:x:r:c:o:p")) != -1) {
		switch (ch) {
		  case 'h':
		    if ((hdr_fp = fopen(optarg, "r")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    h_opt = 1;
		    break;

		  case 'f':
		    if ((fvmr_fp = fopen(optarg, "r")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    f_opt = 1;
		    break;

		  case 'x':
		    if ((xyt_fp = fopen(optarg, "r")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    x_opt = 1;
		    break;

		  case 'r':
		    if ((rcs_fp = fopen(optarg, "r")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    r_opt = 1;
		    break;

		  case 'c':
		    if ((cds_fp = fopen(optarg, "r")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    c_opt = 1;
		    break;

		  case 'o':
		    if (stat(optarg, &sb) == 0) {
			fprintf(stderr, 
			  "File '%s' exists, remove it first.\n", optarg);
			exit(EXIT_FAILURE);
		    }
		    if ((out_fp = fopen(optarg, "wb")) == NULL) {
			OPEN_ERR_EXIT(optarg);
		    }
		    o_opt = 1;
		    break;

		  case 'p':
		    p_opt = 1;
		    break;

		  default:
		    printf("%s\n", usage);
		    exit(EXIT_FAILURE);
		    break;
		}
	}
	if (!(h_opt && f_opt && x_opt && o_opt)) {
		printf("%s\n", usage);
		exit(EXIT_FAILURE);
	}

	// Read in the file containing the header information
	if (new_fmr(FMR_STD_ANSI, &fmr) < 0)
		ALLOC_ERR_EXIT("FMR");

	if (load_hdr(hdr_fp, fmr) != 0) {
		fprintf(stderr, "could not read header\n");
		exit(EXIT_FAILURE);
	}

	// Read in the FVMR data and add to the Finger Minutiae Record
	for (i = 0; i < fmr->num_views; i++) {
		if (load_fvmr(fvmr_fp, fmr) != 0) {
			fprintf(stderr,
			    "Error loading finger view minutiae records\n");
			exit(EXIT_FAILURE);
		}
		// Need to add the extended data block header length because
		// it is always present, even when the length is 0
		total_length += FEDB_HEADER_LENGTH;
	}

	// Read in the minutia data records (x,y,theta)
	// XXX Replace this with library call when available
	TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
		if (load_fmds(xyt_fp, fvmr) != 0) {
			fprintf(stderr, "Error loading the X-Y-Theta data\n");
			exit(EXIT_FAILURE);
		}
	}

	// Read in the ridge counts to create the extended data block
	// XXX Replace this with library call when available
	if (r_opt) {
		TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
			if (load_rcs(rcs_fp, fvmr) != 0) {
				fprintf(stderr, 
					"Error loading the Ridge Counts\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	// Read in the core/delta to into the extended data block
	// XXX Replace this with library call when available
	if (c_opt) {
		TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
			if (load_cds(cds_fp, fvmr) != 0) {
				fprintf(stderr, 
					"Error loading the Core/Delta\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	fmr->record_length = total_length;

	// Validate the FMR
	if (validate_fmr(fmr) != VALIDATE_OK) {
		fprintf(stdout, "Finger Minutiae Record is invalid.\n");
	} else {
		fprintf(stdout, "Finger Minutiae Record is valid.\n");
	}

	if (p_opt)
		print_fmr(stdout, fmr);

	// Write out the FMR
	if (write_fmr(out_fp, fmr) != WRITE_OK) {
		fprintf(stderr, "Error writing the Finger Minutiae Record\n");
	} else {
		fprintf(stdout, "Finger Minutiae Record written.\n");
	}

	// Free the entire FMR
	free_fmr(fmr);

	if (o_opt) 
		fclose(out_fp);
	if (h_opt) 
		fclose(hdr_fp);
	if (f_opt) 
		fclose(fvmr_fp);
	if (x_opt) 
		fclose(xyt_fp);
	if (r_opt) 
		fclose(rcs_fp);

	exit(EXIT_SUCCESS);
}
