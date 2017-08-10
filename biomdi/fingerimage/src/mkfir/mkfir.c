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
/* This program uses the ANSI/INCITS Finger Image Record library to           */
/* create a file containing a Finger Image-Base record from on data           */
/* contained in several text files.                                           */
/*                                                                            */
/* Parameters to this program:                                                */
/*   -h <header>      : The name of the header file                           */
/*   -f <fih>         : The name of the finger image header file including    */
/*                      the name of a raster file; multiple instances are     */
/*                      allowed                                               */
/*   -p               : Print the entire FIR to stdout                        */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/param.h>
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
#include <fir.h>

int total_length = 0;

static void
usage()
{
	fprintf(stderr,
	    "usage: mkfir -h <headerfile> -f <fivrfile> [-p] [-to <type>]"
	    " -o <datafile>\n"
	    "\twhere -h, -f, and -o are required\n"
	    "\t -to <type> is one of ISO | ANSI\n");
	exit (EXIT_FAILURE);
}

/******************************************************************************/
/* Load the FIR header info from a text file and set the fields in the finger */
/* image record (FIR).  This is the general record header.                    */
/* The record length will be calculated; value in the file is ignored         */
/******************************************************************************/
int
load_hdr(FILE *fp, struct finger_image_record *fir)
{
	unsigned short eqpt;
	unsigned int cbeff_id;

	// This scan follows the layout in Table 2 of 381-2004
	if (fscanf(fp, "%3c %3c %llu %x %hx %hu %hhu %hhu %hu %hu %hu %hu %hhu %hhu %hu",
	    fir->format_id,
	    fir->spec_version,
	    &fir->record_length,
	    &cbeff_id,
	    &eqpt,
	    &fir->image_acquisition_level,
	    &fir->num_fingers_or_palm_images,
	    &fir->scale_units,
	    &fir->x_scan_resolution,
	    &fir->y_scan_resolution,
	    &fir->x_image_resolution,
	    &fir->y_image_resolution,
	    &fir->pixel_depth,
	    &fir->image_compression_algorithm,
	    &fir->reserved) != 15)
		return (READ_ERROR);
	
	fir->product_identifier_owner = (cbeff_id & HDR_PROD_ID_OWNER_MASK) >>
	    HDR_PROD_ID_OWNER_SHIFT;
	fir->product_identifier_type = cbeff_id & HDR_PROD_ID_TYPE_MASK;
	fir->scanner_id = eqpt & HDR_SCANNER_ID_MASK;
	fir->compliance = (eqpt & HDR_COMPLIANCE_MASK) >> HDR_COMPLIANCE_SHIFT;

	// NULL-terminate the format ID
	fir->format_id[3] = 0x00;

	// NULL-terminate the version number
	fir->spec_version[3] = 0x00;

	if (fir->format_std == FIR_STD_ANSI)
		total_length += FIR_ANSI_HEADER_LENGTH;
	else
		total_length += FIR_ISO_HEADER_LENGTH;
	return (READ_OK);
}

/******************************************************************************/
/* Load a single finger image view record from a text file and add to the     */
/* finger image record.                                                       */
/*                                                                            */
/******************************************************************************/
int
load_fivr(FILE *fp, struct finger_image_record *fir)
{
	struct finger_image_view_record *fivr;
	int ret;
	char filename[MAXPATHLEN];
	char *buf;
	struct stat sb;
	FILE *image_fp;

	if (new_fivr(&fivr) < 0)
		ALLOC_ERR_RETURN("Finger Image View Record");

	// This scan follows the layout of Table 4 in 381-2004
	ret = fscanf(fp, "%u %hhu %hhu %hhu %hhu %hhu %hu %hu %hhu",
		&fivr->length,
		&fivr->finger_palm_position,
		&fivr->count_of_views,
		&fivr->view_number, 
		&fivr->quality,
		&fivr->impression_type,
		&fivr->horizontal_line_length,
		&fivr->vertical_line_length,
		&fivr->reserved);
	if (ret == EOF)
		return (READ_EOF);
	else if (ret != 9)
		return (READ_ERROR);

	if (fscanf(fp, "%s", filename) != 1)
		return (READ_ERROR);

	// Even though we read the length from the header, we set it to
	// the correct size here.
	fivr->length = FIVR_HEADER_LENGTH;

	if (stat(filename, &sb) == 0) {
		if ((image_fp = fopen(filename, "rb")) == NULL) {
			ERRP ("Could not read image file %s", filename);
		}
		else {
			buf = (char *)malloc(sb.st_size);
			if (buf == NULL) {
				ERRP("Could not allocate memory to read image");
			}
			else {
				if (fread(buf, 1, sb.st_size, image_fp) <= 0) {
					ERRP("Could not read image file %s",
					    filename);
				} else {
					fivr->length += sb.st_size;
					add_image_to_fivr(buf, fivr);
				}
			}
		}
	} else {
		ERRP("Could not locate image file %s", filename);
	}

	add_fivr_to_fir(fivr, fir);

	total_length += fivr->length;

	return (READ_OK);
}

int
main(int argc, char *argv[])
{
	FILE *out_fp = NULL;	// for the output file
	FILE *hdr_fp = NULL;	// the header input file
	FILE *fivr_fp = NULL;	// the finger image view record files
	int h_opt, f_opt, o_opt, to_opt, p_opt;
	struct finger_image_record *fir;
	struct stat sb;
	char ch;
	int ret, exit_code;
	int out_type;
	char pm;

	exit_code = EXIT_SUCCESS;
	h_opt = f_opt = o_opt = to_opt = p_opt = 0;
	out_type = FIR_STD_ANSI;
	while ((ch = getopt(argc, argv, "h:f:o:t:p")) != -1) {
		switch (ch) {
			case 'h':
				if ((hdr_fp = fopen(optarg, "r")) == NULL)
					OPEN_ERR_EXIT(optarg);
				h_opt = 1;
				break;
			case 'f':
				if ((fivr_fp = fopen(optarg, "r")) == NULL)
					OPEN_ERR_EXIT(optarg);
				f_opt = 1;
				break;
			case 't':
				pm = *(char *)optarg;
				switch (pm) {
					case 'o':
						out_type = fir_stdstr_to_type(
						    argv[optind]);
						if (out_type < 0)
							usage();
						optind++;
						to_opt++;
						break;
					default:
						usage();
						break;	/* not reached */
				}
				break;
			case 'o':
				if (stat(optarg, &sb) == 0)
					ERR_EXIT("File '%s' exists, remove it first.\n", optarg);

				if ((out_fp = fopen(optarg, "wb")) == NULL)
					OPEN_ERR_EXIT(optarg);
				o_opt = 1;
				break;
			case 'p':
				p_opt = 1;
				break;
			default:
				usage();
				break;
		}
	}
	if ((h_opt && f_opt && o_opt) == 0)
		usage();

	// Read in the file containing the header information
	if (new_fir(out_type, &fir) < 0)
		ALLOC_ERR_EXIT("Finger Image Record (general header)");

	if (load_hdr(hdr_fp, fir) != READ_OK) {
		exit_code = EXIT_FAILURE;
		ERR_OUT("Could not read header");
	}

	// Read in each Finger Image View Record add to the Finger Image Record
	do {
		ret = load_fivr(fivr_fp, fir);
		if (ret == READ_ERROR) {
			exit_code = EXIT_FAILURE;
			ERRP("Reading image view record");
			goto err_out;
		}
	} while (ret == READ_OK);

	fir->record_length = total_length;

	// Validate the Finger Image Record
	if (validate_fir(fir) != VALIDATE_OK) {
		fprintf(stdout, "Finger Image Record is invalid.\n");
	} else {
		fprintf(stdout, "Finger Image Record is valid.\n");
	}

	if (p_opt)
		print_fir(stdout, fir);

	// Write out the FIR
	if (write_fir(out_fp, fir) != WRITE_OK) {
		fprintf(stderr, "Error writing the Finger Image Record\n");
	} else {
		fprintf(stdout, "Finger Image Record written.\n");
	}

err_out:
	free_fir(fir);

	if (o_opt) 
		fclose(out_fp);
	if (h_opt) 
		fclose(hdr_fp);

	if (f_opt)
		fclose(fivr_fp);

	exit(exit_code);
}
