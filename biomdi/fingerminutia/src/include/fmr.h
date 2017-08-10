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
/* Header file for the Finger Minutia Record format, as specified in          */
/* ANSI/INCITS 378-2004 and ISO/IEC 19794-2.                                  */
/*                                                                            */
/*                                                                            */
/* Expected layout of the entire finger minutiae record in memory:            */
/*                                                                            */
/*         FMR                                                                */
/*   +------------+                                                           */
/*   |   header   |                                                           */
/*   |------------|                                                           */
/*   |finger_views|-----+                                                     */
/*   +------------+     |                                                     */
/*                      |                                                     */
/*           +----------+                                                     */
/*           |                                                                */
/*           V  FVMR                              FVMR                        */
/*      +-----------------------------+       +------------+                  */
/*      |record|minutiae_data|extended|  ...  |record| ... |                  */
/*      +-----------------------------+       +------------+                  */
/*                    |            |                                          */
/*                    |            |                                          */
/*                    |            V        FEDB                              */
/*                    |         +------------------------+                    */
/*                    |         | record | extended_data |                    */
/*                    |         +------------------------+                    */
/*                    |                           |                           */
/*                    |                           |                           */
/*                    |                     FED   V           FED             */
/*                    |                    +-----------+     +------------+   */
/*           +--------+                    |record|data| ... |record| ... |   */
/*           |                             +-----------+     +------------+   */
/*           |                                       |                        */
/*           |                                       |                        */
/*      FMD  V                                       V                        */
/*       +------+     +------+                      +-----------------+       */
/*       |record| ... |record|                      | | | | | | | | | |       */
/*       +------+     +------+                      +-----------------+       */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef _FMR_H
#define _FMR_H

#if defined WINDOWS || defined WIN32
	#include <sys/queue.h>
#endif

// Stupid
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define FMR_STD_ANSI			1
#define FMR_STD_ISO			2
#define FMR_STD_ISO_NORMAL_CARD		3
#define FMR_STD_ISO_COMPACT_CARD	4
#define FMR_STD_ANSI07			5

// The identifier that must appear at the beginning of the record header
#define FMR_FORMAT_ID 		"FMR"
#define FMR_FORMAT_ID_LEN 	4

// The version number
#define FMR_ANSI_SPEC_VERSION	" 20"
#define FMR_ISO_SPEC_VERSION	" 20"
#define FMR_ANSI07_SPEC_VERSION	"030"
#define FMR_SPEC_VERSION_LEN	4

#define FMR_MAX_FINGER_CODE	10	// Plain codes are not used in FMR
#define FMR_NUM_FINGER_CODES 	11

#define	FMR_MAX_NUM_MINUTIAE	255

// Define the masks for View # and Impression Type from the minutiae record
#define FVMR_VIEW_NUMBER_MASK	0xF0
#define FVMR_VIEW_NUMBER_SHIFT	4
#define	FVMR_IMPRESSION_MASK	0x0F

// Define the masks for the minutia type and x/y coordinates within a
// minutia data record
#define FMD_MINUTIA_TYPE_MASK	0xC000
#define FMD_RESERVED_MASK	0xC000
#define FMD_MINUTIA_TYPE_SHIFT	14
#define FMD_RESERVED_SHIFT	14
#define FMD_X_COORD_MASK	0x3FFF
#define FMD_Y_COORD_MASK	0x3FFF

// The ISO Compact FMD record has type encoded with the angle value
#define FMD_ISO_COMPACT_MINUTIA_TYPE_MASK	0xC0
#define FMD_ISO_COMPACT_MINUTIA_TYPE_SHIFT	6
#define FMD_ISO_COMPACT_MINUTIA_ANGLE_MASK	0x3F

// Range of the Minutia Quality values
#define FMD_MIN_MINUTIA_QUALITY		0
#define FMD_MAX_MINUTIA_QUALITY		100
#define FMD_UNKNOWN_MINUTIA_QUALITY	0
#define FMD_NOATTTEMPT_MINUTIA_QUALITY	254
#define FMD_FAILED_MINUTIA_QUALITY	255

// Range of Minutia Angle values
#define FMD_MIN_MINUTIA_ANGLE		0
#define FMD_MAX_MINUTIA_ANGLE		179
#define FMD_MAX_MINUTIA_ISONC_ANGLE	255
#define FMD_MAX_MINUTIA_ISOCC_ANGLE	63

// What each unit of angle represents in terms of degrees
#define FMD_ANSI_ANGLE_UNIT		(2)
#define FMD_ISO_ANGLE_UNIT		(360.0 / 256.0)
#define FMD_ISOCC_ANGLE_UNIT		(360.0 / 64.0)

// Types of Minutia
#define FMD_MINUTIA_TYPE_OTHER		0
#define FMD_MINUTIA_TYPE_RIDGE_ENDING	1
#define FMD_MINUTIA_TYPE_BIFURCATION	2

// Range of the Finger Quality values
#define FMR_MIN_FINGER_QUALITY	0
#define FMR_MAX_FINGER_QUALITY	100

#define ISO_UNKNOWN_FINGER_QUALITY	0

// Extended Data Area Type Codes
#define FED_RESERVED		0x0000
#define FED_RIDGE_COUNT		0x0001
#define FED_CORE_AND_DELTA	0x0002

// Ridge Count Extraction Method Codes
#define RCE_NONSPECIFIC		0x00
#define RCE_FOUR_NEIGHBOR	0x01
#define RCE_EIGHT_NEIGHBOR	0x02

// Core masks and shift values, etc.
#define ANSI_CORE_TYPE_MASK		0xC0
#define ANSI_CORE_TYPE_SHIFT		6
#define ISO_CORE_TYPE_MASK		0xC000
#define ISO_CORE_TYPE_SHIFT		14
#define CORE_TYPE_ANGULAR		0x01
#define CORE_TYPE_NONANGULAR		0x00
#define ANSI_CORE_NUM_CORES_MASK	0x0F
#define ISO_CORE_NUM_CORES_MASK		0x3F
#define CORE_X_COORD_MASK		0x3FFF
#define CORE_Y_COORD_MASK		0x3FFF
#define CORE_MIN_NUM			0

// Delta masks and shift values, etc.
#define ANSI_DELTA_TYPE_MASK		0xC0
#define ANSI_DELTA_TYPE_SHIFT		6
#define ISO_DELTA_TYPE_MASK		0xC000
#define ISO_DELTA_TYPE_SHIFT		14
#define DELTA_TYPE_NONANGULAR		0x00
#define DELTA_TYPE_ANGULAR		0x01
#define DELTA_NUM_DELTAS_MASK		0x3F
#define DELTA_X_COORD_MASK		0x3FFF
#define DELTA_Y_COORD_MASK		0x3FFF
#define DELTA_MIN_NUM			0

/*
 * Each element of a finger minutiae record is represented as a stucture
 * in memory. Each element has a COPY_XXX macro defined that can be used
 * to copy the essential fields of the record, those fields that represent
 * actual finger minutiae information, and not accidental fields that are
 * used to internally manage the record.
 * NOTE: The coordinate values are always stored as 16-bit values, for
 * even for compact card formats.  The FMR library stores the CC coordinate
 * as 16-bit, but the coordinate is truncated to 8 bits when written to
 * a file or memory buffer. Therefore, sorting of CC minutiae can be
 * done on the records as represented.
 */
// Representation of a single finger minutiae data record
#define FMD_DATA_LENGTH				6
#define FMD_ISO_NORMAL_DATA_LENGTH		5
#define FMD_ISO_COMPACT_DATA_LENGTH		3
struct finger_minutiae_data {
	unsigned int				format_std;
	unsigned int				index;
#define	fmd_startcopy				type
	unsigned char				type;
	unsigned short				x_coord;
	unsigned char				reserved;
	unsigned short				y_coord;
	unsigned char				angle;
	unsigned char				quality;
#define	fmd_endcopy				list
	TAILQ_ENTRY(finger_minutiae_data)	list;
	struct finger_view_minutiae_record	*fvmr;	// back pointer to the
							// parent FVMR
};
typedef struct finger_minutiae_data FMD;

#if defined WINDOWS || defined WIN32
#define COPY_FMD(src, dst)				\
	memcpy(&dst->fmd_startcopy, &src->fmd_startcopy, \
            (unsigned) ((uint8_t *)&dst->fmd_endcopy -	\
		(uint8_t *)&dst->fmd_startcopy))
#else
#define COPY_FMD(src, dst)				\
	bcopy(&src->fmd_startcopy, &dst->fmd_startcopy, \
            (unsigned) ((uint8_t *)&dst->fmd_endcopy -	\
		(uint8_t *)&dst->fmd_startcopy))
#endif
// Representation of the Ridge Count Format
#define RIDGE_COUNT_DATA_LENGTH		3
struct ridge_count_data {
#define	rcd_startcopy			index_one
	unsigned char			index_one;
	unsigned char			index_two;
	unsigned char			count;
#define	rcd_endcopy			list
	TAILQ_ENTRY(ridge_count_data)	list;
	struct ridge_count_data_block	*rcdb;		// back pointer to
							// parent RCDB
};
typedef struct ridge_count_data RCD;
#if defined WINDOWS || defined WIN32
#define COPY_RCD(src, dst)				\
	memcpy(&dst->rcd_startcopy, &src->rcd_startcopy, \
            (unsigned) ((uint8_t *)&dst->rcd_endcopy -	\
		(uint8_t *)&dst->rcd_startcopy))
#else
#define COPY_RCD(src, dst)				\
	bcopy(&src->rcd_startcopy, &dst->rcd_startcopy, \
            (unsigned) ((uint8_t *)&dst->rcd_endcopy -	\
		(uint8_t *)&dst->rcd_startcopy))
#endif
#define RIDGE_COUNT_HEADER_LENGTH	1
struct ridge_count_data_block {
	unsigned char			method;
	TAILQ_HEAD(, ridge_count_data)	ridge_counts;
	struct finger_extended_data	*fed;
};
typedef struct ridge_count_data_block RCDB;

// Representation of the Core Format
#define CORE_DATA_MIN_LENGTH		4
#define CORE_DATA_HEADER_LENGTH		1
#define CORE_ANGLE_LENGTH		1
struct core_data {
	unsigned int			format_std;
#define	cd_startcopy			type
	unsigned char			type;	// Only for ISO
	unsigned short			x_coord;
	unsigned short			y_coord;
	unsigned char			angle;
#define	cd_endcopy			list
	TAILQ_ENTRY(core_data)		list;
	struct core_delta_data_block	*cddb;	
};
typedef struct core_data CD;

#if defined WINDOWS || defined WIN32
#define COPY_CD(src, dst)				\
	memcpy(&dst->cd_startcopy, &src->cd_startcopy,	\
            (unsigned) ((uint8_t *)&dst->cd_endcopy -	\
		(uint8_t *)&dst->cd_startcopy))
#else
#define COPY_CD(src, dst)				\
	bcopy(&src->cd_startcopy, &dst->cd_startcopy,	\
            (unsigned) ((uint8_t *)&dst->cd_endcopy -	\
		(uint8_t *)&dst->cd_startcopy))
#endif
// Representation of the Delta Format
#define DELTA_DATA_MIN_LENGTH		4
#define DELTA_DATA_HEADER_LENGTH	1
#define DELTA_ANGLE_LENGTH		1
struct delta_data {
	unsigned int			format_std;
#define	dd_startcopy			type
	unsigned char			type;	// Only for ISO
	unsigned short			x_coord;
	unsigned short			y_coord;
	unsigned char			angle1;
	unsigned char			angle2;
	unsigned char			angle3;
#define	dd_endcopy			list
	TAILQ_ENTRY(delta_data)		list;
	struct core_delta_data_block	*cddb;
};
typedef struct delta_data DD;

#if defined WINDOWS || defined WIN32
#define COPY_DD(src, dst)				\
	memcpy(&dst->dd_startcopy, &src->dd_startcopy,	\
            (unsigned) ((uint8_t *)&dst->dd_endcopy -	\
		(uint8_t *)&dst->dd_startcopy))
#else
#define COPY_DD(src, dst)				\
	bcopy(&src->dd_startcopy, &dst->dd_startcopy,	\
            (unsigned) ((uint8_t *)&dst->dd_endcopy -	\
		(uint8_t *)&dst->dd_startcopy))
#endif

#define CORE_DATA_HEADER_LENGTH		1
struct core_delta_data_block {
	unsigned int			format_std;
#define	cddb_startcopy			core_type
	unsigned char			core_type;	// Only for ANSI
	unsigned char			num_cores;
	TAILQ_HEAD(, core_data)		cores;
	unsigned char			delta_type;	// Only for ANSI
	unsigned char			num_deltas;
#define	cddb_endcopy			deltas
	TAILQ_HEAD(, delta_data)	deltas;
	struct finger_extended_data	*fed;
};
typedef struct core_delta_data_block CDDB;

#if defined WINDOWS || defined WIN32
#define COPY_CDDB(src, dst)					\
	memcpy(&dst->cddb_startcopy, &src->cddb_startcopy,	\
            (unsigned) ((uint8_t *)&dst->cddb_endcopy -		\
		(uint8_t *)&dst->cddb_startcopy))
#else
#define COPY_CDDB(src, dst)					\
	bcopy(&src->cddb_startcopy, &dst->cddb_startcopy,	\
            (unsigned) ((uint8_t *)&dst->cddb_endcopy -		\
		(uint8_t *)&dst->cddb_startcopy))
#endif

// Representation of a single finger view extended data record. A FED is
// one of three types: a Ridge Count, Core/Delta, or Unknown Data type.
// When the FED is allocated, it's type is fixed at that point. 
#define FED_HEADER_LENGTH		4
struct finger_extended_data {
	unsigned int				format_std;
#define fed_startcopy				type_id
	unsigned short				type_id;
	unsigned short				length;
#define fed_endcopy				data
	struct ridge_count_data_block	*rcdb;
	struct core_delta_data_block	*cddb;
	char				*data;	// of size length-4
	// Flag to indicate whether a partial EDB was read
	unsigned int 				partial;
	TAILQ_ENTRY(finger_extended_data)	list;
	struct finger_extended_data_block	*fedb;	// back pointer to
							// parent FEDB
};
typedef struct finger_extended_data FED;

#if defined WINDOWS || defined WIN32
#define COPY_FED(src, dst)				\
	memcpy(&dst->fed_startcopy, &src->fed_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fed_endcopy -	\
		(uint8_t *)&dst->fed_startcopy))
#else
#define COPY_FED(src, dst)				\
	bcopy(&src->fed_startcopy, &dst->fed_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fed_endcopy -	\
		(uint8_t *)&dst->fed_startcopy))
#endif

// Representation of the entire finger view extended data block, which
// is comprised of multiple extended data items
#define FEDB_HEADER_LENGTH		2
struct finger_extended_data_block {
	unsigned int					format_std;
#define fedb_startcopy					block_length
	unsigned short					block_length;
#define fedb_endcopy					partial
	// Flag to indicate whether a partial EDB was read
	unsigned int 					partial;
	TAILQ_HEAD(, finger_extended_data)		extended_data;
	struct finger_view_minutiae_record		*fvmr;
};
typedef struct finger_extended_data_block FEDB;

#if defined WINDOWS || defined WIN32
#define COPY_FEDB(src, dst)					\
	memcpy(&dst->fedb_startcopy, &src->fedb_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fedb_endcopy -		\
		(uint8_t *)&dst->fedb_startcopy))
#else
#define COPY_FEDB(src, dst)					\
	bcopy(&src->fedb_startcopy, &dst->fedb_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fedb_endcopy -		\
		(uint8_t *)&dst->fedb_startcopy))
#endif
// Representation of the Finger View Minutiae Record combined with the 
// optional Extended Data
#define FVMR_HEADER_LENGTH	4

// XXX The field names of this struct should be prefixed with fvmr_
struct finger_view_minutiae_record {
	unsigned int				format_std;
#define fvmr_startcopy				finger_number
	unsigned char				finger_number;
	unsigned char				view_number;
	unsigned char				impression_type;
	unsigned char				finger_quality;
	unsigned char				number_of_minutiae;
	/* The next five fields are ANSI '07 only */
	unsigned short				x_image_size;
	unsigned short				y_image_size;
	unsigned short				x_resolution;
	unsigned short				y_resolution;
	unsigned int				algorithm_id;
#define fvmr_endcopy				partial
	// Flag to indicate a partial FVMR was read
	unsigned int 				partial;
	TAILQ_HEAD(, finger_minutiae_data)	minutiae_data;
	struct finger_extended_data_block	*extended;	// optional
	TAILQ_ENTRY(finger_view_minutiae_record) list;
	// The remaining fields of this record type are meta-data
	struct finger_minutiae_record		*fmr;	// back pointer to 
							// parent record
};
typedef struct finger_view_minutiae_record FVMR;

#if defined WINDOWS || defined WIN32
#define COPY_FVMR(src, dst)					\
	memcpy(&dst->fvmr_startcopy, &src->fvmr_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fvmr_endcopy -		\
		(uint8_t *)&dst->fvmr_startcopy))
#else
#define COPY_FVMR(src, dst)					\
	bcopy(&src->fvmr_startcopy, &dst->fvmr_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fvmr_endcopy -		\
		(uint8_t *)&dst->fvmr_startcopy))
#endif

// Representation of an entire Finger Minutiae Record
#define	FMR_ANSI_SMALL_HEADER_TYPE		1
#define	FMR_ANSI_LARGE_HEADER_TYPE		2
#define	FMR_ISO_HEADER_TYPE			3
#define	FMR_ANSI07_HEADER_TYPE			4

#define FMR_ANSI_SMALL_HEADER_LENGTH		26
#define FMR_ANSI_LARGE_HEADER_LENGTH		30
#define FMR_ANSI_MIN_RECORD_LENGTH		FMR_ANSI_SMALL_HEADER_LENGTH
// The max length that will fit in the first length field of the ANSI header
#define FMR_ANSI_MAX_SHORT_LENGTH		65535

#define FMR_ISO_HEADER_LENGTH			24
#define FMR_ISO_MIN_RECORD_LENGTH		FMR_ISO_HEADER_LENGTH

#define FMR_ANSI07_HEADER_LENGTH		20
#define FMR_ANSI07_MIN_RECORD_LENGTH		20

// XXX The field names of this struct should be prefixed with fmr_
struct finger_minutiae_record {
	// Representation of the FMR header
	unsigned int				format_std;
#define fmr_startcopy				format_id
	char					format_id[4];
	char					spec_version[4];
	unsigned int				record_length;
	unsigned int				record_length_type;
	// used to identify the size of the record_length field 
	unsigned short				product_identifier_owner;
	unsigned short				product_identifier_type;
	unsigned short				scanner_id;
	unsigned short				compliance;
	unsigned short				x_image_size;
	unsigned short				y_image_size;
	unsigned short				x_resolution;
	unsigned short				y_resolution;
	unsigned char				num_views;
	unsigned char				reserved;
#define fmr_endcopy				finger_views
	// Collection of Finger View records
	TAILQ_HEAD(, finger_view_minutiae_record)	finger_views;
	// The remaining fields of this record type are meta-data
	// Keep the next expected minimum view number; this is used during 
	// record validation.
	unsigned char				next_min_view[FMR_NUM_FINGER_CODES];
};
typedef struct finger_minutiae_record FMR;

#if defined WINDOWS || defined WIN32
#define COPY_FMR(src, dst)				\
	memcpy(&dst->fmr_startcopy, &src->fmr_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fmr_endcopy -	\
		(uint8_t *)&dst->fmr_startcopy))
#else
#define COPY_FMR(src, dst)				\
	bcopy(&src->fmr_startcopy, &dst->fmr_startcopy,	\
            (unsigned) ((uint8_t *)&dst->fmr_endcopy -	\
		(uint8_t *)&dst->fmr_startcopy))
#endif

/******************************************************************************/
/* Define the interface for managing the various pieces of a Finger Minutiae  */
/* Record.                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Allocate and initialize storage for a new Finger Minutiae Record.          */
/* The header record will be initialized to 'NULL' values, and the finger     */
/* view list will be initialized to empty.                                    */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fmr        Address of the pointer to the FMR that will be allocated.     */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fmr(unsigned int format_std, struct finger_minutiae_record **fmr);

/******************************************************************************/
/* Free the storage for a Finger Minutiae Record.                             */
/* This function does a "deep free", meaning that all storage allocated to    */
/* records on lists associated with this FMR are free'd.                      */
/*                                                                            */
/* Parameters:                                                                */
/*   fmr    Pointer to the FMR structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fmr(struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Add a Finger View record to the Finger Minutiae Record.                    */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr   Pointer to the Finger View Minutiae Record that will be added.    */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/*                                                                            */
/******************************************************************************/
void
add_fvmr_to_fmr(struct finger_view_minutiae_record *fvmr, 
		struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Allocate and initialize storage for a single Finger View Record, including */
/* the Finger View Minutiae Record and Extended Data pointer.                 */
/* The record will be initialized to 'NULL' values, and the Finger View       */
/* Minutiae Record data list list will be initialized to empty.               */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fvmr   Address of the pointer to the FVMR structure that will be         */
/*          allocated.                                                        */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fvmr(unsigned int format_std, struct finger_view_minutiae_record **fvmr);

/******************************************************************************/
/* Free the storage for a single Finger View Record.                          */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Finger View will also be free'd.             */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr   Pointer to the FV  structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fvmr(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Add a Finger Minutia Data record to the Finger View Minutia Record.        */
/*                                                                            */
/* Parameters:                                                                */
/*   fmd    Pointer to the Finger Minutiae Data record that will be added.    */
/*   fv     Pointer to the Finger View record.                                */
/*                                                                            */
/******************************************************************************/
void
add_fmd_to_fvmr(struct finger_minutiae_data *fmd, 
		struct finger_view_minutiae_record *fv);

/******************************************************************************/
/* Add a Finger Extended Data Block to the Finger View Minutia Record.        */
/* Only one FEDB can be attached to the FVMR, unlike most other associations  */
/* in the FMR data structures, where multiple records can be associated       */
/* with a single block.                                                       */
/*                                                                            */
/* Parameters:                                                                */
/*   fmd    Pointer to the Finger Minutiae Data record that will be added.    */
/*   fv     Pointer to the Finger View record.                                */
/*                                                                            */
/******************************************************************************/
void
add_fedb_to_fvmr(struct finger_extended_data_block *fedb, 
		 struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Allocate and initialize storage for a single Finger Minutiae Data Record.  */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fmd    Address of the pointer to the FV structure that will be allocated.*/
/*   index  Index number of the minutiae (position within the record)         */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fmd(unsigned int format_std, struct finger_minutiae_data **fmd,
    unsigned int index);

/******************************************************************************/
/* Free the storage for a single Finger Minutiae Data Record.                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fmd    Pointer to the FMD structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fmd(struct finger_minutiae_data *fmd);

/******************************************************************************/
/* Find the center of mass for a set of finger minutiae data records.         */
/*                                                                            */
/* Parameters:                                                                */
/*   fmds   Pointer to the an array of FMDs.                                  */
/*   mcount The number of minutiae in the input array.                        */
/*   x      Pointer to the X coordiniate of center, set on return             */
/*   y      Pointer to the Y coordiniate of center, set on return             */
/*                                                                            */
/******************************************************************************/
void
find_center_of_minutiae_mass(FMD **fmds, int mcount, int *x, int *y);

/******************************************************************************/
/* Allocate and initialize storage for a single Finger Extended Data Block.   */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fedb   Address of the pointer to the Extended Data block that will       */
/*          be allocated.                                                     */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fedb(unsigned int format_std, struct finger_extended_data_block **fedb);

/******************************************************************************/
/* Free the storage for a single Finger Extended Data Block.                  */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Extended Data block will also be free'd.     */
/*                                                                            */
/* Parameters:                                                                */
/*   fedb   Pointer to the Extended Data block structure that will be free'd. */
/*                                                                            */
/******************************************************************************/
void
free_fedb(struct finger_extended_data_block *fedb);

/******************************************************************************/
/* Add a Finger Extended Data record to the Extended Data Block.              */
/*                                                                            */
/* Parameters:                                                                */
/*   fed    Pointer to the Finger Extended Data record that will be added.    */
/*   fedb   Pointer to the Finger Extended Data Block.                        */
/*                                                                            */
/******************************************************************************/
void
add_fed_to_fedb(struct finger_extended_data *fed, 
		struct finger_extended_data_block *fedb);

/******************************************************************************/
/* Allocate and initialize storage for a single Finger Extended Data record.  */
/* This function will also allocate the structure to hold the specific type   */
/* of extended data. For example, if type_id is FED_CORE_AND_DELTA, a         */
/* CDDB structure will be allocated to hold the new core and delta records.   */
/* Therefore, there is no need to call new_rcdb() or new_cddb() after calling */
/* this function.                                                             */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fed       Address of the pointer to the Extended Data record that will   */
/*             be allocated.                                                  */
/*   type_id   Type of extended data.                                         */
/*   length    The length of the extended data.                               */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fed(unsigned int format_std, struct finger_extended_data **fed,
	unsigned short type_id, unsigned short length);

/******************************************************************************/
/* Free the storage for a single Finger Extended Data record.                 */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Extended Data record will also be free'd.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fed    Pointer to the Extended Data record that will be free'd.          */
/*                                                                            */
/******************************************************************************/
void
free_fed(struct finger_extended_data *fed);

/******************************************************************************/
/* Allocate and initialize storage for a Ridge Count Data Block.              */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   rcdb   Address of the pointer to the Ridge Count Data block that will    */
/*          be allocated.                                                     */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int     
new_rcdb(struct ridge_count_data_block **rcdb);

/******************************************************************************/
/* Free the storage for a single Ridge Count Data Block.                      */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Ridge Count Data block will also be free'd.  */
/*                                                                            */
/* Parameters:                                                                */
/*   rcdb   Pointer to the Ridge Count Data block structure that will be      */
/*          free'd.                                                           */
/*                                                                            */
/******************************************************************************/
void    
free_rcdb(struct ridge_count_data_block *rcdb);

/******************************************************************************/
/* Add a Ridge Count Data record to the Ridge Count Data Block.               */
/*                                                                            */
/* Parameters:                                                                */
/*   rcd    Pointer to the Ridge Count Data record that will be added.        */
/*   rcdb   Pointer to the Ridge Count Data Block.                            */
/*                                                                            */
/******************************************************************************/
void
add_rcd_to_rcdb(struct ridge_count_data *rcd,
		struct ridge_count_data_block *rcdb);

/******************************************************************************/
/* Allocate the storage for a single Ridge Count Data record.                 */
/*                                                                            */
/* Parameters:                                                                */
/*   rcd    Address of the pointer to the Ridge Count Data structure that     */
/*          will be allocated.                                                */
/*                                                                            */
/******************************************************************************/
int
new_rcd(struct ridge_count_data **rcd);

/******************************************************************************/
/* Free the storage for a single Ridge Count Data record.                     */
/*                                                                            */
/* Parameters:                                                                */
/*   rcd    Pointer to the Ridge Count Data record that will be free'd.       */
/*                                                                            */
/******************************************************************************/
void
free_rcd(struct ridge_count_data *rcd);

/******************************************************************************/
/* Allocate and initialize storage for a Core and Delta Data Block.           */
/* The record will be initialized to 'NULL' values.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   cddb   Address of the pointer to the Core and Delta Data Block that      */
/*          will be allocated.                                                */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_cddb(unsigned int format_std, struct core_delta_data_block **cddb);

/******************************************************************************/
/* Free the storage for a single Core and Delta Data Block.                   */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Core Data Block will also be free'd.         */
/*                                                                            */
/* Parameters:                                                                */
/*   cddb    Pointer to the Core and Delta Data block structure that will     */
/*           be freed.                                                        */
/*                                                                            */
/******************************************************************************/
void    
free_cddb(struct core_delta_data_block *cddb);

/******************************************************************************/
/* Add a Core Data record to a Core and Delta Data Block.                     */
/*                                                                            */
/* Parameters:                                                                */
/*   cd     Pointer to the Core Data record that will be added.               */
/*   cddb   Pointer to the Core and Delta Data Block.                         */
/*                                                                            */
/******************************************************************************/
void
add_cd_to_cddb(struct core_data *cd, struct core_delta_data_block *cdb);

/******************************************************************************/
/* Add a Delta Data record to the Core and Delta Data Block.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   dd     Pointer to the Delta Data record that will be added.              */
/*   ddb    Pointer to the Delta Data Block.                                  */
/*                                                                            */
/******************************************************************************/
void
add_dd_to_cddb(struct delta_data *dd, struct core_delta_data_block *ddb);

/******************************************************************************/
/* Allocate the storage for a single Core Data record.                        */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   cd     Address of the pointer to the Core Data structure that will be    */
/*          allocated.                                                        */
/*                                                                            */
/******************************************************************************/
int
new_cd(unsigned int format_std, struct core_data **cd);

/******************************************************************************/
/* Free the storage for a single Core Data record.                            */
/*                                                                            */
/* Parameters:                                                                */
/*   cd     Pointer to the Core Data record that will be free'd.              */
/*                                                                            */
/******************************************************************************/
void
free_cd(struct core_data *cd);

/******************************************************************************/
/* Allocate the storage for a single Delta Data record.                       */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   dd     Address of the pointer to the Delta Data structure that will be   */
/*          allocated.                                                        */
/*                                                                            */
/******************************************************************************/
int
new_dd(unsigned int format_std, struct delta_data **dd);

/******************************************************************************/
/* Free the storage for a single Delta Data record.                           */
/*                                                                            */
/* Parameters:                                                                */
/*   dd     Pointer to the Delta Data record that will be free'd.             */
/*                                                                            */
/******************************************************************************/
void
free_dd(struct delta_data *dd);

/******************************************************************************/
/* Define the interface for reading and writing Finger Minutiae Records       */
/******************************************************************************/

/******************************************************************************/
/* Read a complete Finger Minutiae Record from a file, or buffer, filling in  */
/* the fields of the header record, including all of the Finger Views.        */
/* This function does not do any validation of the data being read.           */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fmr    Pointer to the FMR.                                               */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fmr(FILE *fp, struct finger_minutiae_record *fmr);

int
scan_fmr(BDB *fmdb, struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Write a Finger Minutiae Record to a file or memory buffer.                 */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fmr(FILE *fp, struct finger_minutiae_record *fmr);

int
push_fmr(BDB *fmdb, struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Print an entire finger Minutiae Record to a file in human-readable form.   */
/* This function does not validate the record.                                */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fmr(FILE *fp, struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Validate a Finger Minutiae Record by checking the conformance of the       *//* header and all of the Finger Views to the ANSI/INCITS 378-2004             */
/* specification. Diagnostic messages are written to stderr.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fmr(struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Define the interface for reading and writing Finger View Minutiae records  */
/******************************************************************************/

/******************************************************************************/
/* Read a single Finger View Minutiae Record from a file or memory buffer.    */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fvmr   Pointer to the Finger View Minutiae Record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr);

int
scan_fvmr(BDB *fmdb, struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Write a single Finger View Minutiae Record to a file or buffer.            */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fvmr   Pointer to the Finger View Minutiae Record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr);

int
push_fvmr(BDB *fmdb, struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Print a FVMR to a file in human-readable form.                             */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fvmr   Pointer to the FVMR                                               */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fvmr(FILE *fp, struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Validate a Finger View Minutiae Record by checking the conformance of the  */
/* minutiae record to the ANSI/INCITS 378-2004 specification.                 */
/* Diagnostic messages are written to stderr.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr   Pointer to the Finger View Minutiae Record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fvmr(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Add a Finger Minutiae Data record to a Finger View Minutiae Record.        */
/*                                                                            */
/* Parameters:                                                                */
/*   fmd    Pointer to the Finger Minutiae Data record.                       */
/*   fvmr   Pointer to the Finger View Minutiae Record.                       */
/*                                                                            */
/******************************************************************************/
void
add_fmd_to_fvmr(struct finger_minutiae_data *fmd,
		struct finger_view_minutiae_record *fvmr);


/******************************************************************************/
/* Define the interface for reading and writing Finger Minutiae Data records  */
/******************************************************************************/

/******************************************************************************/
/* Read a single Finger Minutiae Data record from a file or buffer.           */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fmd    Pointer to the Finger Minutiae Data record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fmd(FILE *fp, struct finger_minutiae_data *fmd);

int
scan_fmd(BDB *fmdb, struct finger_minutiae_data *fmd);

/******************************************************************************/
/* Write a single Finger Minutiae Data record to a file or buffer.            */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fmd    Pointer to the Finger Minutiae Data record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fmd(FILE *fp, struct finger_minutiae_data *fmd);

int
push_fmd(BDB *fmdb, struct finger_minutiae_data *fmd);

/******************************************************************************/
/* Print a single Finger Minutiae Data record to a file in human-readable     */
/* form.                                                                      */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmd    Pointer to the Finger Minutiae Data record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fmd(FILE *fp, struct finger_minutiae_data *fmd);

/******************************************************************************/
/* Validate a Finger Minutiae Data record by checking the conformance of the  */
/* minutiae data to the ANSI/INCITS 378-2004 specification.                   */
/* Diagnostic messages are written to stderr.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fmd    Pointer to the Finger Minutiae Data record.                       */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fmd(struct finger_minutiae_data *fmd);

/******************************************************************************/
/* Define the interface for reading and writing finger Extended Data records  */
/******************************************************************************/

/******************************************************************************/
/* Read an entire Extended Data Block from a file or memory buffer.           */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fedb   Pointer to the Extended Data block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fedb(FILE *fp, struct finger_extended_data_block *fedb);

int
scan_fedb(BDB *fmdb, struct finger_extended_data_block *fedb);

/******************************************************************************/
/* Write an entire Extended Data Block to a file or buffer.                   */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fedb   Pointer to the Extended Data block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fedb(FILE *fp, struct finger_extended_data_block *fed);

int
push_fedb(BDB *fmdb, struct finger_extended_data_block *fed);

/******************************************************************************/
/* Print an entire Extended Data Block to a file in human-readable form.      */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fedb   Pointer to the Extended Data block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fedb(FILE *fp, struct finger_extended_data_block *fed);

/******************************************************************************/
/* Validate a entire Extended Data Block by checking the conformance of the   */
/* set of extended data records to the ANSI/INCITS 378-2004 specification.    */
/* Diagnostic messages are written to stderr.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fedb   Pointer to the Extended Data block.                               */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fedb(struct finger_extended_data_block *fedb);

/******************************************************************************/
/* Read a single Extended Data record from a file or a memory buffer.         */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fed    Pointer to the Extended Data record.                              */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fed(FILE *fp, struct finger_extended_data *fed);

int
scan_fed(BDB *fmdb, struct finger_extended_data *fed);

/******************************************************************************/
/* Write a single Extended Data record to a file or buffer.                   */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   fed    Pointer to the Extended Data record.                              */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fed(FILE *fp, struct finger_extended_data *fed);

int
push_fed(BDB *fmdb, struct finger_extended_data *fed);

/******************************************************************************/
/* Print a single Extended Data record to a file in human-readable form.      */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fed    Pointer to the Extended Data record.                              */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fed(FILE *fp, struct finger_extended_data *fed);

/******************************************************************************/
/* Validate a single Extended Data record by checking the conformance of the  */
/* minutiae data to the ANSI/INCITS 378-2004 specification.                   */
/* Diagnostic messages are written to stderr.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fed    Pointer to the Extended Data record.                              */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fed(struct finger_extended_data *fed);

/******************************************************************************/
/* Define the interface for Ridge Count Data Block records.                   */
/******************************************************************************/
/******************************************************************************/
/* Functions to read an entire Ridge Count Data Block and a single Ridge      */
/* Count Data record from a file or memory buffer.                            */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   rcdb   Pointer to the Ridge Count Data Block record.                     */
/*   rcd    Pointer to the Ridge Count Data record.                           */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_rcdb(FILE *fp, struct ridge_count_data_block *rcdb);

int
scan_rcdb(BDB *fmdb, struct ridge_count_data_block *rcdb);

int
read_rcd(FILE *fp, struct ridge_count_data *rcd);

int
scan_rcd(BDB *fmdb, struct ridge_count_data *rcd);

/******************************************************************************/
/* Functions to write an entire Ridge Count Data Block, and a single Ridge    */
/* Count Data record to a file or buffer.                                     */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   rcdb   Pointer to the Ridge Count Data Block record.                     */
/*   rcd    Pointer to the Ridge Count Data record.                           */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_rcdb(FILE *fp, struct ridge_count_data_block *rcdb);

int
push_rcdb(BDB *fmdb, struct ridge_count_data_block *rcdb);

int
write_rcd(FILE *fp, struct ridge_count_data *rcd);

int
push_rcd(BDB *fmdb, struct ridge_count_data *rcd);

/******************************************************************************/
/* Functions to print an entire Ridge Count Data Block, and a single Ridge    */
/* Count Data record to a file in human-readable form.                        */
/*                                                                            */
/*   fp     The open file pointer.                                            */
/*   rcdb   Pointer to the Ridge Count Data Block record.                     */
/*   rcd    Pointer to the Ridge Count Data record.                           */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_rcdb(FILE *fp, struct ridge_count_data_block *rcdb);

int
print_rcd(FILE *fp, struct ridge_count_data *rcd);

/******************************************************************************/
/* Functions to validate an entire Ridge Count Data Block, and a single Ridge */
/* Count Data record. Validation of the RCDB include validation of all the    */
/* associated RCD records.                                                    */
/*                                                                            */
/* Parameters:                                                                */
/*   rcdb   Pointer to the Ridge Count Data Block record.                     */
/*   rcd    Pointer to the Ridge Count Data record.                           */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_rcdb(struct ridge_count_data_block *rcdb);

int
validate_rcd(struct ridge_count_data *rcd);

/******************************************************************************/
/* Define the interface for Core and Delta Data Format records.               */
/******************************************************************************/
/******************************************************************************/
/* Functions to read an entire Core and Delta Data Block, a single Core Data  */
/* record, and a single Delta Data Record from a file.                        */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp          The open file pointer.                                       */
/*   fmdb        Pointer to the biometric data block containing minutiae data.*/
/*   cddb        Pointer to the Core and Delta Data Block record.             */
/*   cd          Pointer to the Core Data record.                             */
/*   core_type   The type of the Core Data.                                   */
/*   dd          Pointer to the Delta Data record.                            */
/*   delta_type   The type of the Delta Data.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_cddb(FILE *fp, struct core_delta_data_block *cddb);

int
scan_cddb(BDB *fmdb, struct core_delta_data_block *cddb);

int
read_cd(FILE *fp, struct core_data *cd, unsigned char core_type);

int
scan_cd(BDB *fmdb, struct core_data *cd, unsigned char core_type);

int
read_dd(FILE *fp, struct delta_data *dd, unsigned char delta_type);

int
scan_dd(BDB *fmdb, struct delta_data *dd, unsigned char delta_type);

/******************************************************************************/
/* Functions to write an entire Core Data Block, a single Core Data record,   */
/* and a single Delta Data record to a file or buffer.                        */
/* Fields within the FILE and BDB structs are modified by these functions.    */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fmdb   Pointer to the biometric data block containing minutiae data.     */
/*   cddb   Pointer to the Core and Delta Data Block record.                  */
/*   cd     Pointer to the Core Data record.                                  */
/*   dd     Pointer to the Delta Data record.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_cddb(FILE *fp, struct core_delta_data_block *cddb);

int
push_cddb(BDB *fmdb, struct core_delta_data_block *cddb);

int
write_cd(FILE *fp, struct core_data *cd);

int
push_cd(BDB *fmdb, struct core_data *cd);

int
write_dd(FILE *fp, struct delta_data *dd);

int
push_dd(BDB *fmdb, struct delta_data *dd);

/******************************************************************************/
/* Functions to print an entire Core Data Block, and a single Core Data       */
/* record to a file in human-readable form.                                   */
/*                                                                            */
/*   fp     The open file pointer.                                            */
/*   cddb   Pointer to the Core and Delta Data Block record.                  */
/*   cd     Pointer to the Core Data record.                                  */
/*   dd     Pointer to the Delta Data record.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_cddb(FILE *fp, struct core_delta_data_block *cddb);

int
print_cd(FILE *fp, struct core_data *cd);

int
print_dd(FILE *fp, struct delta_data *dd);

/******************************************************************************/
/* Functions to validate an entire Core Data Block, and a single Core Data    */
/* record. Validation of the CDB include validation of all the CD records.    */
/*                                                                            */
/* Parameters:                                                                */
/*   cddb   Pointer to the Core and Delta Data Block record.                  */
/*   cd     Pointer to the Core Data record.                                  */
/*   dd     Pointer to the Delta Data record.                                 */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_cddb(struct core_delta_data_block *cddb);

int
validate_cd(struct core_data *cd);

int
validate_dd(struct delta_data *dd);

/******************************************************************************/
/* The next set of functions operate at a more abstract level. These function */
/* are to be used to retrieve aggregrate data from the FMR, FVMR, etc.        */
/* For any of the functions that return arrays of data, or other aggregate    */
/* data blocks, the memory for the block must be allocated by the caller.     */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Return the count of FVMRs contained within a FMR.                          */
/*                                                                            */
/* Parameters:                                                                */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/* Returns:                                                                   */
/*   Count of FVMR records.                                                   */
/******************************************************************************/
int
get_fvmr_count(struct finger_minutiae_record *fmr);

/******************************************************************************/
/* Fill an array of pointers to the FVMR records that are part of an FMR.     */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fmr    Pointer to the Finger Minutiae Record.                            */
/*   fvmrs  Address of the array that will be filled with FVMR pointers.      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of FVMR records, -1 if error.                                      */
/******************************************************************************/
int
get_fvmrs(struct finger_minutiae_record *fmr,
	  struct finger_view_minutiae_record *fvmrs[]);

/******************************************************************************/
/* Return the count of FMDs contained within a FVMR.                          */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/* Returns:                                                                   */
/*   Count of FMD records.                                                    */
/******************************************************************************/
int
get_fmd_count(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Fill an array of pointers to the FMD records that are part of an FVMR.     */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*   fmds    Address of the array that will be filled with FMD pointers.      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of FMD records, -1 if error.                                       */
/******************************************************************************/
int
get_fmds(struct finger_view_minutiae_record *fvmr,
         struct finger_minutiae_data *fmds[]);

/******************************************************************************/
/* Return the count of Ridge Count Data records contained in a FVMR.          */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of RCD records.                                                    */
/******************************************************************************/
int
get_rcd_count(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Fill an array of pointers to the RCD records that are part of a FVMR.      */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*   rcds   Address of the array that will be filled with RCD pointers.       */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of RCD records, -1 if error.                                       */
/******************************************************************************/
int
get_rcds(struct finger_view_minutiae_record *fvmr,
	 struct ridge_count_data *rcds[]);

/******************************************************************************/
/* Return the count of Core Data records contained in a FVMR.                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of Core Data records.                                              */
/******************************************************************************/
int
get_core_count(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Fill an array of pointers to the Core Data records that are part of        */
/* a FVMR.                                                                    */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*   cores  Address of the array that will be filled with Core Data pointers. */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of Core Data records, -1 if error.                                 */
/******************************************************************************/
int
get_cores(struct finger_view_minutiae_record *fvmr,
	  struct core_data *cores[]);

/******************************************************************************/
/* Return the count of Delta Data records contained in a FVMR.                */
/*                                                                            */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of Delta Data records.                                             */
/******************************************************************************/
int
get_delta_count(struct finger_view_minutiae_record *fvmr);

/******************************************************************************/
/* Fill an array of pointers to the Delta Data records that are part of       */
/* a FVMR.                                                                    */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fvmr    Pointer to the Finger View Minutiae Record.                      */
/*   deltas Address of the array that will be filled with Delta Data pointers.*/
/*                                                                            */
/* Returns:                                                                   */
/*   Count of Delta Data records, -1 if error.                                */
/******************************************************************************/
int
get_deltas(struct finger_view_minutiae_record *fvmr,
           struct delta_data *deltas[]);

#endif /* !_FMR_H */
