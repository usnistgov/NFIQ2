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
/* This file contains the functions necessary to sort a set of minutiae       */
/* using the Cartesian X-Y and Y-X methods.                                   */
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
 * Compare two minutiae records by the major and minor coordinates. These
 * values are set to X-Y or Y-X based on sorting criteria.
 */
static int
compare_by_coords(const void *m1, const void *m2)
{
	struct minutia_sort_data *lm1, *lm2;

	lm1 = (struct minutia_sort_data *)m1;
	lm2 = (struct minutia_sort_data *)m2;

	if (lm1->maj_coord == lm2->maj_coord)
		if (lm1->min_coord < lm2->min_coord)
			return (-1);
		else if (lm1->min_coord > lm2->min_coord)
			return (1);
		else
			return (0);
	else
		if (lm1->maj_coord < lm2->maj_coord)
			return (-1);
		else
			return (1);
}

/*
 * Sort a set of finger minutiae data in Cartesian X-Y order.
 */
void
sort_fmd_by_xy(FMD **fmds, int mcount)
{
	int m;
	struct minutia_sort_data *msds;

	if (mcount == 0)
		return;

	/* Allocate an array to hold the sorting criteria for the FMDs */
	msds = (struct minutia_sort_data *)malloc(mcount *
	    sizeof(struct minutia_sort_data));
	if (msds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	for (m = 0; m < mcount; m++) {
		msds[m].fmd = fmds[m];
		msds[m].maj_coord = fmds[m]->x_coord;
		msds[m].min_coord = fmds[m]->y_coord;
	}
	
	qsort(msds, mcount, sizeof(struct minutia_sort_data),
	    compare_by_coords);

	for (m = 0; m < mcount; m++)
		fmds[m] = msds[m].fmd;

	free(msds);
}

/*
 * Sort a set of finger minutiae data in Cartesian Y-X order.
 */
void
sort_fmd_by_yx(FMD **fmds, int mcount)
{
	int m;
	struct minutia_sort_data *msds;

	if (mcount == 0)
		return;

	/* Allocate an array to hold the sorting criteria for the FMDs */
	msds = (struct minutia_sort_data *)malloc(mcount *
	    sizeof(struct minutia_sort_data));
	if (msds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	for (m = 0; m < mcount; m++) {
		msds[m].fmd = fmds[m];
		msds[m].maj_coord = fmds[m]->y_coord;
		msds[m].min_coord = fmds[m]->x_coord;
	}
	
	qsort(msds, mcount, sizeof(struct minutia_sort_data),
	    compare_by_coords);

	for (m = 0; m < mcount; m++)
		fmds[m] = msds[m].fmd;

	free(msds);
}
