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
/* Routines that read, write, and print a Facial Data Block that complies     */
/* with Face Recognition Format for Data Interchange (ANSI/INCITS 385-2004)   */
/* record format.                                                             */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <frf.h>

int
new_fdb(FDB **fdb)
{
	FDB *lfdb;
                
	lfdb = (FDB *)malloc(sizeof(FDB));
	if (lfdb == NULL) {
		perror("Failed to allocate Facial Block");
		return -1;
	}       
	memset((void *)lfdb, 0, sizeof(FDB));
	TAILQ_INIT(&lfdb->feature_points);

	*fdb = lfdb;

	return 0;
}

void
free_fdb(FDB *fdb)
{
	FPB *fpb;

	while (!TAILQ_EMPTY(&fdb->feature_points)) {
		fpb = TAILQ_FIRST(&fdb->feature_points);
		TAILQ_REMOVE(&fdb->feature_points, fpb, list);
		free_fpb(fpb);
	}
	free(fdb);
}

static int
internal_read_fdb(FILE *fp, BDB *fdbdb, FDB *fdb)
{
	unsigned int i;
	int ret;
	FPB *fpb;
	unsigned int lval;
	long long llval;

	// Read the Facial Information Block first
	// Block Length
	LGET(&fdb->block_length, fp, fdbdb);

	// Number of Feature Points
	SGET(&fdb->num_feature_points, fp, fdbdb);

	// Gender
	CGET(&fdb->gender, fp, fdbdb);

	// Eye Color
	CGET(&fdb->eye_color, fp, fdbdb);

	// Hair Color
	CGET(&fdb->hair_color, fp, fdbdb);

	// Feature Mask; need to shift the bits around because the length
	// is less than what will fit in a long integer
	lval = 0;
	OGET(&lval, 1, FEATURE_MASK_LEN, fp, fdbdb);
	lval = ntohl(lval);
	fdb->feature_mask = lval >> 8;

	// Expression
	SGET(&fdb->expression, fp, fdbdb);

	// Pose Angles
	CGET(&fdb->pose_angle_yaw, fp, fdbdb);
	CGET(&fdb->pose_angle_pitch, fp, fdbdb);
	CGET(&fdb->pose_angle_roll, fp, fdbdb);

	// Pose Angle Uncertainties
	CGET(&fdb->pose_angle_uncertainty_yaw, fp, fdbdb);
	CGET(&fdb->pose_angle_uncertainty_pitch, fp, fdbdb);
	CGET(&fdb->pose_angle_uncertainty_roll, fp, fdbdb);

	// Read the Feature Points(s)
	for (i = 1; i <= fdb->num_feature_points; i++) {
		if (new_fpb(&fpb) < 0) {
			fprintf(stderr, "error allocating FPB %u\n", i);
			goto err_out;
		}
		if (fp != NULL)
			ret = read_fpb(fp, fpb);
		else
			ret = scan_fpb(fdbdb, fpb);
		if (ret == READ_OK)
			add_fpb_to_fdb(fpb, fdb);
		else if (ret == READ_EOF)
			goto eof_out;
		else
			goto err_out;

	}

	// Read the Image Information
	// Face Image Type
	CGET(&fdb->face_image_type, fp, fdbdb);

	// Image Data Type
	CGET(&fdb->image_data_type, fp, fdbdb);

	// Width
	SGET(&fdb->width, fp, fdbdb);

	// Height
	SGET(&fdb->height, fp, fdbdb);

	// Image Color Space
	CGET(&fdb->image_color_space, fp, fdbdb);

	// Source Type
	CGET(&fdb->source_type, fp, fdbdb);

	// Device Type
	SGET(&fdb->device_type, fp, fdbdb);

	// Quality
	SGET(&fdb->quality, fp, fdbdb);

	// Read the optional Image Data; the length of the image data is 
	// equal to the Facial Image Block Length minus the sum of the
	// Facial Information Block, Feature Points, and Image Information.

	llval = fdb->block_length - FRF_FIB_LENGTH - 
		    (fdb->num_feature_points * FRF_FPB_LENGTH) - FRF_IIB_LENGTH;

	if (llval < 0)
		ERR_OUT("Block length too short to account for image");

	lval = (unsigned int)llval;
	fdb->image_data = malloc(lval);
	if (fdb->image_data == NULL)
		ERR_OUT("Allocating image data\n");

	OGET(fdb->image_data, 1, lval, fp, fdbdb);
	fdb->image_len = lval;

        return READ_OK;

eof_out:
        return READ_EOF;

err_out:
        return READ_ERROR;
}

int
read_fdb(FILE *fp, FDB *fdb)
{
	return (internal_read_fdb(fp, NULL, fdb));
}

int
scan_fdb(BDB *fdbdb, FDB *fdb)
{
	return (internal_read_fdb(NULL, fdbdb, fdb));
}

static int
internal_write_fdb(FILE *fp, BDB *fdbdb, FDB *fdb)
{
	int ret;
	FPB *fpb;
	unsigned int lval;

	LPUT(fdb->block_length, fp, fdbdb);
	SPUT(fdb->num_feature_points, fp, fdbdb);
	CPUT(fdb->gender, fp, fdbdb);
	CPUT(fdb->eye_color, fp, fdbdb);
	CPUT(fdb->hair_color, fp, fdbdb);

	// Convert the feature mask, stored as a value longer than
	// what is needed.
	lval = fdb->feature_mask << 8;
	lval = htonl(lval);
	OPUT(&lval, 1, FEATURE_MASK_LEN, fp, fdbdb);

	SPUT(fdb->expression, fp, fdbdb);

	CPUT(fdb->pose_angle_yaw, fp, fdbdb);
	CPUT(fdb->pose_angle_pitch, fp, fdbdb);
	CPUT(fdb->pose_angle_roll, fp, fdbdb);
	CPUT(fdb->pose_angle_uncertainty_yaw, fp, fdbdb);
	CPUT(fdb->pose_angle_uncertainty_pitch, fp, fdbdb);
	CPUT(fdb->pose_angle_uncertainty_roll, fp, fdbdb);

	// Write the Feature Point Blocks
	TAILQ_FOREACH(fpb, &fdb->feature_points, list) {
		if (fp != NULL)
			ret = write_fpb(fp, fpb);
		else
			ret = push_fpb(fdbdb, fpb);
		if (ret != WRITE_OK)
			goto err_out;
	}

	// Write the Image Information block
	CPUT(fdb->face_image_type, fp, fdbdb);
	CPUT(fdb->image_data_type, fp, fdbdb);
	SPUT(fdb->width, fp, fdbdb);
	SPUT(fdb->height, fp, fdbdb);
	CPUT(fdb->image_color_space, fp, fdbdb);
	CPUT(fdb->source_type, fp, fdbdb);
	SPUT(fdb->device_type, fp, fdbdb);
	SPUT(fdb->quality, fp, fdbdb);

	// Write the image data
	if (fdb->image_data != NULL)
		OPUT(fdb->image_data, 1, fdb->image_len, fp, fdbdb);

        return WRITE_OK;

err_out:
        return WRITE_ERROR;
}

int
write_fdb(FILE *fp, FDB *fdb)
{
	return (internal_write_fdb(fp, NULL, fdb));
}

int
push_fdb(BDB *fdbdb, FDB *fdb)
{
	return (internal_write_fdb(NULL, fdbdb, fdb));
}

int
print_fdb(FILE *fp, FDB *fdb)
{
	FPB *fpb;
	int ret = PRINT_OK;
	int error;

	FPRINTF(fp, "Facial Data Block\n");
	FPRINTF(fp, "\tBlock Length is %u\n", fdb->block_length);
	FPRINTF(fp, "\tNumber of Feature Points = %u\n", 
			fdb->num_feature_points);
	FPRINTF(fp, "\tGender is %u, Eye Color is %u, Hair Color is %u\n",
			fdb->gender, fdb->eye_color, fdb->hair_color);
	FPRINTF(fp, "\tFeature Mask is 0x%06x\n", fdb->feature_mask);
	FPRINTF(fp, "\tExpression is 0x%02x\n", fdb->expression);
	FPRINTF(fp, "\tPose Angles: Yaw = %u, Pitch = %u, Roll = %u\n",
			fdb->pose_angle_yaw, fdb->pose_angle_pitch,
			fdb->pose_angle_roll);
	FPRINTF(fp, "\tPose Angle Uncertainties: "
			"Yaw = %u, Pitch = %u, Roll = %u\n",
			fdb->pose_angle_uncertainty_yaw, 
			fdb->pose_angle_uncertainty_pitch,
			fdb->pose_angle_uncertainty_roll);

	// Print out the Feature Point Blocks
	TAILQ_FOREACH(fpb, &fdb->feature_points, list) {
		error = print_fpb(fp, fpb);
		if (error != PRINT_OK)
			ret = PRINT_ERROR;
	}

	// Print out the Image Information Block
	FPRINTF(fp, "Image Information Block\n");
	FPRINTF(fp, "\tFace Image: Type is %u, Image Data Type is %u\n",
			fdb->face_image_type, fdb->image_data_type);
	FPRINTF(fp, "\tSize is %ux%u\n", fdb->width, fdb->height);

	FPRINTF(fp, "\tColor Space is %u, Source Type is %u, "
			"Device Type is %u\n",
			fdb->image_color_space, fdb->source_type,
			fdb->device_type);
	FPRINTF(fp, "\tQuality is %u\n", fdb->quality);
	FPRINTF(fp, "\tImage length is %u\n", fdb->image_len);

        return ret;

err_out:
        return PRINT_ERROR;
}

void
add_fpb_to_fdb(FPB *fpb, FDB *fdb)
{
        fpb->fdb = fdb;
        TAILQ_INSERT_TAIL(&fdb->feature_points, fpb, list);
}

int
add_image_to_fdb(char *filename, FDB *fdb)
{
	struct stat sb;
	FILE *fp;

	if (stat(filename, &sb) != 0)
		ERR_OUT("File '%s' not accessible.\n", filename);

        if ((fp = fopen(filename, "rb")) == NULL) 
                ERR_OUT("Could not open '%s'", filename);

	fdb->image_data = malloc(sb.st_size);
	if (fdb->image_data == NULL)
		ERR_OUT("Allocating image data\n");

	OREAD(fdb->image_data, 1, sb.st_size, fp);

	fdb->image_len = sb.st_size;
	fdb->block_length += sb.st_size;

        return READ_OK;

eof_out:
        return READ_EOF;

err_out:
        return READ_ERROR;
}
