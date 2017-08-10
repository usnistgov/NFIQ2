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
/* This program will convert a ANSI/INCITS 378-2004 Finger Minutiae Record    */
/* into a ANSI/NIST Type 9 record embedded in the ANSI/NIST data record.      */
/*                                                                            */
/* For more information, see:                                                 */
/*  'Finger Minutiae Format for Data Interchange', ANSI INCITS 378-2004.      */
/*  'ANSI - Data Format for the Interchange of Fingerprint, Facial, &         */
/*  Scar Mark & Tattoo (SMT) Information', ANSI/NIST-ITL 1-2000,              */
/*  NIST Spectial Publication 500-245.                                        */
/*                                                                            */
/* Parameters to this program:                                                */
/*    -i <m1file>   The input file containing the raw Finger Minutiae Record. */
/*    -o <an2kfile> The output file to contain the raw ANSI/NIST record.      */
/*    -v            Optionally verify the Finger Minutiae Record. The program */
/*                  will exit with an error code if the FMR is invalid, and   */
/*                  won't create the ANSI/NIST file.                          */
/*    -f            Optionally specifies a file that contains a list of       */
/*                  file names for the 8-bit grayscale image files.  The      */
/*                  files are read in order and placed within a the Type-13   */
/*                  record with the IDC corresponding to the Type-9 record    */
/*                  that precedes the Type-13 record.                         */
/*    -t            Optionally specifies a file that contains the Type-2      */
/*                  record information. See fmr2an2k(1) for information.      */
/*                                                                            */
/******************************************************************************/
#define _XOPEN_SOURCE   1

#include "fmr2an2k.h"
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <an2k.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
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
		"usage:\n\tfmr2an2k -i <m1file> -o <an2kfile>"
		" [-f <imglist>] [-t <type2file] [-v]\n"
		"\t\t -i:  Specifies the M1 input file\n"
		"\t\t -o:  Specifies the AN2K output file\n"
		"\t\t -f:  Specifies a file containing the list of"
		" fingerprint images\n"
		"\t\t -t: Specifies the file containing Type-2 record info\n"
		"\t\t -v: Verify the FMR file\n");
}

/******************************************************************************/
/* Convert X-Y coordinates from FMR format (origin at upper-left) to          */
/* ANSI/NIST format (origin at lower-left), and from a pixel number to        */
/* units of .01mm from the origin of the AN2K coordinate system.              */
/******************************************************************************/
static void
convert_xy(unsigned short x_size, unsigned short y_size,
	   unsigned short x_res, unsigned short y_res,
	   unsigned short fmr_x, unsigned short fmr_y, 
	   unsigned int *ansi_x, unsigned int *ansi_y)
{
	float factor;
	float ty, tsize;

	// Resolution in FMR is expressed as pix/cm; AN2K uses pix/mm. Also,
	// the AN2K coordinate is a distance from the origin in .01mm units.
	// To convert from pixel to AN2K distance, convert pix/cm to pix/mm,
	// then to .01mm units, which is the same as dividing FMR resolution 
	// by 1000, and dividing the pixel number by that factor.
	if (x_res != 0) {
		factor = (float)x_res / 1000;
		*ansi_x = (unsigned short)(((float)fmr_x / factor) + 0.5);
	} else
		*ansi_x = 0;

	// Convert from upper left to lower left for the Y coordinate, using
	// floating point for the interim calculations to conserve precision

	if (y_res != 0) {
		factor = (float)y_res / 1000;

		// Because the coordinate system is based at 0, subtract one 
		// from size
		tsize = (float)(y_size - 1) / factor;
		ty = (float)fmr_y / factor;
		//*ansi_y = (unsigned short)((tsize - ty) + 0.5);
		// NOT compensating for truncation appears more accurate
		*ansi_y = (unsigned short)(tsize - ty); 
	} else
		*ansi_y = 0;
}

/******************************************************************************/
/* Convert an angle (theta) value from FMR format to ANSI/NIST format.        */
/******************************************************************************/
static void
convert_theta(unsigned char fmr_theta, unsigned int *ansi_theta)
{
	// FMR angles are in increments of 2, so 45 = 90 degrees.
	// Also, FMR angles are formed in the opposite manner as AN2K, so
	// flip the angle by 180 degrees.
	*ansi_theta = (((unsigned int)fmr_theta * 2) + 180) % 360;
}

/******************************************************************************/
/* Convert the quality measure from FMR format to ANSI/NIST format.           */
/******************************************************************************/
static void
convert_quality(unsigned char fmr_qual, unsigned int *ansi_qual)
{
	// XXX implement
	*ansi_qual = 0;
}

/******************************************************************************/
/* Convert the minutia type from FMR format to ANSI/NIST format.              */
/******************************************************************************/
static void
convert_type(unsigned char fmr_type, char *ansi_type)
{
	switch (fmr_type) {
		case FMD_MINUTIA_TYPE_RIDGE_ENDING :
			*ansi_type = 'A';
			break;

		case FMD_MINUTIA_TYPE_BIFURCATION :
			*ansi_type = 'B';
			break;

		default :
			*ansi_type = 'D';
			break;
	}
}

/******************************************************************************/
/* Create an ANSI/NIST Type-1 record with predefined values. The record will  */
/* be allocated in this function, and must be freed by the caller.            */
/* Only the mandatory fields are placed in the Type-1 record.                 */
/* Returns:                                                                   */
/*	 0 Success                                                            */
/*	-1 Failure                                                            */
/******************************************************************************/
static int
create_type1(RECORD **anrecord) 
{
	ITEM *item = NULL;
	SUBFIELD *subfield = NULL;
	FIELD *field = NULL;
	RECORD *lrecord;	// For local convenience
	char buf[32];
	char *date_str;
	time_t tod;
	struct tm *tm;

	if (new_ANSI_NIST_record(anrecord, TYPE_1_ID) != 0) 
		ALLOC_ERR_EXIT("Type-1 Record");

	lrecord = *anrecord;

	/*** 1.001 Logical record length ***/
	// Set the length to 0 for now; it will be updated when the record
	// is closed
	APPEND_TYPE1_FIELD(lrecord, LEN_ID, "0");

	/*** 1.002 - Version number ***/
	snprintf(buf, sizeof(buf), "%04d", VERSION_0300);
	APPEND_TYPE1_FIELD(lrecord, VER_ID, buf);

	/*** 1.003 - File content ***/
	snprintf(buf, sizeof(buf), "%d", TYPE_1_ID);
	// Allocate a new subfield and set the first item
	if (value2subfield(&subfield, buf) != 0)
		ERR_OUT("allocating Type-1 subfield");
	// Add the second item to the subfield, the count of remaining records
	if (value2item(&item, "0") != 0)
		ERR_OUT("allocating Type-1 item");
	if (append_ANSI_NIST_subfield(subfield, item) != 0)
		ERR_OUT("appending Type-1 item");
	// Add the subfield to the field
	if (new_ANSI_NIST_field(&field, TYPE_1_ID, CNT_ID) != 0)
		ERR_OUT("allocating Type-1 field");
	if (append_ANSI_NIST_field(field, subfield) != 0)
		ERR_OUT("appending Type-1 subfield");

	// Add the entire field to the record
	if (append_ANSI_NIST_record(lrecord, field) != 0)
		ERR_OUT("appending Type-1 field");

	/*** 1.004 - Type of transaction ***/
	APPEND_TYPE1_FIELD(lrecord, 4, "LFFS");

	/*** 1.005 - Date ***/
	if (get_ANSI_NIST_date(&date_str) != 0)
		ERR_OUT("getting ANSI/NIST date");
	APPEND_TYPE1_FIELD(lrecord, DAT_ID, date_str);
	free(date_str);

	/*** 1.007 - Destination agency identifier ***/
	APPEND_TYPE1_FIELD(lrecord, 7, "ANSI/NIST");

	/*** 1.008 - Originating agency identifier ***/
	APPEND_TYPE1_FIELD(lrecord, 8, "M1/FMR");

	/*** 1.009 - Transaction control number ***/
	// Use the current UTC time string YYYYMMDDHHMMSS
	tod = time(NULL);
	tm = gmtime(&tod);
	snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02d",
	    tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, 
	    tm->tm_min, tm->tm_sec);
	
	APPEND_TYPE1_FIELD(lrecord, 9, buf);

	/*** 1.011 - Native scanning resolution ***/
	// XXX Set the NSR to the minimum, but we may want to base this
	// XXX value on something from the FMR
	snprintf(buf, sizeof(buf), "%5.2f", MIN_RESOLUTION);
	APPEND_TYPE1_FIELD(lrecord, 11, buf);

	/*** 1.012 - Nominal transmitting resolution ***/
	APPEND_TYPE1_FIELD(lrecord, 12, buf);

	if (update_ANSI_NIST_tagged_record_LEN(lrecord) != 0)
		ERR_OUT("updating record length");
	
	return 0;

err_out:
	fprintf(stderr, "Error creating Type-1 record\n");
	if (item != NULL)
		free_ANSI_NIST_item(item);
	if (subfield != NULL)
		free_ANSI_NIST_subfield(subfield);
	if (field != NULL)
		free_ANSI_NIST_field(field);
	if (lrecord != NULL)
		free_ANSI_NIST_record(lrecord);

	return -1;
}

/******************************************************************************/
/* Update the Type-1 record with new information on other records that are    */
/* being added to the ANSI/NIST file.                                         */
/******************************************************************************/
static int
update_type1(ANSI_NIST *ansi_nist, RECORD *record, unsigned int type, 
		unsigned int idc)
{
	SUBFIELD *subfield = NULL;
	FIELD *field = NULL;
	ITEM *item = NULL;
	int field_idx;
	int saved_len;
	char buf[8];

	if (lookup_ANSI_NIST_field(&field, &field_idx, CNT_ID, record) == FALSE)
		ERR_OUT("locating CNT field of Type-1 record");

	// Save away the current length of the field so we can add
	// the change in length to the record
	saved_len = field->num_bytes;

	// Create a new subfield to contain the new logical record identifier
	snprintf(buf, sizeof(buf), "%d", type);
	if (value2subfield(&subfield, buf) != 0)
		ERR_OUT("creating new subfield");

	// Add the second item to the subfield, the IDC
	snprintf(buf, sizeof(buf), IDC_FMT, idc);
	if (value2item(&item, buf) != 0)
		ERR_OUT("creating new item");
	if (append_ANSI_NIST_subfield(subfield, item) != 0)
		ERR_OUT("appending item to subfield");

	// Add the subfield to the field
	if (append_ANSI_NIST_field(field, subfield) != 0)
		ERR_OUT("adding subfield to field");

	// Update the record length with the change in field length
	record->num_bytes += field->num_bytes - saved_len;
	if (update_ANSI_NIST_tagged_record_LEN(record) != 0)
		goto err_out2;

	// Update the sum of the Type-2 to Type-16 logical records, contained
	// in the first subfield of the Type-1 record.
	// The index numbeers are off-by-one.
	if (increment_numeric_item(0,		// record index
				   2,		// field index
				   0,		// subfield index
				   1,		// item index
				   ansi_nist,	// ansi_nist record
				   NULL) < 0)
		goto err_out2;

	return 0;

err_out:
	if (item != NULL)
		free(item);
	if (subfield != NULL)
		free(subfield);
	
	return -1;

err_out2:	// This exit point doesn't free any memory because the
		// fields have been shoved into the record
	return -1;
}

/******************************************************************************/
/* Create an ANSI/NIST Type-2 record with text values taken from the input    */
/* file given on the command line.                                            */
/* Returns:                        
                                           */
/*	 0 Success                                                            */
/*	-1 Failure                                                            */
/******************************************************************************/
static int
create_type2(RECORD **anrecord, FILE *fp, unsigned int idc)
{
	FIELD *field = NULL;
	SUBFIELD *subfield = NULL;
	RECORD *lrecord;	// For local convenience
	int field_num;
	char buf[MAX_TYPE2_FIELD_SIZE + 1];
	char *s;

	if (new_ANSI_NIST_record(anrecord, TYPE_2_ID) != 0) 
		ALLOC_ERR_EXIT("Type-2 Record");

	lrecord = *anrecord;

	/*** 2.001 - Length                                    ***/ 
	// Set to 0 now, will recalculate later
	APPEND_TYPE2_FIELD(lrecord, LEN_ID, "0");

	/*** 2.002 - IDC value                                 ***/ 
	snprintf(buf, sizeof(buf), IDC_FMT, idc);
	APPEND_TYPE2_FIELD(lrecord, IDC_ID, buf);

	while (1) {
		// Read the field number, followed by single whitespace char,
		// followed by the string to place in the field.
		if (fscanf(fp, "%d", &field_num) < 0) {
			if (feof(fp)) {
				break;
			} else {
				ERR_OUT("reading Type-2 buf file.\n");
			}
		}
		fgetc(fp);	// skip the single whitespace char
		s = fgets(buf, MAX_TYPE2_FIELD_SIZE, fp);
		buf[strlen(buf) - 1] = '\0';
		if (s == NULL) {
			if (feof(fp)) {
				break;
			} else {
				ERR_OUT("reading Type-2 buf file.\n");
			}
		}
		/*** 2.xxx - User-defined field   ***/
		if (value2subfield(&subfield, buf) != 0)
			ERR_OUT("creating new Type-2 subfield");
		if (new_ANSI_NIST_field(&field, TYPE_2_ID, field_num) != 0)
			ERR_OUT("creating new Type-2 field");
		if (append_ANSI_NIST_field(field, subfield) != 0)
			ERR_OUT("appending Type-2 subfield");
		if (append_ANSI_NIST_record(lrecord, field) != 0)
			ERR_OUT("appending Type-2 field");

	}

	// Calculate and update the record length field
	if (update_ANSI_NIST_tagged_record_LEN(lrecord) != 0)
		ERR_OUT("updating Type-2 record length");

	return 0;

err_out:
	return -1;
}

/******************************************************************************/
/* Create an ANSI/NIST Type-9 record with predefined values. The record will  */
/* be allocated in this function, and must be freed by the caller.            */
/* Returns:                                                                   */
/*	 0 Success                                                            */
/*	-1 Failure                                                            */
/******************************************************************************/
static int
create_type9(RECORD **anrecord, struct finger_view_minutiae_record *fvmr, 
	     unsigned int idc)
{
	FIELD *field = NULL;
	SUBFIELD *subfield = NULL;
	ITEM *item = NULL;
	RECORD *lrecord;	// For local convenience
	struct finger_minutiae_data **fmds = NULL;
	struct ridge_count_data **rcds = NULL;
	struct core_data **cds;
	struct delta_data **dds;
	char buf[16];
	unsigned int mincnt, minidx, rdgcnt;
	int cnt, i;
	unsigned int x, y; 

	if (new_ANSI_NIST_record(anrecord, TYPE_9_ID) != 0) 
		ALLOC_ERR_EXIT("Type-9 Record");

	lrecord = *anrecord;

	/*** 9.001 - Length                                    ***/ 
	// Set to 0 now, will recalculate later
	APPEND_TYPE9_FIELD(lrecord, LEN_ID, "0");

	/*** 9.002 - IDC value                                 ***/ 
	snprintf(buf, sizeof(buf), IDC_FMT, idc);
	APPEND_TYPE9_FIELD(lrecord, IDC_ID, buf);

	/*** 9.003 - Impression type                           ***/ 
	CRW(fvmr->impression_type, MIN_TABLE_5_CODE, MAX_TABLE_5_CODE,
		"Impression type");
	snprintf(buf, sizeof(buf), "%d", fvmr->impression_type);
	APPEND_TYPE9_FIELD(lrecord, IMP_ID, buf);

	/*** 9.004 - Minutiae format                           ***/
	APPEND_TYPE9_FIELD(lrecord, FMT_ID, STD_STR);

	/*** 9.005 - Originating fingerprint reading system    ***/
	if (value2subfield(&subfield, "EXISTING IMAGE") != 0)
		ERR_OUT("creating Type-9 subfield");
	if (value2item(&item, AUTO_STR) != 0)
		ERR_OUT("creating Type-9 item");
	if (append_ANSI_NIST_subfield(subfield, item) != 0)
		ERR_OUT("appending Type-9 item");
	if (new_ANSI_NIST_field(&field, TYPE_9_ID, OFR_ID) != 0)
		ERR_OUT("creating Type-9 field");
	if (append_ANSI_NIST_field(field, subfield) != 0)
		ERR_OUT("appending Type-9 subfield");
	if (append_ANSI_NIST_record(lrecord, field) != 0)
		ERR_OUT("appending Type-9 field");

	/*** 9.006 - Finger position                           ***/ 
	snprintf(buf, sizeof(buf), "%02d", fvmr->finger_number);
	APPEND_TYPE9_FIELD(lrecord, FGP2_ID, buf);

	/*** 9.007 - Fingerprint pattern classification        ***/ 
	if (value2subfield(&subfield, TBL_STR) != 0)
		ERR_OUT("creating Type-9 subfield");
	if (value2item(&item, "UN") != 0)
		ERR_OUT("creating Type-9 item");
	if (append_ANSI_NIST_subfield(subfield, item) != 0)
		ERR_OUT("appending Type-9 item");
	if (new_ANSI_NIST_field(&field, TYPE_9_ID, FPC_ID) != 0)
		ERR_OUT("creating Type-9 field");
	if (append_ANSI_NIST_field(field, subfield) != 0)
		ERR_OUT("appending Type-9 subfield");
	if (append_ANSI_NIST_record(lrecord, field) != 0)
		ERR_OUT("appending Type-9 field");

	/*** 9.008 - Core position                             ***/ 
	cnt = get_core_count(fvmr);
	if (cnt > 0) {
		if (new_ANSI_NIST_field(&field, TYPE_9_ID, CRP_ID) != 0)
			ERR_OUT("allocating field");

		cds = (struct core_data **) malloc(
			cnt * sizeof(struct core_data **));
		if (cds == NULL)
			ALLOC_ERR_EXIT("Core data");

		if (get_cores(fvmr, cds) != cnt)
			ERR_OUT("retrieving core data");

		for (i = 0; i < cnt; i++) {
			convert_xy(fvmr->fmr->x_image_size, 
				   fvmr->fmr->y_image_size,
				   fvmr->fmr->x_resolution,
				   fvmr->fmr->y_resolution,
				   cds[i]->x_coord, 
				   cds[i]->y_coord, 
				   &x, &y);
			snprintf(buf, sizeof(buf), "%04u%04u", x, y);
			if (value2subfield(&subfield, buf) != 0)
				ERR_OUT("creating subfield");
			if (append_ANSI_NIST_field(field, subfield) != 0)
				ERR_OUT("appending subfield");
		}
		if (append_ANSI_NIST_record(lrecord, field) != 0)
			ERR_OUT("adding field to record");

	} else if (cnt < 0)
		ERR_OUT("getting core record count");

	/*** 9.009 - Delta(s) position                         ***/ 
	cnt = get_delta_count(fvmr);
	if (cnt > 0) {
		if (new_ANSI_NIST_field(&field, TYPE_9_ID, DLT_ID) != 0)
			ERR_OUT("creating Type-9 field");

		dds = (struct delta_data **) malloc(
			cnt * sizeof(struct delta_data **));
		if (dds == NULL)
			ALLOC_ERR_EXIT("Delta data");

		if (get_deltas(fvmr, dds) != cnt)
			ERR_OUT("retrieving delta data");

		for (i = 0; i < cnt; i++) {
			convert_xy(fvmr->fmr->x_image_size, 
				   fvmr->fmr->y_image_size,
				   fvmr->fmr->x_resolution,
				   fvmr->fmr->y_resolution,
				   dds[i]->x_coord, 
				   dds[i]->y_coord, 
				   &x, &y);
			snprintf(buf, sizeof(buf), "%04u%04u", x, y);
			if (value2subfield(&subfield, buf) != 0)
				ERR_OUT("creating subfield");
			if (append_ANSI_NIST_field(field, subfield) != 0)
				ERR_OUT("appending subfield");
		}
		if (append_ANSI_NIST_record(lrecord, field) != 0)
			ERR_OUT("adding field to record");

	} else if (cnt < 0)
		ERR_OUT("getting delta record count");

	/*** 9.010 - Number of minutiae                        ***/ 
	mincnt = get_fmd_count(fvmr);
	if (mincnt < 0)
		ERR_OUT("getting minutiae count");

	snprintf(buf, sizeof(buf), "%d", mincnt);
	APPEND_TYPE9_FIELD(lrecord, MIN_ID, buf);

	/*** 9.011 - Minutiae ridge count indicator            ***/ 
	rdgcnt = get_rcd_count(fvmr);
	if (rdgcnt > 0) {
		rcds = (struct ridge_count_data **) malloc(
			rdgcnt * sizeof(struct ridge_count_data **));
		if (rcds == NULL)
			ALLOC_ERR_EXIT("Ridge Count data");

		if (get_rcds(fvmr, rcds) != rdgcnt)
			ERR_OUT("retrieving ridge count data");

		APPEND_TYPE9_FIELD(lrecord, RDG_ID, "1");
	} else if (rdgcnt < 0)
		ERR_OUT("getting ridge record count");
	else
		APPEND_TYPE9_FIELD(lrecord, RDG_ID, "0");

	/*** 9.012 - Minutiae and ridge count data             ***/ 
	fmds = (struct finger_minutiae_data **) malloc(
		mincnt * sizeof(struct finger_minutiae_data **));
	if (fmds == NULL)
		ALLOC_ERR_EXIT("Finger Minutiae data");

	if (get_fmds(fvmr, fmds) != mincnt)
		ERR_OUT("retrieving minutiae data");

	if (new_ANSI_NIST_field(&field, TYPE_9_ID, MRC_ID) != 0)
		ERR_OUT("creating Type-9 field");

	for (minidx = 0; minidx < mincnt; minidx++) {
		unsigned int theta, rdgidx, minqual;
		char mintype;
		int idxnum = minidx + 1;

		// Index number
		snprintf(buf, sizeof(buf), "%03d", idxnum);
		if (value2subfield(&subfield, buf) != 0)
			ERR_OUT("creating Type-9 subfield");

		// X, Y, and theta values
		convert_xy(fvmr->fmr->x_image_size, fvmr->fmr->y_image_size,
			   fvmr->fmr->x_resolution, fvmr->fmr->y_resolution,
			   fmds[minidx]->x_coord, fmds[minidx]->y_coord, 
			   &x, &y);
		convert_theta(fmds[minidx]->angle, &theta);
		snprintf(buf, sizeof(buf), "%04u%04u%03u", x, y, theta);
		if (value2item(&item, buf) != 0)
			ERR_OUT("creating Type-9 item");
		if (append_ANSI_NIST_subfield(subfield, item) != 0)
			ERR_OUT("appending Type-9 item");

		// Quality measure
		convert_quality(fmds[minidx]->quality, &minqual);
		snprintf(buf, sizeof(buf), "%u", minqual);
		if (value2item(&item, buf) != 0)
			ERR_OUT("creating Type-9 item");
		if (append_ANSI_NIST_subfield(subfield, item) != 0)
			ERR_OUT("appending Type-9 item");

		// Minutia type designation
		convert_type(fmds[minidx]->type, &mintype);
		snprintf(buf, sizeof(buf), "%c", mintype);
		if (value2item(&item, buf) != 0)
			ERR_OUT("creating Type-9 item");
		if (append_ANSI_NIST_subfield(subfield, item) != 0)
			ERR_OUT("appending Type-9 item");

		// Ridge count data: If the one of the index numbers
		// in the record matches the minutia index, then add that
		// ridge count data to the Type-9 record, using the index
		// number that is the 'other'.
		for (rdgidx = 0; rdgidx < rdgcnt; rdgidx++) {
		    if ((rcds[rdgidx]->index_one == idxnum) ||
			(rcds[rdgidx]->index_two == idxnum)) {
			    snprintf(buf, sizeof(buf), "%u,%u", 
				(rcds[rdgidx]->index_one == idxnum) ?
			 	rcds[rdgidx]->index_two :
				rcds[rdgidx]->index_one,
				rcds[rdgidx]->count);

			    if (value2item(&item, buf) != 0)
					ERR_OUT("creating Type-9 item");
			    if (append_ANSI_NIST_subfield(subfield, item) != 0)
					ERR_OUT("appending Type-9 item");
		    }
		}

		if (append_ANSI_NIST_field(field, subfield) != 0)
			ERR_OUT("appending Type-9 subfield");
	}
	free(fmds);
	if (append_ANSI_NIST_record(lrecord, field) != 0)
		ERR_OUT("appending Type-9 field");
	/*** End of minutiae and ridge count                 */

	// Calculate and update the record length field
	if (update_ANSI_NIST_tagged_record_LEN(lrecord) != 0)
		ERR_OUT("updating Type-9 record length");

	return 0;

err_out:
	fprintf(stderr, "Error creating Type-9 record\n");
	if (item != NULL)
		free_ANSI_NIST_item(item);
	if (subfield != NULL)
		free_ANSI_NIST_subfield(subfield);
	if (field != NULL)
		free_ANSI_NIST_field(field);
	if (lrecord != NULL)
		free_ANSI_NIST_record(lrecord);
	if (fmds != NULL)
		free(fmds);

	return -1;
}

/******************************************************************************/
/* Create an ANSI/NIST Type-13 record with predefined values. The record will */
/* be allocated in this function, and must be freed by the caller.            */
/* Returns:                                                                   */
/*	 0 Success                                                            */
/*	-1 Failure                                                            */
/******************************************************************************/
static int
create_type13(RECORD **anrecord, struct finger_view_minutiae_record *fvmr, 
	    FILE *fp, unsigned int idc)
{
	char fn[MAXPATHLEN];
	unsigned char *imgdata;
	int imgsize;

	/*** 13.999 - Image data                               ***/ 
	// fp parameter is for the file containing the list of
	// image files
	if (fscanf(fp, "%s", fn) < 0)
		ERR_OUT("reading image list file.\n");

	printf("reading image from file %s\n", fn);
	if (read_binary_image_data(fn, &imgdata, &imgsize) != 0)
		ERR_OUT("reading image data");

	if (image2type_13(anrecord, imgdata, imgsize, 
	    fvmr->fmr->x_image_size, fvmr->fmr->y_image_size,
	    8, (double)(fvmr->fmr->x_resolution / 10.0),
	    "NONE",	// Compression String
	    idc, fvmr->impression_type,
	    "NIST 894.03") != 0)
		ERR_OUT("converting image to Type-13 record");

	// Calculate and update the record length field
	if (update_ANSI_NIST_tagged_record_LEN(*anrecord) != 0)
		ERR_OUT("updating Type-13 record length");

	return 0;

err_out:
	return -1;
}

/* Global option indicators */
int i_opt, o_opt, f_opt, v_opt, t_opt;

/* Global file pointers */
FILE *fmr_fp = NULL;	// the FMR (378-2004) input file
FILE *an2k_fp = NULL;	// for the ANSI/NIST output file
FILE *img_fp = NULL;	// for the optional image list file
FILE *text_fp = NULL;	// for the user-defined text (Type-2) record info

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
static void
close_files()
{
	if (fmr_fp != NULL)
		fclose(fmr_fp);
	if (an2k_fp != NULL)
		fclose(an2k_fp);
	if (img_fp != NULL)
		fclose(img_fp);
	if (text_fp != NULL)
		fclose(text_fp);
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	char ch;
	struct stat sb;

	i_opt = o_opt = f_opt = v_opt = t_opt = 0;
	while ((ch = getopt(argc, argv, "i:o:f:t:v")) != -1) {
		switch (ch) {
		    case 'v':
			v_opt = 1;
			break;

		    case 'i':
			if ((fmr_fp = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			i_opt = 1;
			break;

		    case 'f':	// Fingerprint image list file
			if ((img_fp = fopen(optarg, "r")) == NULL)
				OPEN_ERR_EXIT(optarg);
			f_opt = 1;
			break;

		    case 'o':
			if (stat(optarg, &sb) == 0) {
		    	    fprintf(stderr,
				"File '%s' exists, remove it first.\n", optarg);
			    exit(EXIT_FAILURE);
			}
			if ((an2k_fp = fopen(optarg, "wb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			o_opt = 1;
			break;
				
		    case 't':
			if ((text_fp = fopen(optarg, "r")) == NULL)
				OPEN_ERR_EXIT(optarg);
			t_opt = 1;
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
	struct finger_minutiae_record *fmr = NULL;
	struct finger_view_minutiae_record **fvmrs = NULL;

	ANSI_NIST *ansi_nist;
	RECORD *anrecord;
	RECORD *type1;

	int rcount, i;
	unsigned int idc;

	get_options(argc, argv);

	// Allocate the FMR record in memory
	if (new_fmr(FMR_STD_ANSI, &fmr) < 0)
		ALLOC_ERR_EXIT("FMR");

	// Read the FMR
	if (read_fmr(fmr_fp, fmr) != READ_OK) {
		fprintf(stderr, "Could not read FMR from file.\n");
		exit(EXIT_FAILURE);
	}

	if (v_opt) {
		if (validate_fmr(fmr) != VALIDATE_OK) {
		    fprintf(stdout, "Finger Minutiae Record is NOT valid.\n");
		    exit(EXIT_FAILURE);
		} else {
		    fprintf(stdout, "Finger Minutiae Record is valid.\n");
		}
	}

	// Create the ANSI/NIST block
	if (alloc_ANSI_NIST(&ansi_nist) != 0) 
		ALLOC_ERR_EXIT("ANSI/NIST Block");

	// Create and add the Type-1 record
	create_type1(&type1);
	if (update_ANSI_NIST(ansi_nist, type1) != 0) 
		ERR_OUT("inserting Type-1 Record");

	// Create and add the Type-2 record, with IDC 0, if asked for
	if (t_opt) {
		create_type2(&anrecord, text_fp, 0);
		if (update_type1(ansi_nist, type1, TYPE_2_ID, 0) != 0)
			ERR_OUT("updating Type-1 record");
		if (update_ANSI_NIST(ansi_nist, anrecord) != 0) 
			ERR_OUT("inserting Type-9 record");
	}

	// Create the Type-9 records
	rcount = get_fvmr_count(fmr);
	idc = 0;
	if (rcount > 0) {
		fvmrs = (struct finger_view_minutiae_record **) malloc(
		    rcount * sizeof(struct finger_view_minutiae_record **));
		if (fvmrs == NULL)
			ALLOC_ERR_EXIT("FVMR Array");
		if (get_fvmrs(fmr, fvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");

		// Create and add the Type-9 record
		for (i = 0; i < rcount; i++) {
			if (create_type9(&anrecord, fvmrs[i], idc) != 0)
				ERR_OUT("creating Type-9 record");
			if (update_type1(ansi_nist, type1, TYPE_9_ID, idc) != 0)
				ERR_OUT("updating Type-1 record");
			if (update_ANSI_NIST(ansi_nist, anrecord) != 0) 
				ERR_OUT("inserting Type-9 record");
			if (f_opt) {	// images
			    if (create_type13(&anrecord, fvmrs[i], 
						img_fp, idc) != 0)
				ERR_OUT("creating Type-3 record");
			    if (update_type1(ansi_nist, type1, TYPE_13_ID, 
				    idc) != 0)
				ERR_OUT("updating Type-1 record");
			    if (update_ANSI_NIST(ansi_nist, anrecord) != 0) 
				ERR_OUT("inserting Type-9 record");
			}
			idc++;
		}
		free(fvmrs);
	} else {
		if (rcount == 0)
			fprintf(stderr, "Warning: 0 FVMRs in the FMR\n");
		else {
			fprintf(stderr, "Error retrieving FVMRs from FMR.\n");
			exit (EXIT_FAILURE);
		}
	}

	// Free the entire FMR
	free_fmr(fmr);

	// Write out the new ANSI/NIST record
	if (write_ANSI_NIST(an2k_fp, ansi_nist) != 0) 
		WRITE_ERR_OUT("ANSI/NIST File");

	// Convert the FMR to a Type 9 record`
	// Free the entire ANSI/NIST block
	free_ANSI_NIST(ansi_nist);

	close_files();

	exit(EXIT_SUCCESS);

err_out:
	if (fmr != NULL)
		free_fmr(fmr);

	if (fvmrs != NULL)
		free(fvmrs);

	close_files();

	exit(EXIT_FAILURE);
}
