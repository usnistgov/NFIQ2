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
/* Implementation of the Finger Minutiae Data record processing interface.    */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdimacro.h>
#include <fmr.h>


/*
 * Convert from angle represntation in units to degrees.
 */
static int
fmd_convert_angle(FMD *fmd)
{
	double theta;

	switch (fmd->format_std) {
		case FMR_STD_ANSI:
		case FMR_STD_ANSI07:
			return ((int)fmd->angle * FMD_ANSI_ANGLE_UNIT);
			break;			/* not reached */
		case FMR_STD_ISO:
		case FMR_STD_ISO_NORMAL_CARD:
			theta = round((double)FMD_ISO_ANGLE_UNIT *
			    (double)fmd->angle);
			return ((int)theta);
			break;			/* not reached */
		case FMR_STD_ISO_COMPACT_CARD:
			theta = round((double)FMD_ISOCC_ANGLE_UNIT *
			    (double)fmd->angle);
			return ((int)theta);
			break;			/* not reached */
		default:
			ERRP("%s called with incorrect standard type.\n:",
				__FUNCTION__);
			return (0);
			break;			/* not reached */
	}
}

/*
 * Convert the minutia type to human-readable string.
 */
static char *
fmd_type_string(FMD *fmd)
{
	switch (fmd->type) {
		case FMD_MINUTIA_TYPE_OTHER:
			return ("Other");
			break;			/* not reached */
		case FMD_MINUTIA_TYPE_RIDGE_ENDING:
			return ("Ridge Ending");
			break;			/* not reached */
		case FMD_MINUTIA_TYPE_BIFURCATION:
			return ("Bifurcation");
			break;			/* not reached */
		default:
			return ("Unknown");
			break;			/* not reached */
	}
}

int
new_fmd(unsigned int format_std, struct finger_minutiae_data **fmd,
    unsigned int index)
{
	struct finger_minutiae_data *lfmd;
	lfmd = (struct finger_minutiae_data *)malloc(
		sizeof(struct finger_minutiae_data));
	if (lfmd == NULL) {
		perror("Failed to allocate Finger Minutiae Data record");
		return (-1);
	}
	memset((void *)lfmd, 0, sizeof(struct finger_minutiae_data));
	lfmd->format_std = format_std;
	lfmd->index = index;
	*fmd = lfmd;
	return 0;
}

void
free_fmd(struct finger_minutiae_data *fmd)
{
	free(fmd);
}

/*
 * Read ISO compact card finger minutiae.
 */
static int
read_iso_compact_fmd(FILE *fp, BDB *fmdb, struct finger_minutiae_data *fmd)
{
	unsigned char cval;

	// X Coord
	CGET(&cval, fp, fmdb);
	fmd->x_coord = (unsigned short)cval;

	// Y Coord
	CGET(&cval, fp, fmdb);
	fmd->y_coord = (unsigned short)cval;

	// Type/angle
	CGET(&cval, fp, fmdb);
	fmd->type = (unsigned char)
	    ((cval & FMD_ISO_COMPACT_MINUTIA_TYPE_MASK) >>
		FMD_ISO_COMPACT_MINUTIA_TYPE_SHIFT);
	fmd->angle = (unsigned char)(cval & FMD_ISO_COMPACT_MINUTIA_ANGLE_MASK);

	fmd->reserved = 0;

	// There is no quality value in the ISO compact record
	fmd->quality = ISO_UNKNOWN_FINGER_QUALITY;

	return READ_OK;
eof_out:
	return READ_EOF;
err_out:
	return READ_ERROR;
}

/*
 * Read ANSI, ISO, and ISO normal card finger minutiae.
 */
static int
read_ansi_iso_fmd(FILE *fp, BDB *fmdb, struct finger_minutiae_data *fmd)
{
	unsigned short sval;
	unsigned char cval;

	// Type/X Coord
	SGET(&sval, fp, fmdb);
	fmd->type = (unsigned char)
			((sval & FMD_MINUTIA_TYPE_MASK) >> 
				FMD_MINUTIA_TYPE_SHIFT);
	fmd->x_coord = sval & FMD_X_COORD_MASK;

	// Y Coord
	SGET(&sval, fp, fmdb);

	// We save the reserved field for conformance checking
	fmd->reserved = (unsigned char)
			((sval & FMD_RESERVED_MASK) >> 
				FMD_RESERVED_SHIFT);
	fmd->y_coord = sval & FMD_Y_COORD_MASK;

	// Minutia angle
	CGET(&cval, fp, fmdb);
	fmd->angle = cval;

	if (fmd->format_std != FMR_STD_ISO_NORMAL_CARD) {
		CGET(&cval, fp, fmdb);
		fmd->quality = cval;
	}

	return READ_OK;

eof_out:
	return READ_EOF;
err_out:
	return READ_ERROR;
}

int
read_fmd(FILE *fp, struct finger_minutiae_data *fmd)
{
	if (fmd->format_std == FMR_STD_ISO_COMPACT_CARD)
		return (read_iso_compact_fmd(fp, NULL, fmd));
	else
		return (read_ansi_iso_fmd(fp, NULL, fmd));
}

int
scan_fmd(BDB *fmdb, struct finger_minutiae_data *fmd)
{
	if (fmd->format_std == FMR_STD_ISO_COMPACT_CARD)
		return (read_iso_compact_fmd(NULL, fmdb, fmd));
	else
		return (read_ansi_iso_fmd(NULL, fmdb, fmd));
}

/*
 * Write ISO compact card finger minutiae.
 */
static int
write_iso_compact_fmd(FILE *fp, BDB *fmdb, struct finger_minutiae_data *fmd)
{
	unsigned char cval;

	// X Coord
	cval = fmd->x_coord;
	CPUT(cval, fp, fmdb);

	// Y Coord
	cval = fmd->y_coord;
	CPUT(cval, fp, fmdb);

	// Type/angle
	cval = fmd->type << FMD_ISO_COMPACT_MINUTIA_TYPE_SHIFT;
	cval = cval | (fmd->angle & FMD_ISO_COMPACT_MINUTIA_ANGLE_MASK);
	CPUT(cval, fp, fmdb);

	return WRITE_OK;

err_out:
	return WRITE_ERROR;
}

/*
 * Write ANSI, ISO, and ISO normal card finger minutiae.
 */
static int
write_ansi_iso_fmd(FILE *fp, BDB *fmdb, struct finger_minutiae_data *fmd)
{
	unsigned short sval;
	unsigned char cval;

	// Type/X Coord
	sval = (unsigned short)(fmd->type << FMD_MINUTIA_TYPE_SHIFT);
	sval = sval | (fmd->x_coord & FMD_X_COORD_MASK);
	SPUT(sval, fp, fmdb);

	// Y Coord/Reserved
	sval = fmd->y_coord & FMD_Y_COORD_MASK;
	SPUT(sval, fp, fmdb);

	// Minutia angle
	cval = fmd->angle;
	CPUT(cval, fp, fmdb);

	// Minutia quality
	if (fmd->format_std != FMR_STD_ISO_NORMAL_CARD) {
		cval = fmd->quality;
		CPUT(cval, fp, fmdb);
	}

	return WRITE_OK;

err_out:
	return WRITE_ERROR;
}

int
write_fmd(FILE *fp, struct finger_minutiae_data *fmd)
{
	if (fmd->format_std == FMR_STD_ISO_COMPACT_CARD)
		return (write_iso_compact_fmd(fp, NULL, fmd));
	else
		return (write_ansi_iso_fmd(fp, NULL, fmd));
}

int
push_fmd(BDB *fmdb, struct finger_minutiae_data *fmd)
{
	if (fmd->format_std == FMR_STD_ISO_COMPACT_CARD)
		return (write_iso_compact_fmd(NULL, fmdb, fmd));
	else
		return (write_ansi_iso_fmd(NULL, fmdb, fmd));
}

int
print_fmd(FILE *fp, struct finger_minutiae_data *fmd)
{
	fprintf(fp, "Finger Minutiae Data:\n");
	fprintf(fp, "\tType\t\t: 0x%01x (%s)\n", fmd->type,
	    fmd_type_string(fmd));
	fprintf(fp, "\tCoordinate\t: (%u,%u)\n", fmd->x_coord, fmd->y_coord);
	fprintf(fp, "\tAngle\t\t: %u (%u degrees)\n",
	    fmd->angle, fmd_convert_angle(fmd));
	if ((fmd->format_std == FMR_STD_ANSI) ||
	    (fmd->format_std == FMR_STD_ISO) ||
	    (fmd->format_std == FMR_STD_ANSI07))
		fprintf(fp, "\tQuality\t\t: %u\n", fmd->quality);

	return 0;
}

/******************************************************************************/
/* Find the center-of-mass for a set of minutiae.                             */
/******************************************************************************/
void
find_center_of_minutiae_mass(FMD **fmds, int mcount, int *x, int *y)
{
	int lx, ly, i;

	lx = ly = 0;
	for (i = 0; i < mcount; i++) {
		lx += fmds[i]->x_coord;
		ly += fmds[i]->y_coord;
	}
	*x = lx / mcount;
	*y = ly / mcount; 
}
