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
/* using the Elliptical method.                                               */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <biomdimacro.h>
#include <fmr.h>
#include <fmrsort.h>

#include "fmrprune.h"

/*
 * Select a set of finger minutiae data using the elliptical criteria.
 * The center of mass is used as the center (xc, yc) of the ellipse.
 * The equation is:
 *                  2          2 
 *          (x - xc)   (y - yc)
 *          -------- + --------  =  1
 *              2          2
 *             a          b
 *
 * We use the equation for an ellipse to calculate 'z' for each point.
 * If z < 1, the point lies within the ellipse. For our purposes, we sort
 * on the value of z.
 * The minutiae are sorted in ascending order according to
 * with the closest minutiae appearing at lower array indices.
 * In the case where the z values are equal, the minutia with the smallest
 * angle is lower in the sort order.
 */
static int
compare_by_elliptical(const void *m1, const void *m2)
{
	struct minutia_sort_data *lm1, *lm2;

	lm1 = (struct minutia_sort_data *)m1;
	lm2 = (struct minutia_sort_data *)m2;
	if (lm1->z == lm2->z)
		if (lm1->fmd->angle < lm2->fmd->angle)
			return (-1);
		else if (lm1->fmd->angle > lm2->fmd->angle)
			return (1);
		else
			return (0);
	else
		if (lm1->z < lm2->z)
			return (-1);
		else
			return (1);
}

/* 
 * Select by the elliptical method, as described above. Parameter
 * mcount is the total number of minutiae available. Only minutiae
 * that fall within the ellipse are returned.
 */
void
select_fmd_by_elliptical(FMD **fmds, int *mcount, int a, int b)
{
	int m;
	int x, y;
	double fx, fy;
	struct minutia_sort_data *msds;

	if (*mcount == 0)
		return;

	/* Allocate an array to hold the sorting criteria for the FMDs */
	msds = (struct minutia_sort_data *)malloc(*mcount *
	    sizeof(struct minutia_sort_data));
	if (msds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	find_center_of_minutiae_mass(fmds, *mcount, &x, &y);
#ifdef DEBUG
	printf("Center of mass is (%d, %d)\n", x, y);
#endif
	for (m = 0; m < *mcount; m++) {
		fx = (double)(fmds[m]->x_coord - x) / a;
		fy = (double)(fmds[m]->y_coord - y) / b;
		msds[m].fmd = fmds[m];
		msds[m].z = (fx*fx) + (fy*fy);
#ifdef DEBUG
		printf("Minutiae at (%d, %d) has deltas (%f, %f) and z of %f\n",
		    fmds[m]->x_coord, fmds[m]->y_coord, fx, fy, msds[m].z);
#endif
	}
	qsort(msds, *mcount, sizeof(struct minutia_sort_data),
	    compare_by_elliptical);

	for (m = 0; m < *mcount; m++) {
		if (msds[m].z < 1.0) {
			fmds[m] = msds[m].fmd;
		} else {
			*mcount = m;
			break;
		}
	}
	free(msds);
}
