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

      FILE:    NISTCOM.C
      AUTHORS: Michael Garris
               Craig Watson
      DATE:    02/27/2001
      UPDATED: 03/10/2005 by MDG

      Contains routines responsible for manipulating a specialize
      attribute-value list called a NISTCOM used to hold image
      attributes relevant to image compression, decompression, and
      classification.

      ROUTINES:
#cat: combine_nistcom - takes an initialized FET NISTCOM structure
#cat:             or allocates one if necessary, and updates the
#cat:             structure with general image attributes.
#cat: combine_jpegl_nistcom - takes an initialized FET NISTCOM structure
#cat:             or allocates one if necessary, and updates the
#cat:             structure with JPEGL-specific image attributes.
#cat: combine_wsq_nistcom - takes an initialized FET NISTCOM structure
#cat:             or allocates one if necessary, and updates the
#cat:             structure with WSQ-specific image attributes.
#cat: combine_jpegb_nistcom - takes an initialized FET NISTCOM structure
#cat:             or allocates one if necessary, and updates the
#cat:             structure with JPEGB-specific image attributes.
#cat: del_jpegl_nistcom - takes an initialized FET NISTCOM structure
#cat:             and removes JPEGL compression attributes.
#cat: del_wsq_nistcom - takes an initialized FET NISTCOM structure
#cat:             and removes WSQ compression attributes.
#cat: del_jpegb_nistcom - takes an initialized FET NISTCOM structure
#cat:             and removes JPEGB compression attributes.
#cat: sd_ihead_to_nistcom - takes an IHead header from a specified NIST
#cat:             Special Database file, and parses specific attribute data
#cat:             from the header into an FET NISTCOM structure.
#cat: sd4_ihead_to_nistcom - takes an IHead header from a NIST Special
#cat:             Database 4 file, and parses specific attribute data
#cat:             from the header into an FET NISTCOM structure.
#cat: sd9_10_14_ihead_to_nistcom - takes an IHead header from a NIST Special
#cat:             Database 9,10,14 file, and parses specific attribute data
#cat:             from the header into an FET NISTCOM structure.
#cat: sd18_ihead_to_nistcom - takes an IHead header from a NIST Special
#cat:             Database 18 file, and parses specific attribute data
#cat:             from the header into an FET NISTCOM structure.
#cat: get_sd_class - gets the class from a special database id field
#cat:
#cat: get_class_from_ncic_class_string - gets the class from a special
#cat:             database ncic class string

***********************************************************************/

#include <nistcom.h>
#include <stdio.h>
#include <string.h>
#include <ihead.h>

/*****************************************************************/
int combine_nistcom(NISTCOM **onistcom, const int w, const int h,
                     const int d, const int ppi, const int lossyflag)
{
   int ret, allocflag, n;
   char *lossyval;
   char cbuff[11];
   NISTCOM *nistcom;


   /* ALLOCATION ? */
   if((*onistcom) == (NISTCOM *)NULL){
      if((ret = allocfet_ret(&nistcom, 6)))
         return(ret);
      allocflag = 1;
      /* HEADER */
      if((ret = updatefet_ret(NCM_HEADER, "6", nistcom))){
         if(allocflag){
            freefet(nistcom);
            *onistcom = (NISTCOM *)NULL;
         }
         return(ret);
      }
   }
   else{
      nistcom = *onistcom;
      allocflag = 0;
      n = 6;
   }

   /* WIDTH */
   sprintf(cbuff, "%d", w);
   if((ret = updatefet_ret(NCM_PIX_WIDTH, cbuff, nistcom))){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }

   /* HEIGHT */
   sprintf(cbuff, "%d", h);
   if((ret = updatefet_ret(NCM_PIX_HEIGHT, cbuff, nistcom))){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }

   /* DEPTH */
   sprintf(cbuff, "%d", d);
   if((ret = updatefet_ret(NCM_PIX_DEPTH, cbuff, nistcom))){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }

   /* PPI */
   sprintf(cbuff, "%d", ppi);
   if((ret = updatefet_ret(NCM_PPI, cbuff, nistcom))){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }

   /* LOSSY */
   /* If exists, lookup current LOSSY value. */
   ret = lookupfet(&lossyval, NCM_LOSSY, nistcom);
   /* If error ... */
   if(ret < 0){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }
   /* If LOSSY value found AND is set AND requesting to unset ... */
   if(ret &&
     (strcmp(lossyval,"0") != 0) &&
     (lossyflag == 0)){
      fprintf(stderr, "WARNING : combine_nistcom : ");
      fprintf(stderr, "request to unset lossy flag ignored\n");
   }
   else{
      sprintf(cbuff, "%d", lossyflag);
      if((ret = updatefet_ret(NCM_LOSSY, cbuff, nistcom))){
         if(allocflag){
            freefet(nistcom);
            *onistcom = (NISTCOM *)NULL;
         }
         return(ret);
      }
   }

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom))){
      if(allocflag){
         freefet(nistcom);
         *onistcom = (NISTCOM *)NULL;
      }
      return(ret);
   }

   *onistcom = nistcom;

   return(0);
}

/*****************************************************************/
int combine_jpegl_nistcom(NISTCOM **onistcom, const int w, const int h,
                  const int d, const int ppi, const int lossyflag,
                  const int n_cmpnts, int *hor_sampfctr, int *vrt_sampfctr,
                  const int intrlvflag, const int predict)
{
   int ret, i, allocflag;
   NISTCOM *nistcom;
   char cbuff[MAXFETLENGTH], *cptr;

   if(*onistcom == (NISTCOM *)NULL)
      allocflag = 1;
   else
      allocflag = 0;

   /* Combine image attributes to NISTCOM. */
   if((ret = combine_nistcom(onistcom, w, h, d, ppi, lossyflag)))
      return(ret);

   nistcom = *onistcom;

   /* COLORSPACE - only sure of GRAY */
   if(n_cmpnts == 1){
      if((ret = updatefet_ret(NCM_COLORSPACE, "GRAY", nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }
   }

   if(n_cmpnts > 1){
      /* NUM_COMPONENTS */
      sprintf(cbuff, "%d", n_cmpnts);
      if((ret = updatefet_ret(NCM_N_CMPNTS, cbuff, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }

      /* HV FACTORS */
      cptr = cbuff;
      sprintf(cptr, "%d,%d", hor_sampfctr[0], vrt_sampfctr[0]);
      cptr = cbuff + strlen(cbuff);
      for(i = 1; i < n_cmpnts; i++){
         sprintf(cptr, ":%d,%d", hor_sampfctr[i], vrt_sampfctr[i]);
         cptr = cbuff + strlen(cbuff);
      }

      if((ret = updatefet_ret(NCM_HV_FCTRS, cbuff, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }

      /* INTERLEAVE */
      sprintf(cbuff, "%d", intrlvflag);
      if((ret = updatefet_ret(NCM_INTRLV, cbuff, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }
   }

   /* COMPRESSION */
   if((ret = updatefet_ret(NCM_COMPRESSION, "JPEGL", nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* PREDICT */
   sprintf(cbuff, "%d", predict);
   if((ret = updatefet_ret(NCM_JPEGL_PREDICT, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   return(0);
}

/*****************************************************************/
int combine_wsq_nistcom(NISTCOM **onistcom, const int w, const int h,
                  const int d, const int ppi, const int lossyflag,
                  const float r_bitrate)
{
   int ret, allocflag;
   NISTCOM *nistcom;
   char cbuff[MAXFETLENGTH];

   if(*onistcom == (NISTCOM *)NULL)
      allocflag = 1;
   else
      allocflag = 0;

   /* Combine image attributes to NISTCOM. */
   if((ret = combine_nistcom(onistcom, w, h, d, ppi, lossyflag)))
      return(ret);

   nistcom = *onistcom;

   /* COLORSPACE */
   if((ret = updatefet_ret(NCM_COLORSPACE, "GRAY", nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* COMPRESSION */
   if((ret = updatefet_ret(NCM_COMPRESSION, "WSQ", nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* BITRATE */
   sprintf(cbuff, "%f", r_bitrate);
   if((ret = updatefet_ret(NCM_WSQ_RATE, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   return(0);
}

/*****************************************************************/
int combine_jpegb_nistcom(NISTCOM **onistcom, const int w, const int h,
                  const int d, const int ppi, const int lossyflag,
                  char *colorspace, const int n_cmpnts, const int intrlvflag,
                  const int quality)
{
   int ret, allocflag;
   NISTCOM *nistcom;
   char cbuff[MAXFETLENGTH];
   const char *cptr;

   if(*onistcom == (NISTCOM *)NULL)
      allocflag = 1;
   else
      allocflag = 0;

   /* Combine image attributes to NISTCOM. */
   if((ret = combine_nistcom(onistcom, w, h, d, ppi, lossyflag)))
      return(ret);

   nistcom = *onistcom;

   /* COLORSPACE */
   if((ret = updatefet_ret(NCM_COLORSPACE, colorspace, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   if(n_cmpnts > 1){
      /* NUM_COMPONENTS */
      sprintf(cbuff, "%d", n_cmpnts);
      if((ret = updatefet_ret(NCM_N_CMPNTS, cbuff, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }

      /* HV_FACTORS */
      if(strcmp(colorspace, "RGB") == 0)
         cptr = "1,1:1,1:1,1";
      else if(strcmp(colorspace, "YCbCr") == 0)
         cptr = "2,2:1,1:1,1";
      else{
         fprintf(stderr, "ERROR : combine_jpegb_nistcom : ");
         fprintf(stderr, "unknown/unsupported colorspace = %s\n",
                         colorspace);
         if(allocflag)
            freefet(nistcom);
         return(-2);
      }
      if((ret = updatefet_ret(NCM_HV_FCTRS, cptr, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }

      /* INTERLEAVE */
      sprintf(cbuff, "%d", intrlvflag);
      if((ret = updatefet_ret(NCM_INTRLV, cbuff, nistcom))){
         if(allocflag)
            freefet(nistcom);
         return(ret);
      }
   }

   /* COMPRESSION */
   if((ret = updatefet_ret(NCM_COMPRESSION, "JPEGB", nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* QUALITY */
   sprintf(cbuff, "%d", quality);
   if((ret = updatefet_ret(NCM_JPEGB_QUAL, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom))){
      if(allocflag)
         freefet(nistcom);
      return(ret);
   }

   return(0);
}

/*****************************************************************/
int del_jpegl_nistcom(NISTCOM *nistcom)
{
   int ret;
   char cbuff[MAXFETLENGTH];

   /* COMPRESSION */
   if((ret = deletefet_ret(NCM_COMPRESSION, nistcom)))
      return(ret);

   /* PREDICT */
   if((ret = deletefet_ret(NCM_JPEGL_PREDICT, nistcom)))
      return(ret);

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom)))
      return(ret);

   return(0);
}

/*****************************************************************/
int del_wsq_nistcom(NISTCOM *nistcom)
{
   int ret;
   char cbuff[MAXFETLENGTH];

   /* COMPRESSION */
   if((ret = deletefet_ret(NCM_COMPRESSION, nistcom)))
      return(ret);

   /* BITRATE */
   if((ret = deletefet_ret(NCM_WSQ_RATE, nistcom)))
      return(ret);

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom)))
      return(ret);

   return(0);
}

/*****************************************************************/
int del_jpegb_nistcom(NISTCOM *nistcom)
{
   int ret;
   char cbuff[MAXFETLENGTH];

   /* COMPRESSION */
   if((ret = deletefet_ret(NCM_COMPRESSION, nistcom)))
      return(ret);

   /* QUALITY */
   if((ret = deletefet_ret(NCM_JPEGB_QUAL, nistcom)))
      return(ret);

   /* UPDATE HEADER */
   sprintf(cbuff, "%d", nistcom->num);
   if((ret = updatefet_ret(NCM_HEADER, cbuff, nistcom)))
      return(ret);

   return(0);
}

/************************************/
/* Puts database Ihead into NISTCOM */
/************************************/
int sd_ihead_to_nistcom(NISTCOM **nistcom, IHEAD *ihead, int sd_id)
{
   switch(sd_id) {
      case 4:
         return(sd4_ihead_to_nistcom(nistcom, ihead));
         break;
      case 9:
         return(sd9_10_14_ihead_to_nistcom(nistcom, ihead, sd_id));
         break;
      case 10:
         return(sd9_10_14_ihead_to_nistcom(nistcom, ihead, sd_id));
         break;
      case 14:
         return(sd9_10_14_ihead_to_nistcom(nistcom, ihead, sd_id));
         break;
      case 18:
         return(sd18_ihead_to_nistcom(nistcom, ihead));
         break;
      default:
         fprintf(stderr,
                 "ERROR : sd_ihead_to_nistcom : invalid database id = %d\n",
                 sd_id);
         fprintf(stderr, "        expecting SD 4,9,10,14, or 18\n");
         *nistcom = NULL;
         return(-2);
   }
}

/*******************************/
/* Puts SD4 Ihead into NISTCOM */
/*******************************/
int sd4_ihead_to_nistcom(NISTCOM **onistcom, IHEAD *ihead)
{
   char *hst, *fname, *class_str, class_, *sex, *pname;
   int ret, hst_sz;
   NISTCOM *nistcom = NULL;
   char cbuff[11], id_str[BUFSIZE];

   /** Get information needed from NIST IHEAD Structure **/
   strcpy(id_str, ihead->id);
   id_str[12] = '\0';
   fname = id_str;

   if(id_str[14] == ' ')
      id_str[14] = '\0';
   else
      id_str[15] = '\0';
   class_str = &(id_str[13]);

   sex = &(id_str[16]);

   pname = ihead->parent;

   /* size of HISTORY field "<fname> <class_str> <pname>\0" */
   hst_sz = 2 + strlen(fname) + strlen(class_str) + strlen(pname);

   if((hst = (char *)malloc(hst_sz)) == (char *)NULL){
      fprintf(stderr, "ERROR : sd4_ihead_to_nistcom : malloc : hst\n");
      freefet(nistcom);
      return(-2);
   }
   sprintf(hst, "%s %s %s", fname, class_str, pname);

   /* Build the NISTCOM */
   if((ret = allocfet_ret(&nistcom, 5))) {
      return(ret);
      free(hst);
   }

   if((ret = updatefet_ret(NCM_HEADER, "5", nistcom))) {
      freefet(nistcom);
      free(hst);
      return(ret);
   }


   if((ret = updatefet_ret(NCM_SD_ID, "4", nistcom))) {
      freefet(nistcom);
      free(hst);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_HISTORY, hst, nistcom))) {
      freefet(nistcom);
      free(hst);
      return(ret);
   }
   free(hst);

   if((ret = get_sd_class(ihead->id, 4, &class_))){
      freefet(nistcom);
      return(ret);
   }
   sprintf(cbuff, "%c", class_);
   if((ret = updatefet_ret(NCM_FING_CLASS, cbuff, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_SEX, sex, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   *onistcom = nistcom;
   return(0);
}

/*****************************************/
/* Puts SD9 SD10 SD14 Ihead into NISTCOM */
/*****************************************/
int sd9_10_14_ihead_to_nistcom(NISTCOM **onistcom, IHEAD *ihead,
                               const int sd_id)
{
   char *hst, *fname, *ncic, class_, *sex, *pname, *ink_liv;
   char *nptr;
   int ret, hst_sz;
   NISTCOM *nistcom = NULL;
   char cbuff[11], id_str[BUFSIZE];

   /* Get information needed from NIST IHEAD Structure */
   strcpy(id_str, ihead->id);
   id_str[12] = '\0';
   fname = id_str;

   id_str[14] = '\0';
   sex = &(id_str[13]);

   id_str[16] = '\0';
   ink_liv = &(id_str[15]);

   ncic = &(id_str[17]);
   nptr = ncic;
   /* put "_" in for " " in the "ncic fpw:??" cases to keep class together */
   while(*nptr != '\0') {
      if(*nptr == ' ')
         *nptr = '_';
      nptr++;
   }

   pname = ihead->parent;

   /* size of HISTORY field "<fname> <ncic> <pname>\0" */
   hst_sz = 3 + strlen(fname) + strlen(ncic) + strlen(pname);

   if((hst = (char *)malloc(hst_sz)) == (char *)NULL){
      fprintf(stderr, "ERROR : sd4_ihead_to_nistcom : malloc : hst\n");
      freefet(nistcom);
      return(-2);
   }
/*
   sprintf(hst, "%s %s %s\0", fname, ncic, pname);
*/
   sprintf(hst, "%s %s %s%c", fname, ncic, pname, '\0');


   /* Build the NISTCOM */
   if((ret = allocfet_ret(&nistcom, 7))) {
      free(hst);
      return(ret);
   }

   if(sd_id == 14){
      if((ret = updatefet_ret(NCM_HEADER, "7", nistcom))) {
         freefet(nistcom);
         free(hst);
         return(ret);
      }
      if((ret = updatefet_ret(NCM_PPI, ihead->density, nistcom))) {
         freefet(nistcom);
         free(hst);
         return(ret);
      }
   }
   else{
      if((ret = updatefet_ret(NCM_HEADER, "6", nistcom))) {
         freefet(nistcom);
         free(hst);
         return(ret);
      }
   }

   sprintf(cbuff, "%d", sd_id);
   if((ret = updatefet_ret(NCM_SD_ID, cbuff, nistcom))) {
      freefet(nistcom);
      free(hst);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_HISTORY, hst, nistcom))) {
      freefet(nistcom);
      free(hst);
      return(ret);
   }
   free(hst);

   if((ret = get_sd_class(ihead->id, sd_id, &class_))){
      freefet(nistcom);
      return(ret);
   }
   sprintf(cbuff, "%c", class_);
   if((ret = updatefet_ret(NCM_FING_CLASS, cbuff, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_SEX, sex, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_SCAN_TYPE, ink_liv, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   *onistcom = nistcom;
   return(0);
}

/********************************/
/* Puts SD18 Ihead into NISTCOM */
/********************************/
int sd18_ihead_to_nistcom(NISTCOM **onistcom, IHEAD *ihead)
{
   char *fname, *sex, *age;
   int ret;
   NISTCOM *nistcom;
   char id_str[BUFSIZE];

   /* Get information needed from NIST IHEAD Structure */
   strcpy(id_str, ihead->id);
   id_str[12] = '\0';
   fname = id_str;

   id_str[14] = '\0';
   sex = &(id_str[13]);

   age = &(id_str[15]);

   /* Build the NISTCOM */
   if((ret = allocfet_ret(&nistcom, 6)))
      return(ret);

   if((ret = updatefet_ret(NCM_HEADER, "6", nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_SD_ID, "18", nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_HISTORY, fname, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_SEX, sex, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   if((ret = updatefet_ret(NCM_AGE, age, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   fname[1] = '\0';
   if((ret = updatefet_ret(NCM_FACE_POS, fname, nistcom))) {
      freefet(nistcom);
      return(ret);
   }

   *onistcom = nistcom;
   return(0);
}

/*******************************************************************/
int get_sd_class(char *id_str, const int sd_id, char *oclass)
{
   int ret, n, seqnum;
   char class_, ncic_str[BUFSIZE];

   if(sd_id == 4){
      if((n = sscanf(id_str, "%*s %c", &class_)) <= 0){
         fprintf(stderr, "ERROR : get_sd_class : getting class");
         fprintf(stderr, "letter for Special Database 4\n");
         return(-2);
      }
   }
   else if(sd_id == 10){
      if((n = sscanf(id_str, "%*c%*c%d.%*s %*s %*s %s", &seqnum, ncic_str)) <= 0){
         fprintf(stderr, "ERROR : get_sd_class : getting seqnum and ");
         fprintf(stderr, "ncic classes for Special Database 10\n");
         return(-3);
      }

      if((ret = get_class_from_ncic_class_string(ncic_str, seqnum, &class_)))
         return(ret);
   }
   else if(sd_id == 9 || sd_id == 14){
      if((n = sscanf(id_str, "%*c%d.%*s %*s %*s %s", &seqnum, ncic_str)) <= 0){
         fprintf(stderr, "ERROR : get_sd_class : getting seqnum and ");
         fprintf(stderr, "ncic classes for Special Database 9 or 14\n");
         return(-4);
      }

      if((ret = get_class_from_ncic_class_string(ncic_str, seqnum, &class_)))
         return(ret);
   }
   else{
      fprintf(stderr, "ERROR : get_sd_class : Invalid");
      fprintf(stderr, "database id number (%d)\n", sd_id);
      return(-5);
   }

   *oclass = class_;
   return(0);
}

/*******************************************************************/
int get_class_from_ncic_class_string(char *ncic_str, const int seqnum, char *oclass)
{
   char class_, *cptr;
   int fing_num, ridge_cnt;

   cptr = ncic_str;

   if(strncmp(cptr, "ac", 2) == 0)
      cptr += 3;

   if(strncmp(cptr, "aa", 2) == 0)
      class_ = 'A';
   else if(strncmp(cptr, "sr", 2) == 0)
      class_ = 'S';
   else if(strncmp(cptr, "tt", 2) == 0)
      class_ = 'T';
   else if(strncmp(cptr, "c", 1) == 0)
      class_ = 'W';
   else if(strncmp(cptr, "d", 1) == 0)
      class_ = 'W';
   else if(strncmp(cptr, "p", 1) == 0)
      class_ = 'W';
   else if(strncmp(cptr, "x", 1) == 0)
      class_ = 'W';
   else {
      ridge_cnt = atoi(cptr);
      if(ridge_cnt < 1 || ridge_cnt > 99){
         fprintf(stderr, "ERROR : get_class_from_ncic_class_string : ");
         fprintf(stderr, "invalid ridge count (%d) from ncic string\n", ridge_cnt);
         return(-2);
      }

      fing_num = seqnum % 10;
      if((fing_num && fing_num <= 5) == (ridge_cnt < 50))
         class_ = 'R';
      else
         class_ = 'L';
   }

   *oclass = class_;
   return(0);
}
