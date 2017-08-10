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
/* using the Random method.                                                   */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <biomdimacro.h>
#include <fmr.h>
#include <fmrsort.h>

/*
 * Sort a set of finger minutiae data in a random order.
 */
static int
compare_by_random(const void *m1, const void *m2)
{
	struct minutia_sort_data *lm1, *lm2;

	lm1 = (struct minutia_sort_data *)m1;
	lm2 = (struct minutia_sort_data *)m2;

	if (lm1->rand == lm2->rand)
		return (0);
	if (lm1->rand < lm2->rand)
		return (-1);
	else
		return (1);
}

void
sort_fmd_by_random(FMD **fmds, int mcount)
{
	int m;
	struct minutia_sort_data *msds;

	if (mcount == 0)
		return;

	srand(time(0));

	/* Allocate an array to hold the sorting criteria for the FMDs */
	msds = (struct minutia_sort_data *)malloc(mcount *
	    sizeof(struct minutia_sort_data));
	if (msds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	for (m = 0; m < mcount; m++) {
		msds[m].fmd = fmds[m];
		msds[m].rand = rand();
	}
	qsort(msds, mcount, sizeof(struct minutia_sort_data),
	    compare_by_random);

	for (m = 0; m < mcount; m++)
		fmds[m] = msds[m].fmd;

	free(msds);
}
