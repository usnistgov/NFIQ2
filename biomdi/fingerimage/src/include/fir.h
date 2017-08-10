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
/* Header file for the Finger Image Record format, as specified in            */
/* ANSI/INCITS 381-2004.                                                      */
/*                                                                            */
/*                                                                            */
/* Expected layout of the entire finger minutiae record in memory:            */
/*                                                                            */
/*         FIR                                                                */
/*   +--------------------------------------+                                 */
/*   |   general record header (36 bytes)   |                                 */
/*   +--------------------------------------+                                 */
/*   |                                                                        */
/*   |                                                                        */
/*   +------------------------|     +--------+--------+                       */
/*   |   single finger record |-->  | view 1 | view 2 |                       */
/*   +------------------------|     +--------+--------+                       */
/*   |   single finger record |                                               */
/*   +------------------------|                                               */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef _FIR_H
#define _FIR_H

// Stupid
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define FIR_STD_ANSI	1
#define FIR_STD_ISO	2

// The identifier that must appear at the beginning of the record header
#define FIR_FORMAT_ID 		"FIR"
#define FIR_FORMAT_ID_LEN 	4

// The version number
#define FIR_SPEC_VERSION	"010"
#define FIR_SPEC_VERSION_LEN	4

// Representation of a single finger image view record; includes the
// finger image header record and the image data.
#define FIVR_HEADER_LENGTH	14
#define FIR_MIN_VIEW_COUNT	1
#define FIR_MAX_VIEW_COUNT	255
struct finger_image_view_record {
#define fivr_startcopy
	unsigned int				length;
	unsigned char				finger_palm_position;
	unsigned char				count_of_views;
	unsigned char				view_number;
	unsigned char				quality;
	unsigned char				impression_type;
	unsigned short				horizontal_line_length; 
						/* aka width in pixels */
	unsigned short				vertical_line_length;	
						/* aka height in pixels */ 
	unsigned char				reserved;

	// Pointer to the image data that is read from the record
	char					*image_data;
#define fivr_endcopy
	TAILQ_ENTRY(finger_image_view_record)	list;
	struct finger_image_record		*fir;	// back pointer to the
							// parent record
};
typedef struct finger_image_view_record FIVR;

// Representation of an entire Finger Image General Record Header
#define FIR_ANSI_HEADER_LENGTH	36
#define FIR_ISO_HEADER_LENGTH	32
#define FIR_RECORD_LENGTH_LEN	6
#define FIR_SCALE_UNITS_INCH	1
#define FIR_SCALE_UNITS_CM	2
#define FIR_MAX_SCAN_RESOLUTION	1000
#define FIR_MIN_PIXEL_DEPTH		1
#define FIR_MAX_PIXEL_DEPTH		16

struct finger_image_record {
	unsigned int				format_std;
#define fir_startcopy
	// Representation of the FIR header
	char					format_id[4];
	char					spec_version[4];
	unsigned long long			record_length;
	unsigned short				product_identifier_owner;
	unsigned short				product_identifier_type;
	unsigned short				scanner_id;
	unsigned short				compliance;
	unsigned short				image_acquisition_level;
	unsigned char				num_fingers_or_palm_images;
	unsigned char				scale_units;
	unsigned short				x_scan_resolution;
	unsigned short				y_scan_resolution;
	unsigned short				x_image_resolution;
	unsigned short				y_image_resolution;
	unsigned char				pixel_depth;
	unsigned char				image_compression_algorithm;
	unsigned short				reserved;
#define fir_endcopy
	// Collection of Finger View records
	TAILQ_HEAD(, finger_image_view_record)	finger_views;
};
/*
 * Record length must be at least as long as the genral header
 * plus the length of one view header.
 */
#define FIR_MIN_RECORD_LENGTH (FIR_HEADER_LENGTH + FIVR_HEADER_LENGTH)

typedef struct finger_image_record FIR;

/******************************************************************************/
/* Define the interface for managing the various pieces of a Finger Image     */
/* Record.                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Allocate and initialize storage for a new Finger Image Record.             */
/* The header record will be initialized to 'NULL' values, and the finger     */
/* view list will be initialized to empty.                                    */
/*                                                                            */
/* Parameters:                                                                */
/*   format_std The standard for record (ANSI, ISO, etc.)                     */
/*   fir    Address of the pointer to the FIR that will be allocated.         */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fir(unsigned int format_std, struct finger_image_record **fir);

/******************************************************************************/
/* Free the storage for a Finger Image Record.                                */
/* This function does a "deep free", meaning that all storage allocated to    */
/* records on lists associated with this FIR are free'd.                      */
/*                                                                            */
/* Parameters:                                                                */
/*   fir    Pointer to the FIR structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fir(struct finger_image_record *fir);

/******************************************************************************/
/* Add a Finger Image View record to the Finger Image Record.                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fivr   Pointer to the Finger View Image Record that will be added.       */
/*   fir    Pointer to the Finger Image Record.                               */
/*                                                                            */
/******************************************************************************/
void
add_fivr_to_fir(struct finger_image_view_record *fivr, 
		struct finger_image_record *fir);

/******************************************************************************/
/* Allocate and initialize storage for a single Finger Image View Record      */
/* The record will be initialized to 'NULL' values, and the image data list   */
/* will be initialized to empty.                                              */
/*                                                                            */
/* Parameters:                                                                */
/*   fivr   Address of the pointer to the FIVR structure that will be         */
/*          allocated.                                                        */
/*                                                                            */
/* Returns:                                                                   */
/*   0      Success                                                           */
/*  -1      Failure                                                           */
/*                                                                            */
/******************************************************************************/
int
new_fivr(struct finger_image_view_record **fivr);

/******************************************************************************/
/* Free the storage for a single Finger Image View Record.                    */
/* This function does a "deep free", meaning that all memory allocated for    */
/* any lists associated with the Finger Image View will also be free'd.       */
/*                                                                            */
/* Parameters:                                                                */
/*   fivr   Pointer to the FV  structure that will be free'd.                 */
/*                                                                            */
/******************************************************************************/
void
free_fivr(struct finger_image_view_record *fivr);

/******************************************************************************/
/* Add a Finger Image Data record to the Finger Image View Record.            */
/*                                                                            */
/* Parameters:                                                                */
/*   image  Pointer to the image data.                                        */
/*   fivr   Pointer to the Finger Image View Record.                          */
/*                                                                            */
/******************************************************************************/
void
add_image_to_fivr(char *image, struct finger_image_view_record *fivr);

/******************************************************************************/
/* Read a complete Finger Image Record from a file, filling in the fields     */
/* of the header record, including all of the Finger Views.                   */
/* This function does not do any validation of the data being read.           */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fir    Pointer to the FIR.                                               */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fir(FILE *fp, struct finger_image_record *fir);

/******************************************************************************/
/* Write a Finger Image Record to a file.                                     */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fir    Pointer to the Finger Image Record.                               */
/*                                                                            */
/* Returns:                                                                   */
/*        WRITE_OK     Success                                                */
/*        WRITE_ERROR  Failure                                                */
/******************************************************************************/
int
write_fir(FILE *fp, struct finger_image_record *fir);

/******************************************************************************/
/* Print an entire finger Image Record to a file in human-readable form.      */
/* This function does not validate the record.                                */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fir    Pointer to the Finger Image Record.                               */
/*                                                                            */
/* Returns:                                                                   */
/*       PRINT_OK     Success                                                 */
/*       PRINT_ERROR  Failure                                                 */
/******************************************************************************/
int
print_fir(FILE *fp, struct finger_image_record *fir);

/******************************************************************************/
/* Validate a Finger Image Record by checking the conformance of the          */
/* header and all of the Finger Views to the ANSI/INCITS 381-2004             */
/* specification. Diagnostic messages are written to stderr.                  */
/*                                                                            */
/* Parameters:                                                                */
/*   fir    Pointer to the Finger Image Record.                               */
/*                                                                            */
/* Returns:                                                                   */
/*       VALIDATE_OK       Record does conform                                */
/*       VALIDATE_ERROR    Record does NOT conform                            */
/******************************************************************************/
int
validate_fir(struct finger_image_record *fir);

/******************************************************************************/
/* Define the interface for reading and writing Finger Image View records     */
/******************************************************************************/

/******************************************************************************/
/* Read a single Finger Image View Record from a file.                        */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fivr   Pointer to the Finger Image View Record.                          */
/*                                                                            */
/* Returns:                                                                   */
/*        READ_OK     Success                                                 */
/*        READ_EOF    End of file encountered                                 */
/*        READ_ERROR  Failure                                                 */
/******************************************************************************/
int
read_fivr(FILE *fp, struct finger_image_view_record *fivr);

/******************************************************************************/
/* Write a single Finger Image View Record to a file.                         */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fivr   Pointer to the Finger Image View Record.                          */
/*                                                                            */
/* Returns:                                                                   */
/*    WRITE_OK     Success                                                    */
/*    WRITE_ERROR  Failure                                                    */
/******************************************************************************/
int
write_fivr(FILE *fp, struct finger_image_view_record *fivr);

/******************************************************************************/
/* Print a FIVR to a file in human-readable form.                             */
/*                                                                            */
/* Parameters:                                                                */
/*   fp     The open file pointer.                                            */
/*   fivr   Pointer to the FIVR                                               */
/*                                                                            */
/* Returns:                                                                   */
/*   PRINT_OK     Success                                                     */
/*   PRINT_ERROR  Failure                                                     */
/******************************************************************************/
int
print_fivr(FILE *fp, struct finger_image_view_record *fivr);

/******************************************************************************/
/* Validate a Finger Image View Record by checking the conformance of the     */
/* minutiae record to the ANSI/INCITS 381-2004 specification.                 */
/* Diagnostic messages are written to stderr.                                 */
/*                                                                            */
/* Parameters:                                                                */
/*   fivr   Pointer to the Finger Image Image Record.                          */
/*                                                                            */
/* Returns:                                                                   */
/*   VALIDATE_OK       Record does conform                                    */
/*   VALIDATE_ERROR    Record does NOT conform                                */
/******************************************************************************/
int
validate_fivr(struct finger_image_view_record *fivr);

/******************************************************************************/
/* The next set of functions operate at a more abstract level. These function */
/* are to be used to retrieve aggregrate data from the FIR, FIVR, etc.        */
/* For any of the functions that return arrays of data, or other aggregate    */
/* data blocks, the memory for the block must be allocated by the caller.     */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Return the count of FIVRs contained within a FIR.                          */
/*                                                                            */
/* Parameters:                                                                */
/*   fir    Pointer to the Finger Image Record.                               */
/* Returns:                                                                   */
/*   Count of FIVR records.                                                   */
/******************************************************************************/
int
get_fivr_count(struct finger_image_record *fir);

/******************************************************************************/
/* Fill an array of pointers to the FIVR records that are part of an FIR.     */
/* The memory for the array must be allocated prior to calling this function. */
/* Parameters:                                                                */
/*   fir    Pointer to the Finger Image Record.                               */
/*   fivrs  Address of the array that will be filled with FIVR pointers.      */
/*                                                                            */
/* Returns:                                                                   */
/*   Count of FIVR records, -1 if error.                                      */
/******************************************************************************/
int
get_fivrs(struct finger_image_record *fir,
	  struct finger_image_view_record *fivrs[]);

/******************************************************************************/
/* Copy a FIVR. Copying a FIVR must be done with routine so the internal      */
/* record keeping in the FIVR will not be disturbed.                          */
/* Parameters:                                                                */
/*   src    Pointer to the source FIVR.                                       */
/*   dst    Pointer to the destination FIVR.                                  */
/*                                                                            */
/******************************************************************************/
void
copy_fivr(struct finger_image_view_record *src,
    struct finger_image_view_record *dst);

/******************************************************************************/
/* Map a standard name represented as a string to a numeric value.            */
/* Parameters:                                                                */
/*     stdstr String with name of standard ("ANSI", "ISO", etc.)              */
/* Return:                                                                    */
/*   One of the standard types defined in fir.h, or -1 on error.              */
/******************************************************************************/
int
fir_stdstr_to_type(char *stdstr);

#endif /* !_FIR_H */
