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
/*                                                                            */
/* This file contains the implementations of the Finger Minutiae Record       */
/* processing functions. Memory management, reading, writing, and display     */
/* functions are included herein.                                             */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <fmr.h>

/******************************************************************************/
/* Implement the interface for allocating and freeing minutiae records        */
/******************************************************************************/
int
new_fmr(unsigned int format_std, struct finger_minutiae_record **fmr)
{
	struct finger_minutiae_record *lfmr;
	lfmr = (struct finger_minutiae_record *)malloc(
		sizeof(struct finger_minutiae_record));
	if (lfmr == NULL) {
		perror("Failed allocating memory for FMR");
		return -1;
	}
	memset((void *)lfmr, 0, sizeof(struct finger_minutiae_record));
	TAILQ_INIT(&lfmr->finger_views);
	lfmr->format_std = format_std;
	*fmr = lfmr;
	return 0;
}

void
free_fmr(struct finger_minutiae_record *fmr)
{
	struct finger_view_minutiae_record *fvmr;

	// Free the Finger View Minutiae Records contained within the FMR
	while (!TAILQ_EMPTY(&fmr->finger_views)) {
		fvmr = TAILQ_FIRST(&fmr->finger_views);
		TAILQ_REMOVE(&fmr->finger_views, fvmr, list);
		free_fvmr(fvmr);
	}

	// Free the FMR itself
	free(fmr);

}

/******************************************************************************/
/* Implement the interface for reading and writing finger minutiae records    */
/******************************************************************************/

static int
internal_read_fmr(FILE *fp, BDB *fmdb, struct finger_minutiae_record *fmr)
{
	unsigned int lval = 0;
	unsigned short sval = 0;
	unsigned char cval = 0;
	struct finger_view_minutiae_record *fvmr;
	int i;
	int ret;

	fvmr = NULL;
	/* ISO normal and compact card formats have no record header, so
	 * we just set some FMR fields, allocate one FVMR, and proceed to
	 * read into that FVMR.
	 */
	if ((fmr->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (fmr->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		fmr->num_views = 1;
		fmr->record_length = 0;
	} else {
		// Read the header information first
		// Format ID
		OGET(fmr->format_id, 1, FMR_FORMAT_ID_LEN, fp, fmdb);

		// Spec Version
		OGET(fmr->spec_version, 1, FMR_SPEC_VERSION_LEN, fp, fmdb);

		if (fmr->format_std == FMR_STD_ISO) {
			LGET(&lval, fp, fmdb);
			fmr->record_length = lval;
			fmr->record_length_type = FMR_ISO_HEADER_TYPE;
		} else if (fmr->format_std == FMR_STD_ANSI07) {
			LGET(&lval, fp, fmdb);
			fmr->record_length = lval;
			fmr->record_length_type = FMR_ANSI07_HEADER_TYPE;
		} else {
			/* ANSI 04 Record Length; read two bytes, and if that
			 * is not 0, that is the length; otherwise, the length
			 * is in the next 4 bytes
			 */
			SGET(&sval, fp, fmdb);
			if (sval == 0) {
				LGET(&lval, fp, fmdb);
				fmr->record_length = lval;
				fmr->record_length_type =
				    FMR_ANSI_LARGE_HEADER_TYPE;
			} else {
				fmr->record_length = sval;
				fmr->record_length_type =
				    FMR_ANSI_SMALL_HEADER_TYPE;
			}
		}
		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ANSI07)) {
			// CBEFF Product ID
			SGET(&fmr->product_identifier_owner, fp, fmdb);
			SGET(&fmr->product_identifier_type, fp, fmdb);
		}

		// Capture Eqpt Compliance/Scanner ID
		SGET(&sval, fp, fmdb);
		fmr->scanner_id = sval & HDR_SCANNER_ID_MASK; 
		fmr->compliance = (sval & HDR_COMPLIANCE_MASK) >>
		    HDR_COMPLIANCE_SHIFT; 
 
		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ISO)) {
			SGET(&sval, fp, fmdb);
			fmr->x_image_size = sval;
			SGET(&sval, fp, fmdb);
			fmr->y_image_size = sval;
			SGET(&sval, fp, fmdb);
			fmr->x_resolution = sval;
			SGET(&sval, fp, fmdb);
			fmr->y_resolution = sval;
		}
		CGET(&cval, fp, fmdb);
		fmr->num_views = cval;

		// reserved fields
		CGET(&cval, fp, fmdb);
		fmr->reserved = cval;
	}

	// Read the finger views
	for (i = 1; i <= fmr->num_views; i++) {
		if (new_fvmr(fmr->format_std, &fvmr) < 0)
			ERR_OUT("Could not allocate FVMR %d", i);

		if (fp != NULL)
			ret = read_fvmr(fp, fvmr);
		else
			ret = scan_fvmr(fmdb, fvmr);
		if (ret == READ_OK) {
			if (fmr->format_std == FMR_STD_ISO_NORMAL_CARD)
				fmr->record_length = fvmr->number_of_minutiae *
				    FMD_ISO_NORMAL_DATA_LENGTH;
			else if (fmr->format_std == FMR_STD_ISO_COMPACT_CARD)
				fmr->record_length = fvmr->number_of_minutiae *
				    FMD_ISO_COMPACT_DATA_LENGTH;
			add_fvmr_to_fmr(fvmr, fmr);

		} else if (ret == READ_EOF) {
			if (fvmr->partial)
				add_fvmr_to_fmr(fvmr, fmr);
			return READ_EOF;
		}
		else
			ERR_OUT("Could not read entire FVMR %d; Contents:", i);
	}

	return READ_OK;

eof_out:
	ERRP("EOF encountered in %s", __FUNCTION__);
	return READ_EOF;
err_out:
	if (fvmr != NULL)
		print_fvmr(stderr, fvmr);
	return READ_ERROR;
}

int
read_fmr(FILE *fp, struct finger_minutiae_record *fmr)
{
	return (internal_read_fmr(fp, NULL, fmr));
}

int
scan_fmr(BDB *fmdb, struct finger_minutiae_record *fmr)
{
	return (internal_read_fmr(NULL, fmdb, fmr));
}

static int
internal_write_fmr(FILE *fp, BDB *fmdb, struct finger_minutiae_record *fmr)
{
	unsigned short sval;
	unsigned char cval;
	int ret;
	struct finger_view_minutiae_record *fvmr;

	if ((fmr->format_std == FMR_STD_ANSI) ||
	    (fmr->format_std == FMR_STD_ISO) ||
	    (fmr->format_std == FMR_STD_ANSI07)) {

		// Write the header
		// Format ID
		OPUT(fmr->format_id, 1, FMR_FORMAT_ID_LEN, fp, fmdb);

		// Spec Version
		OPUT(fmr->spec_version, 1, FMR_SPEC_VERSION_LEN, fp, fmdb);

		if ((fmr->format_std == FMR_STD_ISO) ||
		    (fmr->format_std == FMR_STD_ANSI07)) {
			LPUT(fmr->record_length, fp, fmdb);
		} else {
			/* ANSI 04 Record Length; if the length is greater than
			 * what will fit in two bytes, store it in the next
			 * four bytes.
			 */
			if (fmr->record_length > FMR_ANSI_MAX_SHORT_LENGTH) {
				sval = 0;
				SPUT(sval, fp, fmdb);
				LPUT(fmr->record_length, fp, fmdb);
			} else {
				SPUT(fmr->record_length, fp, fmdb);
			}
		}
		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ANSI07)) {
			// CBEFF Product ID
			SPUT(fmr->product_identifier_owner, fp, fmdb);
			SPUT(fmr->product_identifier_type, fp, fmdb);
		}

		// Capture Eqpt Compliance/Scanner ID
		sval = (fmr->compliance << HDR_COMPLIANCE_SHIFT) |
		    fmr->scanner_id;
		SPUT(sval, fp, fmdb);

		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ISO)) {
			SPUT(fmr->x_image_size, fp, fmdb);
			SPUT(fmr->y_image_size, fp, fmdb);
			SPUT(fmr->x_resolution, fp, fmdb);
			SPUT(fmr->y_resolution, fp, fmdb);
		}
		CPUT(fmr->num_views, fp, fmdb);

		// reserved field
		cval = 0;
		CPUT(cval, fp, fmdb);
	}

	// Write the finger views
	TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
		if (fp != NULL)
			ret = write_fvmr(fp, fvmr);
		else
			ret = push_fvmr(fmdb, fvmr);
		if (ret != WRITE_OK)
			ERR_OUT("Could not write FVMR");
	}

	return WRITE_OK;

err_out:
	return WRITE_ERROR;
}

int
write_fmr(FILE *fp, struct finger_minutiae_record *fmr)
{
	return (internal_write_fmr(fp, NULL, fmr));
}

int
push_fmr(BDB *fmdb, struct finger_minutiae_record *fmr)
{
	return (internal_write_fmr(NULL, fmdb, fmr));
}

int
print_fmr(FILE *fp, struct finger_minutiae_record *fmr)
{
	int ret;
	int i;
	struct finger_view_minutiae_record *fvmr;

	if ((fmr->format_std == FMR_STD_ANSI) ||
	    (fmr->format_std == FMR_STD_ISO) ||
	    (fmr->format_std == FMR_STD_ANSI07)) {

		// Print the header information 
		fprintf(fp, "Format ID\t\t: %s\nSpec Version\t\t: %s\n",
			fmr->format_id, fmr->spec_version);

		fprintf(fp, "Record Length\t\t: %u\n", fmr->record_length);
		if (fmr->format_std == FMR_STD_ANSI)
			fprintf(fp, "CBEFF Product ID\t: 0x%04x%04x\n",
			    fmr->product_identifier_owner,
			    fmr->product_identifier_type);

		fprintf(fp, "Capture Eqpt\t\t: Compliance, ");
		if (fmr->compliance == 0) {
			fprintf(fp, "None given");
		} else {
			if (fmr->compliance & HDR_APPENDIX_F_MASK) {
				fprintf(fp, "Appendix F");
			} else {
				fprintf(fp, "Unknown");
			}
		}
		fprintf(fp, "; ID, 0x%03x\n", fmr->scanner_id);

		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ISO)) {
			fprintf(fp, "Image Size\t\t: %ux%u\n",
				fmr->x_image_size, fmr->y_image_size);
			fprintf(fp, "Image Resolution\t: %ux%u\n",
				fmr->x_resolution, fmr->y_resolution);
		}
		fprintf(fp, "Number of Views\t\t: %u\n", fmr->num_views);

		fprintf(fp, "\n");
	}
	// Print the finger views
	i = 1;
	TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
		fprintf(fp, "[%03d] ", i++);
		ret = print_fvmr(stdout, fvmr);
		if (ret != PRINT_OK)
			ERR_OUT("Could not print FVMR");
	}
	return PRINT_OK;

err_out:
	return PRINT_ERROR;
}

void
add_fvmr_to_fmr(struct finger_view_minutiae_record *fvmr, 
		struct finger_minutiae_record *fmr)
{
	fvmr->fmr = fmr;
	TAILQ_INSERT_TAIL(&fmr->finger_views, fvmr, list);
}

/******************************************************************************/
/* Implementation of the higher level access routines.                        */
/******************************************************************************/

int
get_fvmr_count(struct finger_minutiae_record *fmr)
{
	return (int) fmr->num_views;
}

int
get_fvmrs(struct finger_minutiae_record *fmr,
	  struct finger_view_minutiae_record *fvmrs[])
{
	int count = 0;
	struct finger_view_minutiae_record *fvmr;

	TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
		fvmrs[count] = fvmr;
		count++;
	}

	return count;
}
