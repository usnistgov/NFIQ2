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
/* Header file for the Iris Image Data Record format, as specified in the     */
/* Iris Exchange (IREX) Evaluation 2008 specification.                        */
/*                                                                            */
/* Layout of the entire iris image biometric data block in memory:            */
/*                                                                            */
/*       Iris Image Biometric Data Block                                      */
/*   +-------------------------------------+                                  */
/*   |  iris record header (46 bytes)      |                                  */
/*   +-------------------------------------+                                  */
/*   Array[2] {                                                               */
/*       +--------------------------------------------+                       */
/*       |  iris biometric subtype header (3 bytes)   |                       */
/*       +--------------------------------------------+                       */
/*       List {                                                               */
/*           +--------------------------------|     +------------+            */
/*           |  iris image header (>= 58)     |-->  | image data |            */
/*           +--------------------------------|     +------------+            */
/*             +----------------------------+                                 */
/*             | ROI masked data (3)        |                                 */
/*             +----------------------------+                                 */
/*             +----------------------------+                                 */
/*             | UNSEG polar data (12)      |                                 */
/*             +----------------------------+                                 */
/*             +----------------------------+     +-------------------+       */
/*             | commmon image data (>= 28) |-->  | freeman code data |       */
/*             +----------------------------+     +-------------------+       */
/*       }                                                                    */
/*   }                                                                        */
/*                                                                            */
/******************************************************************************/
#ifndef _IID_H
#define _IID_H

#define IID_FORMAT_ID				"IIR"
#define IID_FORMAT_ID_LEN			4

#define IID_IMAGE_KIND_RECTLINEAR_NO_ROI_NO_CROPPING	0x01
#define IID_IMAGE_KIND_RECTLINEAR_NO_ROI_CROPPING	0x03
#define IID_IMAGE_KIND_RECTLINEAR_MASKING_CROPPING	0x07
#define IID_IMAGE_KIND_UNSEGMENTED_POLAR		0x10
#define IID_IMAGE_KIND_RECTILINEAR_UNSEGMENTED_POLAR	0x30

#define IID_FORMAT_VERSION_LEN			4

#define IID_DEVICE_UNIQUE_ID_LEN		16
#define IID_CAPTURE_DEVICE_UNDEF		0

#define IID_MIN_EYES				0
#define IID_MAX_EYES				2

#define IID_RECORD_HEADER_LENGTH		46
#define IID_IBSH_LENGTH				3
#define IID_IIH_LENGTH				15

/* Bit definitions for the Iris Record Header image properties field. */
#define IID_HORIZONTAL_ORIENTATION_MASK		0x0003
#define IID_HORIZONTAL_ORIENTATION_SHIFT	0
#define IID_VERTICAL_ORIENTATION_MASK		0x000C
#define IID_VERTICAL_ORIENTATION_SHIFT		2
#define IID_SCAN_TYPE_MASK			0x0030
#define IID_SCAN_TYPE_SHIFT			4
#define IID_IRIS_OCCLUSIONS_MASK		0x0040
#define IID_IRIS_OCCLUSIONS_SHIFT		6
#define IID_OCCLUSION_FILLING_MASK		0x0080
#define IID_OCCLUSION_FILLING_SHIFT		7

#define IID_ORIENTATION_UNDEF			0
#define IID_ORIENTATION_BASE			1
#define IID_ORIENTATION_FLIPPED			2
#define IID_SCAN_TYPE_CORRECTED			0
#define IID_SCAN_TYPE_PROGRESSIVE		1
#define IID_SCAN_TYPE_INTERLACE_FRAME		2
#define IID_SCAN_TYPE_INTERLACE_FIELD		3
#define IID_IROCC_UNDEF				0
#define IID_IROCC_PROCESSED			1
#define IID_IROCC_ZEROFILL			0
#define IID_IROCC_UNITFILL			1
#define IID_IRBNDY_UNDEF			0
#define IID_IRBNDY_PROCESSED			1

#define IID_IMAGEFORMAT_MONO_RAW		0x0002
#define IID_IMAGEFORMAT_RGB_RAW			0x0004
#define IID_IMAGEFORMAT_MONO_JPEG		0x0006
#define IID_IMAGEFORMAT_RGB_JPEG		0x0008
#define IID_IMAGEFORMAT_MONO_JPEG_LS		0x000A
#define IID_IMAGEFORMAT_RGB_JPEG_LS		0x000C
#define IID_IMAGEFORMAT_MONO_JPEG2000		0x000E
#define IID_IMAGEFORMAT_RGB_JPEG2000		0x0010
#define IID_WIDTH_UNDEF				0
#define IID_HIEGHT_UNDEF			0
#define IID_INTENSITY_DEPTH_UNDEF		0
#define IID_TRANS_UNDEF				0
#define IID_TRANS_STD				1
#define IID_DEVICE_UNIQUE_ID_SERIAL_NUMBER	'D'
#define IID_DEVICE_UNIQUE_ID_MAC_ADDRESS	'M'
#define IID_DEVICE_UNIQUE_ID_PROCESSOR_ID	'P'
#define IID_DEVICE_UNIQUE_ID_NONE		"0000000000000000"

/*
 * Class codes for converting codes to strings. See the iid_code_str()
 * function defined below.
 */
#define IID_CODE_CATEGORY_ORIENTATION		0
#define IID_CODE_CATEGORY_SCAN_TYPE		1
#define IID_CODE_CATEGORY_OCCLUSION		2
#define IID_CODE_CATEGORY_OCCLUSION_FILLING	3
#define IID_CODE_CATEGORY_IMAGE_FORMAT		4
#define IID_CODE_CATEGORY_IMAGE_TRANSFORMATION	5
#define IID_CODE_CATEGORY_EYE_POSITION		6
#define IID_CODE_CATEGORY_KIND_OF_IMAGERY		7

struct iris_record_header {
#define irh_startcopy		format_id
	char			format_id[IID_FORMAT_ID_LEN];
	char			format_version[IID_FORMAT_VERSION_LEN];
	uint8_t			kind_of_imagery;
	uint32_t		record_length;
	uint16_t		capture_device_id;
	uint8_t			num_eyes;
	uint16_t		record_header_length;
	uint8_t			horizontal_orientation;
	uint8_t			vertical_orientation;
	uint8_t			scan_type;
	uint8_t			iris_occlusions;
	uint8_t			occlusion_filling;
	uint16_t		diameter;
	uint16_t		image_format;
	uint16_t		image_width;
	uint16_t		image_height;
	uint8_t			intensity_depth;
	uint8_t			image_transformation;
	char			device_unique_id[IID_DEVICE_UNIQUE_ID_LEN];
#define irh_endcopy		dummy
	uint32_t		dummy;
};
typedef struct iris_record_header IRH;

#define IID_ROT_ANGLE_MIN			0
#define IID_ROT_ANGLE_MAX			0xFFFE
#define IID_ROT_ANGLE_UNDEF			0xFFFF
#define IID_ROT_UNCERTAIN_MIN			0
#define IID_ROT_UNCERTAIN_MAX			0xFFFE
#define IID_ROT_UNCERTAIN_UNDEF			0xFFFF
#define IID_MIN_IMAGE_QUALITY			0
#define IID_MAX_IMAGE_QUALITY			100
#define IID_IMAGE_QUALITY_NOT_COMPUTED		254
#define IID_IMAGE_QUALITY_NOT_AVAILABLE		255

struct iris_image_header {
#define iih_startcopy				image_length
	uint32_t				image_length;
	uint16_t				image_number;
	uint8_t					image_quality;
	uint16_t				quality_algo_vendor_id;
	uint16_t				quality_algo_id;
	uint16_t				rotation_angle;
	uint16_t				rotation_uncertainty;
	struct roi_mask				roi_mask;
	struct unsegmented_polar		unsegmented_polar;
	struct image_ancillary			image_ancillary;
#define iih_endcopy				image_data
	uint8_t					*image_data;
	TAILQ_ENTRY(iris_image_header)		list;
	struct iris_biometric_subtype_header	*ibsh; /* ptr to parent rec */
};
typedef struct iris_image_header IIH;

#define IID_EYE_UNDEF				0x00
#define IID_EYE_RIGHT				0x01
#define IID_EYE_LEFT				0x02
#define IID_EYE_MIN_IMAGES			1
#define IID_EYE_MAX_IMAGES			65535

struct iris_biometric_subtype_header {
#define ibsh_startcopy				eye_position
	uint8_t					eye_position;
	uint16_t				num_images;
#define ibsh_endcopy				iibdb
	struct iris_image_biometric_data_block *iibdb; /* ptr to parent block */
	TAILQ_HEAD(, iris_image_header)	image_headers;
};
typedef struct iris_biometric_subtype_header IBSH;

struct iris_image_biometric_data_block {
	IRH				record_header;
	IBSH				*biometric_subtype_headers[2];
};
typedef struct iris_image_biometric_data_block IIBDB;

/******************************************************************************/
/* Define the interface for allocating and freeing iris image data blocks.    */
/******************************************************************************/
int new_iih(IIH **iih);
void free_iih(IIH *iih);

int new_ibsh(IBSH **ibsh);
void free_ibsh(IBSH *ibsh);

int new_iibdb(IIBDB **iibdb);
void free_iibdb(IIBDB *iibdb);

/******************************************************************************/
/* Define the interface for reading/writing/verifying iris image data blocks. */
/******************************************************************************/
/******************************************************************************/
/* Functions to read Iris Image records from a file, or buffer. Each function */
/* reads/scans the complete record, including all sub-records. For example,   */
/* read_iibdb() reads the record header, the Iris Image headers, and all the  */
/* Biometric Subtype Headers. The FILE and BDB structs are modified by these  */
/* functions.                                                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   bdb    Pointer to the biometric data block containing iris data.         */
/*   iih    Pointer to the output iris image header structure.                */
/*   ibsh   Pointer to the output biometric subtype header structure.         */
/*   ibsh   Pointer to the output iris image biometric datablock structure.   */
/*                                                                            */
/* Return:                                                                    */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int read_iibdb(FILE *fp, IIBDB *iibdb);
int scan_iibdb(BDB *bdb, IIBDB *iibdb);
int read_ibsh(FILE *fp, IBSH *ibsh);
int scan_ibsh(BDB *bdb, IBSH *ibsh);
int read_iih(FILE *fp, IIH *iih);
int scan_iih(BDB *bdb, IIH *iih);

/******************************************************************************/
/* Functions to write Iris Image records from a file, or buffer. Each         */
/*  function writes/pushes the complete record, including all sub-records.    */
/* The FILE and BDB structs are modified by these functions.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   bdb    Pointer to the biometric data block containing iris data.         */
/*   iih    Pointer to the output iris image header structure.                */
/*   ibsh   Pointer to the output biometric subtype header structure.         */
/*   ibsh   Pointer to the output iris image biometric datablock structure.   */
/*                                                                            */
/* Return:                                                                    */
/*        WRITE_OK    Success                                                 */
/*        WRITE_ERROR Failure                                                 */
/******************************************************************************/
int write_iibdb(FILE *fp, IIBDB *iibdb);
int push_iibdb(BDB *bdb, IIBDB *iibdb);
int write_ibsh(FILE *fp, IBSH *ibsh);
int push_ibsh(BDB *bdb, IBSH *ibsh);
int write_iih(FILE *fp, IIH *iih);
int push_iih(BDB *bdb, IIH *iih);

/******************************************************************************/
/* Functions to print Iris Image records to a file in human-readable form.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   iih    Pointer to the output iris image header structure.                */
/*   ibsh   Pointer to the output biometric subtype header structure.         */
/*   ibsh   Pointer to the output iris image biometric datablock structure.   */
/*                                                                            */
/* Return:                                                                    */
/*        WRITE_OK    Success                                                 */
/*        WRITE_ERROR Failure                                                 */
/******************************************************************************/
int print_iih(FILE *fp, IIH *iih);
int print_ibsh(FILE *fp, IBSH *ibsh);
int print_iibdb(FILE *fp, IIBDB *iibdb);

/******************************************************************************/
/* Functions to validate Iris Image records according to the requirements of  */
/* the ISO/IEC 19794-6:2005 Iris Image Data standard.                         */
/*                                                                            */
/* Parameters:                                                                */
/*   iih    Pointer to the output iris image header structure.                */
/*   ibsh   Pointer to the output biometric subtype header structure.         */
/*   ibsh   Pointer to the output iris image biometric datablock structure.   */
/*                                                                            */
/* Return:                                                                    */
/*        VALIDATE_OK       Record does conform                               */
/*        VALIDATE_ERROR    Record does NOT conform                           */
/******************************************************************************/
int validate_ibsh(IBSH *ibsh);
int validate_iibdb(IIBDB *fir);
int validate_iih(IIH *iih);

void add_iih_to_ibsh(IIH *iih, IBSH *ibsh);

/******************************************************************************/
/* Defintion of the higher level access routines.                             */
/******************************************************************************/
int get_ibsh_count(IIBDB *iibdb);
int get_ibshs(IIBDB *iibdb, IBSH *ibshs[]);
int get_iih_count(IBSH *ibsh);
int get_iihs(IBSH *ibsh, IIH *iihs[]);

/* Convert a code (image orientation, scan type, etc. to a string. The
 * 'category' argument must be one of those defined above. This function will
 * either return a string matching the code within the category, or 'INVALID'
 * for an invalid code for the category.
 */
char * iid_code_to_str(int category, int code);

/*
 * Clone a complete Iris Image Data Block. The caller is responsible for
 * freeing the clone. Will return -1 on allocation failures, 0 otherwise.
 * Parameters:
 *    src      Pointer to the source data block
 *    dst      Pointer to the destination data block
 *    cloneimg Flag indicating whether to clone the image data
 * The macros that follow copy the data items in each structure, but do
 * not copy the linkages between data blocks.
 */
int clone_iibdb(IIBDB *src, IIBDB **dst, int cloneimg);

#define COPY_IIH(_src, _dst)						\
	bcopy(&_src->iih_startcopy, &_dst->iih_startcopy,		\
	    (unsigned)((uint8_t *)&_dst->iih_endcopy -			\
		(uint8_t *)&_dst->iih_startcopy))

#define COPY_IRH(_src, _dst)						\
	bcopy(&_src->irh_startcopy, &_dst->irh_startcopy,		\
	    (unsigned)((uint8_t *)&_dst->irh_endcopy -			\
		(uint8_t *)&_dst->irh_startcopy))

#define COPY_IBSH(_src, _dst)						\
	bcopy(&_src->ibsh_startcopy, &_dst->ibsh_startcopy,		\
	    (unsigned)((uint8_t *)&_dst->ibsh_endcopy -			\
		(uint8_t *)&_dst->ibsh_startcopy))

#endif 	/* _IID_H */
