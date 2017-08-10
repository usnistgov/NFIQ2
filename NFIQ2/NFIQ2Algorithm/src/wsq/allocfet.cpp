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
      LIBRARY: FET - Feature File/List Utilities

      FILE:    ALLOCFET.C
      AUTHOR:  Michael Garris
      DATE:    01/11/2001
      UPDATED: 03/10/2005 by MDG

      Contains routines responsibile allocating data structures
      used to hold attribute-value paired lists.

      ROUTINES:
#cat: allocfet - allocates and initialized an empty fet structure.
#cat:            Exits on error.
#cat: allocfet_ret - allocates and initialized an empty fet structure.
#cat:            Returns on error.
#cat: reallocfet - reallocates an fet structure of a
#cat:            specified length.  Exits on error.
#cat: reallocfet_ret - reallocates an fet structure of a
#cat:            specified length.  Returns on error.

***********************************************************************/

#include <stdio.h>
#include <fet.h>
#include <util.h>

/********************************************************************/
FET *allocfet(int numfeatures)
{
   FET *fet;

   fet = (FET *)malloc(sizeof(FET));
   if (fet == (FET *)NULL)
      syserr("allocfet","malloc","fet");
   /* calloc here is required */
   fet->names = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->names == (char **)NULL)
      syserr("allocfet","calloc","fet->names");
   fet->values = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->values == (char **)NULL)
      syserr("allocfet","calloc","fet->values");
   fet->alloc = numfeatures;
   fet->num = 0;
   return(fet);
}

/********************************************************************/
int allocfet_ret(FET **ofet, int numfeatures)
{
   FET *fet;

   fet = (FET *)malloc(sizeof(FET));
   if (fet == (FET *)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : malloc : fet\n");
      return(-2);
   }
   /* calloc here is required */
   fet->names = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->names == (char **)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : calloc : fet->names\n");
      free(fet);
      return(-3);
   }
   fet->values = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->values == (char **)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : calloc : fet->values\n");
      free(fet->names);
      free(fet);
      return(-4);
   }
   fet->alloc = numfeatures;
   fet->num = 0;

   *ofet = fet;

   return(0);
}

/********************************************************************/
FET *reallocfet(FET *fet, int newlen)
{
   if (fet == (FET *)NULL || fet->alloc == 0)
      return(allocfet(newlen));

   fet->names = (char **)realloc(fet->names, newlen * sizeof(char *));
   if (fet->names == (char **)NULL)
      fatalerr("reallocfet", "realloc", "space for increased fet->names");
   fet->values = (char **)realloc(fet->values, newlen * sizeof(char *));
   if (fet->values == (char **)NULL)
      fatalerr("reallocfet", "realloc", "space for increased fet->values");
   fet->alloc = newlen;

   return(fet);
}

/********************************************************************/
int reallocfet_ret(FET **ofet, int newlen)
{
   int ret;
   FET *fet;

   fet = *ofet;

   /* If fet not allocated ... */
   if ((fet == (FET *)NULL || fet->alloc == 0)){
      /* Allocate the fet. */
      if((ret = allocfet_ret(ofet, newlen)))
         /* Return error code. */
         return(ret);
      /* Otherwise allocation was successful. */
      return(0);
   }

   /* Oherwise, reallocate fet. */
   fet->names = (char **)realloc(fet->names, newlen * sizeof(char *));
   if (fet->names == (char **)NULL){
      fprintf(stderr, "ERROR : reallocfet_ret : realloc : fet->names\n");
      return(-2);
   }
   fet->values = (char **)realloc(fet->values, newlen * sizeof(char *));
   if (fet->values == (char **)NULL){
      fprintf(stderr, "ERROR : reallocfet_ret : realloc : fet->values");
      return(-3);
   }
   fet->alloc = newlen;

   return(0);
}
