/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility  whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/

#include <sys/queue.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <fir.h>

/******************************************************************************/
/* Helper functions                                                           */
/******************************************************************************/
static char *
comp_algo_to_str(short algo)
{
	switch (algo) {

	case COMPRESSION_ALGORITHM_UNCOMPRESSED_NO_BIT_PACKED:
		return "Uncompressed - no bit packing";
	case COMPRESSION_ALGORITHM_UNCOMPRESSED_BIT_PACKED:
		return "Uncompressed - bit packed";
	case COMPRESSION_ALGORITHM_COMPRESSED_WSQ:
		return "Compressed - WSQ";
	case COMPRESSION_ALGORITHM_COMPRESSED_JPEG:
		return "Compressed - JPEG";
	case COMPRESSION_ALGORITHM_COMPRESSED_JPEG2000:
		return "Compressed - JPEG2000";
	case COMPRESSION_ALGORITHM_COMPRESSED_PNG:
		return "PNG";
	default:
		return "Invalid";
	}
}

/******************************************************************************/
/* Implement the interface for allocating and freeing finger image records    */
/******************************************************************************/
int
new_fir(unsigned int format_std, struct finger_image_record **fir)
{
	struct finger_image_record *lfir;

	switch (format_std) {
		case FIR_STD_ANSI:
		case FIR_STD_ISO:
			break;
		default:
			ALLOC_ERR_RETURN("Invalid record standard type");
			break;
	}
	lfir = (struct finger_image_record *)malloc(
	    sizeof(struct finger_image_record));
	if (lfir == NULL)
		ALLOC_ERR_RETURN("Failed allocating memory for FIR");

	memset((void *)lfir, 0, sizeof(struct finger_image_record));
	TAILQ_INIT(&lfir->finger_views);
	lfir->format_std = format_std;
	*fir = lfir;
	return (0);
}

void
free_fir(struct finger_image_record *fir)
{
	struct finger_image_view_record *fivr;

	while (!TAILQ_EMPTY(&fir->finger_views)) {
		fivr = TAILQ_FIRST(&fir->finger_views);
		TAILQ_REMOVE(&fir->finger_views, fivr, list);
		free_fivr(fivr);
	}
	free(fir);
}

void
add_fivr_to_fir(struct finger_image_view_record *fivr, 
		struct finger_image_record *fir)
{
	fivr->fir = fir;
	TAILQ_INSERT_TAIL(&fir->finger_views, fivr, list);
}

/******************************************************************************/
/* Implement the interface for reading/writing/printing finger image records  */
/******************************************************************************/
int
read_fir(FILE *fp, struct finger_image_record *fir)
{
	struct finger_image_view_record *fivr;
	unsigned short sval;
	unsigned long long llval;
	int i;
	int ret;

	OREAD(fir->format_id, 1, FIR_FORMAT_ID_LEN, fp);
	OREAD(fir->spec_version, 1, FIR_SPEC_VERSION_LEN, fp);

	SREAD(&sval, fp);
	LREAD(&fir->record_length, fp);
	llval = sval;
	llval = llval << 32;
	fir->record_length += llval;

	if (fir->format_std == FIR_STD_ANSI) {
		SREAD(&fir->product_identifier_owner, fp);
		SREAD(&fir->product_identifier_type, fp);
	}

	// Capture Eqpt Compliance/Scanner ID
	SREAD(&sval, fp);
	if (fir->format_std == FIR_STD_ANSI) {
		fir->scanner_id = sval & HDR_SCANNER_ID_MASK;
		fir->compliance = (sval & HDR_COMPLIANCE_MASK) >>
		    HDR_COMPLIANCE_SHIFT;
	} else {
		fir->scanner_id = sval & HDR_SCANNER_ID_MASK;
	}

	SREAD(&fir->image_acquisition_level, fp);
	CREAD(&fir->num_fingers_or_palm_images, fp);
	CREAD(&fir->scale_units, fp);
	
        SREAD(&fir->x_scan_resolution, fp);
        SREAD(&fir->y_scan_resolution, fp);
        SREAD(&fir->x_image_resolution, fp);
        SREAD(&fir->y_image_resolution, fp);
        CREAD(&fir->pixel_depth, fp);
        CREAD(&fir->image_compression_algorithm, fp);
        SREAD(&fir->reserved, fp);

	// Read the image views
	for (i = 1; i <= fir->num_fingers_or_palm_images; i++) {
		if (new_fivr(&fivr) < 0) 
			ERR_OUT("Could not allocate FIVR %d", i);

		ret = read_fivr(fp, fivr);
		if (ret == READ_OK)
			add_fivr_to_fir(fivr, fir);
		else if (ret == READ_EOF)
			// XXX Handle a partial read?
			return (READ_EOF);
		else
			ERR_OUT("Could not read entire FIVR %d", i);
	}

	return (READ_OK);

eof_out:
	ERRP("EOF encountered in %s", __FUNCTION__);
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
write_fir(FILE *fp, struct finger_image_record *fir)
{
	struct finger_image_view_record *fivr;
	unsigned short sval;
	unsigned long lval;
	unsigned long long llval;
	int ret;

	OWRITE(fir->format_id, sizeof(char), FIR_FORMAT_ID_LEN, fp);
	OWRITE(fir->spec_version, sizeof(char), FIR_SPEC_VERSION_LEN, fp);

        // The six byte length...
	llval = fir->record_length >> 32;
	sval = (unsigned short)llval;
	lval = (unsigned long)fir->record_length;
	SWRITE(sval, fp);
	LWRITE(lval, fp);

	if (fir->format_std == FIR_STD_ANSI) {
		SWRITE(fir->product_identifier_owner, fp);
		SWRITE(fir->product_identifier_type, fp);
	}

	if (fir->format_std == FIR_STD_ANSI) {
		sval = (fir->compliance << HDR_COMPLIANCE_SHIFT) |
		    fir->scanner_id;
		SWRITE(sval, fp);
	} else {
		SWRITE(fir->scanner_id, fp);
	}

	SWRITE(fir->image_acquisition_level, fp);
        CWRITE(fir->num_fingers_or_palm_images, fp);
        CWRITE(fir->scale_units, fp);
        SWRITE(fir->x_scan_resolution, fp);
        SWRITE(fir->y_scan_resolution, fp);
        SWRITE(fir->x_image_resolution, fp);
        SWRITE(fir->y_image_resolution, fp);
        CWRITE(fir->pixel_depth, fp);
        CWRITE(fir->image_compression_algorithm, fp);
        SWRITE(fir->reserved, fp);

	// Write the image views
	TAILQ_FOREACH(fivr, &fir->finger_views, list) {
		ret = write_fivr(fp, fivr);
		if (ret != WRITE_OK)
			ERR_OUT("Could not write FIVR");
	}

	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
print_fir(FILE *fp, struct finger_image_record *fir)
{
	int ret;
	int i;
	struct finger_image_view_record *fivr;
	char *str;

	// Print the header information
	FPRINTF(fp, "Format ID\t\t\t: %s\nSpec Version\t\t\t: %s\n",
	    fir->format_id, fir->spec_version);

	FPRINTF(fp, "Record Length\t\t\t: %llu\n", fir->record_length);

	if (fir->format_std == FIR_STD_ANSI) {
		FPRINTF(fp, "CBEFF Product ID\t\t: 0x%04x%04x\n",
		    fir->product_identifier_owner,
		    fir->product_identifier_type);
	}

	if (fir->format_std == FIR_STD_ANSI) {
		FPRINTF(fp, "Capture Eqpt\t\t\t: Compliance, ");
		if (fir->compliance == 0) {
			FPRINTF(fp, "None given");
		} else {
			if (fir->compliance & HDR_APPENDIX_F_MASK) {
				FPRINTF(fp, "Appendix F");
			} else {
				FPRINTF(fp, "Unknown");
			}
		}
		FPRINTF(fp, "; ID, 0x%03x\n", fir->scanner_id);
		FPRINTF(fp, "Image acquisition level\t\t: %u\n",
		    fir->image_acquisition_level);
	} else {
		FPRINTF(fp, "Capture Device ID: 0x%04x\n", fir->scanner_id);
	}
	FPRINTF(fp, "Number of images\t\t: %u\n",
	    fir->num_fingers_or_palm_images);

	if (fir->scale_units == FIR_SCALE_UNITS_CM)
		str = "cm";
	else if (fir->scale_units == FIR_SCALE_UNITS_INCH)
		str = "inch";
	else
		str = "invalid";
	FPRINTF(fp, "Scale units\t\t\t: %s\n", str);

	FPRINTF(fp, "Scan resolution\t\t\t: %u X %u\n", fir->x_scan_resolution,
	    fir->y_scan_resolution);
	FPRINTF(fp, "Image resolution\t\t: %u X %u\n", fir->x_image_resolution,
	    fir->y_image_resolution);
	FPRINTF(fp, "Pixel depth\t\t\t: %u\n", fir->pixel_depth);
	FPRINTF(fp, "Image compression algorithm\t: %s\n",
	    comp_algo_to_str(fir->image_compression_algorithm));
	FPRINTF(fp, "Reserved\t\t\t: %u\n", fir->reserved);
	FPRINTF(fp, "\n");

	// Print the finger views
	i = 1;
	TAILQ_FOREACH(fivr, &fir->finger_views, list) {
		FPRINTF(fp, "(%03d) ", i++);
		ret = print_fivr(fp, fivr);
		if (ret != PRINT_OK)
			ERR_OUT("Could not print FIVR");
	}
	FPRINTF(fp, "\n");
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

/******************************************************************************/
/* Implementation of the higher level access routines.                        */
/******************************************************************************/
int
get_fivr_count(struct finger_image_record *fir)
{
	return (fir->num_fingers_or_palm_images);
}

int
get_fivrs(struct finger_image_record *fir,
	  struct finger_image_view_record *fivrs[])
{
	int count = 0;

	struct finger_image_view_record *fivr;

	TAILQ_FOREACH(fivr, &fir->finger_views, list) {
		fivrs[count] = fivr;
		count++;
	}
	return (count);
}

int
fir_stdstr_to_type(char *stdstr)
{
	if (strcmp(stdstr, "ANSI") == 0)
		return (FIR_STD_ANSI);
	if (strcmp(stdstr, "ISO") == 0)
		return (FIR_STD_ISO);
	return (-1);
}

