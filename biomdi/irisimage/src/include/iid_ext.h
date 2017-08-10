/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/
/******************************************************************************/
/* Header file for the NIST IREX extensions to the Iris Image Data Record     */
/* format, as specified in  ISO/IEC 19794-6:2005. See "Iris Exchange (IREX)   */
/* Evaluation 2008: Concept, Evaluation Plan and API" at                      */
/* http://iris.nist.gov/irex/                                                 */
/*                                                                            */
/******************************************************************************/
#ifndef _IID_EXT_H
#define _IID_EXT_H

#define IID_ROIMASK_LENGTH		3
#define IID_UNSEG_POLAR_LENTH		12
#define IID_IMAGE_ANCILLARY_MIN_LENGTH	28
#define IID_IMAGE_ANCILLARY_LENGTH(_ancillary)				\
	(IID_IMAGE_ANCILLARY_MIN_LENGTH +				\
	    (_ancillary)->pupil_iris_boundary_freeman_code_length +	\
	    (_ancillary)->sclera_iris_boundary_freeman_code_length)
	    

struct roi_mask {
	uint8_t			upper_eyelid_mask;
	uint8_t			lower_eyelid_mask;
	uint8_t			sclera_mask;
};
typedef struct roi_mask ROIMASK;

struct unsegmented_polar {
	uint16_t		num_samples_radially;
	uint16_t		num_samples_circumferentially;
	uint16_t		inner_outer_circle_x;
	uint16_t		inner_outer_circle_y;
	uint16_t		inner_circle_radius;
	uint16_t		outer_circle_radius;
};
typedef struct unsegmented_polar UNSEGPOLAR;

#define IID_EXT_COORD_NOT_COMPUTED	0x7FFF

#define IID_EXT_FCCB_HEADER_LEN		6
struct freeman_chain_code_block {
	uint16_t		start_x;
	uint16_t		start_y;
	uint16_t		num_codes;
	uint8_t		       *fcc;
};
typedef struct freeman_chain_code_block FCCB;

struct image_ancillary {
	int16_t			pupil_center_of_ellipse_x;
	int16_t			pupil_center_of_ellipse_y;
	int16_t			pupil_semimajor_intersection_x;
	int16_t			pupil_semimajor_intersection_y;
	int16_t			pupil_semiminor_intersection_x;
	int16_t			pupil_semiminor_intersection_y;
	int16_t			iris_center_of_ellipse_x;
	int16_t			iris_center_of_ellipse_y;
	int16_t			iris_semimajor_intersection_x;
	int16_t			iris_semimajor_intersection_y;
	int16_t			iris_semiminor_intersection_x;
	int16_t			iris_semiminor_intersection_y;
	uint16_t		pupil_iris_boundary_freeman_code_length;
	FCCB			pupil_iris_boundary_freeman_code_data;
	uint16_t		sclera_iris_boundary_freeman_code_length;
	FCCB			sclera_iris_boundary_freeman_code_data;
};
typedef struct image_ancillary IMAGEANCILLARY;

/******************************************************************************/
/* Define the interface for allocating and freeing iris image data blocks.    */
/******************************************************************************/
int new_roimask(ROIMASK **roimask);
void free_roimask(ROIMASK *roimask);

int new_unsegpolar(UNSEGPOLAR **unsegpolar);
void free_unsegpolar(UNSEGPOLAR *unsegpolar);

int new_image_ancillary(IMAGEANCILLARY **ancillary);
void free_image_ancillary(IMAGEANCILLARY *ancillary);

/******************************************************************************/
/* Define the interface for reading/writing/verifying iris image data blocks. */
/******************************************************************************/
/******************************************************************************/
/* Functions to read Iris Image "extra" data from a file, or buffer.          */
/* The FILE and BDB structs are modified by these functions.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fp          The open file pointer.                                       */
/*   bdb         Pointer to the biometric data block containing extra data.   */
/*   roimask     Pointer to the output ROI mask structure.                    */
/*   unsegpolar  Pointer to the output UNSEG POLAR data structure.            */
/*   ancillary   Pointer to the output ancilary data structure.               */
/*                                                                            */
/* Return:                                                                    */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int read_roimask(FILE *fp, ROIMASK *roimask);
int scan_roimask(BDB *bdb, ROIMASK *roimask);
int read_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar);
int scan_unsegpolar(BDB *bdb, UNSEGPOLAR *unsegpolar);
int read_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary);
int scan_image_ancillary(BDB *bdb, IMAGEANCILLARY *ancillary);

/******************************************************************************/
/* Functions to write Iris Image "extra" data from a file, or buffer.         */
/* The FILE and BDB structs are modified by these functions.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fp          The open file pointer.                                       */
/*   bdb         Pointer to the biometric data block containing extra data.   */
/*   roimask     Pointer to the output ROI mask structure.                    */
/*   unsegpolar  Pointer to the output UNSEG POLAR data structure.            */
/*   ancillary   Pointer to the output ancilary data structure.               */
/*                                                                            */
/* Return:                                                                    */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int write_roimask(FILE *fp, ROIMASK *roimask);
int push_roimask(BDB *bdb, ROIMASK *roimask);
int write_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar);
int push_unsegpolar(BDB *bdb, UNSEGPOLAR *unsegpolar);
int write_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary);
int push_image_ancillary(BDB *bdb, IMAGEANCILLARY *ancillary);

int print_roimask(FILE *fp, ROIMASK *roimask);
int print_unsegpolar(FILE *fp, UNSEGPOLAR *unsegpolar);
int print_image_ancillary(FILE *fp, IMAGEANCILLARY *ancillary);

int validate_roimask(ROIMASK *roimask);
int validate_unsegpolar(UNSEGPOLAR *unsegpolar);
int validate_image_ancillary(IMAGEANCILLARY *ancillary);

#endif 	/* _IID_EXT_H */
