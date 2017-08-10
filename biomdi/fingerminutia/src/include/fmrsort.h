/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

/*
 * Define a structure that will be used to sort the minutiae.
 */
struct minutia_sort_data {
	FMD	*fmd;
	int	distance;	// linear distance between two points
	double	z;		// floating point distance
	int	rand;		// a random number associated with the record
	unsigned short	maj_coord;	// The major coordinate
	unsigned short	min_coord;	// The minor coordinate
};

/*
 * Declare the sorting functions. All of these functions sort in ascending
 * order. It is up to the caller the reverse the sort order if that is
 * desired.
 *
 * The sort methods are defined in ISO/IEc 19794-2, "Biometric data interchange
 * formats: Finger minutiae data".
 */

/* sort_fmd_by_polar() modifies the input array by sorting the minutiae
 * using the Polar distance from either the center of mass of the minutiae,
 * or the supplied coordinate.
 * Minutiae with a shorter distance value are stored at lower array entries.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 *   centx  : The coordinates of the center to use for minutiae. This
 *   centy    is only used when the usecm parameter is false.
 *   usecm  : If true, this routine will calculate the center of mass of
 *            all minutiae and use that as the polar pruning point. Otherwise,
 *            the supplied (x, y) coordinate is used.
 */
void sort_fmd_by_polar(FMD **fmds, int mcount, unsigned short centx,
    unsigned short centy, int usecm);

/* sort_fmd_by_random() modifies the input array by sorting the minutiae
 * randomly.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 */
void sort_fmd_by_random(FMD **fmds, int mcount);

/* sort_fmd_by_xy() modifies the input array by sorting the minutiae
 * according to Cartesian x-y coordinates: Sort by the X coordinate,
 * and if they are equal, use the Y coordinate.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 */
void sort_fmd_by_xy(FMD **fmds, int mcount);

/* sort_fmd_by_yy() modifies the input array by sorting the minutiae
 * according to Cartesian y-x coordinates: Sort by the Y coordinate,
 * and if they are equal, use the X coordinate.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 */
void sort_fmd_by_yx(FMD **fmds, int mcount);

/* sort_fmd_by_angle() modifies the input array by sorting the minutiae
 * according to Angle. If two angles are equivalent, the sort order is
 * indeterminate.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 */
void sort_fmd_by_angle(FMD **fmds, int mcount);

/* sort_fmd_by_quality() modifies the input array by sorting the minutiae
 * according to quality. If two quality values are equal, the sort order is
 * indeterminate.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae.
 */
void sort_fmd_by_quality(FMD **fmds, int mcount);
