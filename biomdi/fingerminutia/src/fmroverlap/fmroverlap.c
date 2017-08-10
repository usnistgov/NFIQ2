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
/* This program will determine the intersection of two sets of minutiae       */
/* produce some textual output containing the number of minutiae in each file */
/* along with the number of minutiae in common.                               */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <fmr.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

/* Global option indicators */
static int v_opt;
static long r_opt;

/* Global total count of overlapping minutia */
static int total_overlapping_count = 0;

/* Global file pointers */
static FILE *in_fp[2];	// the FMR (378-2004) input files

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
static void
usage()
{
	fprintf(stderr, 
	    "usage:\n\tfmroverlap -i <file1> -i <file2> -r <num> [-v]\n"
	    "\t\t -r: Specifies the radius of interest\n"
	    "\t\t -v: Be verbose\n");
}

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
static void
close_files()
{
	if (in_fp[0] != NULL)
		(void)fclose(in_fp[0]);
	if (in_fp[1] != NULL)
		(void)fclose(in_fp[1]);
	in_fp[0] = NULL;
	in_fp[1] = NULL;
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
static void
get_options(int argc, char *argv[])
{
	int ch;
	int i_opt;

	i_opt = v_opt = 0;
	r_opt = 0;
	while ((ch = getopt(argc, argv, "i:r:v")) != -1) {
		switch (ch) {

		    case 'i':
			if (i_opt == 2) {
				usage();
				goto err_out;
			}
			if ((in_fp[i_opt] = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			i_opt++;
			break;

		    case 'r':
			r_opt = strtol(optarg, NULL, 10);
			if (r_opt == 0 && errno == EINVAL)
                                ERR_OUT("Radius must be numeric");
			if (r_opt < 0)
				ERR_OUT("Radius must be greater than 0");
			break;

		    case 'v':
			v_opt++;
			break;

		    default:
			usage();
			break;
		}
	}

	if ((i_opt != 2) | (r_opt == 0)) {
		usage();
		goto err_out;
	}
	return;

err_out:
	close_files();
	exit(EXIT_FAILURE);
}

static double
dtheta2(const FMD *a, const FMD *b)
{
   const double d = 2.0*(a->angle - b->angle);	/* 2 converts units to degrees */
   const double r = d*M_PI/180.0;		/* convert to radians */
   const double mod360 = 180.0*acos(cos(r))/M_PI; /* avoid wrap around: 5 - 355 = 10 */
   return mod360*mod360;
}

static double
distance2D(const FMD *a, const FMD *b)
{
	int dx = (int)(a->x_coord - b->x_coord);
	int dy = (int)(a->y_coord - b->y_coord);
	double d = sqrt((double)(dx*dx + dy*dy));
	return (d);
}

#ifndef MAX
#define MAX(a,b)	(((a)>(b))?(a):(b))
#endif

#define DEGTORAD(d)	((d) * M_PI / 180.0)

#if 0
/* distance3D is not used, but leave it in for future use. */
static double
distance3D(const FMD *a, const FMD *b)
{
	int dx, dy;
	double d, dtheta;

	dx = (int)(a->x_coord - b->x_coord);
	dy = (int)(a->y_coord - b->y_coord);
	d = sqrt((double)(dx*dx + dy*dy));

	/* While mostly ad hoc this formula makes:
	 * d big when minutiae angles are at 90 degrees,
	 * d the same as the 2D version when minutiae angles are the same,
	 * caps the size increase to (fudge) a factor of 4, and
	 * avoids div by zero
	 */

	/* Double the angle values because INCITS 378 specifies that
	 * they are one-half the actual value.
	 */
	dtheta = (double)((a->angle * 2) - (b->angle * 2));
	d = d / (MAX(0.25, fabs(cos(DEGTORAD(dtheta)))));
	return (d);
}
#endif

/*
 * Compare the minutia records contained in two finger view records.
 * If the finger numbers don't match, this function just returns.
 * Otherwise, the distance between the minutia (one from each view)
 * is calculated. If this distance is less than the specified
 * distance (given as the radius of a circle around the minutiae),
 * the minutia pair is counted as overlapping.
 */
static void
compare_fvmrs(FVMR *fvmr1, FVMR *fvmr2)
{
	FMD **fmds[2] = {NULL, NULL};
	int mcount[2];
        int mcount_common = 0;
	unsigned char *paired[2] = {NULL, NULL};
	int i, j, r, minj;
	double **dmat = NULL;
	double mind, ratio;
        double meanpaireddistance = 0.0;
        double meanpairedangle2 = 0.0;  /* mean square diff in angle */

	mcount[0] = get_fmd_count(fvmr1);
	mcount[1] = get_fmd_count(fvmr2);

	if ((fvmr1->finger_number != fvmr2->finger_number) ||
	    (mcount[0] == 0) || (mcount[1] == 0))
	{
		fprintf(stdout, "%d %d 0 0 0 0\n", mcount[0], mcount[1]);
 		return;
        }


	fmds[0] = (FMD **)malloc(mcount[0] * sizeof(FMD *));
	if (fmds[0] == NULL)
		ALLOC_ERR_OUT("FMR Array");
	if (get_fmds(fvmr1, fmds[0]) != mcount[0])
		ERR_OUT("getting FMRs from first FVMR");


	fmds[1] = (FMD **)malloc(mcount[1] * sizeof(FMD *));
	if (fmds[1] == NULL)
		ALLOC_ERR_OUT("FMR Array");
	if (get_fmds(fvmr2, fmds[1]) != mcount[1])
		ERR_OUT("getting FMRs from second FVMR");

	if (v_opt > 1) {
		printf("The first FVMR:\n");
		(void)print_fvmr(stdout, fvmr1);
		printf("The second FVMR:\n");
		(void)print_fvmr(stdout, fvmr2);
	}

	paired[0] = (unsigned char *)malloc(mcount[0] * sizeof(unsigned char));
	if (paired[0] == NULL)
		ALLOC_ERR_OUT("Paired flags");
	paired[1] = (unsigned char *)malloc(mcount[1] * sizeof(unsigned char));
	if (paired[1] == NULL)
		ALLOC_ERR_OUT("Paired flags");
	bzero((void *)paired[0], mcount[0]);
	bzero((void *)paired[1], mcount[1]);

	dmat = (double **)malloc(mcount[0] * sizeof(double *));
	if (dmat == NULL)
		ALLOC_ERR_OUT("Distance array");
	for (i = 0; i < mcount[0]; i++) {
		dmat[i] = (double *)malloc(mcount[1] * sizeof(double));
		if (dmat[i] == NULL)
			ALLOC_ERR_OUT("Distance array");
	}

	for (i = 0; i < mcount[0]; i++)
		for (j = 0; j < mcount[1]; j++)
			dmat[i][j] = distance2D(fmds[0][i], fmds[1][j]);

	for (r = 0; r <= r_opt; r++) {
		for (i = 0; i < mcount[0]; i++) {
			if (paired[0][i] == 1)
				continue;
			mind = 100000;
			minj = -1;
			for (j = 0; j < mcount[1]; j++) {
				if ((paired[1][j] == 0) &&
				    (dmat[i][j] < mind)) {
					mind = dmat[i][j];
					minj = j;
				}
			}
		/* There will be times when everything in the second record
		 * is paired whence minj will still be -1. This might occur
		 * when there are fewer minutiae in the second template than
		 * in the first. But if we have found a close candidate, then
		 * determine if it is close enough and not already paired.
		 */
			if ((minj > -1) && (mind <= r) &&
			    (paired[1][minj] == 0)) {
				if (v_opt > 0) {
					printf("Distance of %.2f calculated "
					"for this matching pair:\n",
					    dmat[i][minj]);
					print_fmd(stdout, fmds[0][i]);
					print_fmd(stdout, fmds[1][minj]);
					printf("-----------------------\n");
				}
				paired[0][i] = paired[1][minj] = 1;
				meanpaireddistance += dmat[i][minj];
                                meanpairedangle2 += dtheta2(fmds[0][i], fmds[1][minj]);
				mcount_common++;
			}
		}
	}
	/* Final summary stats are:
	 * 1. the size of the intersection set divided by the size of the smaller of the
	 * number of minutiae in the two input templates - this will be on range [0,1].
	 * 2. the mean displacement of those minutiae that are found to be paired
	 */
	ratio = (double)mcount_common / (double)((mcount[0] < mcount[1]) ?
	    mcount[0] : mcount[1]);

        if (mcount_common > 0)
        {
	   meanpaireddistance /= (double)mcount_common;
	   meanpairedangle2 = sqrt(meanpairedangle2 / (double)mcount_common); /* rms */
        }
	fprintf(stdout, "%d %d %d %lf %lf %lf\n", mcount[0], mcount[1], mcount_common,
               ratio, meanpaireddistance, meanpairedangle2);
	total_overlapping_count += mcount_common;

err_out:
	if (dmat != NULL) {
		for (i = 0; i < mcount[0]; i++)
			if (dmat[i] != NULL)
				free(dmat[i]);
		free (dmat);
	}
	if (paired[0] != NULL)
		free (paired[0]);
	if (paired[1] != NULL)
		free (paired[1]);
	if (fmds[0] != NULL)
		free(fmds[0]);
	if (fmds[1] != NULL)
		free(fmds[1]);
	return;
}

int
main(int argc, char *argv[])
{
	FMR *fmrs[2] = {NULL, NULL};
	FVMR **fvmrs[2] = {NULL, NULL};
	int i, fvmrcnt[2];
	int exit_status = EXIT_FAILURE;

	get_options(argc, argv);

	if (new_fmr(FMR_STD_ANSI, &fmrs[0]) < 0)
		ALLOC_ERR_OUT("Input FMR");
	if (new_fmr(FMR_STD_ANSI, &fmrs[1]) < 0)
		ALLOC_ERR_OUT("Input FMR");

	if (read_fmr(in_fp[0], fmrs[0]) != READ_OK) {
		fprintf(stderr, "Could not read FMR from the first file.\n");
		goto err_out;
	}
	if (read_fmr(in_fp[1], fmrs[1]) != READ_OK) {
		fprintf(stderr, "Could not read FMR from the second file.\n");
		goto err_out;
	}

	for (i = 0; i < 2; i++) {
		/* Get all of the finger view records for each FMR */
		fvmrcnt[i] = get_fvmr_count(fmrs[i]);
		if (fvmrcnt[i] > 0) {
			fvmrs[i] = (FVMR **)malloc(fvmrcnt[i] * sizeof(FVMR *));
			if (fvmrs[i] == NULL)
				ALLOC_ERR_OUT("FVMR Array");
			if (get_fvmrs(fmrs[i], fvmrs[i]) != fvmrcnt[i])
				ERR_OUT("getting FVMRs from FMR");

		} else {
			if (fvmrcnt[i] == 0)
				ERR_OUT("there are no FVMRs in the input FMR");
			else
				ERR_OUT("retrieving FVMRs from input FMR");
		}
	}

	/* the files must contain the same number of FVMR's */
	if (fvmrcnt[0] != fvmrcnt[1])
		ERR_OUT("Files do not contain same number of finger view minutia records");

	/* Loop through the FVMRs from the first file, and compare
	 * to the corresponding FVMR in the second.
	 */
	for (i = 0; i < fvmrcnt[0]; i++)
		compare_fvmrs(fvmrs[0][i], fvmrs[1][i]);

	close_files();

	/* printf("Total number of overlapping minutiae is %d.\n",
	 *   total_overlapping_count);
        */

	exit_status = EXIT_SUCCESS;

err_out:
	if (fmrs[0] != NULL)
		free_fmr(fmrs[0]);
	if (fmrs[1] != NULL)
		free_fmr(fmrs[1]);
	if (fvmrs[0] != NULL)
		free(fvmrs[0]);
	if (fvmrs[1] != NULL)
		free(fvmrs[1]);

	close_files();

	exit(exit_status);
}
