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
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#if defined WINDOWS || defined WIN32
	#include <string.h>
#else
	#include <strings.h>
#endif

#include <biomdimacro.h>
#include <fmr.h>

/*
 * Convert an FVMR from ISO and ISO Normal Card formats to ANSI.
 * The finger minutiae data is copied, and the angle is converted to the
 * ANSI representation.
 */
int
iso2ansi_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres)
{
	FMD **ifmds = NULL;
	FMD *ofmd;
	int m, mcount;
	double theta;
	double conversion_factor;
	double xcm, ycm;
	double xunits, yunits;

	COPY_FVMR(ifvmr, ofvmr);

	/* For ANSI07, the coord and resolution info
	 * is stored in the FVMR; copy it from the
	 * FMR header into each FVMR; it doesn't hurt
	 * to do this for every output type.
	 */
	ofvmr->x_image_size = ifvmr->fmr->x_image_size;
	ofvmr->y_image_size = ifvmr->fmr->y_image_size;
	ofvmr->x_resolution = ifvmr->fmr->x_resolution;
	ofvmr->y_resolution = ifvmr->fmr->y_resolution;

	*length = FVMR_HEADER_LENGTH;
	mcount = get_fmd_count(ifvmr);
	if (mcount == 0)
		return (0);

	ifmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (ifmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(ifvmr, ifmds) != mcount)
		ERR_OUT("getting FMDs from FVMR");

	/* The ISO minutia record uses all possible values for the
	 * angle, so convert from units to actual theta values.
	 */
	conversion_factor = FMD_ISO_ANGLE_UNIT;
	for (m = 0; m < mcount; m++) {
		if (new_fmd(ofvmr->format_std, &ofmd, m) != 0)
			ALLOC_ERR_RETURN("Output FMD");

		COPY_FMD(ifmds[m], ofmd);
		if (ifvmr->format_std == FMR_STD_ISO_NORMAL_CARD) {
			/* Convert the minutiae using fixed normal card
			 * resolution; ISO NC is 0.01 p/mm, so convert
			 * to 1 p/mm */
			xunits = (double)ifmds[m]->x_coord * 0.01;
			yunits = (double)ifmds[m]->y_coord * 0.01;

			/* Convert from p/mm to p/cm */
			xcm = (xunits * xres) / 10.0;
			ycm = (yunits * yres) / 10.0;

			ofmd->x_coord = (unsigned short)(0.5 + xcm);
			ofmd->y_coord = (unsigned short)(0.5 + ycm);
		}
		theta = round(conversion_factor * (double)(ifmds[m]->angle));
		ofmd->angle = (unsigned char)(round(theta / FMD_ANSI_ANGLE_UNIT));
		/* Check the edge condition where theta rounds greater than
		 * max angle value */
		if (ofmd->angle > FMD_MAX_MINUTIA_ANGLE)
			ofmd->angle = FMD_MAX_MINUTIA_ANGLE;

		/* Convert quality from ANSI04/ISO05 to ANSI07. */
		ofmd->quality = ifmds[m]->quality;
		if (ofvmr->format_std == FMR_STD_ANSI07) {
			if (ifmds[m]->quality == FMD_UNKNOWN_MINUTIA_QUALITY) {
				ofmd->quality = FMD_NOATTTEMPT_MINUTIA_QUALITY;
			}
		}
		add_fmd_to_fvmr(ofmd, ofvmr);
		*length += FMD_DATA_LENGTH;
	}

	free(ifmds);
	return (0);

err_out:
	if (ifmds != NULL)
		free(ifmds);
	return (-1);
}

/*
 * Convert an FVMR from ISO Compact Card format to ANSI.
 */
int
isocc2ansi_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres)
{

	FMD **ifmds = NULL;
	FMD *ofmd;
	int m, mcount;
	double theta;
	double conversion_factor;
	double xcm, ycm;
	double xunits, yunits;

	COPY_FVMR(ifvmr, ofvmr);
	*length = FVMR_HEADER_LENGTH;
	mcount = get_fmd_count(ifvmr);
	if (mcount == 0)
		return (0);

	ifmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (ifmds == NULL)
		ALLOC_ERR_RETURN("FMD array");
	if (get_fmds(ifvmr, ifmds) != mcount)
		ERR_OUT("getting FMDs from FVMR");

	conversion_factor = FMD_ISOCC_ANGLE_UNIT;
	for (m = 0; m < mcount; m++) {
		if (new_fmd(ofvmr->format_std, &ofmd, m) != 0)
			ALLOC_ERR_RETURN("Output FMD");
		COPY_FMD(ifmds[m], ofmd);
		theta = conversion_factor * (double)(ifmds[m]->angle);
		theta = round(theta + 0.5);
		ofmd->angle = (unsigned char)(round(theta / FMD_ANSI_ANGLE_UNIT));
		ofmd->quality = FMD_UNKNOWN_MINUTIA_QUALITY;

		/* ISO CC is 0.1 p/mm, so convert to 1 p/mm */
		xunits = (double)ifmds[m]->x_coord * 0.1;
		yunits = (double)ifmds[m]->y_coord * 0.1;

		/* Convert from p/mm to p/cm */
		xcm = (xunits * xres) / 10.0;
		ycm = (yunits * yres) / 10.0;

		ofmd->x_coord = (unsigned short)(0.5 + xcm);
		ofmd->y_coord = (unsigned short)(0.5 + ycm);

		add_fmd_to_fvmr(ofmd, ofvmr);
		*length += FMD_DATA_LENGTH;
	}

	free(ifmds);
	return (0);

err_out:
	return (-1);
}
