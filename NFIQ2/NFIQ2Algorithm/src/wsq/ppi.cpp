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


/***********************************************************************
      LIBRARY: WSQ - Grayscale Image Compression

      FILE:    PPI.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    01/17/2001
      UPDATED: 04/25/2005 by MDG

      Contains routines responsible for determining the scan
      resolution of a WSQ compressed image by attempting to
      locate and parse a NISTCOM comment in the datastream.

      ROUTINES:
#cat: read_ppi_wsq - Given a WSQ compressed data stream, attempts to
#cat:                read a NISTCOM comment from an open file and
#cat:                if possible return the pixel scan resulution
#cat:                (PPI value) stored therein.
#cat: getc_ppi_wsq - Given a WSQ compressed data stream, attempts to
#cat:                read a NISTCOM comment from a memory buffer and
#cat:                if possible return the pixel scan resulution
#cat:                (PPI value) stored therein.

***********************************************************************/

#include <stdio.h>
#include <wsq.h>

/************************************************************************/
int read_ppi_wsq(int *oppi, FILE *infp)
{
   int ret;
   long savepos;
   int ppi;
   char *value;
   NISTCOM *nistcom;

   /* Save current position in filestream ... */
   if((savepos = ftell(infp)) < 0){
      fprintf(stderr, "ERROR : read_ppi_wsq : ");
      fprintf(stderr, "ftell : couldn't determine current position\n");
      return(-2);
   }
   /* Set file pointer to beginning of filestream ... */
   if(fseek(infp, 0L, SEEK_SET) < 0){
      fprintf(stderr, "ERROR : read_ppi_wsq : ");
      fprintf(stderr, "fseek : couldn't set pointer to start of file\n");
      return(-3);
   }

   /* Get ppi from NISTCOM, if one exists ... */
   if((ret = read_nistcom_wsq(&nistcom, infp))){
      /* Reset file pointer to original position in filestream ... */
      if(fseek(infp, savepos, SEEK_SET) < 0){
         fprintf(stderr, "ERROR : read_ppi_wsq : ");
         fprintf(stderr, "fseek : couldn't reset file pointer\n");
         return(-4);
      }
      return(ret);
   }
   if(nistcom != (NISTCOM *)NULL){
      if((ret = extractfet_ret(&value, NCM_PPI, nistcom))){
         freefet(nistcom);
         /* Reset file pointer to original position in filestream ... */
         if(fseek(infp, savepos, SEEK_SET) < 0){
            fprintf(stderr, "ERROR : read_ppi_wsq : ");
            fprintf(stderr, "fseek : couldn't reset file pointer\n");
            return(-5);
         }
         return(ret);
      }
      if(value != (char *)NULL){
         ppi = atoi(value);
         free(value);
      }
      /* Otherwise, PPI not in NISTCOM, so ppi = -1. */
      else
         ppi = -1;
      freefet(nistcom);
   }
   /* Otherwise, NISTCOM does NOT exist, so ppi = -1. */
   else
      ppi = -1;

   /* Reset file pointer to original position in filestream ... */
   if(fseek(infp, savepos, SEEK_SET) < 0){
      fprintf(stderr, "ERROR : read_ppi_wsq : ");
      fprintf(stderr, "fseek : couldn't reset file pointer\n");
      return(-6);
   }

   *oppi = ppi;

   return(0);
}

/************************************************************************/
int getc_ppi_wsq(int *oppi, unsigned char *idata, const int ilen)
{
   int ret;
   int ppi;
   char *value;
   NISTCOM *nistcom;

   /* Get ppi from NISTCOM, if one exists ... */
   if((ret = getc_nistcom_wsq(&nistcom, idata, ilen)))
      return(ret);
   if(nistcom != (NISTCOM *)NULL){
      if((ret = extractfet_ret(&value, NCM_PPI, nistcom))){
         freefet(nistcom);
         return(ret);
      }
      if(value != (char *)NULL){
         ppi = atoi(value);
         free(value);
      }
      /* Otherwise, PPI not in NISTCOM, so ppi = -1. */
      else
         ppi = -1;
      freefet(nistcom);
   }
   /* Otherwise, NISTCOM does NOT exist, so ppi = -1. */
   else
      ppi = -1;

   *oppi = ppi;

   return(0);
}
