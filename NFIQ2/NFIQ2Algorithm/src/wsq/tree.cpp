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

      FILE:    TREE.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    11/24/1999

      Contains routines responsible for manipulating tree structures
      used in WSQ image compression.

      ROUTINES:
#cat: build_wsq_trees - Builds WSQ decomposition trees.
#cat:
#cat: build_w_tree - Build subband x-y locations for creating wavelets.
#cat:
#cat: w_tree4 - Derives location and size of subband splits.
#cat:
#cat: build_q_tree - Build WSQ quantization tree of all 64 wavelet
#cat:                subband x-y locations and sizes.
#cat: q_tree16 - Derive location and size for a 4x4 window of subbands.
#cat:
#cat: q_tree4 - Derive location and size for 2x2 window of subbands.
#cat:

***********************************************************************/

#include <stdio.h>
#include <wsq.h>
#include <ihead.h>

/************************************************************************/
/*              Routines used to generate the "trees" used              */
/*              in creating the wavelet subbands (w_tree)               */
/*              and when quantizing the subbands (q_tree) in            */
/*              the WSQ compression/decompression algorithms.           */
/************************************************************************/
/* Build WSQ decomposition trees.                                       */
/************************************************************************/
void build_wsq_trees(W_TREE w_tree[], const int w_treelen,
                     Q_TREE q_tree[], const int q_treelen,
                     const int width, const int height)
{
   /* Build a W-TREE structure for the image. */
   build_w_tree(w_tree, width, height);
   /* Build a Q-TREE structure for the image. */
   build_q_tree(w_tree, q_tree);
}

/********************************************************************/
/* Routine to obtain subband "x-y locations" for creating wavelets. */
/********************************************************************/
void build_w_tree(
   W_TREE w_tree[],   /* wavelet tree structure */
   const int width,   /* image width            */
   const int height)  /* image height           */
{
   int lenx, lenx2, leny, leny2;  /* starting lengths of sections of
                                     the image being split into subbands */
   int node;

   for(node = 0; node < 20; node++) {
      w_tree[node].inv_rw = 0;
      w_tree[node].inv_cl = 0;
   }
   w_tree[2].inv_rw = 1;
   w_tree[4].inv_rw = 1;
   w_tree[7].inv_rw = 1;
   w_tree[9].inv_rw = 1;
   w_tree[11].inv_rw = 1;
   w_tree[13].inv_rw = 1;
   w_tree[16].inv_rw = 1;
   w_tree[18].inv_rw = 1;
   w_tree[3].inv_cl = 1;
   w_tree[5].inv_cl = 1;
   w_tree[8].inv_cl = 1;
   w_tree[9].inv_cl = 1;
   w_tree[12].inv_cl = 1;
   w_tree[13].inv_cl = 1;
   w_tree[17].inv_cl = 1;
   w_tree[18].inv_cl = 1;

   w_tree4(w_tree, 0, 1, width, height, 0, 0, 1);

   if((w_tree[1].lenx % 2) == 0) {
      lenx = w_tree[1].lenx / 2;
      lenx2 = lenx;
   }
   else {
      lenx = (w_tree[1].lenx + 1) / 2;
      lenx2 = lenx - 1;
   }

   if((w_tree[1].leny % 2) == 0) {
      leny = w_tree[1].leny / 2;
      leny2 = leny;
   }
   else {
      leny = (w_tree[1].leny + 1) / 2;
      leny2 = leny - 1;
   }

   w_tree4(w_tree, 4, 6, lenx2, leny, lenx, 0, 0);
   w_tree4(w_tree, 5, 10, lenx, leny2, 0, leny, 0);
   w_tree4(w_tree, 14, 15, lenx, leny, 0, 0, 0);

   w_tree[19].x = 0;
   w_tree[19].y = 0;
   if((w_tree[15].lenx % 2) == 0)
      w_tree[19].lenx = w_tree[15].lenx / 2;
   else
      w_tree[19].lenx = (w_tree[15].lenx + 1) / 2;

   if((w_tree[15].leny % 2) == 0)
      w_tree[19].leny = w_tree[15].leny / 2;
   else
      w_tree[19].leny = (w_tree[15].leny + 1) / 2;

   if(debug > 1) {
      for(node = 0; node < 20; node++)
         fprintf(stderr,
         "t%d -> x = %d  y = %d : dx = %d  dy = %d : ir = %d  ic = %d\n",
         node, w_tree[node].x, w_tree[node].y,
         w_tree[node].lenx, w_tree[node].leny,
         w_tree[node].inv_rw, w_tree[node].inv_cl);
      fprintf(stderr, "\n\n");
   }

   return;
}

/***************************************************************/
/* Gives location and size of subband splits for build_w_tree. */
/***************************************************************/
void w_tree4(
   W_TREE w_tree[],    /* wavelet tree structure                      */
   const int start1,   /* w_tree locations to start calculating       */
   const int start2,   /*    subband split locations and sizes        */
   const int lenx,     /* (temp) subband split location and sizes     */
   const int leny,
   const int x,
   const int y,
   const int stop1)    /* 0 normal operation, 1 used to avoid marking */
                       /*    size and location of subbands 60-63      */
{
   int evenx, eveny;   /* Check length of subband for even or odd */
   int p1, p2;         /* w_tree locations for storing subband sizes and
                          locations */
   
   p1 = start1;
   p2 = start2;

   evenx = lenx % 2;
   eveny = leny % 2;

   w_tree[p1].x = x;
   w_tree[p1].y = y;
   w_tree[p1].lenx = lenx;
   w_tree[p1].leny = leny;
   
   w_tree[p2].x = x;
   w_tree[p2+2].x = x;
   w_tree[p2].y = y;
   w_tree[p2+1].y = y;

   if(evenx == 0) {
      w_tree[p2].lenx = lenx / 2;
      w_tree[p2+1].lenx = w_tree[p2].lenx;
   }
   else {
      if(p1 == 4) {
         w_tree[p2].lenx = (lenx - 1) / 2;
         w_tree[p2+1].lenx = w_tree[p2].lenx + 1;
      }
      else {
         w_tree[p2].lenx = (lenx + 1) / 2;
         w_tree[p2+1].lenx = w_tree[p2].lenx - 1;
      }
   }
   w_tree[p2+1].x = w_tree[p2].lenx + x;
   if(stop1 == 0) {
      w_tree[p2+3].lenx = w_tree[p2+1].lenx;
      w_tree[p2+3].x = w_tree[p2+1].x;
   }
   w_tree[p2+2].lenx = w_tree[p2].lenx;


   if(eveny == 0) {
      w_tree[p2].leny = leny / 2;
      w_tree[p2+2].leny = w_tree[p2].leny;
   }
   else {
      if(p1 == 5) {
         w_tree[p2].leny = (leny - 1) / 2;
         w_tree[p2+2].leny = w_tree[p2].leny + 1;
      }
      else {
         w_tree[p2].leny = (leny + 1) / 2;
         w_tree[p2+2].leny = w_tree[p2].leny - 1;
      }
   }
   w_tree[p2+2].y = w_tree[p2].leny + y;
   if(stop1 == 0) {
      w_tree[p2+3].leny = w_tree[p2+2].leny;
      w_tree[p2+3].y = w_tree[p2+2].y;
   }
   w_tree[p2+1].leny = w_tree[p2].leny;
}

/****************************************************************/
void build_q_tree(
   W_TREE *w_tree,  /* wavelet tree structure */
   Q_TREE *q_tree)   /* quantization tree structure */
{
   int node;

   q_tree16(q_tree,3,w_tree[14].lenx,w_tree[14].leny,
              w_tree[14].x,w_tree[14].y, 0, 0);
   q_tree16(q_tree,19,w_tree[4].lenx,w_tree[4].leny,
              w_tree[4].x,w_tree[4].y, 0, 1);
   q_tree16(q_tree,48,w_tree[0].lenx,w_tree[0].leny,
              w_tree[0].x,w_tree[0].y, 0, 0);
   q_tree16(q_tree,35,w_tree[5].lenx,w_tree[5].leny,
              w_tree[5].x,w_tree[5].y, 1, 0);
   q_tree4(q_tree,0,w_tree[19].lenx,w_tree[19].leny,
             w_tree[19].x,w_tree[19].y);

   if(debug > 1) {
      for(node = 0; node < 60; node++)
         fprintf(stderr, "t%d -> x = %d  y = %d : lx = %d  ly = %d\n",
         node, q_tree[node].x, q_tree[node].y,
         q_tree[node].lenx, q_tree[node].leny);
      fprintf(stderr, "\n\n");
   }
   return;
}

/*****************************************************************/
void q_tree16(
   Q_TREE *q_tree,   /* quantization tree structure */
   const int start,  /* q_tree location of first subband        */
                     /*   in the subband group being calculated */
   const int lenx,   /* (temp) subband location and sizes */
   const int leny,
   const int x,
   const int y,
   const int rw,  /* NEW */   /* spectral invert 1st row/col splits */
   const int cl)  /* NEW */
{
   int tempx, temp2x;   /* temporary x values */
   int tempy, temp2y;   /* temporary y values */
   int evenx, eveny;    /* Check length of subband for even or odd */
   int p;               /* indicates subband information being stored */

   p = start;
   evenx = lenx % 2;
   eveny = leny % 2;

   if(evenx == 0) {
      tempx = lenx / 2;
      temp2x = tempx;
   }
   else {
      if(cl) {
         temp2x = (lenx + 1) / 2;
         tempx = temp2x - 1;
      }
      else  {
        tempx = (lenx + 1) / 2;
        temp2x = tempx - 1;
      }
   }

   if(eveny == 0) {
      tempy = leny / 2;
      temp2y = tempy;
   }
   else {
      if(rw) {
         temp2y = (leny + 1) / 2;
         tempy = temp2y - 1;
      }
      else {
        tempy = (leny + 1) / 2;
        temp2y = tempy - 1;
      }
   }

   evenx = tempx % 2;
   eveny = tempy % 2;

   q_tree[p].x = x;
   q_tree[p+2].x = x;
   q_tree[p].y = y;
   q_tree[p+1].y = y;
   if(evenx == 0) {
      q_tree[p].lenx = tempx / 2;
      q_tree[p+1].lenx = q_tree[p].lenx;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p].lenx;
   }
   else {
      q_tree[p].lenx = (tempx + 1) / 2;
      q_tree[p+1].lenx = q_tree[p].lenx - 1;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p+1].lenx;
   }
   q_tree[p+1].x = x + q_tree[p].lenx;
   q_tree[p+3].x = q_tree[p+1].x;
   if(eveny == 0) {
      q_tree[p].leny = tempy / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny;
      q_tree[p+3].leny = q_tree[p].leny;
   }
   else {
      q_tree[p].leny = (tempy + 1) / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny - 1;
      q_tree[p+3].leny = q_tree[p+2].leny;
   }
   q_tree[p+2].y = y + q_tree[p].leny;
   q_tree[p+3].y = q_tree[p+2].y;


   evenx = temp2x % 2;

   q_tree[p+4].x = x + tempx;
   q_tree[p+6].x = q_tree[p+4].x;
   q_tree[p+4].y = y;
   q_tree[p+5].y = y;
   q_tree[p+6].y = q_tree[p+2].y;
   q_tree[p+7].y = q_tree[p+2].y;
   q_tree[p+4].leny = q_tree[p].leny;
   q_tree[p+5].leny = q_tree[p].leny;
   q_tree[p+6].leny = q_tree[p+2].leny;
   q_tree[p+7].leny = q_tree[p+2].leny;
   if(evenx == 0) {
      q_tree[p+4].lenx = temp2x / 2;
      q_tree[p+5].lenx = q_tree[p+4].lenx;
      q_tree[p+6].lenx = q_tree[p+4].lenx;
      q_tree[p+7].lenx = q_tree[p+4].lenx;
   }
   else {
      q_tree[p+5].lenx = (temp2x + 1) / 2;
      q_tree[p+4].lenx = q_tree[p+5].lenx - 1;
      q_tree[p+6].lenx = q_tree[p+4].lenx;
      q_tree[p+7].lenx = q_tree[p+5].lenx;
   }
   q_tree[p+5].x = q_tree[p+4].x + q_tree[p+4].lenx;
   q_tree[p+7].x = q_tree[p+5].x;


   eveny = temp2y % 2;

   q_tree[p+8].x = x;
   q_tree[p+9].x = q_tree[p+1].x;
   q_tree[p+10].x = x;
   q_tree[p+11].x = q_tree[p+1].x;
   q_tree[p+8].y = y + tempy;
   q_tree[p+9].y = q_tree[p+8].y;
   q_tree[p+8].lenx = q_tree[p].lenx;
   q_tree[p+9].lenx = q_tree[p+1].lenx;
   q_tree[p+10].lenx = q_tree[p].lenx;
   q_tree[p+11].lenx = q_tree[p+1].lenx;
   if(eveny == 0) {
      q_tree[p+8].leny = temp2y / 2;
      q_tree[p+9].leny = q_tree[p+8].leny;
      q_tree[p+10].leny = q_tree[p+8].leny;
      q_tree[p+11].leny = q_tree[p+8].leny;
   }
   else {
      q_tree[p+10].leny = (temp2y + 1) / 2;
      q_tree[p+11].leny = q_tree[p+10].leny;
      q_tree[p+8].leny = q_tree[p+10].leny - 1;
      q_tree[p+9].leny = q_tree[p+8].leny;
   }
   q_tree[p+10].y = q_tree[p+8].y + q_tree[p+8].leny;
   q_tree[p+11].y = q_tree[p+10].y;


   q_tree[p+12].x = q_tree[p+4].x;
   q_tree[p+13].x = q_tree[p+5].x;
   q_tree[p+14].x = q_tree[p+4].x;
   q_tree[p+15].x = q_tree[p+5].x;
   q_tree[p+12].y = q_tree[p+8].y;
   q_tree[p+13].y = q_tree[p+8].y;
   q_tree[p+14].y = q_tree[p+10].y;
   q_tree[p+15].y = q_tree[p+10].y;
   q_tree[p+12].lenx = q_tree[p+4].lenx;
   q_tree[p+13].lenx = q_tree[p+5].lenx;
   q_tree[p+14].lenx = q_tree[p+4].lenx;
   q_tree[p+15].lenx = q_tree[p+5].lenx;
   q_tree[p+12].leny = q_tree[p+8].leny;
   q_tree[p+13].leny = q_tree[p+8].leny;
   q_tree[p+14].leny = q_tree[p+10].leny;
   q_tree[p+15].leny = q_tree[p+10].leny;
}

/********************************************************************/
void q_tree4(
   Q_TREE *q_tree,   /* quantization tree structure */
   const int start,  /* q_tree location of first subband         */
                     /*    in the subband group being calculated */
   const int lenx,   /* (temp) subband location and sizes */
   const int leny,
   const int x,
   const int  y)        
{
   int evenx, eveny;    /* Check length of subband for even or odd */
   int p;               /* indicates subband information being stored */


   p = start;
   evenx = lenx % 2;
   eveny = leny % 2;


   q_tree[p].x = x;
   q_tree[p+2].x = x;
   q_tree[p].y = y;
   q_tree[p+1].y = y;
   if(evenx == 0) {
      q_tree[p].lenx = lenx / 2;
      q_tree[p+1].lenx = q_tree[p].lenx;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p].lenx;
   }
   else {
      q_tree[p].lenx = (lenx + 1) / 2;
      q_tree[p+1].lenx = q_tree[p].lenx - 1;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p+1].lenx;
   }
   q_tree[p+1].x = x + q_tree[p].lenx;
   q_tree[p+3].x = q_tree[p+1].x;
   if(eveny == 0) {
      q_tree[p].leny = leny / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny;
      q_tree[p+3].leny = q_tree[p].leny;
   }
   else {
      q_tree[p].leny = (leny + 1) / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny - 1;
      q_tree[p+3].leny = q_tree[p+2].leny;
   }
   q_tree[p+2].y = y + q_tree[p].leny;
   q_tree[p+3].y = q_tree[p+2].y;
}
