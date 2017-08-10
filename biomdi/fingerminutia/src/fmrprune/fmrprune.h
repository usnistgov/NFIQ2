/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#define PRUNE_METHOD_POLAR		1
#define PRUNE_METHOD_ELLIPTICAL		2
#define PRUNE_METHOD_RANDOM		3
#define PRUNE_METHOD_RECTANGULAR	4

/* 
 * Select by the rectangular method, as described above. Parameter
 * mcount is the total number of minutiae available, and is set
 * to the number of selected minutiae on return.
 *
 * fmds   - Array of FMD pointers to select; will be modified on output.
 * mcount - Number of minutiae to select on input. On output, will be set
 *          to the actual number of minutiae that were selected.
 * x      - X coordinate of the upper-left point of the rectangle.
 * y      - Y coordinate of the upper-left point of the rectangle.
 * a      - Width of the rectangle.
 * b      - Height of the rectangle.
 */
void
select_fmd_by_rectangular(FMD **fmds, int *mcount, int x, int y, int a, int b);

/* select_fmd_by_elliptical() modifies the input array by storing the minutiae
 * that fall within an ellipse centered on the center of mass of the
 * minutiae.
 * Parameters:
 *   fmds   : The array of pointers to the minutia data records.
 *   mcount : The number of minutiae in input; on output, the actual
 *            number of minutiae that fall within the ellipse.
 *   a      : The semi-major axis of the ellipse.
 *   b      : The semi-minor axis of the ellipse.
 */
void select_fmd_by_elliptical(FMD **fmds, int *mcount, int a, int b);
