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
/* This program uses the ANSI/INCITS Face Recognition Format library to       */
/* create a file containing a Face Recognition Format record based on data    */
/* contained in several text files.                                           */
/*                                                                            */
/*                                                                            */
/* Parameters to this program:                                                */
/*   -h <header>      : The name of the header file                           */
/*   -f <fdb>         : The name of the Facial Data input file including      */
/*                      feature points; multiple instances are allowed        */
/*   -p               : Print the entire FRF to stdout                        */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <frf.h>

#define MAXFDBS		4
int total_length = 0;

/******************************************************************************/
/* Load the FRF header info from a text file and set the fields in the face   */
/* recognition format (FRF) record.                                           */
/* Header format:                                                             */
/* Format-ID(char) Version(char) Length(dec) Num-Faces(dec)                   */
/* The record length will be calculated and value in the file is ignored      */
/******************************************************************************/
int
load_hdr(FILE *fp, struct facial_block *fb)
{

	if (fscanf(fp, "%3c %3c %u %hu",
		fb->format_id, fb->version_num, &fb->record_length,
		&fb->num_faces) < 0)
			READ_ERR_RETURN("header file");
	
	// NULL-terminate the format ID
	fb->format_id[3] = 0x00;

	// NULL-terminate the version number
	fb->version_num[3] = 0x00;

	total_length += FRF_FHB_LENGTH;
	return (READ_OK);
}

/******************************************************************************/
/* Load a single feature point block from a text file and add to the facial   */
/* data block. The feature point block is of the form:                        */
/* feature_type(dec) feature_point-major(dec).feature_point-minor(dec)        */
/* x-coord(dec) y-coord(dec)                                                  */
/******************************************************************************/
int
load_fpb(FILE *fp, struct facial_data_block *fdb)
{
	struct feature_point_block *fpb;

	if (new_fpb(&fpb) < 0)
		ALLOC_ERR_RETURN("Feature Point Block");

	// Read 'dd dd.dd dd dd'
	if (fscanf(fp, "%hhu %hhu%*c%hhu %hu %hu",
		&fpb->type,
		&fpb->major_point,
		&fpb->minor_point,
		&fpb->x_coord,
		&fpb->y_coord) != 5)
			return (READ_ERROR);

	add_fpb_to_fdb(fpb, fdb);

	return (READ_OK);
}

/******************************************************************************/
/* Load a single facial data block from a text file and add to the facial     */
/* block. The facial data block is of the form:                               */
/* num_feature_points(dec) gender(dec) eye-color(hex)                         */
/* hair-color(hex) feature-mask(hex) expression(hex) pose-angle-yaw(int)      */
/* pose-angle-pitch(int) pose-angle-roll(int) uncertainty-yaw(int)            */
/* uncertainty-pitch(int) uncertainty-roll(int)                               */
/*                                                                            */
/******************************************************************************/
int
load_fdb(FILE *fp, struct facial_block *fb)
{
	struct facial_data_block *fdb;
	int i;
	int ret;
	char buf[MAXPATHLEN];

	if (new_fdb(&fdb) < 0)
		ALLOC_ERR_RETURN("Facial Data Block");

	// Read the Facial Information Block
	ret = fscanf(fp, "%hu %hhu %hhu %hhu %x %hx %hhu %hhu %hhu %hhu %hhu %hhu",
		&fdb->num_feature_points,
		&fdb->gender,
		&fdb->eye_color,
		&fdb->hair_color, 
		&fdb->feature_mask,
		&fdb->expression,
		&fdb->pose_angle_yaw,
		&fdb->pose_angle_pitch,
		&fdb->pose_angle_roll,
		&fdb->pose_angle_uncertainty_yaw,
		&fdb->pose_angle_uncertainty_pitch,
		&fdb->pose_angle_uncertainty_roll);
	if (ret == EOF)
		return (READ_EOF);
	else if (ret != 12)
		return (READ_ERROR);

	// read the feature points
	for (i = 0; i < fdb->num_feature_points; i++)
		if (load_fpb(fp, fdb) != READ_OK) {
			ERRP("Could not read feature point block");
			return (READ_ERROR);
		}

	// Read the Image Information Block
	if (fscanf(fp, "%hhu %hhu %hu %hu %hhu %hhu %hu %hu",
		&fdb->face_image_type,
		&fdb->image_data_type,
		&fdb->width,
		&fdb->height,
		&fdb->image_color_space, 
		&fdb->source_type,
		&fdb->device_type,
		&fdb->quality) != 8) {
			ERRP("Could not read image information block");
			return (READ_ERROR);
		}

	fdb->block_length = FRF_FIB_LENGTH + FRF_IIB_LENGTH + 
	    fdb->num_feature_points * FRF_FPB_LENGTH;

	if (fscanf(fp, "%s", buf) < 0) {
		ERRP("Could not read image name");
		return (READ_ERROR);
	}

	add_image_to_fdb(buf, fdb);

	add_fdb_to_fb(fdb, fb);

	total_length += fdb->block_length;

	return (READ_OK);
}

void
usage(void)
{
	printf("usage:\n");
	printf("\tmkfrf -h <headerfile> -f <fdbfile> -o <outfile> [-p]\n");
	printf("\twhere -h, -f, and -o are required\n");
}

int
main(int argc, char *argv[])
{
	FILE *out_fp;	// for the output file
	FILE *hdr_fp;	// the header input file
	FILE *fdb_fp;	// the facial data block input file
	int h_opt, f_opt, o_opt, p_opt;
	struct facial_block *fb;
	struct stat sb;
	char ch;
	int ret, exit_code;

	h_opt = f_opt = o_opt = p_opt = 0;
	exit_code = EXIT_SUCCESS;
	while ((ch = getopt(argc, argv, "h:f:o:p")) != -1) {
		switch (ch) {
			case 'h':
				if ((hdr_fp = fopen(optarg, "r")) == NULL)
					OPEN_ERR_EXIT(optarg);
				h_opt = 1;
				break;

			case 'f':
				if ((fdb_fp = fopen(optarg, "r")) 
				    == NULL)
					OPEN_ERR_EXIT(optarg);
				f_opt = 1;
				break;

			case 'o':
				if (stat(optarg, &sb) == 0)
					ERR_EXIT("File '%s' exists, remove it first.\n", optarg);

				if ((out_fp = fopen(optarg, "wb")) == NULL)
					OPEN_ERR_EXIT(optarg);
				o_opt = 1;
				break;

			case 'p':
				p_opt = 1;
				break;

			default:
				usage();
				exit(EXIT_FAILURE);
				break;
		}
	}
	if (((h_opt && f_opt && o_opt) == 0) || (f_opt > MAXFDBS)) {
		usage();
		exit(EXIT_FAILURE);
	}

	// Read in the file containing the header information
	if (new_fb(&fb) < 0)
		ALLOC_ERR_EXIT("Facial Block");

	if (load_hdr(hdr_fp, fb) != 0) {
		fprintf(stderr, "could not read header\n");
		exit(EXIT_FAILURE);
	}

	// Read in each Facial Data Block and add to the Facial Block
	do {
		ret = load_fdb(fdb_fp, fb);
		if (ret == READ_ERROR) {
			ERRP("Reading facial data block");
			exit_code = EXIT_FAILURE;
			goto err_out;
		}
	} while (ret == READ_OK);

	fb->record_length = total_length;

	// Validate the Facial Block
	if (validate_fb(fb) != VALIDATE_OK) {
		fprintf(stdout, "Facial Block is invalid.\n");
	} else {
		fprintf(stdout, "Facial Block is valid.\n");
	}

	if (p_opt)
		print_fb(stdout, fb);

	// Write out the FMR
	if (write_fb(out_fp, fb) != WRITE_OK) {
		fprintf(stderr, "Error writing the Facial Block\n");
	} else {
		fprintf(stdout, "Facial Block written.\n");
	}

err_out:
	// Free the entire FB
	free_fb(fb);

	if (o_opt) 
		fclose(out_fp);
	if (h_opt) 
		fclose(hdr_fp);

	if (f_opt) 
		fclose(fdb_fp);

	exit(exit_code);
}
