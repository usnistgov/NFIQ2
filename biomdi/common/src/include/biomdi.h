/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef _BIOMDI_H
#define _BIOMDI_H

/*
 * Declare a type, and a function, that will enable the creation of sets of
 * data items as simple arrays, and check for membership. These sets can be
 * used to manage the set of valid values for fields in the biometric data
 * record.
 */

#define MAX_INT_SET_SIZE	32
struct intSet {
	unsigned int is_size;
	uint32_t is_values[MAX_INT_SET_SIZE];
};
typedef struct intSet biomdiIntSet;

int inIntSet(biomdiIntSet S, uint32_t val);

// Header CBEFF ID fields
#define HDR_PROD_ID_OWNER_MASK	0xFFFF0000
#define HDR_PROD_ID_OWNER_SHIFT	16
#define HDR_PROD_ID_TYPE_MASK	0x0000FFFF

// Define the bit layout of the capture equipment info
#define HDR_COMPLIANCE_MASK	0xF000
#define HDR_COMPLIANCE_SHIFT	12
#define HDR_SCANNER_ID_MASK	0x0FFF
#define HDR_APPENDIX_F_MASK	0x0008  // After shifting the compliance field

// Finger position codes
#define MIN_FINGER_CODE		0
#define UNKNOWN_FINGER		0
#define RIGHT_THUMB		1
#define RIGHT_INDEX		2
#define RIGHT_MIDDLE		3
#define RIGHT_RING		4
#define RIGHT_LITTLE		5
#define LEFT_THUMB		6
#define LEFT_INDEX		7
#define LEFT_MIDDLE		8
#define LEFT_RING		9
#define LEFT_LITTLE		10
#define PLAIN_RIGHT_FOUR	13
#define PLAIN_LEFT_FOUR		14
#define PLAIN_THUMBS		15
#define MAX_FINGER_CODE		15

// Palm codes
#define UNKNOWN_PALM		20
#define RIGHT_FULL_PALM		21
#define RIGHT_WRITERS_PALM	22
#define LEFT_FULL_PALM		23
#define LEFT_WRITERS_PALM	24
#define RIGHT_LOWER_PALM	25
#define RIGHT_UPPER_PALM	26
#define LEFT_LOWER_PALM		27
#define LEFT_UPPER_PALM		28
#define RIGHT_OTHER_PALM	29
#define LEFT_OTHER_PALM		30
#define RIGHT_INTERDIGITAL_PALM	31
#define RIGHT_THENAR_PALM	32
#define RIGHT_HYPOTHENAR_PALM	33
#define LEFT_INTERDIGITAL_PALM	34
#define LEFT_THENAR_PALM	35
#define LEFT_HYPOTHENAR_PALM	36

// Impression type codes
#define LIVE_SCAN_PLAIN		0
#define LIVE_SCAN_ROLLED	1
#define NONLIVE_SCAN_PLAIN	2
#define NONLIVE_SCAN_ROLLED	3
#define LATENT_LIFT		7		// From other INCIT standards
#define LATENT 			LATENT_LIFT	// As in 381-2004
#define SWIPE			8
#define LIVE_SCAN_CONTACTLESS	9

// Image Quality values
#define UNDEFINED_IMAGE_QUALITY	254

// Compression algorithm codes
#define COMPRESSION_ALGORITHM_UNCOMPRESSED_NO_BIT_PACKED	0
#define COMPRESSION_ALGORITHM_UNCOMPRESSED_BIT_PACKED		1
#define COMPRESSION_ALGORITHM_COMPRESSED_WSQ			2
#define COMPRESSION_ALGORITHM_COMPRESSED_JPEG			3
#define COMPRESSION_ALGORITHM_COMPRESSED_JPEG2000		4
#define COMPRESSION_ALGORITHM_COMPRESSED_PNG			5

// Gender codes
#define GENDER_UNSPECIFIED	0
#define GENDER_MALE		1
#define GENDER_FEMALE		2
#define GENDER_UNKNOWN		3

// Eye color codes
#define EYE_COLOR_UNSPECIFIED	0x00
#define EYE_COLOR_BLUE		0x01
#define EYE_COLOR_BROWN		0x02
#define EYE_COLOR_GREEN		0x03
#define EYE_COLOR_HAZEL		0x12
#define EYE_COLOR_MAROON	0x22
#define EYE_COLOR_MULTI		0x10
#define EYE_COLOR_PINK		0x20
#define EYE_COLOR_UNKNOWN	0xFF

// Hair color codes
#define HAIR_COLOR_UNSPECIFIED	0x00
#define HAIR_COLOR_BALD		0x01
#define HAIR_COLOR_BLACK	0x02
#define HAIR_COLOR_BLONDE	0x03
#define HAIR_COLOR_BROWN	0x04
#define HAIR_COLOR_GRAY		0x05
#define HAIR_COLOR_RED		0x06
#define HAIR_COLOR_BLUE		0x10
#define HAIR_COLOR_GREEN	0x20
#define HAIR_COLOR_ORANGE	0x30
#define HAIR_COLOR_PINK		0x40
#define HAIR_COLOR_SANDY	0x13
#define HAIR_COLOR_AUBURN	0x14
#define HAIR_COLOR_WHITE	0x15
#define HAIR_COLOR_STRAWBERRY	0x16
#define HAIR_COLOR_UNKNOWN	0xFF

// Feature masks
#define FEATURE_MASK_SPECIFIED		0x000001
#define FEATURE_MASK_GLASSES		0x000002
#define FEATURE_MASK_MOUSTACHE		0x000004
#define FEATURE_MASK_BEARD		0x000008
#define FEATURE_MASK_TEETH_VISIBLE	0x000010
#define FEATURE_MASK_BLINK		0x000020
#define FEATURE_MASK_MOUTH_OPEN		0x000040
#define FEATURE_MASK_LEFT_EYE_PATCH	0x000080
#define FEATURE_MASK_RIGHT_EYE_PATCH	0x000100
#define FEATURE_MASK_BOTH_EYE_PATCH	0x000200
#define FEATURE_MASK_DARK_GLASSES	0x000400
#define FEATURE_MASK_MAJOR_MEDICAL	0x000800
#define FEATURE_MASK_RESERVED		0xFFF000  // all of the reserved bits
#define FEATURE_MASK_LEN		3	  // length in characters

// Expression Flags
#define EXPRESSION_UNSPECIFIED		0x0000
#define EXPRESSION_NEUTRAL		0x0001
#define EXPRESSION_SMILE_NOT_EXPOSED	0x0002
#define EXPRESSION_SMILE_EXPOSED	0x0003
#define EXPRESSION_RAISED_EYEBROWS	0x0004
#define EXPRESSION_EYES_LOOKING_AWAY	0x0005
#define EXPRESSION_SQUINTING		0x0006
#define EXPRESSION_FROWNING		0x0007
// Reserved ranges
#define EXPRESSION_RESERVED_LOW		0x0100
#define EXPRESSION_RESERVED_HIGH	0x7FFF
#define EXPRESSION_RESERVED_VENDOR_LOW	0x8000
#define EXPRESSION_RESERVED_VENDOR_HIGH	0xFFFF
// Expression masks
#define EXPRESSION_HIGH_MASK		0xFF00
#define EXPRESSION_LOW_MASK		0x00FF
#define EXPRESSION_LENGTH		2

// Pose angle masks
#define POSE_ANGLE_YAW			0xFF0000
#define POSE_ANGLE_PITCH		0x00FF00
#define POSE_ANGLE_ROLL			0x0000FF

// Pose angle min/max
#define POSE_ANGLE_MIN			1
#define POSE_ANGLE_MAX			181
#define POSE_ANGLE_UNSPECIFIED		0

// Pose angle uncertainty masks
#define POSE_ANGLE_UNCERTAINTY_YAW	0xFF0000
#define POSE_ANGLE_UNCERTAINTY_PITCH	0x00FF00
#define POSE_ANGLE_UNCERTAINTY_ROLL	0x0000FF

// Pose angle uncertainty min/max
#define POSE_ANGLE_UNCERTAINTY_MIN		1
#define POSE_ANGLE_UNCERTAINTY_MAX		181
#define POSE_ANGLE_UNCERTAINTY_UNSPECIFIED	0

// Face Image Types
#define FACE_IMAGE_TYPE_BASIC			0
#define FACE_IMAGE_TYPE_FULL_FRONTAL		1
#define FACE_IMAGE_TYPE_TOKEN_FRONTAL		2
#define FACE_IMAGE_TYPE_OTHER			3

// Image Data Types
#define IMAGE_DATA_JPEG				0
#define IMAGE_DATA_JPEG2000			1

// Image Color Space Types
#define COLOR_SPACE_TYPE_UNSPECIFIED		0
#define COLOR_SPACE_TYPE_24BIT_RGB		1
#define COLOR_SPACE_TYPE_YUV422			2
#define COLOR_SPACE_TYPE_8BIT_GRAYSCALE		3
#define COLOR_SPACE_TYPE_OTHER			4
#define COLOR_SPACE_TYPE_RESERVED_MIN		5
#define COLOR_SPACE_TYPE_RESERVED_MAX		127
#define COLOR_SPACE_TYPE_VENDOR_MIN		128
#define COLOR_SPACE_TYPE_VENDOR_MAX		255

// Source Type Flags
#define SOURCE_TYPE_UNSPECIFIED			0
#define SOURCE_TYPE_STATIC_PHOTO_UNKNOWN	1
#define SOURCE_TYPE_STATIC_PHOTO_DIGITAL_STILL	2
#define SOURCE_TYPE_STATIC_PHOTO_SCANNER	3
#define SOURCE_TYPE_VIDEO_FRAME_UNKNOWN		4
#define SOURCE_TYPE_VIDEO_FRAME_ANALOGU		5
#define SOURCE_TYPE_VIDEO_FRAME_DIGITAL		6
#define SOURCE_TYPE_UNKNOWN			7
#define SOURCE_TYPE_RESERVED_MIN		8
#define SOURCE_TYPE_RESERVED_MAX		127
#define SOURCE_TYPE_VENDOR_MIN			128
#define SOURCE_TYPE_VENDOR_MAX			255

#endif /* _BIOMDI_H */
