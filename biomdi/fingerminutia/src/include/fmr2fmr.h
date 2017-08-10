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
 * Copy the minutiae from an ANSI FVMR to an ISO FVMR, altering
 * the angle value according to the ISO specification.
 * Parameters:
 *  ifvmr  - FVMR containing the finger minutiae data (FMD) records to copy
 *  ofvmr  - FVMR to contain the copied and modified FMDs
 *  length - Will contain the total length of the output FVMR
 *           (header plus length of all FMDs)
 *  xres   - X resolution of input image
 *  yres   - Y resolution of input image
 */
int ansi2iso_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres);

/*
 * Copy the minutiae from an ANSI FVMR to an ISO Compact Card FVMR, altering
 * minutiae records:
 *   alter the angle value to reflect 6 bit precision
 *   alter the (x,y) coordinates to reflect 0.1 pix per millimeter units
 * Parameters:
 *  ifvmr  - FVMR containing the finger minutiae data (FMD) records to copy
 *  ofvmr  - FVMR to contain the copied and modified FMDs
 *  length - Will contain the total length of the output FVMR
 *           (header plus length of all FMDs)
 *  xres   - X resolution of input image
 *  yres   - Y resolution of input image
 */
int ansi2isocc_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres);

/*
 * Copy the minutiae from an ISO FVMR to an ANSI FVMR, altering
 * the angle value according to the ANSI specification.
 * Parameters:
 *  ifvmr  - FVMR containing the finger minutiae data (FMD) records to copy
 *  ofvmr  - FVMR to contain the copied and modified FMDs
 *  length - Will contain the total length of the output FVMR
 *           (header plus length of all FMDs)
 *  xres   - X resolution of input image
 *  yres   - Y resolution of input image
 */
int iso2ansi_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres);

/*
 * Copy the minutiae from an ISO Compact Card FVMR to an ANSI FVMR, altering
 * minutiae records. Alter the (x,y) coordinates to reflect 0.1 pixel per
 * millimeter units.
 * Parameters:
 *  ifvmr  - FVMR containing the finger minutiae data (FMD) records to copy
 *  ofvmr  - FVMR to contain the copied and modified FMDs
 *  length - Will contain the total length of the output FVMR
 *           (header plus length of all FMDs)
 *  xres   - The X resolution of the fingerprint image.
 *  yres   - The Y resolution of the fingerprint image.
 */
int isocc2ansi_fvmr(FVMR *ifvmr, FVMR *ofvmr, unsigned int *length,
    const unsigned short xres, const unsigned short yres);
