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
/* This file implements the routines to validate ISO 19794-4 Finger Image     */
/* Records according to ISO/IEC 29109-4 conformance testing.                  */
/******************************************************************************/

static biomdiIntSet ansi_image_acquisition_levels = {
	.is_size   = 6,
	.is_values = {
	    10, 20, 30, 31, 40, 41
	}
};
static biomdiIntSet iso_image_acquisition_levels = {
	.is_size   = 7,
	.is_values = {
	    10, 20, 30, 35, 31, 40, 41
	}
};
static biomdiIntSet image_compression_algorithms = {
	.is_size   = 6,
	.is_values = {
	    COMPRESSION_ALGORITHM_UNCOMPRESSED_NO_BIT_PACKED,
	    COMPRESSION_ALGORITHM_UNCOMPRESSED_BIT_PACKED,
	    COMPRESSION_ALGORITHM_COMPRESSED_WSQ,
	    COMPRESSION_ALGORITHM_COMPRESSED_JPEG,
	    COMPRESSION_ALGORITHM_COMPRESSED_JPEG2000,
	    COMPRESSION_ALGORITHM_COMPRESSED_PNG
	}
};
int
validate_fir(struct finger_image_record *fir)
{
	int ret = VALIDATE_OK;
	int hdr_len;
	int check;

	if (strncmp(fir->format_id, FIR_FORMAT_ID, FIR_FORMAT_ID_LEN) != 0) {
		ERRP("Header format ID is [%s], should be [%s]",
		    fir->format_id, FIR_FORMAT_ID);
		ret = VALIDATE_ERROR;
	}
	if (strncmp(fir->spec_version, FIR_SPEC_VERSION, FIR_SPEC_VERSION_LEN) != 0) {
		ERRP("Header spec version is [%s], should be [%s]",
		    fir->spec_version, FIR_SPEC_VERSION);
		ret = VALIDATE_ERROR;
	}
	if (fir->format_std == FIR_STD_ANSI)
		hdr_len = FIR_ANSI_HEADER_LENGTH;
	else
		hdr_len = FIR_ISO_HEADER_LENGTH;
	if (fir->record_length < hdr_len +
	    (fir->num_fingers_or_palm_images * FIVR_HEADER_LENGTH)) {
		ERRP("Record length is too short, minimum is %d", hdr_len);
		ret = VALIDATE_ERROR;
	}
	if (fir->format_std == FIR_STD_ANSI) {
		if (fir->product_identifier_owner == 0) {
			ERRP("Product ID Owner is zero");
			ret = VALIDATE_ERROR;
		}
	}
	if (fir->format_std == FIR_STD_ANSI)
		check = inIntSet(ansi_image_acquisition_levels, 
		    fir->image_acquisition_level);
	else
		check = inIntSet(iso_image_acquisition_levels, 
		    fir->image_acquisition_level);
	if (!check) {
		ERRP("Image acquisition level is invalid");
		ret = VALIDATE_ERROR;
	}
	if (fir->num_fingers_or_palm_images == 0) {
		ERRP("Number of fingers/palms is zero");
		ret = VALIDATE_ERROR;
	}
	if ((fir->scale_units != FIR_SCALE_UNITS_CM) &&
	    (fir->scale_units != FIR_SCALE_UNITS_INCH)) {
		ERRP("Scale units is invalid");
		ret = VALIDATE_ERROR;
	}
	if (fir->x_scan_resolution > FIR_MAX_SCAN_RESOLUTION) {
		ERRP("X scan resolution too large");
		ret = VALIDATE_ERROR;
	}
	if (fir->y_scan_resolution > FIR_MAX_SCAN_RESOLUTION) {
		ERRP("Y scan resolution too large");
		ret = VALIDATE_ERROR;
	}
	if (fir->x_image_resolution > fir->x_scan_resolution) {
		ERRP("X image resolution greater than X scan resolution");
		ret = VALIDATE_ERROR;
	}
	if (fir->y_image_resolution > fir->y_scan_resolution) {
		ERRP("Y image resolution greater than Y scan resolution");
		ret = VALIDATE_ERROR;
	}
	if ((fir->pixel_depth < FIR_MIN_PIXEL_DEPTH) ||
	    (fir->pixel_depth > FIR_MAX_PIXEL_DEPTH)) {
		ERRP("Pixel depth is invalid");
		ret = VALIDATE_ERROR;
	}
	if (!inIntSet(image_compression_algorithms,
	    fir->image_compression_algorithm)) {
		ERRP("Image compression algorithm is invalid");
		ret = VALIDATE_ERROR;
	}
	if (fir->reserved != 0) {
		ERRP("Reserved field is not zero");
		ret = VALIDATE_ERROR;
	}

	return (ret);
}

static biomdiIntSet finger_palm_positions = {
	.is_size   = 31,
	.is_values = {
	    UNKNOWN_FINGER,
	    RIGHT_THUMB,
	    RIGHT_INDEX,
	    RIGHT_MIDDLE,
	    RIGHT_RING,
	    RIGHT_LITTLE,
	    LEFT_THUMB,
	    LEFT_INDEX,
	    LEFT_MIDDLE,
	    LEFT_RING,
	    LEFT_LITTLE,
	    PLAIN_RIGHT_FOUR,
	    PLAIN_LEFT_FOUR,
	    PLAIN_THUMBS,
	    UNKNOWN_PALM,
	    RIGHT_FULL_PALM,
	    RIGHT_WRITERS_PALM,
	    LEFT_FULL_PALM,
	    LEFT_WRITERS_PALM,
	    RIGHT_LOWER_PALM,
	    RIGHT_UPPER_PALM,
	    LEFT_LOWER_PALM,
	    LEFT_UPPER_PALM,
	    RIGHT_OTHER_PALM,
	    LEFT_OTHER_PALM,
	    RIGHT_INTERDIGITAL_PALM,
	    RIGHT_THENAR_PALM,
	    RIGHT_HYPOTHENAR_PALM,
	    LEFT_INTERDIGITAL_PALM,
	    LEFT_THENAR_PALM,
	    LEFT_HYPOTHENAR_PALM
	}
};
static biomdiIntSet impression_types = {
	.is_size   = 7,
	.is_values = {
	    LIVE_SCAN_PLAIN,
	    LIVE_SCAN_ROLLED,
	    NONLIVE_SCAN_PLAIN,
	    NONLIVE_SCAN_ROLLED,
	    LATENT,
	    SWIPE,
	    LIVE_SCAN_CONTACTLESS
	}
};
int
validate_fivr(struct finger_image_view_record *fivr)
{
	int ret;
	int status = VALIDATE_OK;

	if (fivr->length < FIVR_HEADER_LENGTH) {
		ERRP("Record length is less than minimum");
		status = VALIDATE_ERROR;
	}
	if (!inIntSet(finger_palm_positions, fivr->finger_palm_position)) {
		ERRP("Finger position is invalid");
		ret = VALIDATE_ERROR;
	}
	if ((fivr->count_of_views < FIR_MIN_VIEW_COUNT) ||
	    (fivr->count_of_views > FIR_MAX_VIEW_COUNT)) {
		ERRP("Count of views is invalid");
		status = VALIDATE_ERROR;
	}
	if ((fivr->view_number < FIR_MIN_VIEW_COUNT) ||
	    (fivr->view_number > FIR_MAX_VIEW_COUNT)) {
		ERRP("View number is invalid");
		status = VALIDATE_ERROR;
	}
	if (fivr->quality != UNDEFINED_IMAGE_QUALITY) {
		ERRP("Quality is invalid");
		status = VALIDATE_ERROR;
	}
	if (!inIntSet(impression_types, fivr->impression_type)) {
		ERRP("Finger position is invalid");
		ret = VALIDATE_ERROR;
	}
	if (fivr->reserved != 0) {
		ERRP("Reserved is not 0");
		status = VALIDATE_ERROR;
	}
	return (status);
}
