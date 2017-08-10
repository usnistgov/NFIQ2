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


#ifndef _SWAP_H
#define _SWAP_H

#define swap_uint_bytes(_ui_) \
{ \
        unsigned int _b_ = _ui_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_ui_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_int_bytes(_ui_) \
{ \
        int _b_ = _ui_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_ui_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_ushort_bytes(_us_) \
	{ \
	   unsigned short _b_ = _us_; \
	   unsigned char *_f_ = (unsigned char *)&(_b_); \
	   unsigned char *_t_ = (unsigned char *)&(_us_); \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_short_bytes(_a_) \
	{ \
	   short _b_ = _a_; \
	   char *_f_ = (char *) &_b_; \
	   char *_t_ = (char *) &_a_; \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_float_bytes(_flt_) \
{ \
        float _b_ = _flt_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_flt_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_short(_a_) \
	{ \
	   short _b_ = _a_; \
	   char *_f_ = (char *) &_b_; \
	   char *_t_ = (char *) &_a_; \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_image_shorts(_data,_swidth,_sheight) \
	{ \
	unsigned short *_sdata = (unsigned short *)_data; \
	int _i,_wdlen=16; \
 	for (_i = 0;_i<(int)((_swidth/_wdlen)*_sheight);_i++) \
	   swap_short(_sdata[_i]);\
        }

#define swap_int(_a_, _b_) \
	{ \
           int _t_ = _a_; \
           _a_ = _b_; \
           _b_ = _t_; \
        }

#define swap_float(_a_, _b_) \
	{ \
           float _t_ = _a_; \
           _a_ = _b_; \
           _b_ = _t_; \
        }

#define swap_string(_a_, _b_) \
	{ \
	   char *_t_ = _a_; \
	   _a_ = _b_; \
	   _b_ = _t_; \
	}

#endif /* !_SWAP_H */
