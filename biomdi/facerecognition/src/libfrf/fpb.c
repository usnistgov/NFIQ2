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
/*                                                                            */
/* Routines that read, write, and print Feature Point Blocks in compliance    */
/* with the Face Recognition Format for Data Interchange (ANS/INCITS 385-2004)*/
/* format standard.                                                           */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdimacro.h>
#include <frf.h>

int
new_fpb(FPB **fpb)
{
	FPB *lfpb;

	lfpb = (FPB *)malloc(sizeof(FPB));
	if (lfpb == NULL) {
		perror("Failed to allocate Feature Point Block");
		return -1;
	}
	memset((void *)lfpb, 0, sizeof(FPB));
	
	*fpb = lfpb;

	return 0;
}

void
free_fpb(FPB *fpb)
{
	free(fpb);
}

static int
internal_read_fpb(FILE *fp, BDB *fpbdb, FPB *fpb)
{
	unsigned char cval;

	// Feature Type
	CGET(&fpb->type, fp, fpbdb);

	// Feature Point
	CGET(&cval, fp, fpbdb);
	fpb->major_point = (cval & FRF_FEATURE_POINT_MAJOR_MASK) >> 
				FRF_FEATURE_POINT_MAJOR_SHIFT;
	fpb->minor_point = cval & FRF_FEATURE_POINT_MINOR_MASK;

	// Horizontal Position
	SGET(&fpb->x_coord, fp, fpbdb);

	// Vertical Position
	SGET(&fpb->y_coord, fp, fpbdb);

	// Reserved
	SGET(&fpb->reserved, fp, fpbdb);

        return READ_OK;

eof_out:
	return READ_EOF;

err_out:
        return READ_ERROR;
}

int
read_fpb(FILE *fp, FPB *fpb)
{
	return (internal_read_fpb(fp, NULL, fpb));
}

int
scan_fpb(BDB *fpbdb, FPB *fpb)
{
	return (internal_read_fpb(NULL, fpbdb, fpb));
}

static int
internal_write_fpb(FILE *fp, BDB *fpbdb, FPB *fpb)
{
	unsigned char cval;

	CPUT(fpb->type, fp, fpbdb);

	cval = (fpb->major_point << FRF_FEATURE_POINT_MAJOR_SHIFT) | 
		fpb->minor_point;
	CPUT(cval, fp, fpbdb);

	SPUT(fpb->x_coord, fp, fpbdb);
	SPUT(fpb->y_coord, fp, fpbdb);

	SPUT(fpb->reserved, fp, fpbdb);

        return WRITE_OK;

err_out:
        return WRITE_ERROR;
}

int
write_fpb(FILE *fp, FPB *fpb)
{
	return (internal_write_fpb(fp, NULL, fpb));
}

int
push_fpb(BDB *fpbdb, FPB *fpb)
{
	return (internal_write_fpb(NULL, fpbdb, fpb));
}

int
print_fpb(FILE *fp, FPB *fpb)
{
	FPRINTF(fp, "Feature Point Block\n");

	FPRINTF(fp, "\tFeature Type 0x%1u\n", fpb->type);

	FPRINTF(fp, "\tFeature Point %u.%u\n", fpb->major_point,
			fpb->minor_point);

	FPRINTF(fp, "\tCoordinate (%u,%u)\n", fpb->x_coord, fpb->y_coord);

	FPRINTF(fp, "\tReserved 0x%04x\n", fpb->reserved);

        return PRINT_OK;

err_out:
        return PRINT_ERROR;
}
