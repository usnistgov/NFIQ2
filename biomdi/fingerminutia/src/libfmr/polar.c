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
/* This file contains the functions necessary to select a set of minutiae     */
/* using the Polar method.                                                    */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <biomdimacro.h>
#include <fmr.h>
#include <fmrsort.h>

/*
 * Sort a set of finger minutiae data using the polar criteria.
 * The minutiae are sorted in ascending order according to distance to center
 * of mass, with the closest minutiae appearing at lower array indices.
 * (Note that we actually compare the square of the distance)
 * In the case where the distances are equal, the minutia with the smallest
 * angle is lower in the sort order.
 */
static int
compare_by_polar(const void *m1, const void *m2)
{
	struct minutia_sort_data *lm1, *lm2;

	lm1 = (struct minutia_sort_data *)m1;
	lm2 = (struct minutia_sort_data *)m2;
	if (lm1->distance == lm2->distance)
		if (lm1->fmd->angle < lm2->fmd->angle)
			return (-1);
		else if (lm1->fmd->angle > lm2->fmd->angle)
			return (1);
		else
			return (0);
	else
		if (lm1->distance < lm2->distance)
			return (-1);
		else
			return (1);
}

void
sort_fmd_by_polar(FMD **fmds, int mcount, unsigned short centx,
    unsigned short centy, int usecm)
{
	int m;
	int x, y, x_delta, y_delta;
	struct minutia_sort_data *msds;

	if (mcount == 0)
		return;

	/* Allocate an array to hold the sorting criteria for the FMDs */
	msds = (struct minutia_sort_data *)malloc(mcount *
	    sizeof(struct minutia_sort_data));
	if (msds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	if (usecm) {
		find_center_of_minutiae_mass(fmds, mcount, &x, &y);
	} else {
		x = centx;
		y = centy;
	}
	for (m = 0; m < mcount; m++) {
		x_delta = fmds[m]->x_coord - x;
		y_delta = fmds[m]->y_coord - y;
		msds[m].distance = (x_delta*x_delta) + (y_delta*y_delta);
		msds[m].fmd = fmds[m];
	}
	qsort(msds, mcount, sizeof(struct minutia_sort_data),
	    compare_by_polar);

	for (m = 0; m < mcount; m++)
		fmds[m] = msds[m].fmd;

	free(msds);
}
