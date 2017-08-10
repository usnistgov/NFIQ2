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
#include <fmr.h>

/******************************************************************************/
/* This file implements the routines to validate ISO 19794-2 Finger           */
/* Minutiae Records according to ISO/IEC 29109-2 conformance testing.         */
/******************************************************************************/

int
validate_fmr(struct finger_minutiae_record *fmr)
{
	struct finger_view_minutiae_record *fvmr;
	int ret = VALIDATE_OK;
	int error;
	int min_hdr_len = 0;	/* shut up the static analyzer */
	char *ver = NULL;

	if ((fmr->format_std == FMR_STD_ANSI) ||
	    (fmr->format_std == FMR_STD_ISO) ||
	    (fmr->format_std == FMR_STD_ANSI07)) {

		switch (fmr->format_std) {
		case FMR_STD_ANSI:
			min_hdr_len = FMR_ANSI_MIN_RECORD_LENGTH;
			ver = FMR_ANSI_SPEC_VERSION;
			break;
		case FMR_STD_ANSI07:
			min_hdr_len = FMR_ANSI07_MIN_RECORD_LENGTH;
			ver = FMR_ANSI07_SPEC_VERSION;
			break;
		case FMR_STD_ISO:
			min_hdr_len = FMR_ISO_MIN_RECORD_LENGTH;
			ver = FMR_ISO_SPEC_VERSION;
			break;
		}

		// Validate the header
		if (strncmp(fmr->format_id, FMR_FORMAT_ID, FMR_FORMAT_ID_LEN)
		    != 0) {
			ERRP("Header format ID is [%s], should be [%s]",
			fmr->format_id, FMR_FORMAT_ID);
			ret = VALIDATE_ERROR;
		}

		if (strncmp(fmr->spec_version, ver,
		    FMR_SPEC_VERSION_LEN) != 0) {
			ERRP("Header spec version is [%s], should be [%s]",
			fmr->spec_version, ver);
			ret = VALIDATE_ERROR;
		}

		// Record length must be at least as long as the header
		if (fmr->record_length < min_hdr_len) {
			ERRP("Record length is too short, minimum is %d",
			    min_hdr_len);
			ret = VALIDATE_ERROR;
		}

		// CBEFF ID Owner must not be zero
		// This check is taken out for the MINEX tests.
#if !defined(MINEX)
		if (fmr->format_std == FMR_STD_ANSI) {
			if (fmr->product_identifier_owner == 0) {
				ERRP("Product ID Owner is zero");
				ret = VALIDATE_ERROR;
			}
		}
#endif

		if ((fmr->format_std == FMR_STD_ANSI) ||
		    (fmr->format_std == FMR_STD_ISO)) {

			// X resolution shall not be 0
			if (fmr->x_resolution == 0) {
				ERRP("X resolution is set to zero");
				ret = VALIDATE_ERROR;
			}

			// Y resolution shall not be 0
			if (fmr->y_resolution == 0) {
				ERRP("Y resolution is set to zero");
				ret = VALIDATE_ERROR;
			}
		}

		// The reserved field shall not be 0
		if (fmr->reserved != 0) {
			ERRP("The header reserved field is NOT set to zero");
			ret = VALIDATE_ERROR;
		}
	}
	// Validate the finger views
	TAILQ_FOREACH(fvmr, &fmr->finger_views, list) {
		error = validate_fvmr(fvmr);
		if (error != VALIDATE_OK) {
			ret = VALIDATE_ERROR;
			break;
		}
	}

	return (ret);
}

#if defined WINDOWS || defined WIN32
	static biomdiIntSet impressions = {
		6,
		{
			LIVE_SCAN_PLAIN,
			LIVE_SCAN_ROLLED,
			NONLIVE_SCAN_PLAIN,
			NONLIVE_SCAN_ROLLED,
			SWIPE,
			LIVE_SCAN_CONTACTLESS
		}
	};
#else
	static biomdiIntSet impressions = {
		.is_size   = 6,
		.is_values = {
			LIVE_SCAN_PLAIN,
			LIVE_SCAN_ROLLED,
			NONLIVE_SCAN_PLAIN,
			NONLIVE_SCAN_ROLLED,
			SWIPE,
			LIVE_SCAN_CONTACTLESS
		}
	};
#endif
int
validate_fvmr(struct finger_view_minutiae_record *fvmr)
{
	struct finger_minutiae_data *fmd;
	int ret = VALIDATE_OK;
	int valid;
	struct finger_minutiae_record *fmr = fvmr->fmr;

	if ((fvmr->format_std == FMR_STD_ANSI) ||
	    (fvmr->format_std == FMR_STD_ISO)) {

		// Validate the finger number, view number, etc.

		// Note that the finger number check assumes that the range of
		// finger numbers is continuous
		if ((fvmr->finger_number < MIN_FINGER_CODE) || 
		    (fvmr->finger_number > FMR_MAX_FINGER_CODE)) {
			ERRP("Finger number of %u is out of range %u-%u",
			    fvmr->finger_number, MIN_FINGER_CODE,
			    MAX_FINGER_CODE);
			ret = VALIDATE_ERROR;
	}
		// View number
		// The view numbers must increase, starting with 0. The expected
		// minimum finger number is stored in the FMR.
		if ((fmr->next_min_view[fvmr->finger_number] == 0) && 
		    (fvmr->view_number != 0)) {
			ERRP("First view number for finger position %u is %u; "
			    "must start with 0", fvmr->finger_number,
			    fvmr->view_number);
			ret = VALIDATE_ERROR;
		} else if (fvmr->view_number < 
			    fmr->next_min_view[fvmr->finger_number]) {
			ERRP("View number of %u for finger position %u is out"
			    " of sync, expecting minimum value of %u",
			    fvmr->view_number, fvmr->finger_number,
			    fmr->next_min_view[fvmr->finger_number]);
			ret = VALIDATE_ERROR;
		} else {
			fmr->next_min_view[fvmr->finger_number] =
			    fvmr->view_number + 1;
		}

		// Validate impression type code
		if (!inIntSet(impressions, fvmr->impression_type)) {
			ERRP("Impression Type %u is invalid",
				fvmr->impression_type);
			ret = VALIDATE_ERROR;
		}

		// Finger Quality
		if ((fvmr->finger_quality < FMR_MIN_FINGER_QUALITY) ||
		    (fvmr->finger_quality > FMR_MAX_FINGER_QUALITY)) {
			ERRP("Finger Quality %u is out of range %u-%u",
				fvmr->finger_quality, FMR_MIN_FINGER_QUALITY, 
				FMR_MAX_FINGER_QUALITY);
			ret = VALIDATE_ERROR;
		}

	}

	// Number of Minutiae is not constrained by the spec
	// Validate each minutuia data record
	TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
		valid = validate_fmd(fmd);
		if (valid != VALIDATE_OK) {
			ret = VALIDATE_ERROR;
		}
	}

	// Validate the extended data, if present
	if (fvmr->extended != NULL) {
		valid = validate_fedb(fvmr->extended);
		if (valid != VALIDATE_OK) {
			ret = VALIDATE_ERROR;
		}
	}

	return ret;
}

#if defined WINDOWS || defined WIN32
	static biomdiIntSet types = {
		3,
		{
			FMD_MINUTIA_TYPE_OTHER,
			FMD_MINUTIA_TYPE_RIDGE_ENDING,
			FMD_MINUTIA_TYPE_BIFURCATION
		}
	};
#else
	static biomdiIntSet types = {
		.is_size   = 3,
		.is_values = {
			FMD_MINUTIA_TYPE_OTHER,
			FMD_MINUTIA_TYPE_RIDGE_ENDING,
			FMD_MINUTIA_TYPE_BIFURCATION
		}
	};
#endif
int
validate_fmd(struct finger_minutiae_data *fmd)
{
	unsigned short coord;
	int ret = VALIDATE_OK;

	/*
	 * Checking for coord inside image only makes sense when we have
	 * the image size.
	 */
	if ((fmd->format_std == FMR_STD_ANSI) ||
	    (fmd->format_std == FMR_STD_ISO)) {

		// The coordinates must lie within the scanned image
		coord = fmd->fvmr->fmr->x_image_size - 1;
		if (fmd->x_coord > coord) {
		  ERRP("X-coordinate (%u) of Finger Minutia lies outside image",
			fmd->x_coord);
			ret = VALIDATE_ERROR;
		}
		coord = fmd->fvmr->fmr->y_image_size - 1;
		if (fmd->y_coord > coord) {
		  ERRP("Y-coordinate (%u) of Finger Minutia lies outside image",
			fmd->y_coord);
			ret = VALIDATE_ERROR;
		}
	}
	
	// Minutia type is one of these values
	if (!inIntSet(types, fmd->type)) {
		ERRP("Minutia Type %u is not valid", fmd->type);
		ret = VALIDATE_ERROR;
	}

	// Reserved field must be '00'
	if (fmd->reserved != 0) {
		ERRP("Minutia Reserved is %u, should be '00'", fmd->reserved);
		ret = VALIDATE_ERROR;
	}

	// Angle
	if (fmd->format_std == FMR_STD_ANSI) {
		if ((fmd->angle < FMD_MIN_MINUTIA_ANGLE) ||
		    (fmd->angle > FMD_MAX_MINUTIA_ANGLE)) {
			ERRP("Minutia angle %u is out of range %u-%u",
			    fmd->angle, FMD_MIN_MINUTIA_ANGLE,
				FMD_MAX_MINUTIA_ANGLE);
			ret = VALIDATE_ERROR;
		}
	}

	// Quality
	if ((fmd->quality < FMD_MIN_MINUTIA_QUALITY) ||
	    (fmd->quality > FMD_MAX_MINUTIA_QUALITY)) {
		ERRP("Minutia quality %u is out of range %u-%u",
			fmd->quality, FMD_MIN_MINUTIA_QUALITY,
			    FMD_MAX_MINUTIA_QUALITY);
		ret = VALIDATE_ERROR;
	}

	return ret;
}

int
validate_fedb(struct finger_extended_data_block *fedb)
{
	struct finger_extended_data *fed;
	int sum = 0;
	int ret = VALIDATE_OK;

	// The block length must be the sum of the individual data 
	// section lengths
	TAILQ_FOREACH(fed, &fedb->extended_data, list) {
		sum += fed->length;
	}
	if (sum != fedb->block_length) {
		ERRP("Extended Data Block length (%u) is not "\
			"sum of individual data lengths (%u)",
			fedb->block_length, sum);
		ret = VALIDATE_ERROR;
	}

	// Validate the extended data records
	TAILQ_FOREACH(fed, &fedb->extended_data, list) {
		if (validate_fed(fed) != VALIDATE_OK) {
			ERRP("Extended Data Block is not valid");
			ret = VALIDATE_ERROR;
		}
	}
	return (ret);
}

int
validate_fed(struct finger_extended_data *fed)
{
	int ret = VALIDATE_OK;

	switch (fed->type_id) {
	case FED_RIDGE_COUNT :
		ret = validate_rcdb(fed->rcdb);
		break;

	case FED_CORE_AND_DELTA :
		ret = validate_cddb(fed->cddb);
		break;

	default :
		break;
	}

	return (ret);
}

/*
 * This routine will validate the entire record, even if a validation error is
 * encountered at any point.
 */
#if defined WINDOWS || defined WIN32
	static biomdiIntSet methods = {
		3,
		{
			RCE_NONSPECIFIC,
			RCE_FOUR_NEIGHBOR,
			RCE_EIGHT_NEIGHBOR
		}
	};
#else
	static biomdiIntSet methods = {
		.is_size   = 3,
		.is_values = {
			RCE_NONSPECIFIC,
			RCE_FOUR_NEIGHBOR,
			RCE_EIGHT_NEIGHBOR
		}
	};
#endif
int
validate_rcdb(struct ridge_count_data_block *rcdb)
{
	struct ridge_count_data *rcd;
	int ret = VALIDATE_OK;

	// Test the extraction method
	if (!inIntSet(methods, rcdb->method)) {
		ERRP("Extraction method of %u undefined", rcdb->method);
		ret = VALIDATE_ERROR;
	}

	// The index numbers shall be list in increasing order
	// XXX

	// Validate the Ridge Count records
	TAILQ_FOREACH(rcd, &rcdb->ridge_counts, list) {
		if (validate_rcd(rcd) != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}
	return (ret);
}

int
validate_rcd(struct ridge_count_data *rcd)
{
	// The value of index cannot be greater than the number of 
	// minutia records
	if ((rcd->index_one > rcd->rcdb->fed->fedb->fvmr->number_of_minutiae) ||
	    (rcd->index_two > rcd->rcdb->fed->fedb->fvmr->number_of_minutiae)) {
		ERRP("Ridge count index(es) greater than number "
				"number of minutiae");
		return (VALIDATE_ERROR);
	}
	return (VALIDATE_OK);
}

int
validate_cddb(struct core_delta_data_block *cddb)
{
	struct core_data *cd;
	struct delta_data *dd;
	int ret = VALIDATE_OK;

	if (cddb->num_cores < CORE_MIN_NUM) {
		ERRP("Number of cores %u is less than minimum %u",
			cddb->num_cores, CORE_MIN_NUM);
		ret = VALIDATE_ERROR;
	}

	if (cddb->num_deltas < DELTA_MIN_NUM) {
		ERRP("Number of deltas %u is less than minimum %u",
			cddb->num_deltas, DELTA_MIN_NUM);
		ret = VALIDATE_ERROR;
	}

	TAILQ_FOREACH(cd, &cddb->cores, list) {
		if (validate_cd(cd) != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}

	TAILQ_FOREACH(dd, &cddb->deltas, list) {
		if (validate_dd(dd) != VALIDATE_OK)
			ret = VALIDATE_ERROR;
	}
	return (ret);
}

int
validate_cd(struct core_data *cd)
{
	unsigned short coord;
	int ret = VALIDATE_OK;

	// The coordinates must lie within the scanned image
	coord = cd->cddb->fed->fedb->fvmr->fmr->x_image_size - 1;
	if (cd->x_coord > coord) {
	  ERRP("X-coordinate (%u) of Core Data lies outside image", 
		cd->x_coord);
		ret = VALIDATE_ERROR;
	}
	coord = cd->cddb->fed->fedb->fvmr->fmr->y_image_size - 1;
	if (cd->y_coord > coord) {
	  ERRP("Y-coordinate (%u) of Core Data lies outside image",
		cd->y_coord);
		ret = VALIDATE_ERROR;
	}

	// Check the angle
	if ((cd->angle < FMD_MIN_MINUTIA_ANGLE) ||
	    (cd->angle > FMD_MAX_MINUTIA_ANGLE)) {
		ERRP("Core angle %u is out of range %u-%u",
			cd->angle, FMD_MIN_MINUTIA_ANGLE,
			    FMD_MAX_MINUTIA_ANGLE);
		ret = VALIDATE_ERROR;
	}

	return (ret);
}

int
validate_dd(struct delta_data *dd)
{
	unsigned short coord;
	int ret = VALIDATE_OK;

	// The coordinates must lie within the scanned image
	coord = dd->cddb->fed->fedb->fvmr->fmr->x_image_size - 1;
	if (dd->x_coord > coord) {
		ERRP("X-coordinate (%u) of Delta data lies "
			"outside image", dd->x_coord);
		ret = VALIDATE_ERROR;
	}
	coord = dd->cddb->fed->fedb->fvmr->fmr->y_image_size - 1;
	if (dd->y_coord > coord) {
		ERRP("Y-coordinate (%u) of Delta data lies "
			"outside image", dd->y_coord);
		ret = VALIDATE_ERROR;
	}

	// Check the angles
	if ((dd->angle1 < FMD_MIN_MINUTIA_ANGLE) ||
	    (dd->angle1 > FMD_MAX_MINUTIA_ANGLE)) {
		ERRP("Delta angle one %u is out of range %u-%u",
			dd->angle1, FMD_MIN_MINUTIA_ANGLE,
			    FMD_MAX_MINUTIA_ANGLE);
		ret = VALIDATE_ERROR;
	}

	if ((dd->angle2 < FMD_MIN_MINUTIA_ANGLE) ||
	    (dd->angle2 > FMD_MAX_MINUTIA_ANGLE)) {
		ERRP("Delta angle two %u is out of range %u-%u",
			dd->angle2, FMD_MIN_MINUTIA_ANGLE,
			     FMD_MAX_MINUTIA_ANGLE);
		ret = VALIDATE_ERROR;
	}

	if ((dd->angle3 < FMD_MIN_MINUTIA_ANGLE) ||
	    (dd->angle3 > FMD_MAX_MINUTIA_ANGLE)) {
		ERRP("Delta angle three %u is out of range %u-%u",
			dd->angle3, FMD_MIN_MINUTIA_ANGLE,
			    FMD_MAX_MINUTIA_ANGLE);
		ret = VALIDATE_ERROR;
	}

	return (ret);
}
