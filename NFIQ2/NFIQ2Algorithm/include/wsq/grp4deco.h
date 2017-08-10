/*******************************************************************************

License: 
This software and/or related materials was developed at the National Institute
of Standards and Technology (NIST) by employees of the Federal Government
in the course of their official duties. Pursuant to title 17 Section 105
of the United States Code, this software is not subject to copyright
protection and is in the public domain. 

This software and/or related materials have been determined to be not subject
to the EAR (see Part 734.3 of the EAR for exact details) because it is
a publicly available technology and software, and is freely distributed
to any interested party with no licensing requirements.  Therefore, it is 
permissible to distribute this software as a free download from the internet.

Disclaimer: 
This software and/or related materials was developed to promote biometric
standards and biometric technology testing for the Federal Government
in accordance with the USA PATRIOT Act and the Enhanced Border Security
and Visa Entry Reform Act. Specific hardware and software products identified
in this software were used in order to perform the software development.
In no case does such identification imply recommendation or endorsement
by the National Institute of Standards and Technology, nor does it imply that
the products and equipment identified are necessarily the best available
for the purpose.

This software and/or related materials are provided "AS-IS" without warranty
of any kind including NO WARRANTY OF PERFORMANCE, MERCHANTABILITY,
NO WARRANTY OF NON-INFRINGEMENT OF ANY 3RD PARTY INTELLECTUAL PROPERTY
or FITNESS FOR A PARTICULAR PURPOSE or for any purpose whatsoever, for the
licensed product, however used. In no event shall NIST be liable for any
damages and/or costs, including but not limited to incidental or consequential
damages of any kind, including economic damage or injury to property and lost
profits, regardless of whether NIST shall be advised, have reason to know,
or in fact shall know of the possibility.

By using this software, you agree to bear all risk relating to quality,
use and performance of the software and/or related materials.  You agree
to hold the Government harmless from any claim arising from your use
of the software.

*******************************************************************************/


#ifndef _GRP4DECO_H
#define _GRP4DECO_H

/*********************************************************************/
/* grp4deco.h                                                        */
/* Originally decompression.h                                        */
/* UPDATED: 03/14/2005 by MDG                                        */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h> /* Added by MDG on 03-14-05 */
#include <string.h> /* Added by MDG on 03-14-05 */

#define SHORT int

#define True					1 
#define False					0 
#define Debug					False 

#define White					0
#define Black 					1
#define Black_byte				255 /* byte of all black bits: 11111111b */

#define Max_terminating_length	63		/* longest terminating code*/

#define Pixels_per_byte			8  
#define Bits_per_byte			8  
#define Last_bit_in_a_byte		7		/* assumes bits numbered from 0 - 7 */	
#define Last_bit_mask 			1		/* masks the last (low magnitude) bit */ 
#ifndef Default_width_in_pixels
#define Default_width_in_pixels	1728 	/* default width of a scan line */
#define Default_number_of_lines	2200 	/* default length of an image */
#endif

#define Invalid				   -1
/*
#define None					0 
*/
#define Extra_positions			25		/* ensures extra room in allocations */
#define Not_done_yet			0

#define VL3 				   -3  		/* Vertical Left 3 mode */
#define VL2 				   -2  		/* Vertical Left 2 mode */
#define VL1 				   -1 		/* Vertical Left 1 mode */
#define V0						0		/* Vertical mode */	  
#define VR1						1 		/* Vertical Right 1 mode */  
#define VR2						2  		/* Vertical Right 2 mode */ 
#define VR3						3  		/* Vertical Right 3 mode */ 
#define P						4 		/* Pass mode */ 
#define H						5		/* Horizontal mode */
#define EOFB					6		/* End Of File Buffer */

#define No_offset				0		/* no offset during fseek() */
#define End_of_file				2		/* start at EOF during fseek() */
#define Start_of_file			0		/* start at BOF during fseek() */

/*
char  *calloc();
SHORT *malloc();
*/

struct parameters {
	SHORT 	index;			/* indicates current position in "coding_line" */
	SHORT	max_pixel;		/* the number of pixels in a scan line */
	SHORT	*reference_line;/* array of changing elements on reference line */
	SHORT	*coding_line;	/* array of changing elements on coding line */
};

struct decompressed_descriptor {
	char  *data;				/* pointer to decompressed image */
	SHORT  pixels_per_line;		/* the number of pixels in a scan line */
	SHORT  number_of_lines;		/* the number of scan lines in the image */
};

struct compressed_descriptor {
	char  *data;				/* pointer to compressed image */
	SHORT  pixels_per_line;		/* the number of pixels in a scan line */
	SHORT  number_of_lines;		/* the number of scan lines in the image */
	int    length_in_bytes;		/* length of the compressed image in bytes */
};

/*****************************************************************************

	declarations of all the procedures in the group4 decompression routines
	follow.  The names of the files that contain the procedures are enclosed
	in comments above the declarations.
	
******************************************************************************/

/* grp4deco.c */
extern void grp4decomp(unsigned char *, int, int, int, unsigned char *, int *);
extern void control_decompression( struct compressed_descriptor *,
                                   struct decompressed_descriptor *);
extern void read_compressed_file_into_memory(struct compressed_descriptor *);
extern void prepare_to_decompress(struct compressed_descriptor *,
                                  struct decompressed_descriptor *,
                                  struct parameters *);
extern void set_up_first_line_d(struct parameters *);
extern void set_up_first_and_last_changing_elements_d(struct parameters *);
extern void prepare_to_decompress_next_line(struct parameters *);
extern void swap_the_reference_and_coding_lines(struct parameters *);
extern void crash_d(void);
extern SHORT decompress_line(struct parameters *);
extern SHORT get_mode(void);
extern void pass_mode_d(struct parameters *);
extern void vertical_mode_d(struct parameters *, SHORT);
extern void horizontal_mode_d(struct parameters *);
extern void prepare_to_write_bits_d(char *, SHORT);
extern void write_bits_d(unsigned SHORT, unsigned SHORT);
extern void prepare_to_read_bits(char *);
extern SHORT read_bit(void);
extern SHORT find_run_length_code(SHORT);

#endif /* !_GRP4DECO_H */
