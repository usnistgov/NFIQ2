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
/* Header file for the Face Recognition Format record, as specified in        */
/* ANSI/INCITS 385-2004.                                                      */
/*                                                                            */
/******************************************************************************/
#ifndef _FRF_H
#define _FRF_H
#include <biomdimacro.h>

/******************************************************************************/
/* Components of the Facial Header Block                                      */
/******************************************************************************/
// The identifier that must appear at the beginning of the record header
#define FRF_FORMAT_ID 		"FAC"
#define FRF_FORMAT_ID_LENGTH	4

// The version number
#define FRF_VERSION_NUM		"010"
#define FRF_VERSION_NUM_LENGTH	4

// Header Length
#define FRF_FHB_LENGTH		14

// Min number of facial images
#define FRF_MIN_NUM_FACIAL_IMAGES	1

/******************************************************************************/
/* Components of the Image Information Block                                  */
/******************************************************************************/

// Device types
#define FRF_DEVICE_TYPE_UNSPECIFIED		0

// Quality values
#define FRF_IMAGE_QUALITY_UNSPECIFIED		0

/******************************************************************************/
/* Components of the Feature Point Block                                      */
/******************************************************************************/

// Feature types
#define FRF_FEATURE_TYPE_2D		1

// Feature Point masks, shift values
#define FRF_FEATURE_POINT_MAJOR_MASK	0xF0
#define FRF_FEATURE_POINT_MINOR_MASK	0x0F
#define FRF_FEATURE_POINT_MAJOR_SHIFT	4

// Representation of the Feature Point Block
#define FRF_FPB_LENGTH			8
#define FRF_MIN_FPBS			0
struct feature_point_block {
	unsigned char				type;
	unsigned char				major_point;
	unsigned char				minor_point;
	unsigned short				x_coord;
	unsigned short				y_coord;

	// The reserved fied is stored so we can do a validity check
	unsigned short				reserved;

	// List pointers to tie all Feature Point Blocks together
	TAILQ_ENTRY(feature_point_block)	list;

	// Back pointer to the parent Facial Data Block
	struct facial_data_block		*fdb;
};
typedef struct feature_point_block FPB;

/******************************************************************************/
/* Representation of the Facial Data Block. This includes the Facial          */
/* Information, list of Feature Points, Image Information, and Image Data.    */
/******************************************************************************/
#define FRF_FIB_LENGTH				20
#define FRF_IIB_LENGTH				12
#define FRF_IMAGE_DATA_TYPE_JPEG		0
#define FRF_IMAGE_DATA_TYPE_JPEG2000		1

struct facial_data_block {
	// Representation of the Facial Information Block
	unsigned int				block_length;
	unsigned short				num_feature_points;
	unsigned char				gender;
	unsigned char				eye_color;
	unsigned char				hair_color;
	unsigned int				feature_mask;
	unsigned short				expression;
	// angles are stored in decoded form
	unsigned char				pose_angle_yaw; 
	unsigned char				pose_angle_pitch;
	unsigned char				pose_angle_roll;
	unsigned char				pose_angle_uncertainty_yaw;
	unsigned char				pose_angle_uncertainty_pitch;
	unsigned char				pose_angle_uncertainty_roll;
	TAILQ_HEAD(, feature_point_block)	feature_points;

	// Representation of the Image Information Block
	unsigned char				face_image_type;
	unsigned char				image_data_type;
	unsigned short				width;
	unsigned short				height;
	unsigned char				image_color_space;
	unsigned char				source_type;
	unsigned short				device_type;
	unsigned short				quality;

	// Optional Image Data
	unsigned int				image_len;
	void					*image_data;

	// List pointers to tie all the Facial Data Blocks together
	TAILQ_ENTRY(facial_data_block)		list;

	// Back pointer to the parent Facial Block
	struct facial_block			*fb;
};
typedef struct facial_data_block FDB;

/******************************************************************************/
/* Representation of the Facial Block. This contains the Facial Header        */
/* and the list of Facial Data Blocks.                                        */
/******************************************************************************/
struct facial_block {
	char					format_id[4];
	char					version_num[4];		
	unsigned int				record_length;
	unsigned short				num_faces;
	TAILQ_HEAD(, facial_data_block)		facial_data;
};
typedef struct facial_block FB;

/******************************************************************************/
/* Allocate and initialize storage for a single Facial Block.                 */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   fb     Address of a pointer to the Facial Block that will be allocated.  */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fb(FB **fb);

/******************************************************************************/
/* Free the storage for a single Facial Block.                                */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Facial Block will also be free'd.            */
/*                                                                            */
/* Parameters:                                                                */
/*   fb     Pointer to the FB  structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fb(FB *fb);

/******************************************************************************/
/* Read a Facial Block from a file or data block, including the Facial Header */
/* and all of the Facial Data blocks.                                         */
/* This function does not do any validation of the data being read.           */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fbdb   Pointer to the biometric data block containing facial data.       */
/*   fb     Pointer to the Facial Block.                                      */
/*                                                                            */
/* Returns:                                                                   */
/*      READ_OK      Success                                                  */
/*      READ_ERROR   Failure                                                  */
/*                                                                            */
/******************************************************************************/
int
read_fb(FILE *fp, FB *fb);
int
scan_fb(BDB *fbdb, FB *fb);

/******************************************************************************/
/* Write a Facial Block to a file or memory buffer, including the Facial      */
/* Header and all of the Facial Data blocks.                                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fbdb   Pointer to the biometric data block containing facial data.       */
/*   fb     Pointer to the Facial Block.                                      */
/*                                                                            */
/* Returns:                                                                   */
/*      WRITE_OK      Success                                                 */
/*      WRITE_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
write_fb(FILE *fp, FB *fb);
int
push_fb(BDB *fbdb, FB *fb);

/******************************************************************************/
/* Print a Facial Block to a file in human-readable form. The Facial Header   */
/* and all of the Facial Data blocks are printed.                             */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fb     Pointer to the Facial Block.                                      */
/*                                                                            */
/* Returns:                                                                   */
/*      PRINT_OK      Success                                                 */
/*      PRINT_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
print_fb(FILE *fp, FB *fb);

/******************************************************************************/
/* Validate a Facial Block, including the Facial Header and all of the        */
/* Facial Data blocks. Diagnostic messages are written to stderr.             */
/*                                                                            */
/* Parameters:                                                                */
/*   fb     Pointer to the Facial Block.                                      */
/*                                                                            */
/* Returns:                                                                   */
/*      VALIDATE_OK      Success                                              */
/*      VALIDATE_ERROR   Failure                                              */
/*                                                                            */
/******************************************************************************/
int
validate_fb(FB *fb);

/******************************************************************************/
/* Add a Facial Data Block to a Facial Block.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fdb    Pointer to the Facial Data Block record that will be added.       */
/*   fb     Pointer to the Facial Block.                                      */
/*                                                                            */
/******************************************************************************/
void
add_fdb_to_fb(FDB *fdb, FB *fb);

/******************************************************************************/
/* Allocate and initialize storage for a single Feature Point Block.          */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   fpb    Address of a pointer to the Feature Point Block that will be      */
/*          allocated.                                                        */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fpb(FPB **fpb);

/******************************************************************************/
/* Free the storage for a single Feature Point Block.                         */
/*                                                                            */
/* Parameters:                                                                */
/*   fpb    Pointer to the FPB structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fpb(FPB *fpb);

/******************************************************************************/
/* Read a Feature Point Block from a file or memory buffer.                   */
/* This function does not do any validation of the data being read.           */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fpbdb  Pointer to the biometric data block containing feature point data.*/
/*   fpb    Pointer to the Feature Point Block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*      READ_OK      Success                                                  */
/*      READ_ERROR   Failure                                                  */
/*                                                                            */
/******************************************************************************/
int
read_fpb(FILE *fp, FPB *fpb);
int
scan_fpb(BDB *fpbdb, FPB *fpb);

/******************************************************************************/
/* Write a Feature Point Block to a file or memory buffer.                    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fpbdb  Pointer to the biometric data block containing feature point data.*/
/*   fpb    Pointer to the Feature Point Block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*      WRITE_OK      Success                                                 */
/*      WRITE_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
write_fpb(FILE *fp, FPB *fpb);
int
push_fpb(BDB *fpbdb, FPB *fpb);

/******************************************************************************/
/* Print a Feature Point Block to a file in human-readabe form.               */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fpb    Pointer to the Feature Point Block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*      PRINT_OK      Success                                                 */
/*      PRINT_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
print_fpb(FILE *fp, FPB *fpb);

/******************************************************************************/
/* Validate a Feature Point Block. Diagnostic messages are written to stderr. */
/*                                                                            */
/* Parameters:                                                                */
/*   fpb    Pointer to the Feature Point Block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*      VALIDATE_OK      Success                                              */
/*      VALIDATE_ERROR   Failure                                              */
/*                                                                            */
/******************************************************************************/
int
validate_fpb(FPB *fpb);

/******************************************************************************/
/* Allocate and initialize storage for a single Facial Data Block.            */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   fpb    Address of a pointer to the Facial Data Block that will be        */
/*          allocated.                                                        */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fdb(FDB **fdb);

/******************************************************************************/
/* Free the storage for a single Facial Data Block.                           */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Feature Point Block will also be free'd.     */
/*                                                                            */
/* Parameters:                                                                */
/*   fdb    Pointer to the FDB structure that will be free'd.                 */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
void
free_fdb(FDB *fdb);

/******************************************************************************/
/* Read a Facial Data Block from a file or memory buffer.                     */
/* This function does not do any validation of the data being read.           */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fdbdb  Pointer to the biometric data block containing facial data block. */
/*   fdb     Pointer to the Facial Data Block.                                */
/*                                                                            */
/* Returns:                                                                   */
/*      READ_OK      Success                                                  */
/*      READ_ERROR   Failure                                                  */
/*                                                                            */
/******************************************************************************/
int
read_fdb(FILE *fp, FDB *fdb);
int
scan_fdb(BDB *fdbdb, FDB *fdb);

/******************************************************************************/
/* Write a Facial Data Block to a file or memory buffer.                      */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fdbdb  Pointer to the biometric data block containing facial data block. */
/*   fdb    Pointer to the Facial Data Block.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*      WRITE_OK      Success                                                 */
/*      WRITE_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
write_fdb(FILE *fp, FDB *fdb);
int
push_fdb(BDB *fdbdb, FDB *fdb);


/******************************************************************************/
/* Print a Facial Data Block to a file in human-readable form.                */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fdb    Pointer to the Facial Data Block.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*      PRINT_OK      Success                                                 */
/*      PRINT_ERROR   Failure                                                 */
/*                                                                            */
/******************************************************************************/
int
print_fdb(FILE *fp, FDB *fdb);

/******************************************************************************/
/* Validate a Facial Data Block, including all of the associated Feature      */
/* Points. Diagnostic messages are written to stderr.                         */
/*                                                                            */
/* Parameters:                                                                */
/*   fdb    Pointer to the Facial Data Block.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*      VALIDATE_OK      Success                                              */
/*      VALIDATE_ERROR   Failure                                              */
/*                                                                            */
/******************************************************************************/
int
validate_fdb(FDB *fdb);

/******************************************************************************/
/* Add a Feature Point Block to a Facial Data Block.                          */
/*                                                                            */
/* Parameters:                                                                */
/*                                                                            */
/*   fpb    Pointer to the FPB structure to be added.                         */
/*   fdb    Pointer to the FDB structure.                                     */
/*                                                                            */
/******************************************************************************/
void
add_fpb_to_fdb(FPB *fpb, FDB *fdb);

/******************************************************************************/
/* Add an image from a file to the Facial Data Block.  The image_len and      */
/* block_length fields of the FDB will be updated.                            */
/*                                                                            */
/* Parameters:                                                                */
/*                                                                            */
/*   filename The name ofthe file containing the image.                       */
/*   fdb      Pointer to the FDB structure.                                   */
/*                                                                            */
/******************************************************************************/
int
add_image_to_fdb(char *filename, FDB *fdb);

// Min record length in the header
#define FRF_MIN_RECORD_LENGTH	(FRF_FHB_LENGTH + FRF_FIB_LENGTH +	\
				(FRF_MIN_FPBS * FRF_FPB_LENGTH) + FRF_IIB_LENGTH)
				

#endif /* !_FRF_H */
