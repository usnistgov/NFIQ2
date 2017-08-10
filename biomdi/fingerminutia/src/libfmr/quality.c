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
/* using the quality value.                                                   */
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
 * Compare two minutiae records by the minutiae quality.
 */
static int
compare_by_quality(const void *m1, const void *m2)
{
	FMD **lmpp;
	FMD *lm1, *lm2;

	lmpp = (FMD **)m1;
	lm1 = (FMD *)*lmpp;
	lmpp = (FMD **)m2;
	lm2 = (FMD *)*lmpp;

	if (lm1->quality == lm2->quality)
		return (0);
	else
		if (lm1->quality < lm2->quality)
			return (-1);
		else
			return (1);
}

void
sort_fmd_by_quality(FMD **fmds, int mcount)
{
	int m;
	FMD **lfmds;

	if (mcount == 0)
		return;

	/* Allocate an array to hold the sorting criteria for the FMDs */
	lfmds = (FMD **)malloc(mcount * sizeof(FMD *));
	if (lfmds == NULL)
		ALLOC_ERR_EXIT("Sorting criteria array");

	for (m = 0; m < mcount; m++)
		lfmds[m] = fmds[m];
	
	qsort(lfmds, mcount, sizeof(FMD *), compare_by_quality);

	for (m = 0; m < mcount; m++)
		fmds[m] = lfmds[m];

	free(lfmds);
}
