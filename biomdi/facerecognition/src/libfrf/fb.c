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
/* Representation of a Facial Block to contain the facial image information   */
/* contained within a ANSI INCITS 385-2004 data record. The Facial Block      */
/* represents the Facial Header and 0 or more Facial Data blocks.             */
/*                                                                            */
/* This package implements the routines to read and write a Facial Image      */
/* Record, and print that information in human-readable form.                 */
/*                                                                            */
/* Routines are provided to manage memory for Facial Block representations.   */
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
new_fb(FB **fb)
{
	FB *lfb;

	lfb = (FB *)malloc(sizeof(FB));
	if (lfb == NULL) {
		perror("Failed to allocate Facial Block");
		return -1;
	}
	memset((void *)lfb, 0, sizeof(FB));
	TAILQ_INIT(&lfb->facial_data);

	*fb = lfb;
	return 0;
}

void
free_fb(FB *fb)
{
	FDB *fdb;

	// Free the Facial Data Blocks contained within the Facial Block
	while (!TAILQ_EMPTY(&fb->facial_data)) {
		fdb = TAILQ_FIRST(&fb->facial_data);
		TAILQ_REMOVE(&fb->facial_data, fdb, list);
		free_fdb(fdb);
	}
	free(fb);
}

static int
internal_read_fb(FILE *fp, BDB *fbdb, FB *fb)
{
	unsigned int i;
	int ret;
	FDB *fdb;

	// Format ID
	OGET(fb->format_id, 1, FRF_FORMAT_ID_LENGTH, fp, fbdb);

	// Version Number
	OGET(fb->version_num, 1, FRF_VERSION_NUM_LENGTH, fp, fbdb);

	// Length of Record
	LGET(&fb->record_length, fp, fbdb);

	// Number of Faces
	SGET(&fb->num_faces, fp, fbdb);

	// Read the Facial Data Blocks
	for (i = 1; i <= fb->num_faces; i++) {
		if (new_fdb(&fdb) < 0) {
			fprintf(stderr, "error allocating FDB %d\n", i);
			goto err_out;
		}
		if (fp != NULL)
			ret = read_fdb(fp, fdb);
		else
			ret = scan_fdb(fbdb, fdb);
		if (ret == READ_OK)
			add_fdb_to_fb(fdb, fb);
		else if (ret == READ_EOF)
			goto eof_out;
		else
			goto err_out;
	}

	return READ_OK;

eof_out:
	return READ_EOF;
err_out:
	return READ_ERROR;
}

int
read_fb(FILE *fp, FB *fb)
{
	return (internal_read_fb(fp, NULL, fb));
}

int
scan_fb(BDB *fbdb, FB *fb)
{
	return (internal_read_fb(NULL, fbdb, fb));
}

static int
internal_write_fb(FILE *fp, BDB *fbdb, FB *fb)
{
	int ret;
	FDB *fdb;

	OPUT(fb->format_id, 1, FRF_FORMAT_ID_LENGTH, fp, fbdb);

	OPUT(fb->version_num, 1, FRF_VERSION_NUM_LENGTH, fp, fbdb);

	LPUT(fb->record_length, fp, fbdb);

	SPUT(fb->num_faces, fp, fbdb);

	TAILQ_FOREACH(fdb, &fb->facial_data, list) {
		if (fp != NULL)
			ret = write_fdb(fp, fdb);
		else
			ret = push_fdb(fbdb, fdb);
		if (ret != WRITE_OK)
			goto err_out;
	}

	return WRITE_OK;

err_out:
	return WRITE_ERROR;
}

int
write_fb(FILE *fp, FB *fb)
{
	return (internal_write_fb(fp, NULL, fb));
}

int
push_fb(BDB *fbdb, FB *fb)
{
	return (internal_write_fb(NULL, fbdb, fb));
}

int
print_fb(FILE *fp, FB *fb)
{
	int ret;
	FDB *fdb;

	// Facial Header
	FPRINTF(fp, "Format ID\t\t: %s\nSpec Version\t\t: %s\n",
		fb->format_id, fb->version_num);

	FPRINTF(fp, "Record Length\t\t: %u\n", fb->record_length);

	FPRINTF(fp, "Number of Faces\t\t: %u\n", fb->num_faces);

	// Print the Facial Data Blocks
	TAILQ_FOREACH(fdb, &fb->facial_data, list) {
		ret = print_fdb(fp, fdb);
		if (ret != PRINT_OK)
			goto err_out;
	}

	return PRINT_OK;

err_out:
	return PRINT_ERROR;
}

void
add_fdb_to_fb(FDB *fdb, FB *fb)
{
	fdb->fb = fb;
	TAILQ_INSERT_TAIL(&fb->facial_data, fdb, list);
}

