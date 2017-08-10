/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/

/*
 * A set of routines to read and verify parts of an Iris Image record that
 * are extensions to the ISO/IEC 19794-6 Iris Image standard. The extensions
 * are defined in the NIST IREX test specification, but may eventually become
 * part of 19794-6. If that happens, the code will move to the NIST Iris 
 * Image project.
 */

#include <sys/queue.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <biomdi.h>
#include <biomdimacro.h>

#include <iid_ext.h>

/******************************************************************************/
/* Implement the interface for allocating and freeing extension data blocks.  */
/******************************************************************************/
int
new_roimask(ROIMASK **roimask)
{
	ROIMASK *lroimask;

	lroimask = (ROIMASK *)malloc(sizeof(ROIMASK));
	if (lroimask == NULL)
		return (-1);
	memset((void *)lroimask, 0, sizeof(ROIMASK));
	*roimask = lroimask;
	return (0);
}

void
free_roimask(ROIMASK *roimask)
{
	free(roimask);
}

int
new_unsegpolar(UNSEGPOLAR **unsegpolar)
{
	UNSEGPOLAR *lunsegpolar;

	lunsegpolar = (UNSEGPOLAR *)malloc(sizeof(UNSEGPOLAR));
	if (lunsegpolar == NULL)
		return (-1);
	memset((void *)lunsegpolar, 0, sizeof(UNSEGPOLAR));
	*unsegpolar = lunsegpolar;
	return (0);
}

void
free_unsegpolar(UNSEGPOLAR *unsegpolar)
{
	free(unsegpolar);
}

/******************************************************************************/
/* Implement the interface for reading/writing/validating iris image          */
/* extension data blocks.                                                     */
/******************************************************************************/
static int
internal_read_roimask(FILE *fp, BDB *bdb, ROIMASK *roimask)
{
	CGET(&roimask->upper_eyelid_mask, fp, bdb);
	CGET(&roimask->lower_eyelid_mask, fp, bdb);
	CGET(&roimask->sclera_mask, fp, bdb);
	return (READ_OK);
eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_roimask(FILE *fp, ROIMASK *roimask)
{
	return (internal_read_roimask(fp, NULL, roimask));
}

int
scan_roimask(BDB *bdb, ROIMASK *roimask)
{
	return (internal_read_roimask(NULL, bdb, roimask));
}

static int
internal_read_unsegpolar(FILE *fp, BDB *bdb, UNSEGPOLAR *unsegpolar)
{
	SGET(&unsegpolar->num_samples_radially, fp, bdb);
	SGET(&unsegpolar->num_samples_circumferentially, fp, bdb);
	SGET(&unsegpolar->inner_outer_circle_x, fp, bdb);
	SGET(&unsegpolar->inner_outer_circle_y, fp, bdb);
	SGET(&unsegpolar->inner_circle_radius, fp, bdb);
	SGET(&unsegpolar->outer_circle_radius, fp, bdb);
	return (READ_OK);
eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar)
{
	return (internal_read_unsegpolar(fp, NULL, unsegpolar));
}

int
scan_unsegpolar(BDB *bdb, UNSEGPOLAR *unsegpolar)
{
	return (internal_read_unsegpolar(NULL, bdb, unsegpolar));
}

static int
internal_read_image_ancillary(FILE *fp, BDB *bdb, IMAGEANCILLARY *ancillary)
{
	int len;

	SGET(&ancillary->pupil_center_of_ellipse_x, fp, bdb);
	SGET(&ancillary->pupil_center_of_ellipse_y, fp, bdb);
	SGET(&ancillary->pupil_semimajor_intersection_x, fp, bdb);
	SGET(&ancillary->pupil_semimajor_intersection_y, fp, bdb);
	SGET(&ancillary->pupil_semiminor_intersection_x, fp, bdb);
	SGET(&ancillary->pupil_semiminor_intersection_y, fp, bdb);
	SGET(&ancillary->iris_center_of_ellipse_x, fp, bdb);
	SGET(&ancillary->iris_center_of_ellipse_y, fp, bdb);
	SGET(&ancillary->iris_semimajor_intersection_x, fp, bdb);
	SGET(&ancillary->iris_semimajor_intersection_y, fp, bdb);
	SGET(&ancillary->iris_semiminor_intersection_x, fp, bdb);
	SGET(&ancillary->iris_semiminor_intersection_y, fp, bdb);

	SGET(&ancillary->pupil_iris_boundary_freeman_code_length, fp, bdb);
	if (ancillary->pupil_iris_boundary_freeman_code_length != 0) {
		len = ancillary->pupil_iris_boundary_freeman_code_length -
                         IID_EXT_FCCB_HEADER_LEN;
		SGET(&ancillary->pupil_iris_boundary_freeman_code_data.start_x, fp, bdb);
		SGET(&ancillary->pupil_iris_boundary_freeman_code_data.start_y, fp, bdb);
		SGET(&ancillary->pupil_iris_boundary_freeman_code_data.num_codes, fp, bdb);
		ancillary->pupil_iris_boundary_freeman_code_data.fcc =
		    (uint8_t *)malloc(len);
		if (ancillary->pupil_iris_boundary_freeman_code_data.fcc ==NULL)
			goto err_out;
	        OGET(ancillary->pupil_iris_boundary_freeman_code_data.fcc, 1,
			 len, fp, bdb);
	}
	SGET(&ancillary->sclera_iris_boundary_freeman_code_length, fp, bdb);
	if (ancillary->sclera_iris_boundary_freeman_code_length != 0) {
		len = ancillary->sclera_iris_boundary_freeman_code_length -
                         IID_EXT_FCCB_HEADER_LEN;
		SGET(&ancillary->sclera_iris_boundary_freeman_code_data.start_x, fp, bdb);
		SGET(&ancillary->sclera_iris_boundary_freeman_code_data.start_y, fp, bdb);
		SGET(&ancillary->sclera_iris_boundary_freeman_code_data.num_codes, fp, bdb);
		ancillary->sclera_iris_boundary_freeman_code_data.fcc =
		    (uint8_t *)malloc(len);
		if (ancillary->sclera_iris_boundary_freeman_code_data.fcc == NULL)
			goto err_out;
	        OGET(ancillary->sclera_iris_boundary_freeman_code_data.fcc, 1,
		    len, fp, bdb);
	}

	return (READ_OK);
eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary)
{
	return (internal_read_image_ancillary(fp, NULL, ancillary));
}

int
scan_image_ancillary(BDB *bdb, IMAGEANCILLARY *ancillary)
{
	return (internal_read_image_ancillary(NULL, bdb, ancillary));
}

static int
internal_write_roimask(FILE *fp, BDB *bdb, ROIMASK *roimask)
{
	CPUT(roimask->upper_eyelid_mask, fp, bdb);
	CPUT(roimask->lower_eyelid_mask, fp, bdb);
	CPUT(roimask->sclera_mask, fp, bdb);
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_roimask(FILE *fp, ROIMASK *roimask)
{
	return (internal_write_roimask(fp, NULL, roimask));
}

int
push_roimask(BDB *bdb, ROIMASK *roimask)
{
	return (internal_write_roimask(NULL, bdb, roimask));
}

static int
internal_write_unsegpolar(FILE *fp, BDB *bdb, UNSEGPOLAR *unsegpolar)
{
	SPUT(unsegpolar->num_samples_radially, fp, bdb);
	SPUT(unsegpolar->num_samples_circumferentially, fp, bdb);
	SPUT(unsegpolar->inner_outer_circle_x, fp, bdb);
	SPUT(unsegpolar->inner_outer_circle_y, fp, bdb);
	SPUT(unsegpolar->inner_circle_radius, fp, bdb);
	SPUT(unsegpolar->outer_circle_radius, fp, bdb);
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar)
{
	return (internal_write_unsegpolar(fp, NULL, unsegpolar));
}

int
push_unsegpolar(BDB *bdb, UNSEGPOLAR *unsegpolar)
{
	return (internal_write_unsegpolar(NULL, bdb, unsegpolar));
}

static int
internal_write_image_ancillary(FILE *fp, BDB *bdb, IMAGEANCILLARY *ancillary)
{
	int len;

	SPUT(ancillary->pupil_center_of_ellipse_x, fp, bdb);
	SPUT(ancillary->pupil_center_of_ellipse_y, fp, bdb);
	SPUT(ancillary->pupil_semimajor_intersection_x, fp, bdb);
	SPUT(ancillary->pupil_semimajor_intersection_y, fp, bdb);
	SPUT(ancillary->pupil_semiminor_intersection_x, fp, bdb);
	SPUT(ancillary->pupil_semiminor_intersection_y, fp, bdb);
	SPUT(ancillary->iris_center_of_ellipse_x, fp, bdb);
	SPUT(ancillary->iris_center_of_ellipse_y, fp, bdb);
	SPUT(ancillary->iris_semimajor_intersection_x, fp, bdb);
	SPUT(ancillary->iris_semimajor_intersection_y, fp, bdb);
	SPUT(ancillary->iris_semiminor_intersection_x, fp, bdb);
	SPUT(ancillary->iris_semiminor_intersection_y, fp, bdb);

	SPUT(ancillary->pupil_iris_boundary_freeman_code_length, fp, bdb);
	if (ancillary->pupil_iris_boundary_freeman_code_length != 0) {
		len = ancillary->pupil_iris_boundary_freeman_code_length -
                         IID_EXT_FCCB_HEADER_LEN;
		SPUT(ancillary->pupil_iris_boundary_freeman_code_data.start_x,   fp, bdb);
		SPUT(ancillary->pupil_iris_boundary_freeman_code_data.start_y,   fp, bdb);
		SPUT(ancillary->pupil_iris_boundary_freeman_code_data.num_codes, fp, bdb);
	        OPUT(ancillary->pupil_iris_boundary_freeman_code_data.fcc, 1,
		    len, fp, bdb);
	}
	SPUT(ancillary->sclera_iris_boundary_freeman_code_length, fp, bdb);
	if (ancillary->sclera_iris_boundary_freeman_code_length != 0) {
		len = ancillary->sclera_iris_boundary_freeman_code_length -
                         IID_EXT_FCCB_HEADER_LEN;
		SPUT(ancillary->sclera_iris_boundary_freeman_code_data.start_x,   fp, bdb);
		SPUT(ancillary->sclera_iris_boundary_freeman_code_data.start_y,   fp, bdb);
		SPUT(ancillary->sclera_iris_boundary_freeman_code_data.num_codes, fp, bdb);
	        OPUT(ancillary->sclera_iris_boundary_freeman_code_data.fcc, 1,
		    len, fp, bdb);
	}
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary)
{
	return (internal_write_image_ancillary(fp, NULL, ancillary));
}

int
push_image_ancillary(BDB *bdb, IMAGEANCILLARY *ancillary)
{
	return (internal_write_image_ancillary(NULL, bdb, ancillary));
}

int
print_roimask(FILE *fp, ROIMASK *roimask)
{
	FPRINTF(fp, "\tUpper Eyelid Mask\t\t: 0x%02hhX\n",
	    roimask->upper_eyelid_mask);
	FPRINTF(fp, "\tLower Eyelid Mask\t\t: 0x%02hhX\n",
	    roimask->lower_eyelid_mask);
	FPRINTF(fp, "\tSclera Mask\t\t\t: 0x%02hhX\n", roimask->sclera_mask);
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

int
print_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar)
{
	FPRINTF(fp, "\tNum Samples Radially\t\t: %hu\n",
	    unsegpolar->num_samples_radially);
	FPRINTF(fp, "\tNum Samples Circumferentially\t: %hu\n",
	    unsegpolar->num_samples_circumferentially);
	FPRINTF(fp, "\tInner/Outer Circle Coord\t: (%hu, %hu)\n",
	    unsegpolar->inner_outer_circle_x,
	    unsegpolar->inner_outer_circle_y);
	FPRINTF(fp, "\tInner Circle Radius\t\t: %hu\n",
	    unsegpolar->inner_circle_radius);
	FPRINTF(fp, "\tOuter Circle Radius\t\t: %hu\n",
	    unsegpolar->outer_circle_radius);
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

int
print_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary)
{
	if (ancillary->pupil_center_of_ellipse_x ==
	    IID_EXT_COORD_NOT_COMPUTED) {
		FPRINTF(fp, "\tPupil Center of Ellipse\t\t: Not computed\n");
		FPRINTF(fp, "\tPupil Semimajor Intersection\t: Not computed\n");
		FPRINTF(fp, "\tPupil Semiminor Intersection\t: Not computed\n");
	} else {
		FPRINTF(fp, "\tPupil Center of Ellipse\t\t: (%hd, %hd)\n",
		    ancillary->pupil_center_of_ellipse_x,
		    ancillary->pupil_center_of_ellipse_y);
		FPRINTF(fp, "\tPupil Semimajor Intersection\t: (%hd, %hd)\n",
		    ancillary->pupil_semimajor_intersection_x,
		    ancillary->pupil_semimajor_intersection_y);
		FPRINTF(fp, "\tPupil Semiminor Intersection\t: (%hd, %hd)\n",
		    ancillary->pupil_semiminor_intersection_x,
		    ancillary->pupil_semiminor_intersection_y);
	}
	if (ancillary->pupil_center_of_ellipse_x ==
	    IID_EXT_COORD_NOT_COMPUTED) {
		FPRINTF(fp, "\tIris Center of Ellipse\t\t: Not computed\n");
		FPRINTF(fp, "\tIris Semimajor Intersection\t: Not computed\n");
		FPRINTF(fp, "\tIris Semiminor Intersection\t: Not computed\n");
	} else {
		FPRINTF(fp, "\tIris Center of Ellipse\t\t: (%hd, %hd)\n",
		    ancillary->iris_center_of_ellipse_x,
		    ancillary->iris_center_of_ellipse_y);
		FPRINTF(fp, "\tIris Semimajor Intersection\t: (%hd, %hd)\n",
		    ancillary->iris_semimajor_intersection_x,
		    ancillary->iris_semimajor_intersection_y);
	}
	FPRINTF(fp, "\tIris Semiminor Intersection\t: (%hd, %hd)\n",
	    ancillary->iris_semiminor_intersection_x,
	    ancillary->iris_semiminor_intersection_y);

	FPRINTF(fp, "\tPupil-Iris Boundary FCC Length\t: %hu\n",
	    ancillary->pupil_iris_boundary_freeman_code_length);
	if (ancillary->pupil_iris_boundary_freeman_code_length != 0) {
		FPRINTF(fp, "\tPupil-Iris Boundary Start Point\t: (%hu, %hu)\n",
		    ancillary->pupil_iris_boundary_freeman_code_data.start_x,
		    ancillary->pupil_iris_boundary_freeman_code_data.start_y);
		FPRINTF(fp, "\tPupil-Iris Boundary FCC Num Codes: %hu\n",
		    ancillary->pupil_iris_boundary_freeman_code_data.num_codes);
	}
	FPRINTF(fp, "\tSclera-Iris Boundary FCC Length\t: %hu\n",
	    ancillary->sclera_iris_boundary_freeman_code_length);
	if (ancillary->sclera_iris_boundary_freeman_code_length != 0) {
		FPRINTF(fp, "\tSclera-Iris Boundary Start Point: (%hu, %hu)\n",
		    ancillary->sclera_iris_boundary_freeman_code_data.start_x,
		    ancillary->sclera_iris_boundary_freeman_code_data.start_y);
		FPRINTF(fp, "\tSclera-Iris Boundary FCC Num Codes: %hu\n",
		    ancillary->sclera_iris_boundary_freeman_code_data.num_codes);
// XXX what to do with the bit-packed freeman code data? save to file?
	}
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}
