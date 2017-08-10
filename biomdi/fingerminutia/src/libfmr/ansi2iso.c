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

int
ansi2iso_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres)
{
	FMD **ifmds = NULL;
	FMD *ofmd;
	int m, mcount;
	double isotheta;
	double conversion_factor;
	double x, y;
	double xmm, ymm;
	double xunits, yunits;
	int theta;

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

	conversion_factor = 1 / FMD_ISO_ANGLE_UNIT;
	for (m = 0; m < mcount; m++) {
		/* The ISO minutia record units are different than ANSI, so
		 * convert from ANSI units to degrees, then from degrees
		 * to ISO units.
		 */
		if (new_fmd(ofvmr->format_std, &ofmd, m) != 0)
			ALLOC_ERR_RETURN("Output FMD");

		COPY_FMD(ifmds[m], ofmd);
		if (ofvmr->format_std == FMR_STD_ISO_NORMAL_CARD) {
			/* Convert the minutiae using fixed normal card
			 * resolution.
			 */
			x = (double)ifmds[m]->x_coord;
			y = (double)ifmds[m]->y_coord;

			/* millimeters, because INCITS 378 resolution 
			 * values are in pixels per centimeter */
			xmm = 10.0 * x / (double)xres;
			ymm = 10.0 * y / (double)yres;

			/* units of 0.01 pix per mm which is the normal
			 * card format's hardwired sampling freq */
 			xunits = xmm / 0.01;
 			yunits = ymm / 0.01;

			/* round the values - this is what would be
			 * stored in "typical" say 500 dpi operation */
 			ofmd->x_coord = (unsigned short)(0.5 + xunits);
 			ofmd->y_coord = (unsigned short)(0.5 + yunits);
		}
		theta = FMD_ANSI_ANGLE_UNIT * (int)ifmds[m]->angle;
		isotheta = round(conversion_factor * (double)theta);
		ofmd->angle = (unsigned char)isotheta;

		/* Convert the minutia quality from ANSI07 to ANSI04/ISO05 */
		ofmd->quality = ifmds[m]->quality;
		if (ifvmr->format_std == FMR_STD_ANSI07) {
			if ((ifmds[m]->quality == FMD_FAILED_MINUTIA_QUALITY) ||
			    (ifmds[m]->quality ==
				FMD_NOATTTEMPT_MINUTIA_QUALITY)) {
				ofmd->quality = FMD_UNKNOWN_MINUTIA_QUALITY;
			}
		}

		add_fmd_to_fvmr(ofmd, ofvmr);
		if (ofvmr->format_std == FMR_STD_ISO)
			*length += FMD_DATA_LENGTH;
		else
			*length += FMD_ISO_NORMAL_DATA_LENGTH;
	}

	free(ifmds);
	return (0);

err_out:
	if (ifmds != NULL)
		free(ifmds);
	return (-1);
}

/* Convert an FVMR from ANSI to ISO Compact Card format.
 * Note this code does not remove minutiae (as required by 8 bit datatype) nor
 * implement the sort minutiae, (per, say, the cartesian y-x option in 19794-2).
 */
int
ansi2isocc_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres)
{
	FMD **ifmds = NULL;
	FMD *ofmd;
	int mcount, m;
	int theta;
	double conversion_factor;
	double isotheta;
	double x, y;
	double xmm, ymm;
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

	conversion_factor = 1 / FMD_ISOCC_ANGLE_UNIT;
	for (m = 0; m < mcount; m++) {
		if (new_fmd(FMR_STD_ISO_COMPACT_CARD, &ofmd, m) != 0)
			ALLOC_ERR_RETURN("Output FMD");
		COPY_FMD(ifmds[m], ofmd);

		/* The ISO minutia record units are different than ANSI,
		 * so we have to convert from ANSI units to degrees, then
		 * from degrees to ISO units.
		 * Also check the edge condition when hitting the max value.
		 */
		theta = FMD_ANSI_ANGLE_UNIT * (int)ifmds[m]->angle;
		isotheta = round(conversion_factor * (double)theta);
		ofmd->angle = (unsigned char)isotheta;
		if (isotheta > FMD_MAX_MINUTIA_ISOCC_ANGLE)
			ofmd->angle = FMD_MAX_MINUTIA_ISOCC_ANGLE;

		x = (double)ifmds[m]->x_coord;
		y = (double)ifmds[m]->y_coord;

		/* Convert the minutiae using fixed compact card resolution */
		/* millimeters, because INCITS 378 resolution 
		 * values are in pixels per centimeter */
		xmm = 10.0 * x / (double)xres;
		ymm = 10.0 * y / (double)yres;

		/* units of 0.1 pix per mm which is the compact
		 * card format's hardwired sampling freq */
 		xunits = xmm / 0.1;
 		yunits = ymm / 0.1;

		/* round the values */
 		ofmd->x_coord = (unsigned short)(0.5 + xunits);
 		ofmd->y_coord = (unsigned short)(0.5 + yunits);

		add_fmd_to_fvmr(ofmd, ofvmr);
		*length += FMD_ISO_COMPACT_DATA_LENGTH;
	}
	if (ifmds != NULL)
		free(ifmds);
	return (0);

err_out:
	if (ifmds != NULL)
		free(ifmds);
	return (-1);
}
