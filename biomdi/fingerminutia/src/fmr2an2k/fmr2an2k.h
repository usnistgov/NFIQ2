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
/* Header file for the fmr2an2k program. Contains utility macros and          */
/* constant definitions.                                                      */
/*                                                                            */
/******************************************************************************/

// Append a field to a Type-1 record, branching to 'err_out' on error.
#define APPEND_TYPE1_FIELD(record, fieldname, value)			\
	do {								\
		FIELD *__field = NULL;					\
		if(value2field(&__field, TYPE_1_ID, fieldname, value) != 0)\
			goto err_out;					\
		if(append_ANSI_NIST_record(record, __field) != 0)	\
			goto err_out;					\
	} while (0)

#define APPEND_TYPE2_FIELD(record, fieldname, value)			\
	do {								\
		FIELD *__field = NULL;					\
		if(value2field(&__field, TYPE_2_ID, fieldname, value) != 0)\
			goto err_out;					\
		if(append_ANSI_NIST_record(record, __field) != 0)	\
			goto err_out;					\
	} while (0)

#define APPEND_TYPE9_FIELD(record, fieldname, value)			\
	do {								\
		FIELD *__field = NULL;					\
		if(value2field(&__field, TYPE_9_ID, fieldname, value) != 0)\
			goto err_out;					\
		if(append_ANSI_NIST_record(record, __field) != 0)	\
			goto err_out;					\
	} while (0)

// The maximum field size for a Type-2 record. This value is specified in 
// ELECTRONIC FINGERPRINT TRANSMISSION SPECIFICATION (CJIS-RS-0010),
// January 1999.
#define MAX_TYPE2_FIELD_SIZE	120
