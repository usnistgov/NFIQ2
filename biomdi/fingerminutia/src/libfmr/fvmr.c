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
/* Manipulate Finger View records. The FV record is not explicitly part of    */
/* the ANSI-INCITS spec. This package combines the Finger View record         */
/* processing with the Finger View Minutiae Record.                           */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <fmr.h>

/******************************************************************************/
/* Implement the interface for allocating and freeing Finger View Minutiae    */
/* Records                                                                    */
/******************************************************************************/
int
new_fvmr(unsigned int format_std, struct finger_view_minutiae_record **fvmr)
{
	struct finger_view_minutiae_record *lfvmr;

	lfvmr = (struct finger_view_minutiae_record *)malloc(
			sizeof(struct finger_view_minutiae_record));
	if (lfvmr == NULL) {
		perror("Failed to allocate Finger View Minutiae Record");
		return -1;
	}
	memset((void *)lfvmr, 0, sizeof(struct finger_view_minutiae_record));
	lfvmr->format_std = format_std;
	lfvmr->extended = NULL;
	lfvmr->partial = FALSE;
	TAILQ_INIT(&lfvmr->minutiae_data);
	*fvmr = lfvmr;
	return 0;
}

void
free_fvmr(struct finger_view_minutiae_record *fvmr)
{
	struct finger_minutiae_data *fmd;

	// Free the Finger Minutiae Records contained within the FVMR
	while (!TAILQ_EMPTY(&fvmr->minutiae_data)) {
		fmd = TAILQ_FIRST(&fvmr->minutiae_data);
		TAILQ_REMOVE(&fvmr->minutiae_data, fmd, list);
		free_fmd(fmd);
	}
	if (fvmr->extended != NULL) {
		free_fedb(fvmr->extended);
	}

	// Free the FVMR itself
	free(fvmr);
}

/******************************************************************************/
/* Implement the interface for reading and writing Finger View Minutiae       */
/* records.                                                                   */
/******************************************************************************/
static int
internal_read_fvmr(FILE *fp, BDB *fmdb,
    struct finger_view_minutiae_record *fvmr)
{
	unsigned char cval;
	unsigned short sval;
	unsigned int lval;
	int i;
	struct finger_minutiae_data *fmd;
	struct finger_extended_data_block *fedb;
	int ret;

	/* ISO normal and compact card formats don't have a finger view
	 * header, so we will just read the minutiae data directly.
	 */
	if ((fvmr->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (fvmr->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		ret = READ_OK;
		i = 1;
		while (ret == READ_OK) {
			if (new_fmd(fvmr->format_std, &fmd, i) < 0)
				ERR_OUT("Could not allocate FMD %d", i);

			if (fp != NULL)
				ret = read_fmd(fp, fmd);
			else
				ret = scan_fmd(fmdb, fmd);
			if (ret == READ_OK) {
				add_fmd_to_fvmr(fmd, fvmr);
				i++;
				fvmr->number_of_minutiae++;
			} else if (ret == READ_EOF)
				return READ_OK;
			else 
				ERR_OUT("Could not read FMD %d", i);
		}
	}
	CGET(&cval, fp, fmdb);
	fvmr->finger_number = cval;

	if (fvmr->format_std == FMR_STD_ANSI07) {
		CGET(&cval, fp, fmdb);
		fvmr->view_number = cval;
		CGET(&cval, fp, fmdb);
		fvmr->impression_type = cval;
		CGET(&cval, fp, fmdb);
		fvmr->finger_quality = (unsigned short)cval;
		LGET(&lval, fp, fmdb);
		fvmr->algorithm_id = lval;
		SGET(&sval, fp, fmdb);
		fvmr->x_image_size = sval;
		SGET(&sval, fp, fmdb);
		fvmr->y_image_size = sval;
		SGET(&sval, fp, fmdb);
		fvmr->x_resolution = sval;
		SGET(&sval, fp, fmdb);
		fvmr->y_resolution = sval;
	} else {
		CGET(&cval, fp, fmdb);
		fvmr->view_number = (cval & FVMR_VIEW_NUMBER_MASK) >> 
					FVMR_VIEW_NUMBER_SHIFT;
		fvmr->impression_type = (unsigned short)(cval &
		    FVMR_IMPRESSION_MASK);
		CGET(&cval, fp, fmdb);
		fvmr->finger_quality = (unsigned short)cval;
	}
	CGET(&cval, fp, fmdb);
	fvmr->number_of_minutiae = cval;

	// Finger minutiae data
	for (i = 0; i < fvmr->number_of_minutiae; i++) {
		if (new_fmd(fvmr->format_std, &fmd, i+1) < 0)
			ERR_OUT("Could not allocate FMD %d", i);

		if (fp != NULL)
			ret = read_fmd(fp, fmd);
		else
			ret = scan_fmd(fmdb, fmd);
		if (ret == READ_OK)
			add_fmd_to_fvmr(fmd, fvmr);
		else if (ret == READ_EOF)
			goto eof_out;
		else 
			ERR_OUT("Could not read FMD %d", i);
	}

	// Read the extended data block, if it exists
	if (new_fedb(fvmr->format_std, &fedb) < 0)
		ERR_OUT("Could not allocate extended data block");

	if (fp != NULL)
		ret = read_fedb(fp, fedb);
	else
		ret = scan_fedb(fmdb, fedb);
	if (ret == READ_ERROR)
		ERR_OUT("Could not read extended data block");

	// EOF is OK as we may have read a partial block
	if (fedb->partial)
		fvmr->partial = TRUE;
	if (fedb->block_length != 0)
		add_fedb_to_fvmr(fedb, fvmr);
	else
		free_fedb(fedb);

	return ret;

eof_out:
	ERRP("EOF while reading Finger View Minutiae Record");
	return READ_EOF;
err_out:
	return READ_ERROR;
}

int
read_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	return (internal_read_fvmr(fp, NULL, fvmr));
}

int
scan_fvmr(BDB *fmdb, struct finger_view_minutiae_record *fvmr)
{
	return (internal_read_fvmr(NULL, fmdb, fvmr));
}

static int
internal_write_fvmr(FILE *fp, BDB *fmdb,
    struct finger_view_minutiae_record *fvmr)
{
	struct finger_minutiae_data *fmd;
	unsigned char cval;
	int ret;

	/* ISO normal and compact card formats don't have a finger view
	 * header, so we will just write the minutiae data directly.
	 */
	if ((fvmr->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (fvmr->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
			if (fp != NULL)
				ret = write_fmd(fp, fmd);
			else
				ret = push_fmd(fmdb, fmd);
			if (ret != WRITE_OK)
				ERR_OUT("Could not write minutiae data");
		}
		return WRITE_OK;
	}
	CPUT(fvmr->finger_number, fp, fmdb);

	if (fvmr->format_std == FMR_STD_ANSI07) {
		CPUT(fvmr->view_number, fp, fmdb);
		CPUT(fvmr->impression_type, fp, fmdb);
		CPUT(fvmr->finger_quality, fp, fmdb);
		LPUT(fvmr->algorithm_id, fp, fmdb);
		SPUT(fvmr->x_image_size, fp, fmdb);
		SPUT(fvmr->y_image_size, fp, fmdb);
		SPUT(fvmr->x_resolution, fp, fmdb);
		SPUT(fvmr->y_resolution, fp, fmdb);
	} else {
		cval = (fvmr->view_number << FVMR_VIEW_NUMBER_SHIFT) | 
			fvmr->impression_type;
		CPUT(cval, fp, fmdb);
		CPUT(fvmr->finger_quality, fp, fmdb);
	}
	CPUT(fvmr->number_of_minutiae, fp, fmdb);

	// Write each Finger Minutiae Data record
	TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
		if (fp != NULL)
			ret = write_fmd(fp, fmd);
		else
			ret = push_fmd(fmdb, fmd);
		if (ret != WRITE_OK)
			ERR_OUT("Could not write minutiae data");
	}

	// Write the extended data
	if (fp != NULL)
		ret = write_fedb(fp, fvmr->extended);
	else
		ret = push_fedb(fmdb, fvmr->extended);
	if (ret != WRITE_OK)
		ERR_OUT("Could not write extended data block");

	return WRITE_OK;

err_out:
	return WRITE_ERROR;
}

int
write_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	return (internal_write_fvmr(fp, NULL, fvmr));
}

int
push_fvmr(BDB *fmdb, struct finger_view_minutiae_record *fvmr)
{
	return (internal_write_fvmr(NULL, fmdb, fvmr));
}

int
print_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr)
{
	struct finger_minutiae_data *fmd;

	if ((fvmr->format_std == FMR_STD_ANSI) ||
	    (fvmr->format_std == FMR_STD_ANSI07) ||
	    (fvmr->format_std == FMR_STD_ISO)) {

		fprintf(fp, "----------------------------------------------------\n");
		fprintf(fp, "Finger View Minutia Record:\n");
		fprintf(fp, "\tFinger Number\t\t: %u\n", fvmr->finger_number);
		fprintf(fp, "\tView Number\t\t: %u\n", fvmr->view_number);
		fprintf(fp, "\tImpression Type\t\t: %u\n",
		    fvmr->impression_type);
		fprintf(fp, "\tFinger Quality\t\t: %u\n", fvmr->finger_quality);
		if (fvmr->format_std == FMR_STD_ANSI07) {
			fprintf(fp, "\tAlgorithm ID\t\t: 0x%08X\n",
				fvmr->algorithm_id);
			fprintf(fp, "\tImage Size\t\t: %ux%u\n",
				fvmr->x_image_size, fvmr->y_image_size);
			fprintf(fp, "\tImage Resolution\t: %ux%u\n",
				fvmr->x_resolution, fvmr->y_resolution);
		}
		fprintf(fp, "\tNumber of Minutiae\t: %u\n",
		    fvmr->number_of_minutiae);
		fprintf(fp, "\n");
	}

	TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
		fprintf(fp, "(%03d) ", fmd->index);
		if (print_fmd(fp, fmd) != PRINT_OK)
			ERR_OUT("Could not print minutiae data");
	}
	if (fvmr->extended != NULL)
		if (print_fedb(fp, fvmr->extended) != PRINT_OK)
			ERR_OUT("Could not write extended data block");

	fprintf(fp, "----------------------------------------------------\n");
	return PRINT_OK;

err_out:
	return PRINT_ERROR;
}

void
add_fmd_to_fvmr(struct finger_minutiae_data *fmd,
                struct finger_view_minutiae_record *fvmr)
{
	fmd->fvmr = fvmr;
	TAILQ_INSERT_TAIL(&fvmr->minutiae_data, fmd, list);
}

void
add_fedb_to_fvmr(struct finger_extended_data_block *fedb,
		 struct finger_view_minutiae_record *fvmr)
{
		fvmr->extended = fedb;
		fedb->fvmr = fvmr;
}

/******************************************************************************/
/* Implementation of the higher level access routines.                        */
/******************************************************************************/
int
get_fmd_count(struct finger_view_minutiae_record *fvmr)
{
	return (int)fvmr->number_of_minutiae;
}

int
get_fmds(struct finger_view_minutiae_record *fvmr,
    struct finger_minutiae_data *fmds[])
{
	int count = 0;
	struct finger_minutiae_data *fmd;

	TAILQ_FOREACH(fmd, &fvmr->minutiae_data, list) {
		fmds[count] = fmd;
		count++;
	}

	return count;
}

int
get_rcd_count(struct finger_view_minutiae_record *fvmr)
{
	struct finger_extended_data *fed;
	struct ridge_count_data *rcd;
	int count = 0;

	if (fvmr->extended == NULL)
		return 0;

	// Because there is no count in the FMR for the number of Ridge Count
	// records, we calculate the total here
	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_RIDGE_COUNT)
			TAILQ_FOREACH(rcd, &fed->rcdb->ridge_counts, list)
				count++; 
	}

	return count;
}

int
get_rcds(struct finger_view_minutiae_record *fvmr,
    struct ridge_count_data *rcds[])
{
	struct finger_extended_data *fed;
	struct ridge_count_data *rcd;
	int count = 0;

	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_RIDGE_COUNT)
			TAILQ_FOREACH(rcd, &fed->rcdb->ridge_counts, list) {
				rcds[count] = rcd;
				count++; 
		}
	}
	return count;
}

int
get_core_count(struct finger_view_minutiae_record *fvmr)
{
	struct finger_extended_data *fed;
	int count = 0;

	if (fvmr->extended == NULL)
		return 0;

	// Add up the core count from each Core-Delta data block because
	// there my be more than one CDDB in the extended data
	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_CORE_AND_DELTA)
			count += fed->cddb->num_cores;
	}
	return count;
}

int
get_cores(struct finger_view_minutiae_record *fvmr,
    struct core_data *cores[])
{
	struct finger_extended_data *fed;
	struct core_data *core;
	int count = 0;

	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_CORE_AND_DELTA)
			TAILQ_FOREACH(core, &fed->cddb->cores, list) {
				cores[count] = core;
				count++;
			}
	}
	return count;
}

int
get_delta_count(struct finger_view_minutiae_record *fvmr)
{
	struct finger_extended_data *fed;
	int count = 0;

	if (fvmr->extended == NULL)
		return 0;

	// Add up the delta count from each Core-Delta data block because
	// there my be more than one CDDB in the extended data
	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_CORE_AND_DELTA)
			count += fed->cddb->num_deltas;
	}

	return count;
}

int
get_deltas(struct finger_view_minutiae_record *fvmr,
    struct delta_data *deltas[])
{
	struct finger_extended_data *fed;
	struct delta_data *delta;
	int count = 0;

	TAILQ_FOREACH(fed, &fvmr->extended->extended_data, list) {
		if (fed->type_id == FED_CORE_AND_DELTA)
			TAILQ_FOREACH(delta, &fed->cddb->deltas, list) {
				deltas[count] = delta;
				count++;
			}
	}
	return count;
}
