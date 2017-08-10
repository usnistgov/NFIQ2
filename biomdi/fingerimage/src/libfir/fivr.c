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
/* Implement the interface for allocating and freeing Image View records      */
/******************************************************************************/
int
new_fivr(struct finger_image_view_record **fivr)
{
	struct finger_image_view_record *lfivr;

	lfivr = (struct finger_image_view_record *)malloc(
	    sizeof(struct finger_image_view_record));
	if (lfivr == NULL) {
		perror("Failed allocating memory for FIVR");
		return (-1);
	}
	memset((void *)lfivr, 0, sizeof(struct finger_image_view_record));
	*fivr = lfivr;
	return (0);
}

void
free_fivr(struct finger_image_view_record *fivr)
{
	if (fivr->image_data != NULL)
		free(fivr->image_data);
	free (fivr);
}

void
add_image_to_fivr(char *image, struct finger_image_view_record *fivr)
{
	fivr->image_data = image;
}

/******************************************************************************/
/* Implement the interface for reading and writing Finger Image View records  */
/******************************************************************************/

int
read_fivr(FILE *fp, struct finger_image_view_record *fivr)
{
	LREAD(&fivr->length, fp);
	CREAD(&fivr->finger_palm_position, fp);
	CREAD(&fivr->count_of_views, fp);
	CREAD(&fivr->view_number, fp);
	CREAD(&fivr->quality, fp);
	CREAD(&fivr->impression_type, fp);
	SREAD(&fivr->horizontal_line_length, fp);
	SREAD(&fivr->vertical_line_length, fp);
	CREAD(&fivr->reserved, fp);
	// XXX Need stronger constraints here on length
	if (fivr->length > FIVR_HEADER_LENGTH) {
		fivr->image_data = (char *)malloc(
		    fivr->length - FIVR_HEADER_LENGTH);
		if (fivr->image_data == NULL)
			ERR_OUT("Could not allocate memory for image data");
		else
			OREAD(fivr->image_data, 1,
			    fivr->length - FIVR_HEADER_LENGTH, fp);
	}
	return (READ_OK);
eof_out:
	ERRP("EOF during read of FIVR encountered in %s", __FUNCTION__);
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
write_fivr(FILE *fp, struct finger_image_view_record *fivr)
{
	LWRITE(fivr->length, fp);
	CWRITE(fivr->finger_palm_position, fp);
	CWRITE(fivr->count_of_views, fp);
	CWRITE(fivr->view_number, fp);
	CWRITE(fivr->quality, fp);
	CWRITE(fivr->impression_type, fp);
	SWRITE(fivr->horizontal_line_length, fp);
	SWRITE(fivr->vertical_line_length, fp);
	CWRITE(fivr->reserved, fp);
	if (fivr->image_data != NULL) {
		OWRITE(fivr->image_data, sizeof(char),
			    fivr->length - FIVR_HEADER_LENGTH, fp);
	}
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
print_fivr(FILE *fp, struct finger_image_view_record *fivr)
{
	FPRINTF(fp, "--- Finger Image View Record ---\n");
	FPRINTF(fp, "Length\t\t\t: %u\n", fivr->length);
	FPRINTF(fp, "Position\t\t: %u\n", fivr->finger_palm_position);
	FPRINTF(fp, "Count of views\t\t: %u\n", fivr->count_of_views);
	FPRINTF(fp, "View number\t\t: %u\n", fivr->view_number);
	FPRINTF(fp, "Quality\t\t\t: %u\n", fivr->quality);
	FPRINTF(fp, "Impression type\t\t: %u\n", fivr->impression_type);
	FPRINTF(fp, "Image size\t\t: %u X %u\n", fivr->horizontal_line_length,
	    fivr->vertical_line_length);
	FPRINTF(fp, "Reserved\t\t: %u\n", fivr->reserved);
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

void
copy_fivr(struct finger_image_view_record *src, 
    struct finger_image_view_record *dst)
{

	dst->length = src->length;
	dst->finger_palm_position = src->finger_palm_position;
	dst->count_of_views = src->count_of_views;
	dst->view_number = src->view_number;
	dst->quality = src->quality;
	dst->impression_type = src->impression_type;
	dst->horizontal_line_length = src->horizontal_line_length;
	dst->vertical_line_length = src->vertical_line_length;
	dst->reserved = src->reserved;
}
