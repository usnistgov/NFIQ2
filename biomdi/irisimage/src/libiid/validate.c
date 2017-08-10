/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/

#include <sys/queue.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <iid_ext.h>
#include <iid.h>

/******************************************************************************/
/* This file implements the routines to validate ISO 19794-6 Iris Image       */
/* Records according to ISO/IEC 29109-6 conformance testing.                  */
/******************************************************************************/

int
validate_iih(IIH *iih)
{
	int ret = VALIDATE_OK;

	if (iih->image_number == 0) {
		ERRP("Image number is 0");
		ret = VALIDATE_ERROR;
	}
	if (iih->image_number > iih->ibsh->num_images) {
		ERRP("Image number %hu greater greater than total of %hu",
		    iih->image_number, iih->ibsh->num_images);
		ret = VALIDATE_ERROR;
	}
	if (iih->rotation_angle != IID_ROT_ANGLE_UNDEF)
		CRSR(iih->rotation_angle, IID_ROT_ANGLE_MIN,
		    IID_ROT_ANGLE_MAX, "Rotation angle");
	if (iih->rotation_uncertainty != IID_ROT_UNCERTAIN_UNDEF)
		CRSR(iih->rotation_uncertainty, IID_ROT_UNCERTAIN_MIN,
		    IID_ROT_UNCERTAIN_MAX, "Rotation Uncertainty");
	return (ret);
}

static biomdiIntSet eye_positions = {
	.is_size   = 3,
	.is_values = {
	    IID_EYE_UNDEF,
	    IID_EYE_RIGHT,
	    IID_EYE_LEFT,
	}
};
int
validate_ibsh(IBSH *ibsh)
{
	int ret = VALIDATE_OK;
	int error;
	IIH *iih;

	if (!inIntSet(eye_positions, ibsh->eye_position)) {
		ERRP("Eye Position 0x%02hhX invalid", ibsh->eye_position);
		ret = VALIDATE_ERROR;
	}
	CRSR(ibsh->num_images, IID_EYE_MIN_IMAGES, IID_EYE_MAX_IMAGES,
	    "Number of Images");
	TAILQ_FOREACH(iih, &ibsh->image_headers, list) {
		error = validate_iih(iih);
		if (error != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}
	return (ret);
}

static biomdiIntSet kinds_of_imagery = {
	.is_size   = 5,
	.is_values = {
	    IID_IMAGE_KIND_RECTLINEAR_NO_ROI_NO_CROPPING,
	    IID_IMAGE_KIND_RECTLINEAR_NO_ROI_CROPPING,
	    IID_IMAGE_KIND_RECTLINEAR_MASKING_CROPPING,
	    IID_IMAGE_KIND_UNSEGMENTED_POLAR,
	    IID_IMAGE_KIND_RECTILINEAR_UNSEGMENTED_POLAR,
	}
};
static biomdiIntSet image_formats = {
	.is_size   = 8,
	.is_values = {
	    IID_IMAGEFORMAT_MONO_RAW,
	    IID_IMAGEFORMAT_RGB_RAW,
	    IID_IMAGEFORMAT_MONO_JPEG,
	    IID_IMAGEFORMAT_RGB_JPEG,
	    IID_IMAGEFORMAT_MONO_JPEG_LS,
	    IID_IMAGEFORMAT_RGB_JPEG_LS,
	    IID_IMAGEFORMAT_MONO_JPEG2000,
	    IID_IMAGEFORMAT_RGB_JPEG2000
	}
};
static biomdiIntSet image_transformations = {
	.is_size   = 2,
	.is_values = {
	    IID_TRANS_UNDEF,
	    IID_TRANS_STD
	}
};
int
validate_iibdb(IIBDB *iibdb)
{
	int ret = VALIDATE_OK;
	int i;
	int error;
	IRH rh = iibdb->record_header;

	if (rh.format_id[IID_FORMAT_ID_LEN - 1] != 0) {
		ERRP("Header format ID is not NULL-terminated.");
		ret = VALIDATE_ERROR;
	} else {
		if (strncmp(rh.format_id, IID_FORMAT_ID,
		    IID_FORMAT_ID_LEN) != 0) {
			ERRP("Header format ID is [%s], should be [%s]",
			    rh.format_id, IID_FORMAT_ID);
			ret = VALIDATE_ERROR;
		}
	}
	/* The version is restricted to numeric ASCII strings, but we
	 * don't have a specific version string to check for.
	 */
	if (rh.format_version[IID_FORMAT_VERSION_LEN - 1] != 0) {
		ERRP("Header format version is not NULL-terminated.");
		ret = VALIDATE_ERROR;
	}
	for (i = 0; i < IID_FORMAT_VERSION_LEN - 1; i++) {
		if (!(isdigit(rh.format_version[i]))) {
			ERRP("Header format ID version is non-numeric.");
			ret = VALIDATE_ERROR;
			break;
		}
	}
	if (!inIntSet(kinds_of_imagery, rh.kind_of_imagery)) {
		ERRP("Kind of imagery %hhu invalid", rh.kind_of_imagery);
		ret = VALIDATE_ERROR;
	}

	CRSR(rh.num_eyes, IID_MIN_EYES, IID_MAX_EYES, "Number of Eyes");
	CSR(rh.record_header_length, IID_RECORD_HEADER_LENGTH,
	    "Record Header Length");
	//XXX Should we check bitfields in iris image properties?
	//XXX should we check iris diameter against image size?

	if (!inIntSet(image_formats, rh.image_format)) {
		ERRP("Image format 0x%04hX invalid", rh.image_format);
		ret = VALIDATE_ERROR;
	}
	if (!inIntSet(image_transformations, rh.image_transformation)) {
		ERRP("Image transformation %hhu invalid",
		    rh.image_transformation);
		ret = VALIDATE_ERROR;
	}
	if ((rh.device_unique_id[0] != IID_DEVICE_UNIQUE_ID_SERIAL_NUMBER) &&
	    (rh.device_unique_id[0] != IID_DEVICE_UNIQUE_ID_MAC_ADDRESS) &&
	    (rh.device_unique_id[0] != IID_DEVICE_UNIQUE_ID_PROCESSOR_ID) &&
	    (memcmp(rh.device_unique_id, IID_DEVICE_UNIQUE_ID_NONE,
		IID_DEVICE_UNIQUE_ID_LEN) != 0)) {
		ERRP("Device Unique ID Invalid");
		ret = VALIDATE_ERROR;
	}
	
	/* Validate the image headers */
	for (i = 0; i < iibdb->record_header.num_eyes; i++) {
		/* Make sure we actually read the biometric subtype */
		if (iibdb->biometric_subtype_headers[i] != NULL) {
			error = validate_ibsh(
			    iibdb->biometric_subtype_headers[i]);
			if (error != VALIDATE_OK)
				ret = VALIDATE_ERROR;
		}
	}
	return (ret);
}

int
validate_roimask(ROIMASK *roimask)
{
//XXX implement whatever constraints there are, if any
	int ret = VALIDATE_OK;

	return (ret);
}

int
validate_unsegpolar(UNSEGPOLAR *unsegpolar)
{
//XXX implement whatever constraints there are, if any
	int ret = VALIDATE_OK;

	return (ret);
}

int
validate_image_ancillary(IMAGEANCILLARY *ancillary)
{
//XXX implement whatever constraints there are, if any
	int ret = VALIDATE_OK;

	return (ret);
}
