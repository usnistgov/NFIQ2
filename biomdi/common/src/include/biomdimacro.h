/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */
/*
 * Collection of macros that are useful.
 */

#ifndef _BIOMDIMACRO_H
#define _BIOMDIMACRO_H 

#if defined WINDOWS || defined WIN32
	#include <windows.h>
#else
	#include <arpa/inet.h>
#endif

/******************************************************************************/
/* Common definitions used throughout                                         */
/******************************************************************************/
// Return codes for the read* functions
#define READ_OK		0
#define READ_EOF	1
#define READ_ERROR	2

/* Return codes for the write* functions */
#define WRITE_OK	0
#define WRITE_ERROR	1

/* Return codes for the print* functions */
#define PRINT_OK	0
#define PRINT_ERROR	1

/* Return codes for the validate* functions */
#define VALIDATE_OK	0
#define VALIDATE_ERROR	1

#define NULL_VERBOSITY_LEVEL	0
#define ERR_VERBOSITY_LEVEL	1
#define INFO_VERBOSITY_LEVEL	2
#define MAX_VERBOSITY_LEVEL	2

/*
 * A structure to represent the buffer that contains biometric data.
 */
struct biometric_data_buffer {
	uint32_t		bdb_size;	// Max size of the buffer
	uint8_t			*bdb_start;	// Beginning read/write location
	uint8_t			*bdb_end;	// Last read/write location
	uint8_t			*bdb_current;	// Current read/write location
};
typedef struct biometric_data_buffer BDB;

#define INIT_BDB(bdb, ptr, size) do {					\
	(bdb)->bdb_size = size;						\
	(bdb)->bdb_start = (bdb)->bdb_current = ptr;			\
	(bdb)->bdb_end = ptr + size;					\
} while (0)

#define REWIND_BDB(bdb)	do {						\
	(bdb)->bdb_current = (bdb)->bdb_start;				\
} while (0)

/*
 * Dump the contents of a BDB to stdout, 16 octets per row, in Hex.
 */
#define DUMP_BDB(bdb) do {						\
	int idx, len;							\
	len = (bdb)->bdb_current - (bdb)->bdb_start;			\
	for (idx = 0; idx < len; idx++) {				\
		printf("%02hhX ", ((uint8_t *)((bdb)->bdb_start))[idx]);\
		if (((idx+1) % 16) == 0)				\
			printf("\n");					\
	}								\
} while (0)

/*
 * Note that in order to use most of these macros, two labels, 'err_out'
 * and 'eof_out' must be defined within the function that uses the macro.
 * The presumption is that code starting at 'err_out' will unwind any state
 * (closing files, freeing memory, etc.) before exiting the function with
 * an error code.
 *
 */

/*
 * Read an opaque object from a file.
 */
#define OREAD(ptr, size, nmemb, stream)					\
	do {								\
		if (fread(ptr, size, nmemb, stream) < nmemb) {		\
		  if (feof(stream)) {					\
			goto eof_out;					\
		  } else {						\
		    fprintf(stderr, 					\
			    "Error reading at position %ld from %s:%d\n",\
			    ftell(stream), __FILE__, __LINE__);		\
			goto err_out;					\
		  }							\
		}							\
	} while (0)

#define FPRINTF(stream, ...)						\
	do {								\
		if (fprintf(stream, __VA_ARGS__) < 0) {			\
		  fprintf(stderr, 					\
			    "Error printing at position %ld from %s:%d\n",\
			    ftell(stream), __FILE__, __LINE__);		\
			goto err_out;					\
		}							\
	} while (0)

/*
 * Macros to read a single 8-bit, 16-bit, or 32-bit unsigned value
 * from a file and convert from big-endian to host native format.
 */
#define CREAD(ptr, stream)						\
	do {								\
		uint8_t __cval;						\
		OREAD(&__cval, 1, 1, stream);				\
		*ptr = __cval;						\
	} while (0)

#define SREAD(ptr, stream)						\
	do {								\
		uint16_t __sval;					\
		OREAD(&__sval, 2, 1, stream);				\
		*ptr = ntohs(__sval);					\
	} while (0)

#define LREAD(ptr, stream)						\
	do {								\
		uint32_t __lval;					\
		OREAD(&__lval, 4, 1, stream);				\
		*ptr = ntohl(__lval);					\
	} while (0)

/* 
 * Copy an opaque object from a buffer.
 */
#define OSCAN(ptr, size, bdb)						\
	do {								\
		if (((bdb)->bdb_current + size) > (bdb)->bdb_end)	\
			goto eof_out;					\
		(void)memcpy(ptr, (bdb)->bdb_current, size);		\
		(bdb)->bdb_current += size;				\
	} while (0)

/*
 * Macros to copy a single 8-bit, 16-bit, or 32-bit unsigned value
 * from a buffer and convert from big-endian to host native format.
 */
#define CSCAN(ptr, bdb)							\
	do {								\
		OSCAN(ptr, 1, bdb);					\
	} while (0)

#define SSCAN(ptr, bdb)							\
	do {								\
		uint16_t __sval;					\
		OSCAN(&__sval, 2, bdb);					\
		*ptr = ntohs(__sval);					\
	} while (0)

#define LSCAN(ptr, bdb)							\
	do {								\
		uint32_t __lval;					\
		OSCAN(&__lval, 4, bdb);					\
		*ptr = ntohl(__lval);					\
	} while (0)

/*
 * Conditionally read from the correct data source, file or buffer.
 */
#define OGET(ptr, size, nmemb, stream, bdb)				\
	do {								\
		if (stream != NULL)					\
			OREAD(ptr, size, nmemb, stream);		\
		else							\
			OSCAN(ptr, size*nmemb, bdb);			\
	} while (0)

#define CGET(ptr, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			CREAD(ptr, stream);				\
		else							\
			CSCAN(ptr, bdb);				\
	} while (0)

#define SGET(ptr, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			SREAD(ptr, stream);				\
		else							\
			SSCAN(ptr, bdb);				\
	} while (0)

#define LGET(ptr, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			LREAD(ptr, stream);				\
		else							\
			LSCAN(ptr, bdb);				\
	} while (0)

/*
 * Write an opaque object to a file.
 */
#define OWRITE(ptr, size, nmemb, stream)				\
	do {								\
		if (fwrite(ptr, size, nmemb, stream) < nmemb) {		\
		  fprintf(stderr, 					\
			    "Error writing at position %ld from %s:%d\n",\
			    ftell(stream), __FILE__, __LINE__);		\
			goto err_out;					\
		}							\
	} while (0)

/*
 * Macros to write a single 8-bit, 16-bit, or 32-bit unsigned value
 * to a file, converting from host native format to big-endian.
 */
#define CWRITE(val, stream)						\
	do {								\
		uint8_t __cval = (uint8_t)(val);			\
		OWRITE(&__cval, 1, 1, stream);				\
	} while (0)

#define SWRITE(val, stream)						\
	do {								\
		uint16_t __sval = (uint16_t)htons(val);			\
		OWRITE(&__sval, 2, 1, stream);				\
	} while (0)

#define LWRITE(val, stream)						\
	do {								\
		uint32_t __lval = (uint32_t)htonl(val);			\
		OWRITE(&__lval, 4, 1, stream);				\
	} while (0)

/* 
 * Copy an opaque object to a buffer.
 */
#define OPUSH(ptr, size, bdb)						\
	do {								\
		if (((bdb)->bdb_current + size) > (bdb)->bdb_end)	\
			goto err_out;					\
		(void)memcpy((bdb)->bdb_current, ptr, size);		\
		(bdb)->bdb_current += size;				\
	} while (0)

/*
 * Macros to copy a single 8-bit, 16-bit, or 32-bit unsigned value
 * into a buffer, converting from host native format to big-endian.
 */

#define CPUSH(val, bdb)							\
	do {								\
		uint8_t __cval = (uint8_t)(val);			\
		OPUSH(&__cval, 1, bdb);					\
	} while (0)

#define SPUSH(val, bdb)							\
	do {								\
		uint16_t __sval = (uint16_t)htons(val);			\
		OPUSH(&__sval, 2, bdb);					\
	} while (0)

#define LPUSH(val, bdb)							\
	do {								\
		uint32_t __lval = (uint32_t)htonl(val);			\
		OPUSH(&__lval, 4, bdb);					\
	} while (0)

/*
 * Conditionally write to the correct data source, file or buffer.
 */
#define OPUT(ptr, size, nmemb, stream, bdb)				\
	do {								\
		if (stream != NULL)					\
			OWRITE(ptr, size, nmemb, stream);		\
		else							\
			OPUSH(ptr, size*nmemb, bdb);			\
	} while (0)

#define CPUT(val, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			CWRITE(val, stream);				\
		else							\
			CPUSH(val, bdb);				\
	} while (0)

#define SPUT(val, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			SWRITE(val, stream);				\
		else							\
			SPUSH(val, bdb);				\
	} while (0)

#define LPUT(val, stream, bdb)						\
	do {								\
		if (stream != NULL)					\
			LWRITE(val, stream);				\
		else							\
			LPUSH(val, bdb);				\
	} while (0)

/*
 * Print INFO and ERROR messages to the appropriate place.
 */

#define INFOP(...)							\
	do {								\
		fprintf(stdout, "INFO: ");				\
		fprintf(stdout, __VA_ARGS__);				\
		fprintf(stdout, ".\n");					\
	} while (0)

#define ERRP(...)							\
	do {								\
		fprintf(stderr, "ERROR: ");				\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, ".\n");					\
	} while (0)

/*
 * Other common things to check and take action on error.
*/
#define OPEN_ERR_EXIT(fn)						\
	do {								\
		fprintf(stderr, "Could not open file %s: ", fn);	\
		fprintf(stderr, "%s\n", strerror(errno));		\
		exit(EXIT_FAILURE);					\
	} while (0)

#define READ_ERR_RETURN(...)						\
	do {								\
		fprintf(stderr, "Error reading ");			\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		return (READ_ERROR);					\
	} while (0)

#define READ_ERR_OUT(...)						\
	do {								\
		fprintf(stderr, "Error reading ");			\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		goto err_out;						\
	} while (0)

#define ALLOC_ERR_EXIT(msg)						\
	do {								\
		fprintf(stderr, "Error allocating %s.", msg);		\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		exit(EXIT_FAILURE);					\
	} while (0)

#define ALLOC_ERR_RETURN(msg)						\
	do {								\
		fprintf(stderr, "Error allocating %s.", msg);		\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		return (-1);						\
	} while (0)

#define ALLOC_ERR_OUT(msg)						\
	do {								\
		fprintf(stderr, "Error allocating %s.", msg);		\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		goto err_out;						\
	} while (0)

#define WRITE_ERR_OUT(...)						\
	do {								\
		fprintf(stderr, "Error writing ");			\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		goto err_out;						\
	} while (0)

#define ERR_OUT(...)							\
	do {								\
		fprintf(stderr, "ERROR: ");				\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		goto err_out;						\
	} while (0)

#define ERR_EXIT(...)							\
	do {								\
		fprintf(stderr, "ERROR: ");				\
		fprintf(stderr, __VA_ARGS__);				\
		fprintf(stderr, " (line %d in %s).\n", __LINE__, __FILE__);\
		exit(EXIT_FAILURE);					\
	} while (0)

/*
 * Check in Range and Warn: Check if a value falls within a given integer
 * range, and print given message if not in range.
 */
#define CRW(value, low, high, msg)					\
	do {								\
		if (((value) < (low)) || (value) > (high)) {		\
			INFOP("%s not in range", msg);\
		}							\
	} while (0)

/*
 * Check in Range and Set Return: Check if a value falls within a given
 * integer range, and set variable 'ret' to indicate an error. Will print
 * an optional message.
 */
#define CRSR(value, low, high, msg)					\
	do {								\
		if (((value) < (low)) || (value) > (high)) {		\
			if(msg != NULL)					\
				ERRP("%s not in range", msg);		\
			ret = VALIDATE_ERROR;				\
		}							\
	} while (0)

/*                
 * Compare and Set Return: Macro to compare two integer values and set  
 * variable 'ret' to indicate an error. Will print an optional message. 
 * Parameter 'valid' is considered the valid value.
 */                     
#define CSR(value, valid, msg)						\
	do {								\
		if ((value) != (valid)) {				\
			if(msg != NULL)					\
			    ERRP("%s not %d", msg, valid);	\
			ret = VALIDATE_ERROR;				\
		}							\
	} while (0)							\

/*                
 * Negative Compare and Set Return: Macro to compare two integer values
 * and set variable 'ret' to indicate an error if the values are equal.
 * Will print an optional message.
 */                     
#define NCSR(value, invalid, msg)					\
	do {								\
		if ((value) == (invalid)) {				\
			if(msg != NULL)					\
			    ERRP( "%s invalid value %d", msg, value);	\
			ret = VALIDATE_ERROR;				\
		}							\
	} while (0)							\

/*
 * SOME systems may not have the latest queue(3) package, so borrow some
 * of the macros from queue.h on a system that does have the complete 
 * package.
 */
/*
 * Copyright (c) 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef TAILQ_FIRST
#define TAILQ_FIRST(head) ((head)->tqh_first)
#endif

#ifndef TAILQ_NEXT
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)
#endif

#ifndef TAILQ_EMPTY
#define TAILQ_EMPTY(head) ((head)->tqh_first == NULL)
#endif

#ifndef TAILQ_FOREACH
#define TAILQ_FOREACH(var, head, field)                                 \
	for (var = TAILQ_FIRST(head); var; var = TAILQ_NEXT(var, field))
#endif

#endif /* !_BIOMDIMACRO_H  */
