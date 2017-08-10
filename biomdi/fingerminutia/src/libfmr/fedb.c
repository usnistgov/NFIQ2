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
/* Implement the interface for processing of finger Extended Data blocks.     */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdimacro.h>
#include <fmr.h>

// Define a constant for the size of the Extended Data Header, in bytes, which
// is comprised of the Extended Area Type ID and Extended Data Length
#define EXTENDED_DATA_HDR_LEN	4

/******************************************************************************/
/* Declare some internal routines for use later.                              */
/******************************************************************************/
static int
internal_read_fed(FILE *fp, BDB *fmdb, struct finger_extended_data *fed);

static int
internal_write_fed(FILE *fp, BDB *fmdb, struct finger_extended_data *fed);

static int
internal_read_rcdb(FILE *fp, BDB *fmdb, struct ridge_count_data_block *rcdb);

static int
internal_write_rcdb(FILE *fp, BDB *fmdb, struct ridge_count_data_block *rcdb);

static int
internal_read_cddb(FILE *fp, BDB *fmdb, struct core_delta_data_block *cddb);

static int
internal_write_cddb(FILE *fp, BDB *fmdb, struct core_delta_data_block *cddb);

static int
internal_read_cd(FILE *fp, BDB *fmdb, struct core_data *cd,
    unsigned char core_type);

static int
internal_write_cd(FILE *fp, BDB *fmdb, struct core_data *cd);

static int
internal_read_dd(FILE *fp, BDB *fmdb, struct delta_data *dd,
    unsigned char delta_type);

static int
internal_write_dd(FILE *fp, BDB *fmdb, struct delta_data *dd);

/******************************************************************************/
/* Internal routines to convert numeric IDs to strings.                       */
/******************************************************************************/
static char
 *fed_type_id_str(unsigned short type_id) 
{
	switch (type_id) {

	case FED_RESERVED :
		return "reserved";
		break;

	case FED_RIDGE_COUNT :
		return "ridge count";
		break;

	case FED_CORE_AND_DELTA :
		return "core and delta";
		break;

	default:
		return "unknown";
		break;
	}
}

/******************************************************************************/
/* Internal routines add a RCDB or CDDB to a FED.                             */
/******************************************************************************/
static void
add_rcdb_to_fed(struct ridge_count_data_block *rcdb,
		struct finger_extended_data *fed)
{
	// Prevent memory leaks by freeing any existing RCDB
	if (fed->rcdb != NULL)
		free_rcdb(fed->rcdb);

	rcdb->fed = fed;
	fed->rcdb = rcdb;
}

static void
add_cddb_to_fed(struct core_delta_data_block *cddb,
		struct finger_extended_data *fed)
{
	// Prevent memory leaks by freeing any existing CDDB
	if (fed->cddb != NULL)
		free_cddb(fed->cddb);

	cddb->fed = fed;
	fed->cddb = cddb;
}

/******************************************************************************/
/* Process the Finger Extended Data Block record.                             */
/******************************************************************************/
int
new_fedb(unsigned int format_std, struct finger_extended_data_block **fedb)
{
	struct finger_extended_data_block *lfedb;

	lfedb = (struct finger_extended_data_block *)malloc(
			sizeof(struct finger_extended_data_block));
	if (lfedb == NULL) {
		perror("Failed to allocate Finger Extended Data block");
		return (-1);
	}
	memset((void *)lfedb, 0, sizeof(struct finger_extended_data_block));
	lfedb->partial = FALSE;
	lfedb->format_std = format_std;
	TAILQ_INIT(&lfedb->extended_data);
	*fedb = lfedb;
	return (0);
}

void
free_fedb(struct finger_extended_data_block *fedb)
{
	struct finger_extended_data *fed;

	// Loop through the extended data records and free them
	while (!TAILQ_EMPTY(&fedb->extended_data)) {
		fed = TAILQ_FIRST(&fedb->extended_data);
		TAILQ_REMOVE(&fedb->extended_data, fed, list);
		free_fed(fed);
	}

	// Free the Finger Extended Data Block itself
	free(fedb);
}

static int
internal_read_fedb(FILE *fp, BDB *fmdb, struct finger_extended_data_block *fedb)
{
	unsigned short sval1, sval2;
	short block_length;
	struct finger_extended_data *fed = NULL;
	int ret;

	// Block length
	SGET(&sval1, fp, fmdb);
	fedb->block_length = sval1;

	// If the block length is 0, then just return
	if (fedb->block_length == 0) {
		return (READ_OK);
	}
	block_length = fedb->block_length;

	// Read all of the extended data records
	while (block_length > 0) {

		// Type ID code
		SGET(&sval1, fp, fmdb);

		// Length
		SGET(&sval2, fp, fmdb);

		// If the extended data length is 0, or greater than the
		// remaining extended data block length, something is wrong
		// in the file.
		if (sval2 == 0) 
			ERR_OUT("Extended data length is 0");
		if (sval2 > block_length) 
			ERR_OUT("Extended data length %d is larger than remaining block length of %u", sval2, block_length);

		if (new_fed(fedb->format_std, &fed, sval1, sval2) < 0)
			ERR_OUT("Cannot create new extended data block");

		ret = internal_read_fed(fp, fmdb, fed);
		if (ret == READ_EOF) {
			if (fed->partial) {
				add_fed_to_fedb(fed, fedb);
				fedb->partial = TRUE;
			}
			return (READ_EOF);
		}
		if (ret == READ_ERROR)
			ERR_OUT("Could not extended data record");
		add_fed_to_fedb(fed, fedb);
		block_length -= fed->length;
	}

	return (READ_OK);

eof_out:
	ERRP("Premature EOF while reading extended data block");
	return (READ_EOF);
err_out:
	if (fed != NULL)
		free_fed(fed);
	return (READ_ERROR);
}

int
read_fedb(FILE *fp, struct finger_extended_data_block *fedb)
{
	return (internal_read_fedb(fp, NULL, fedb));
}

int
scan_fedb(BDB *fmdb, struct finger_extended_data_block *fedb)
{
	return (internal_read_fedb(NULL, fmdb, fedb));
}

static int
internal_write_fedb(FILE *fp, BDB *fmdb,
    struct finger_extended_data_block *fedb)
{
	unsigned short sval;
	struct finger_extended_data *fed;

	// Block length is always written; it may be 0
	if (fedb == NULL) {
		sval = 0;
	} else {
		sval = fedb->block_length;
	}
	SPUT(sval, fp, fmdb);

	// If no extended data block, then just return
	if (fedb == NULL) {
		return (WRITE_OK);
	}

	TAILQ_FOREACH(fed, &fedb->extended_data, list) {
		internal_write_fed(fp, fmdb, fed);
	}

	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_fedb(FILE *fp, struct finger_extended_data_block *fedb)
{
	return (internal_write_fedb(fp, NULL, fedb));
}

int
push_fedb(BDB *fmdb, struct finger_extended_data_block *fedb)
{
	return (internal_write_fedb(NULL, fmdb, fedb));
}

int
print_fedb(FILE *fp, struct finger_extended_data_block *fedb)
{
	struct finger_extended_data *fed;

	fprintf(fp, "\n");
	fprintf(fp, "Finger Extended Data: Block Length is %u.\n", 
		fedb->block_length);

	// Each extended data record associated with the Finger
	// Extended Data Block
	fprintf(fp, "Finger Extended Data Record(s):\n");
	TAILQ_FOREACH(fed, &fedb->extended_data, list) {
		fprintf(fp, "\tType ID\t: 0x%04x (%s)\n", fed->type_id, 
			fed_type_id_str(fed->type_id));
		fprintf(fp, "\tLength\t: %u\n", fed->length);

		print_fed(fp, fed);
		fprintf(fp, "\n");

	}

	return (PRINT_OK);
}

void
add_fed_to_fedb(struct finger_extended_data *fed,
                struct finger_extended_data_block *fedb)
{
	fed->fedb = fedb;
	TAILQ_INSERT_TAIL(&fedb->extended_data, fed, list);
}

int 
new_fed(unsigned int format_std, struct finger_extended_data **fed,
	unsigned short type_id, unsigned short length)
{
	struct finger_extended_data *lfed;
	struct ridge_count_data_block *rcdb;
	struct core_delta_data_block *cddb;
	int ret;

	lfed = (struct finger_extended_data*)malloc(
			sizeof(struct finger_extended_data));
	if (lfed == NULL) {
		perror("Failed to allocate Finger Extended Data record");
		return (-1);
	}
	memset((void *)lfed, 0, sizeof(struct finger_extended_data));

	lfed->format_std = format_std;
	lfed->type_id = type_id;
	lfed->length = length;
	lfed->partial = FALSE;
	switch (type_id) {

	case FED_RIDGE_COUNT :
		ret = new_rcdb(&rcdb);
		if (ret != 0)
			ERR_OUT("Could not create new ridge count block");
		add_rcdb_to_fed(rcdb, lfed);
		break;

	case FED_CORE_AND_DELTA :
		ret = new_cddb(format_std, &cddb);
		if (ret != 0)
			ERR_OUT("Could not create new core/delta block");
		add_cddb_to_fed(cddb, lfed);
		break;

	default :
		lfed->data = (char *)malloc(length - EXTENDED_DATA_HDR_LEN);
		if (lfed->data == NULL)
			ERR_OUT("Could not allocate extended data block");
		memset((void *)lfed->data, 0, sizeof(char));
		break;
	}

	*fed = lfed;
	return (0);
err_out:
	free(lfed);
	return (-1);
}

void 
free_fed(struct finger_extended_data *fed)
{
	switch (fed->type_id) {

	case FED_RIDGE_COUNT :
		free_rcdb(fed->rcdb);
		break;

	case FED_CORE_AND_DELTA :
		free_cddb(fed->cddb);
		break;

	default :
		free((void *)fed->data);
		break;
	}
	free((void *)fed);
}

static int
internal_read_fed(FILE *fp, BDB *fmdb, struct finger_extended_data *fed)
{
	int ret = READ_OK;

	switch (fed->type_id) {

	case FED_RIDGE_COUNT :
		ret = internal_read_rcdb(fp, fmdb, fed->rcdb);
		if (!TAILQ_EMPTY(&fed->rcdb->ridge_counts))
			fed->partial = TRUE;
		break;

	case FED_CORE_AND_DELTA :
		ret = internal_read_cddb(fp, fmdb, fed->cddb);
		if ((!TAILQ_EMPTY(&fed->cddb->cores)) || 
		    (!TAILQ_EMPTY(&fed->cddb->deltas)))
			fed->partial = TRUE;
		break;

	default :
		// Unknown data type
		/* The data length includes the type ID and length sizes,
		 * so subtract them out
		 */
		OGET(fed->data, fed->length - EXTENDED_DATA_HDR_LEN, 1,
		    fp, fmdb);
		break;
	}
		
	return (ret);

eof_out:
	ERRP("Premature EOF while reading extended data area");
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_fed(FILE *fp, struct finger_extended_data *fed)
{
	return (internal_read_fed(fp, NULL, fed));
}

int
scan_fed(BDB *fmdb, struct finger_extended_data *fed)
{
	return (internal_read_fed(NULL, fmdb, fed));
}

static int
internal_write_fed(FILE *fp, BDB *fmdb, struct finger_extended_data *fed)
{

	SPUT(fed->type_id, fp, fmdb);
	SPUT(fed->length, fp, fmdb);

	switch (fed->type_id) {

	case FED_RIDGE_COUNT :
		return(internal_write_rcdb(fp, fmdb, fed->rcdb));
		break;

	case FED_CORE_AND_DELTA :
		return(internal_write_cddb(fp, fmdb, fed->cddb));
		break;

	default :
		OPUT(fed->data, 1, fed->length - 4, fp, fmdb);
		break;
	}
	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_fed(FILE *fp, struct finger_extended_data *fed)
{
	return (internal_write_fed(fp, NULL, fed));
}

int
push_fed(BDB *fmdb, struct finger_extended_data *fed)
{
	return (internal_write_fed(NULL, fmdb, fed));
}

int
print_fed(FILE *fp, struct finger_extended_data *fed)
{
	int i;

	switch (fed->type_id) {

	case FED_RIDGE_COUNT :
		print_rcdb(fp, fed->rcdb);
		break;

	case FED_CORE_AND_DELTA :
		print_cddb(fp, fed->cddb);
		break;

	default :
		fprintf(fp, "Unknown data type.\n");
		// Data; length of data is length in record - 4
		fprintf(fp, "\tData\t: 0x");
		for (i = 0; i < fed->length - 4; i++) {
			fprintf(fp, "%02x", fed->data[i]);
		}
		break;
	}
	return (PRINT_OK);
}

/******************************************************************************/
/* Process the Ridge Count Data blocks and records.                           */
/******************************************************************************/
int
new_rcdb(struct ridge_count_data_block **rcdb)
{
	struct ridge_count_data_block *lrcdb;

	lrcdb = (struct ridge_count_data_block *)malloc(
			sizeof(struct ridge_count_data_block));
	if (lrcdb == NULL) {
		perror("Failed to allocate Ridge Count Data Block");
		return (-1);
	}
	memset((void *)lrcdb, 0, sizeof(struct ridge_count_data_block));
	TAILQ_INIT(&lrcdb->ridge_counts);

	*rcdb = lrcdb;
	return (0);
}

void
free_rcdb(struct ridge_count_data_block *rcdb)
{
	struct ridge_count_data *rcd;

	// Free the Ridge Count Data records associated with the RCDB
	while (!TAILQ_EMPTY(&rcdb->ridge_counts)) {
		rcd = TAILQ_FIRST(&rcdb->ridge_counts);
		TAILQ_REMOVE(&rcdb->ridge_counts, rcd, list);
		free_rcd(rcd);
	}

	// Free the RCDB itself
	free(rcdb);
}

void
add_rcd_to_rcdb(struct ridge_count_data *rcd,
	struct ridge_count_data_block *rcdb)
{
	rcd->rcdb = rcdb;
	TAILQ_INSERT_TAIL(&rcdb->ridge_counts, rcd, list);
}

int
new_rcd(struct ridge_count_data **rcd)
{
	struct ridge_count_data *lrcd;

	lrcd = (struct ridge_count_data *)malloc(
			sizeof(struct ridge_count_data));
	if (lrcd == NULL) {
		perror("Failed to allocate Ridge Count Data");
		return (-1);
	}
	memset((void *)lrcd, 0, sizeof(struct ridge_count_data));

	*rcd = lrcd;
	return (0);
}

void
free_rcd(struct ridge_count_data *rcd)
{
	free(rcd);
}

static int
internal_read_rcdb(FILE *fp, BDB *fmdb, struct ridge_count_data_block *rcdb)
{
	unsigned char cval;
	short block_length;
	int ret;
	struct ridge_count_data *rcd;

	// Method
	CGET(&cval, fp, fmdb);
	rcdb->method = cval;

	// The block length of the Ridge Count Data Block is derived
	// from the length of the Extended Data length
	block_length = rcdb->fed->length - FED_HEADER_LENGTH - 1;
	while (block_length > 0) {
		ret = new_rcd(&rcd);
		if (ret != 0)
			ERR_OUT("Could not allocate new ridge count data");

		if (fp != NULL)
			ret = read_rcd(fp, rcd);
		else
			ret = scan_rcd(fmdb, rcd);
		if (ret == READ_EOF)
			return (READ_EOF);
		if (ret == READ_ERROR)
			ERR_OUT("Could not read ridge count data");

		add_rcd_to_rcdb(rcd, rcdb);
		block_length -= RIDGE_COUNT_DATA_LENGTH;
	}
	return (READ_OK);

eof_out:
	ERRP("EOF while reading Ridge Count data block");
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_rcdb(FILE *fp, struct ridge_count_data_block *rcdb)
{
	return (internal_read_rcdb(fp, NULL, rcdb));
}

int
scan_rcdb(BDB *fmdb, struct ridge_count_data_block *rcdb)
{
	return (internal_read_rcdb(NULL, fmdb, rcdb));
}

int
read_rcd(FILE *fp, struct ridge_count_data *rcd)
{
	CREAD(&rcd->index_one, fp);
	CREAD(&rcd->index_two, fp);
	CREAD(&rcd->count, fp);
	
	return (READ_OK);

eof_out:
	ERRP("EOF while reading Ridge Count data area");
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
scan_rcd(BDB *fmdb, struct ridge_count_data *rcd)
{
	CSCAN(&rcd->index_one, fmdb);
	CSCAN(&rcd->index_two, fmdb);
	CSCAN(&rcd->count, fmdb);
	
	return (READ_OK);
eof_out:
	return (READ_EOF);
}

static int
internal_write_rcdb(FILE *fp, BDB *fmdb, struct ridge_count_data_block *rcdb)
{
	int ret;
	struct ridge_count_data *rcd;

	CPUT(rcdb->method, fp, fmdb);
	TAILQ_FOREACH(rcd, &rcdb->ridge_counts, list) {
		if (fp != NULL)
			ret = write_rcd(fp, rcd);
		else
			ret = push_rcd(fmdb, rcd);
		if (ret != WRITE_OK)
			ERR_OUT("Could not write ridge count data");
	}
	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_rcdb(FILE *fp, struct ridge_count_data_block *rcdb)
{
	return (internal_write_rcdb(fp, NULL, rcdb));
}

int
push_rcdb(BDB *fmdb, struct ridge_count_data_block *rcdb)
{
	return (internal_write_rcdb(NULL, fmdb, rcdb));
}

int
write_rcd(FILE *fp, struct ridge_count_data *rcd)
{
	CWRITE(rcd->index_one, fp);
	CWRITE(rcd->index_two, fp);
	CWRITE(rcd->count, fp);
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
push_rcd(BDB *fmdb, struct ridge_count_data *rcd)
{
	CPUSH(rcd->index_one, fmdb);
	CPUSH(rcd->index_two, fmdb);
	CPUSH(rcd->count, fmdb);
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
print_rcdb(FILE *fp, struct ridge_count_data_block *rcdb)
{
	struct ridge_count_data *rcd;
	int ret;

	// The extraction method is in the first byte
	if (!TAILQ_EMPTY(&rcdb->ridge_counts)) {
		FPRINTF(fp, "\tMethod is ");
		switch (rcdb->method) {
			case RCE_NONSPECIFIC :
				FPRINTF(fp, "Nonspecific\n");
				break;

			case RCE_FOUR_NEIGHBOR : 
				FPRINTF(fp, "Four-neighbor\n");
				break;
	
			case RCE_EIGHT_NEIGHBOR :
				FPRINTF(fp, "Eight-neighbor\n");
				break;

			default:
				FPRINTF(fp, "Unknown");
				break;
		}
		TAILQ_FOREACH(rcd, &rcdb->ridge_counts, list) {
			ret = print_rcd(fp, rcd);
			if (ret != PRINT_OK)
				ERR_OUT("Could not write ridge count data");
		}
	}
	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

int
print_rcd(FILE *fp, struct ridge_count_data *rcd)
{
	FPRINTF(fp, "\t\tIndex 1 = %u, Index 2 = %u, Count = %u\n",
			rcd->index_one, rcd->index_two, rcd->count);
	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

/******************************************************************************/
/* Process the Core and Delta Data Format block and records.                  */
/******************************************************************************/
int
new_cddb(unsigned int format_std, struct core_delta_data_block **cddb)
{
	struct core_delta_data_block *lcddb;

	lcddb = (struct core_delta_data_block *)malloc(
			sizeof(struct core_delta_data_block));
	if (lcddb == NULL) {
		perror("Failed to allocate Core Data Block");
		return (-1);
	}
	memset((void *)lcddb, 0, sizeof(struct core_delta_data_block));
	lcddb->format_std = format_std;
	TAILQ_INIT(&lcddb->cores);
	TAILQ_INIT(&lcddb->deltas);

	*cddb = lcddb;
	return (0);
}

void
free_cddb(struct core_delta_data_block *cddb)
{
	struct core_data *cd;
	struct delta_data *dd;

	// Free the Core Data records associated with the CDDB
	while (!TAILQ_EMPTY(&cddb->cores)) {
		cd = TAILQ_FIRST(&cddb->cores);
		TAILQ_REMOVE(&cddb->cores, cd, list);
		free_cd(cd);
	}
	// Free the Delta Data records associated with the CDDB
	while (!TAILQ_EMPTY(&cddb->deltas)) {
		dd = TAILQ_FIRST(&cddb->deltas);
		TAILQ_REMOVE(&cddb->deltas, dd, list);
		free_dd(dd);
	}

	// Free the CDDB itself
	free(cddb);
}

void
add_cd_to_cddb(struct core_data *cd, struct core_delta_data_block *cddb)
{
	cd->cddb = cddb;
	TAILQ_INSERT_TAIL(&cddb->cores, cd, list);
}

void
add_dd_to_cddb(struct delta_data *dd, struct core_delta_data_block *cddb)
{
	dd->cddb = cddb;
	TAILQ_INSERT_TAIL(&cddb->deltas, dd, list);
}

int
new_cd(unsigned int format_std, struct core_data **cd)
{
	struct core_data *lcd;

	lcd = (struct core_data *)malloc(sizeof(struct core_data));
	if (lcd == NULL) {
		perror("Failed to allocate Core Data");
		return (-1);
	}
	memset((void *)lcd, 0, sizeof(struct core_data));
	lcd->format_std = format_std;

	*cd = lcd;
	return (0);
}

void
free_cd(struct core_data *cd)
{
	free(cd);
}

int
new_dd(unsigned int format_std, struct delta_data **dd)
{
	struct delta_data *ldd;

	ldd = (struct delta_data *)malloc(sizeof(struct delta_data));
	if (ldd == NULL) {
		perror("Failed to allocate Delta Data");
		return (-1);
	}
	memset((void *)ldd, 0, sizeof(struct delta_data));
	ldd->format_std = format_std;

	*dd = ldd;
	return (0);
}

void
free_dd(struct delta_data *dd)
{
	free(dd);
}

static int
internal_read_cddb(FILE *fp, BDB *fmdb, struct core_delta_data_block *cddb)
{
	struct core_data *cd;
	struct delta_data *dd;
	unsigned char cval;
	int ret, i;

	// ANSI Core Type, Number of Cores
	CGET(&cval, fp, fmdb);
	if (cddb->format_std == FMR_STD_ANSI) {
		cddb->core_type = (cval & ANSI_CORE_TYPE_MASK) >>
		    ANSI_CORE_TYPE_SHIFT;
		cddb->num_cores = cval & ANSI_CORE_NUM_CORES_MASK;
	} else {
		// Core type is stored with each core data element
		cddb->num_cores = cval & ISO_CORE_NUM_CORES_MASK;
	}

	// Read each Core Data record
	for (i = 0; i < cddb->num_cores; i++) {
		ret = new_cd(cddb->format_std, &cd);
		if (ret != 0)
			ERR_OUT("Could not allocate core data record");

		ret = internal_read_cd(fp, fmdb, cd, cddb->core_type);
		if (ret == READ_EOF)
			return (READ_EOF);
		if (ret == READ_ERROR)
			ERR_OUT("Could not read core data record");
		add_cd_to_cddb(cd, cddb);
	}

	// Delta Type, Number of Deltas
	CGET(&cval, fp, fmdb);
	if (cddb->format_std == FMR_STD_ANSI) {
		cddb->delta_type = (cval & ANSI_DELTA_TYPE_MASK) >>
		    ANSI_DELTA_TYPE_SHIFT;
	}
	cddb->num_deltas = cval & DELTA_NUM_DELTAS_MASK;

	// Read each Delta Data record
	for (i = 0; i < cddb->num_deltas; i++) {
		ret = new_dd(cddb->format_std, &dd);
		if (ret != 0)
			ERR_OUT("Could not allocate delta data record");

		ret = internal_read_dd(fp, fmdb, dd, cddb->delta_type);
		if (ret == READ_EOF)
			return (READ_EOF);
		if (ret == READ_ERROR)
			ERR_OUT("Could not read delta data record");
		add_dd_to_cddb(dd, cddb);
	}

	return (READ_OK);

eof_out:
	ERRP("Premature EOF while when reading Core/Delta data block");
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_cddb(FILE *fp, struct core_delta_data_block *cddb)
{
	return (internal_read_cddb(fp, NULL, cddb));
}

int
scan_cddb(BDB *fmdb, struct core_delta_data_block *cddb)
{
	return (internal_read_cddb(NULL, fmdb, cddb));
}

static int
internal_read_cd(FILE *fp, BDB *fmdb, struct core_data *cd,
    unsigned char core_type)
{
	unsigned short sval;

	// X Coordinate
	SGET(&sval, fp, fmdb);
	cd->x_coord = sval & CORE_X_COORD_MASK;

	// ISO type
	if ((cd->format_std == FMR_STD_ISO) ||
	    (cd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (cd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		cd->type = (unsigned char) ((sval & ISO_CORE_TYPE_MASK) >>
		    ISO_CORE_TYPE_SHIFT);
	}

	// Y Coordinate
	SGET(&sval, fp, fmdb);
	cd->y_coord = sval & CORE_Y_COORD_MASK;

	// The presence of angular information is defined by the type of 
	// Core data
	if (core_type != CORE_TYPE_ANGULAR)
		return (READ_OK);

	// Optional Core Angle
	CGET(&cd->angle, fp, fmdb);

	return (READ_OK);

eof_out:
	ERRP("Premature EOF while reading Core data area");
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_cd(FILE *fp, struct core_data *cd, unsigned char core_type)
{
	return (internal_read_cd(fp, NULL, cd, core_type));
}

int
scan_cd(BDB *fmdb, struct core_data *cd, unsigned char core_type)
{
	return (internal_read_cd(NULL, fmdb, cd, core_type));
}

static int
internal_read_dd(FILE *fp, BDB *fmdb, struct delta_data *dd,
    unsigned char delta_type)
{
	unsigned short sval;

	// X Coordinate
	SGET(&sval, fp, fmdb);
	dd->x_coord = sval & DELTA_X_COORD_MASK;

	// ISO type
	if ((dd->format_std == FMR_STD_ISO) ||
	    (dd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (dd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		dd->type = (unsigned char) ((sval & ISO_DELTA_TYPE_MASK) >>
		    ISO_DELTA_TYPE_SHIFT);
	}

	// Y Coordinate
	SGET(&sval, fp, fmdb);
	dd->y_coord = sval & DELTA_Y_COORD_MASK;

	// The presence of angular information is defined by the type of 
	// Delta data
	if (delta_type != DELTA_TYPE_ANGULAR)
		return (READ_OK);

	// Angle One
	CGET(&dd->angle1, fp, fmdb);

	// Angle Two
	CGET(&dd->angle2, fp, fmdb);

	// Angle Three
	CGET(&dd->angle3, fp, fmdb);

	return (READ_OK);

eof_out:
	ERRP("Premature EOF while reading Delta data area");
	return (READ_EOF);

err_out:
	return (READ_ERROR);
}

int
read_dd(FILE *fp, struct delta_data *dd, unsigned char delta_type)
{
	return (internal_read_dd(fp, NULL, dd, delta_type));
}

int
scan_dd(BDB *fmdb, struct delta_data *dd, unsigned char delta_type)
{
	return (internal_read_dd(NULL, fmdb, dd, delta_type));
}

static int
internal_write_cddb(FILE *fp, BDB *fmdb, struct core_delta_data_block *cddb)
{
	struct core_data *cd;
	struct delta_data *dd;
	unsigned char cval = 0;

	if ((cddb->format_std == FMR_STD_ISO) ||
	    (cddb->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (cddb->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		cval = cddb->num_cores;
	} else {
		cval = (cddb->core_type << ANSI_CORE_TYPE_SHIFT) |
		    cddb->num_cores;
	}
	CPUT(cval, fp, fmdb);

	TAILQ_FOREACH(cd, &cddb->cores, list) {
		if (internal_write_cd(fp, fmdb, cd) != WRITE_OK)
			ERR_OUT("Could not write core data record");
	}

	if ((cddb->format_std == FMR_STD_ISO) ||
	    (cddb->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (cddb->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		cval = cddb->num_deltas;
	} else {
		cval = (cddb->delta_type << ANSI_DELTA_TYPE_SHIFT) |
		    cddb->num_deltas;
	}
	CPUT(cval, fp, fmdb);

	TAILQ_FOREACH(dd, &cddb->deltas, list) {
		if (internal_write_dd(fp, fmdb, dd) != WRITE_OK)
			ERR_OUT("Could not write delta data record");
	}
	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_cddb(FILE *fp, struct core_delta_data_block *cddb)
{
	return (internal_write_cddb(fp, NULL, cddb));
}

int
push_cddb(BDB *fmdb, struct core_delta_data_block *cddb)
{
	return (internal_write_cddb(NULL, fmdb, cddb));
}

static int
internal_write_cd(FILE *fp, BDB *fmdb, struct core_data *cd)
{
	unsigned short sval;

	// X coord and ISO type
	if ((cd->format_std == FMR_STD_ISO) ||
	    (cd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (cd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		sval = (unsigned short)(cd->type << ISO_CORE_TYPE_SHIFT);
		sval = sval | cd->x_coord;
	} else {
		sval = cd->x_coord;
	}
	SPUT(sval, fp, fmdb);
	SPUT(cd->y_coord, fp, fmdb);
	if (cd->cddb->core_type != CORE_TYPE_ANGULAR)
		return (WRITE_OK);
	CPUT(cd->angle, fp, fmdb);

	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_cd(FILE *fp, struct core_data *cd)
{
	return (internal_write_cd(fp, NULL, cd));
}

int
push_cd(BDB *fmdb, struct core_data *cd)
{
	return (internal_write_cd(NULL, fmdb, cd));
}

static int
internal_write_dd(FILE *fp, BDB *fmdb, struct delta_data *dd)
{
	unsigned short sval;

	// X coord and ISO type
	if ((dd->format_std == FMR_STD_ISO) ||
	    (dd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (dd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		sval = (unsigned short)(dd->type << ISO_DELTA_TYPE_SHIFT);
		sval = sval | dd->x_coord;
	} else {
		sval = dd->x_coord;
	}
	SPUT(sval, fp, fmdb);
	SPUT(dd->y_coord, fp, fmdb);
	if (dd->cddb->delta_type != DELTA_TYPE_ANGULAR)
		return (WRITE_OK);
	CPUT(dd->angle1, fp, fmdb);
	CPUT(dd->angle2, fp, fmdb);
	CPUT(dd->angle3, fp, fmdb);

	return (WRITE_OK);

err_out:
	return (WRITE_ERROR);
}

int
write_dd(FILE *fp, struct delta_data *dd)
{
	return (internal_write_dd(fp, NULL, dd));
}

int
push_dd(BDB *fmdb, struct delta_data *dd)
{
	return (internal_write_dd(NULL, fmdb, dd));
}

#if defined WINDOWS || defined WIN32
	static int
#else
	static inline int
#endif
print_core_type(FILE *fp, unsigned char type)
{
	FPRINTF(fp, "Type is ");
	if (type == CORE_TYPE_ANGULAR) 
		FPRINTF(fp, "angular, ");
	else if (type == CORE_TYPE_NONANGULAR)
		FPRINTF(fp, "non-angular, ");
	else
		FPRINTF(fp, "unknown (%u), ", type);
	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

#if defined WINDOWS || defined WIN32
	static int
#else
	static inline int
#endif
print_delta_type(FILE *fp, unsigned char type)
{
	FPRINTF(fp, "Type is ");
	if (type == DELTA_TYPE_ANGULAR) 
		FPRINTF(fp, "angular, ");
	else if (type == DELTA_TYPE_NONANGULAR)
		FPRINTF(fp, "non-angular, ");
	else
		FPRINTF(fp, "unknown (%u), ", type);
	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

int
print_cddb(FILE *fp, struct core_delta_data_block *cddb)
{
	struct core_data *cd;
	struct delta_data *dd;

	if (!TAILQ_EMPTY(&cddb->cores)) {
	
		FPRINTF(fp, "\tCore information: ");
		if (cddb->format_std == FMR_STD_ANSI)
			if (print_core_type(fp, cddb->core_type) != PRINT_OK)
				goto err_out;
		FPRINTF(fp, "number of cores is %u\n", cddb->num_cores);

		TAILQ_FOREACH(cd, &cddb->cores, list) {
			if (print_cd(fp, cd) != PRINT_OK)
				ERR_OUT("Could not print core data record");
		}
	}

	if (!TAILQ_EMPTY(&cddb->deltas)) {
		FPRINTF(fp, "\tDelta information: ");
		if (cddb->format_std == FMR_STD_ANSI)
			if (print_delta_type(fp, cddb->delta_type) != PRINT_OK)
				goto err_out;
		FPRINTF(fp, "number of deltas is %u\n", cddb->num_deltas);

		TAILQ_FOREACH(dd, &cddb->deltas, list) {
			if (print_dd(fp, dd) != PRINT_OK)
				ERR_OUT("Could not print delta data record");
		}
	}

	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

int
print_cd(FILE *fp, struct core_data *cd)
{
	unsigned char type;

	if ((cd->format_std == FMR_STD_ISO) ||
	    (cd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (cd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		if (print_core_type(fp, cd->type) != PRINT_OK)
			goto err_out;
		type = cd->type;
	} else {
		type = cd->cddb->core_type;
	}
		
	FPRINTF(fp, "\t\tCoordinate = (%u,%u), ", cd->x_coord, cd->y_coord);

	if (type == CORE_TYPE_ANGULAR)
		FPRINTF(fp, "angle is %u\n", cd->angle);
	else
		FPRINTF(fp, "no angle\n");

	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

int
print_dd(FILE *fp, struct delta_data *dd)
{
	unsigned char type;

	if ((dd->format_std == FMR_STD_ISO) ||
	    (dd->format_std == FMR_STD_ISO_NORMAL_CARD) ||
	    (dd->format_std == FMR_STD_ISO_COMPACT_CARD)) {
		if (print_core_type(fp, dd->type) != PRINT_OK)
			goto err_out;
		type = dd->type;
	} else {
		type = dd->cddb->delta_type;
	}
		
	FPRINTF(fp, "\t\tCoordinate = (%u,%u), ", dd->x_coord, dd->y_coord);

	if (type == DELTA_TYPE_ANGULAR)
		FPRINTF(fp, "angles are %u,%u,%u\n",
			dd->angle1, dd->angle2, dd->angle3);
	else
		FPRINTF(fp, "no angles\n");

	return (PRINT_OK);

err_out:
	return (PRINT_ERROR);
}

