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
/* This file implements the routines to validate ISO 19794-5 Face             */
/* Recognition Records according to ISO/IEC 29109-5 conformance testing.      */
/******************************************************************************/
#include <sys/queue.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <frf.h>
#include <biomdi.h>
#include <biomdimacro.h>

int
validate_fb(struct facial_block *fb)
{
	int ret = VALIDATE_OK;
	int error;
	struct facial_data_block *fdb;

	// Validate the header
	if (strncmp(fb->format_id, FRF_FORMAT_ID, FRF_FORMAT_ID_LENGTH) != 0) {
		fprintf(stderr, "Header format ID is [%s], should be [%s]\n",
		fb->format_id, FRF_FORMAT_ID);
		ret = VALIDATE_ERROR;
	}

	if (strncmp(fb->version_num, FRF_VERSION_NUM, FRF_VERSION_NUM_LENGTH) != 0) {
		fprintf(stderr, "Header spec version is [%s], should be [%s]\n",
		fb->version_num, FRF_VERSION_NUM);
		ret = VALIDATE_ERROR;
	}

	// Record length is minimum of header length + facial data block length
	if (fb->record_length < FRF_MIN_RECORD_LENGTH) {
		fprintf(stderr, "Record length of %u is short, minimum is %d\n",
			fb->record_length, FRF_MIN_RECORD_LENGTH);
		ret = VALIDATE_ERROR;
	}

	if (fb->num_faces < FRF_MIN_NUM_FACIAL_IMAGES) {
		fprintf(stderr, "Number of facial images is %u, min is %u\n",
			fb->num_faces, FRF_MIN_NUM_FACIAL_IMAGES);
		ret = VALIDATE_ERROR;
	}

	// Validate all the facial data blocks
	TAILQ_FOREACH(fdb, &fb->facial_data, list) {
		error = validate_fdb(fdb);
		if (error != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}

	return (ret);
}

static biomdiIntSet genders = {
	.is_size   = 4,
	.is_values = {
	    GENDER_UNSPECIFIED,
	    GENDER_MALE,
	    GENDER_FEMALE,
	    GENDER_UNKNOWN
	}
};
static biomdiIntSet eye_colors = {
	.is_size   = 9,
	.is_values = {
	    EYE_COLOR_UNSPECIFIED,
	    EYE_COLOR_BLUE,
	    EYE_COLOR_BROWN,
	    EYE_COLOR_GREEN,
	    EYE_COLOR_HAZEL,
	    EYE_COLOR_MAROON,
	    EYE_COLOR_MULTI,
	    EYE_COLOR_PINK,
	    EYE_COLOR_UNKNOWN
	}
};
static biomdiIntSet hair_colors = {
	.is_size   = 16,
	.is_values = {
	    HAIR_COLOR_UNSPECIFIED,
	    HAIR_COLOR_BALD,
	    HAIR_COLOR_BLACK,
	    HAIR_COLOR_BLONDE,
	    HAIR_COLOR_BROWN,
	    HAIR_COLOR_GRAY,
	    HAIR_COLOR_RED,
	    HAIR_COLOR_BLUE,
	    HAIR_COLOR_GREEN,
	    HAIR_COLOR_ORANGE,
	    HAIR_COLOR_PINK,
	    HAIR_COLOR_SANDY,
	    HAIR_COLOR_AUBURN,
	    HAIR_COLOR_WHITE,
	    HAIR_COLOR_STRAWBERRY,
	    HAIR_COLOR_UNKNOWN
	}
};
static biomdiIntSet face_image_types = {
	.is_size   = 4,
	.is_values = {
	    FACE_IMAGE_TYPE_BASIC,
	    FACE_IMAGE_TYPE_FULL_FRONTAL,
	    FACE_IMAGE_TYPE_TOKEN_FRONTAL,
	    FACE_IMAGE_TYPE_OTHER
	}
};
int
validate_fdb(struct facial_data_block *fdb)
{
	int ret = VALIDATE_OK;
	int error;
	struct feature_point_block *fpb;

	// Gender
	if (!inIntSet(genders, fdb->gender)) {
		fprintf(stderr, "Gender is invalid.\n");
                ret = VALIDATE_ERROR;
	}

	// Eye color
	if (!inIntSet(eye_colors, fdb->eye_color)) {
		fprintf(stderr, "Eye color is invalid.\n");
                ret = VALIDATE_ERROR;
	}

	// Hair color
	if (!inIntSet(hair_colors, fdb->hair_color)) {
		fprintf(stderr, "Hair color is invalid.\n");
                ret = VALIDATE_ERROR;
	}

	// Feature Mask
	// This check is commented out for now because it is debatable
	// whether using the reserved bits violates conformance. So
	// we ignore them for now.
	//if (fdb->feature_mask & FEATURE_MASK_RESERVED) {
	//	fprintf(stderr, "Feature Mask is using reserved flags.\n");
	//	ret = VALIDATE_ERROR;
	//}

	// Expression
	if ((fdb->expression >= EXPRESSION_RESERVED_LOW) && 
	    (fdb->expression <= EXPRESSION_RESERVED_HIGH)) {
		fprintf(stderr, "Expresssion is in reserved range.\n");
                ret = VALIDATE_ERROR;
	}

	// Pose Angles
	if ((fdb->pose_angle_yaw != POSE_ANGLE_UNSPECIFIED) &&
	    ((fdb->pose_angle_yaw < POSE_ANGLE_MIN) ||
	    (fdb->pose_angle_yaw > POSE_ANGLE_MAX))) {
		fprintf(stderr, "Pose Angle Yaw is invalid.\n");
                ret = VALIDATE_ERROR;
	}
	if ((fdb->pose_angle_pitch != POSE_ANGLE_UNSPECIFIED) &&
	    ((fdb->pose_angle_pitch < POSE_ANGLE_MIN) ||
	    (fdb->pose_angle_pitch > POSE_ANGLE_MAX))) {
		fprintf(stderr, "Pose Angle Pitch is invalid.\n");
                ret = VALIDATE_ERROR;
	}
	if ((fdb->pose_angle_roll != POSE_ANGLE_UNSPECIFIED) &&
	    ((fdb->pose_angle_roll < POSE_ANGLE_MIN) ||
	    (fdb->pose_angle_roll > POSE_ANGLE_MAX))) {
		fprintf(stderr, "Pose Angle Roll is invalid.\n");
                ret = VALIDATE_ERROR;
	}

	// Pose Angle Uncertainties
	if ((fdb->pose_angle_uncertainty_yaw != 
		POSE_ANGLE_UNCERTAINTY_UNSPECIFIED) &&
	    ((fdb->pose_angle_uncertainty_yaw < POSE_ANGLE_UNCERTAINTY_MIN) ||
	    (fdb->pose_angle_uncertainty_yaw > POSE_ANGLE_UNCERTAINTY_MAX))) {
		fprintf(stderr, "Pose Angle Uncertainty Yaw is invalid.\n");
                ret = VALIDATE_ERROR;
	}
	if ((fdb->pose_angle_uncertainty_pitch != 
		POSE_ANGLE_UNCERTAINTY_UNSPECIFIED) &&
	    ((fdb->pose_angle_uncertainty_pitch < POSE_ANGLE_UNCERTAINTY_MIN) ||
	    (fdb->pose_angle_uncertainty_pitch > POSE_ANGLE_UNCERTAINTY_MAX))) {
		fprintf(stderr, "Pose Angle Uncertainty Pitch is invalid.\n");
                ret = VALIDATE_ERROR;
	}
	if ((fdb->pose_angle_uncertainty_roll != 
		POSE_ANGLE_UNCERTAINTY_UNSPECIFIED) &&
	    ((fdb->pose_angle_uncertainty_roll < POSE_ANGLE_UNCERTAINTY_MIN) ||
	    (fdb->pose_angle_uncertainty_roll > POSE_ANGLE_UNCERTAINTY_MAX))) {
		fprintf(stderr, "Pose Angle Uncertainty Roll is invalid.\n");
                ret = VALIDATE_ERROR;
	}

	// Validate the Feature Point Blocks
	TAILQ_FOREACH(fpb, &fdb->feature_points, list) {
		error = validate_fpb(fpb);
		if (error != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}

	// Image Information Block
	// Facial Image Type
	if (!inIntSet(face_image_types, fdb->face_image_type)) {
		fprintf(stderr, "Image Type is invalid.\n");
		ret = VALIDATE_ERROR;
	}

	// Image Data Type
	if ((fdb->image_data_type != IMAGE_DATA_JPEG) &&
	    (fdb->image_data_type != IMAGE_DATA_JPEG2000))
	{
		fprintf(stderr, "Image Data Type is invalid.\n");
		ret = VALIDATE_ERROR;
	}

	// Image Color Space
	if ((fdb->image_color_space >= COLOR_SPACE_TYPE_RESERVED_MIN) &&
	    (fdb->image_color_space <= COLOR_SPACE_TYPE_RESERVED_MAX)) {
		fprintf(stderr, "Image Color Space is in reserved range.\n");
		ret = VALIDATE_ERROR;
	}

	// Source Type
	if ((fdb->source_type >= SOURCE_TYPE_RESERVED_MIN) &&
	    (fdb->source_type <= SOURCE_TYPE_RESERVED_MAX)) {
		fprintf(stderr, "Image Source Type is in reserved range.\n");
		ret = VALIDATE_ERROR;
	}

	// There is no validation for Device Type

	// Quality
	if (fdb->quality != FRF_IMAGE_QUALITY_UNSPECIFIED) {
		fprintf(stderr, "Image Quality is invalid.\n");
		ret = VALIDATE_ERROR;
	}

	// XXX Verify the Image data?
	
        return ret;

}

int
validate_fpb(struct feature_point_block *fpb)
{
        return VALIDATE_OK;
}
