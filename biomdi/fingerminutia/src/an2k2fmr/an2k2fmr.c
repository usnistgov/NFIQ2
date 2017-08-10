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
/* This program will convert an ANSI/NIST Type 9 record embedded in a         */
/* ANSI/NIST data file into a ANSI/INCITS 378-2004 Finger Minutiae Record     */
/* contained within a M1 FMR data file.                                       */
/*                                                                            */
/* Each Type-9 record in the AN2K file is mapped into a pair of Finger        */
/* Finger Minutiae Record and Finger View Minutiae Records in the output      */
/* M1 file. To construct the FMR record, the AN2K image record (Type-3,4,5,   */
/* 6, or 13) is searched for based on the Type-9 IDC value. If an image       */
/* record does not exist, reasonable values are subsitituted.                 */
/*                                                                            */
/* The resulting output M1 file may contain more than one complete M1 record  */
/* (FMR, FVMR, Extended Data Block).                                          */
/*                                                                            */
/* For more information, see:                                                 */
/*  'Finger Minutiae Format for Data Interchange', ANSI INCITS 378-2004.      */
/*  'ANSI - Data Format for the Interchange of Fingerprint, Facial, &         */
/*  Scar Mark & Tattoo (SMT) Information', ANSI/NIST-ITL 1-2000,              */
/*  NIST Spectial Publication 500-245.                                        */
/*                                                                            */
/* Parameters to this program:                                                */
/*    -i <an2fkile> The output file containing the raw Finger Minutiae Record.*/
/*    -o <m1file>   The input file to contain the raw ANSI/NIST record.       */
/*    -v            Optionally verify the Finger Minutiae Record. The program */
/*                  will exit with an error code if the FMR is invalid, and   */
/*                  won't create the M1 file.                                 */
/*                                                                            */
/******************************************************************************/
#define _XOPEN_SOURCE   1

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <an2k.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <fmr.h>

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
void
usage()
{
	fprintf(stderr, 
		"usage:\n\tan2k2fmr -i <an2kfile> -o <m1file> [-v]\n"
		"\t\t -i:  Specifies the AN2K input file\n"
		"\t\t -o:  Specifies the M1 output file\n"
		"\t\t -v: Verify the FMR before creating the file\n");
}

/******************************************************************************/
/* Convert X-Y coordinates from AN2K format (origin at lower-left) to         */
/* M1/FMR format (origin at upper-left), and from units of .01mm to the       */
/* pixel number from the origin of the M1/FMR coordinate system.              */
/******************************************************************************/
void
convert_xy(unsigned short x_size, unsigned short y_size,
	   unsigned short x_res, unsigned short y_res,
	   unsigned short ansi_x, unsigned short ansi_y,
	   unsigned short *fmr_x, unsigned short *fmr_y)
{
	float factor, ty;

	// Resolution in FMR is expressed as pix/cm; AN2K uses pix/mm. Also,
	// the AN2K coordinate is a distance from the origin in .01mm units.
	// To convert from AN2K distance to pixel, convert pix/mm to pix/cm,
	// then factor out the .01mm units. This calculation is the same as
	// dividing the FMR resolution by 1000.
	if (x_res != 0) {
		factor = (float)x_res / 1000;
		*fmr_x = (unsigned short)(((float)ansi_x * factor) + 0.5);
	} else
		*fmr_x = 0;

	if (y_res != 0) {
		// Because the coordinate system is based at 0, subtract one
		// from size
		factor = (float)y_res / 1000;
		ty = (float)ansi_y * factor;
                *fmr_y = (unsigned short)(y_size - 1 - ty);
	} else 
		*fmr_y = 0;

}

/******************************************************************************/
/* Convert an angle (theta) value from FMR format to ANSI/NIST format.        */
/******************************************************************************/
void
convert_theta(unsigned int ansi_theta, unsigned char *fmr_theta)
{
	unsigned int ival;

	// FMR angles are in increments of 2, so 45 = 90 degrees.
	// Also, FMR angles are formed in the opposite manner as AN2K, so
	// flip the angle by 180 degrees.
	ival = ((ansi_theta + 180) % 360) / 2;
	*fmr_theta = (unsigned char)ival;
}

/******************************************************************************/
/* Convert the quality measure from FMR format to ANSI/NIST format.           */
/******************************************************************************/
void
convert_quality(int ansi_qual, unsigned char *fmr_qual)
{
	// XXX implement
	*fmr_qual = (unsigned char)ansi_qual;
}

/******************************************************************************/
/* Convert the minutia type from ANSI/NIST format to FMR format.              */
/******************************************************************************/
void
convert_type(char ansi_type, unsigned char *fmr_type)
{
	switch (ansi_type) {
		case 'A' :
			*fmr_type = FMD_MINUTIA_TYPE_RIDGE_ENDING;
			break;

		case 'B' :
			*fmr_type = FMD_MINUTIA_TYPE_BIFURCATION;
			break;

		default :
			*fmr_type = FMD_MINUTIA_TYPE_OTHER;
			break;
	}
}

/******************************************************************************/
/* Convert the scale units from a Type-4 image record to X/Y resolution.     */
/******************************************************************************/
void
convert_type4_ISR(RECORD *rec, unsigned short *x_res, unsigned short *y_res)
{
	FIELD *field;
	int val;
	int idx;
	float f;

	if (lookup_ANSI_NIST_field(&field, &idx, ISR_ID, rec) == FALSE)
		ERR_OUT("Lookup of Type-4 ISR_ID");
	val = strtol(field->subfields[0]->items[0]->value, NULL, 10);

	if ((val != 0) && (val != 1))
			ERR_OUT("Type-4 ISR is invalid");

	switch (val) {
		case 0:			// minimum resolution
			f = MIN_RESOLUTION * 10;	// px/mm -> px/cm
			*x_res = (unsigned short)(f + 0.5);
			f = MIN_RESOLUTION * 10;	// px/mm -> px/cm
			*y_res = (unsigned short)(f + 0.5);
			break;
		// May want to ERR out here
		default:
			*x_res = 0;
			*y_res = 0;
			break;
	}

err_out:
	return;
}

/******************************************************************************/
/* Convert the scale units from a Type-13 image record to X/Y resolution.     */
/******************************************************************************/
void
convert_type13_SLC(RECORD *rec, unsigned short *x_res, unsigned short *y_res)
{
	FIELD *field;
	int val;
	int hps, vps;
	int idx;
	float f;

	if (lookup_ANSI_NIST_field(&field, &idx, SLC_ID, rec) == FALSE)
		ERR_OUT("Lookup of SLC_ID");
	val = strtol(field->subfields[0]->items[0]->value, NULL, 10);

	if ((val == 1) || (val == 2)) {
		if (lookup_ANSI_NIST_field(&field, &idx, HPS_ID, rec) == FALSE)
			ERR_OUT("Lookup of HPS_ID");
		hps = strtol(field->subfields[0]->items[0]->value, NULL, 10);
		if (lookup_ANSI_NIST_field(&field, &idx, VPS_ID, rec) == FALSE)
			ERR_OUT("Lookup of VPS_ID");
		vps = strtol(field->subfields[0]->items[0]->value, NULL, 10);
	}

	switch (val) {
		case 1:			// pixels/inch
			f = (float)hps / 2.54;
			*x_res = (unsigned short)f;
			f = (float)vps / 2.54;
			*y_res = (unsigned short)f;
			break;
		case 2:			// pixels/cm
			*x_res = hps;
			*y_res = vps;
			break;
		// May want to ERR out here
		default:
			*x_res = 0;
			*y_res = 0;
			break;
	}

err_out:
	return;
}

/******************************************************************************/
/* Set the image characteristics in the FMR from info contained in the        */
/* AN2K image record.                                                         */
/******************************************************************************/
int
set_fmr_img(struct finger_minutiae_record *fmr, RECORD *rec)
{
	FIELD *field;
	int idx;
	int factor;

	// Some image record types give resolution as half the minimum or
	// native, so set a factor for the resolution
	switch (rec->type) {
		case TYPE_3_ID:
		case TYPE_5_ID:
			factor = 2;
			break;
		default:
			factor = 1;
			break;
	}
	
	switch (rec->type) {
	case TYPE_13_ID:
		INFOP("Image contained within Type-13 record");
		convert_type13_SLC(rec, &fmr->x_resolution, &fmr->y_resolution);
		break;
	case TYPE_4_ID:
		INFOP("Image contained within Type-4 record");
		convert_type4_ISR(rec, &fmr->x_resolution, &fmr->y_resolution);
		break;
	default:
		// XXX: implement
		fmr->x_resolution = 0;
		fmr->y_resolution = 0;
		break;
	}

	if (lookup_ANSI_NIST_field(&field, &idx, HLL_ID, rec) == FALSE)
		ERR_OUT("Lookup of field HLL_ID");
	fmr->x_image_size = 
	    strtol(field->subfields[0]->items[0]->value, NULL, 10);

	if (lookup_ANSI_NIST_field(&field, &idx, VLL_ID, rec) == FALSE)
		ERR_OUT("Lookup of VLL_ID");
	fmr->y_image_size =
	    strtol(field->subfields[0]->items[0]->value, NULL, 10);;

	return 0;

err_out:
	return -1;
}

/******************************************************************************/
/* Initialize the header information within the FMR from a combination        */
/* of AN2K record data and some reasonable assumptions.                       */
/******************************************************************************/
int
init_fmr(struct finger_minutiae_record *fmr, ANSI_NIST *ansi_nist, int idc)
{
	RECORD *rec;
	int idx;
	int ret;

	strcpy(fmr->format_id, FMR_FORMAT_ID);
	strcpy(fmr->spec_version, FMR_ANSI_SPEC_VERSION);
	fmr->record_length = FMR_ANSI_SMALL_HEADER_LENGTH;
	fmr->record_length_type = FMR_ANSI_SMALL_HEADER_TYPE;
	fmr->product_identifier_owner = 0; // XXX: replace with something valid?
	fmr->product_identifier_type = 0; // XXX: replace with something valid?
	fmr->scanner_id = 0;
	fmr->compliance = 0;

	INFOP("Looking for image with IDC %d", idc);
	ret = lookup_fingerprint_with_IDC(&rec, &idx, idc, 1, ansi_nist);
	if (ret < 0)
		ERR_OUT("System error locating image record");
	if (ret == TRUE) {
		INFOP("Processing data from image with IDC %d", idc);
		if (set_fmr_img(fmr, rec) != 0)
			ERR_OUT("Initializing FMR image info");
	} else {
		INFOP("Using default image values");
		fmr->x_image_size = 0;
		fmr->y_image_size = 0;
		fmr->x_resolution = 0;
		fmr->y_resolution = 0;
	}

	fmr->num_views = 0;

	return 0;

err_out:
	return -1;
}

/******************************************************************************/
/* Create a FVMR by converting the info from a AN2K Type-9 record to an       */
/* M1 finger minutiae record.                                                 */
/******************************************************************************/
int
init_fvmr(struct finger_view_minutiae_record *fvmr, RECORD *anrecord)
{
	int idx;
	int subfield, item;
	unsigned short x, y, q;
	int tval;
	char buf[8];
	struct finger_minutiae_data *fmd;
	struct finger_extended_data *fed;
	struct finger_extended_data_block *fedb;
	struct ridge_count_data *rcd;
	struct core_data *cd;
	struct delta_data *dd;
	int have_fedb = 0;	/* So we have only one extended data block */
	int have_rcdb = 0;
	int have_cddb = 0;
	FIELD *field;
	
	/*
	 * Create an array of counts for the finger positions; used to 
	 * maintain the view number for the finger across calls.
	 */
	static int fgp_view[MAX_TABLE_6_CODE] = {0}; // from an2k.h

	/*** Finger number                 ***/
	if (lookup_ANSI_NIST_field(&field, &idx, FGP2_ID, anrecord) == FALSE)
		ERR_OUT("FGP field not found");
	fvmr->finger_number = (unsigned char)
	    strtol(field->subfields[0]->items[0]->value, NULL, 10);

	/*** View number/impression type    ***/
	// XXX: Check finger_number in range
	fvmr->view_number = (unsigned char)fgp_view[fvmr->finger_number];
	fgp_view[fvmr->finger_number]++;

	if (lookup_ANSI_NIST_field(&field, &idx, IMP_ID, anrecord) == FALSE)
		ERR_OUT("IMP_ID field not found");
	fvmr->impression_type = (unsigned char)
	    strtol(field->subfields[0]->items[0]->value, NULL, 10);

	/*** Finger quality                 ***/
	// XXX: What should the overall finger quality be set to?
	fvmr->finger_quality = 0;

	/* Add the core records */
	if (lookup_ANSI_NIST_field(&field, &idx, CRP_ID, anrecord) == TRUE) {
		 if (have_fedb == 0) {
			if (new_fedb(FMR_STD_ANSI, &fedb) != 0)
				ALLOC_ERR_OUT("Extended Data Block");
			have_fedb = 1;
		}
		/* The FEDB length does NOT include the 
		 * block length field itself, so we add the
		 * the new core/delta data block length only.
		 */
		if (have_cddb == 0) {
			if (new_fed(FMR_STD_ANSI, &fed, FED_CORE_AND_DELTA,
			    FED_HEADER_LENGTH) != 0)
				ALLOC_ERR_EXIT("Extended Data record");
			have_cddb = 1;
		}
		fed->length += CORE_DATA_HEADER_LENGTH;

		for (subfield = 0; subfield < field->num_subfields;
		    subfield++) {
			if (new_cd(FMR_STD_ANSI, &cd) != 0)
				ALLOC_ERR_EXIT("Core Data");
			/* The x,y coordinates are strung together; 
			 * separate them. */
			memcpy(buf, 
			    field->subfields[subfield]->items[0]->value, 4);
			buf[4] = '\0';
			cd->x_coord =
			    (unsigned short)strtoul(buf, (char **)NULL, 10);
			memcpy(buf, 
			    &field->subfields[subfield]->items[0]->value[4], 4);
			buf[4] = '\0';
			cd->y_coord =
			    (unsigned short)strtoul(buf, (char **)NULL, 10);
			/* No angle in AN2K; leave at default of 0 */
			fed->length += CORE_DATA_MIN_LENGTH;
			fed->cddb->num_cores++;
			add_cd_to_cddb(cd, fed->cddb);
		}
	}
	/* Add the delta records */
	if (lookup_ANSI_NIST_field(&field, &idx, DLT_ID, anrecord) == TRUE) {
		 if (have_fedb == 0) {
			if (new_fedb(FMR_STD_ANSI, &fedb) != 0)
				ALLOC_ERR_OUT("Extended Data Block");
			have_fedb = 1;
		}
		/* The FEDB length does NOT include the 
		 * block length field itself, so we add the
		 * the new core/delta data block length only.
		 */
		if (have_cddb == 0) {
			if (new_fed(FMR_STD_ANSI, &fed, FED_CORE_AND_DELTA,
			    FED_HEADER_LENGTH) != 0)
				ALLOC_ERR_EXIT("Extended Data record");
			have_cddb = 1;
		}
		fed->length += DELTA_DATA_HEADER_LENGTH;

		for (subfield = 0; subfield < field->num_subfields;
		    subfield++) {
			if (new_dd(FMR_STD_ANSI, &dd) != 0)
				ALLOC_ERR_EXIT("Delta Data");
			memcpy(buf, 
			    field->subfields[subfield]->items[0]->value, 4);
			buf[4] = '\0';
			dd->x_coord =
			    (unsigned short)strtoul(buf, (char **)NULL, 10);
			memcpy(buf, 
			    &field->subfields[subfield]->items[0]->value[4], 4);
			buf[4] = '\0';
			dd->y_coord =
			    (unsigned short)strtoul(buf, (char **)NULL, 10);
			/* No angles in AN2K; leave at default of 0 */
			fed->length += DELTA_DATA_MIN_LENGTH;
			fed->cddb->num_deltas++;
			add_dd_to_cddb(dd, fed->cddb);
		}
	}
	if (have_cddb) {
		fedb->block_length += fed->length;
		add_fed_to_fedb(fed, fedb);
	}

	/*** Number of minutiae             ***/
	if (lookup_ANSI_NIST_field(&field, &idx, MIN_ID, anrecord) == FALSE)
		ERR_OUT("Number of minutiae field not found");
	tval = (int) strtol(field->subfields[0]->items[0]->value, NULL, 10);
	if (tval > FMR_MAX_NUM_MINUTIAE) {
		INFOP("Using %d minutiae instead of AN2K record value of %d",
			FMR_MAX_NUM_MINUTIAE, tval);
		tval = FMR_MAX_NUM_MINUTIAE;
	}
	fvmr->number_of_minutiae = (unsigned char)tval;

	/*** Finger minutiae data           ***/
	if (lookup_ANSI_NIST_field(&field, &idx, MRC_ID, anrecord) == FALSE)
		ERR_OUT("Minutiae and ridge count data field not found");

	/* For each minutiae index number, create the minutiae data records */
	for (subfield = 0; subfield < fvmr->number_of_minutiae; subfield++) {
		if (new_fmd(FMR_STD_ANSI, &fmd, subfield) != 0)
			ALLOC_ERR_OUT("finger minutiae data record");

		/* The x,y,theta values are in the second item,
		 * strung together; separate them.
		 */
		memcpy(buf, field->subfields[subfield]->items[1]->value, 4);
		buf[4] = '\0';
		x = (unsigned short)strtoul(buf, (char **)NULL, 10);
		
		memcpy(buf, &field->subfields[subfield]->items[1]->value[4], 4);
		buf[4] = '\0';
		y = (unsigned short)strtoul(buf, (char **)NULL, 10);

		convert_xy(fvmr->fmr->x_image_size, fvmr->fmr->y_image_size,
		    fvmr->fmr->x_resolution, fvmr->fmr->y_resolution,
		    x, y,
		    &fmd->x_coord, &fmd->y_coord);

		memcpy(buf, &field->subfields[subfield]->items[1]->value[8], 3);
		buf[3] = '\0';
		convert_theta(strtoul(buf, (char **)NULL, 10), &fmd->angle);

		q = (unsigned short)strtoul(
		    field->subfields[subfield]->items[2]->value,
			(char **)NULL, 10);
		convert_quality(q, &fmd->quality);

		convert_type(field->subfields[subfield]->items[3]->value[0],
		    &fmd->type);

		/* Ridge count data is stored as items 5 .. num_items in
		 * 'second-index,count' format. The first index is stored
		 * in the first item of the field.
		 */
		if (field->subfields[subfield]->num_items > 4) {
		   	if (have_fedb == 0) {
				if (new_fedb(FMR_STD_ANSI, &fedb) != 0)
					ALLOC_ERR_OUT("Extended Data Block");
				have_fedb = 1;
			}
			/* The FEDB length does NOT include the 
			 * block length field itself, so we add the
			 * the new ridge count data block length only.
			 */
			if (have_rcdb == 0) {
				if (new_fed(FMR_STD_ANSI, &fed, FED_RIDGE_COUNT,
				    FED_HEADER_LENGTH) != 0)
					ALLOC_ERR_EXIT("Extended Data record");
				have_rcdb = 1;
				fed->length += RIDGE_COUNT_HEADER_LENGTH;
				// XXX Set fed->rcdb->method
			}

			for (item = 4; 
			    item < field->subfields[subfield]->num_items;
			    item++) {
				char *c;
				if (new_rcd(&rcd) != 0)
					ALLOC_ERR_EXIT("Ridge Count Data");
				rcd->index_one = (unsigned short)strtoul(
				    field->subfields[subfield]->items[0]->value,
				    (char **)NULL, 10);
				c = strtok(
				    field->subfields[subfield]->items[item]->value,
				    ",");
				rcd->index_two = (unsigned short)strtoul(c,
				    (char **)NULL, 10);
				c = strtok(NULL, ",");
				rcd->count = (unsigned short)strtoul(c,
				    (char **)NULL, 10);
				fed->length += RIDGE_COUNT_DATA_LENGTH;
				add_rcd_to_rcdb(rcd, fed->rcdb);
			}
		}
		add_fmd_to_fvmr(fmd, fvmr);
	}
	if (have_rcdb) {
		fedb->block_length += fed->length;
		add_fed_to_fedb(fed, fedb);
	}

	/* There is only one extended data block per FVMR */
	if (have_fedb)
		add_fedb_to_fvmr(fedb, fvmr);

	return 0;

err_out:
	//XXX free memory for FEDs, FEDBs, RCDBs 
	return -1;
}

/* Global option indicators */
int i_opt, o_opt, v_opt;

/* Global file pointers */
FILE *fmr_fp = NULL;	// the FMR (378-2004) input file
FILE *an2k_fp = NULL;	// for the ANSI/NIST output file

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
void
close_files()
{
	if (fmr_fp != NULL)
		fclose(fmr_fp);
	if (an2k_fp != NULL)
		fclose(an2k_fp);
}               

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
void
get_options(int argc, char *argv[])
{
	char ch;
	struct stat sb;

	i_opt = o_opt = v_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:v")) != -1) {
		switch (ch) {
		    case 'v':
			v_opt = 1;
			break;

		    case 'i':
			if ((an2k_fp = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			i_opt = 1;
			break;

		    case 'o':
			if (stat(optarg, &sb) == 0) {
		    	    fprintf(stderr,
				"File '%s' exists, remove it first.\n", optarg);
			    exit(EXIT_FAILURE);
			}
			if ((fmr_fp = fopen(optarg, "wb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			o_opt = 1;
			break;
				
		    case '?':
		    default:
			usage();
			break;
		}
	}

	if ((i_opt && o_opt) == 0) {
		usage();
		goto err_out;
	}
	return;

err_out:
	close_files();
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	struct finger_minutiae_record *fmr;
	struct finger_view_minutiae_record *fvmr = NULL;

	ANSI_NIST *ansi_nist;
	FIELD *field;
	int i;
	int idc;
	int idx;

	get_options(argc, argv);

	if (alloc_ANSI_NIST(&ansi_nist) != 0)
		ALLOC_ERR_EXIT("AN2K record");

	if (read_ANSI_NIST(an2k_fp, ansi_nist) != 0)
		ERR_OUT("Could not read AN2K file.");

	/* Create the M1 finger minutiae record */
	for (i = 1; i < ansi_nist->num_records; i++) {
		if (ansi_nist->records[i]->type == TYPE_9_ID) {

			/*** Image designation character   ***/
			if (lookup_ANSI_NIST_field(&field, &idx, IDC_ID, 
			    ansi_nist->records[i]) == FALSE)
				ERR_OUT("IDC field not found");
			idc = strtol(field->subfields[0]->items[0]->value, 
			    NULL, 10);

			if (new_fmr(FMR_STD_ANSI, &fmr) != 0)
				ALLOC_ERR_EXIT("FMR");

			if (init_fmr(fmr, ansi_nist, idc) != 0)
				ERR_OUT("Initializing FMR");

			if (new_fvmr(FMR_STD_ANSI, &fvmr) != 0)
				ALLOC_ERR_EXIT("FVMR");
			add_fvmr_to_fmr(fvmr, fmr);

			if (init_fvmr(fvmr, ansi_nist->records[i]) != 0)
				ERR_OUT("Could not convert Type-9 record");

			fmr->num_views++;
			fmr->record_length += FVMR_HEADER_LENGTH + 
			    (FMD_DATA_LENGTH * fvmr->number_of_minutiae);
			if (fvmr->extended != NULL)
				fmr->record_length += FEDB_HEADER_LENGTH +
				    fvmr->extended->block_length;
			if (write_fmr(fmr_fp, fmr) != WRITE_OK) 
			    ERR_OUT("Could not write finger minutiae record");

			if (v_opt) {
				if (validate_fmr(fmr) != VALIDATE_OK)
				    ERR_OUT("Finger Minutiae Record is NOT valid.\n");
				else
				    fprintf(stdout, "Finger Minutiae Record is valid.\n");
			}

			free_fmr(fmr);

		}
	}

	free_ANSI_NIST(ansi_nist);

	close_files();
	exit(EXIT_SUCCESS);

err_out:

	close_files();
	exit(EXIT_FAILURE);
}
