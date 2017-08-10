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
/* This program will convert a PGM file into a INCITS 381 Finger Image Record */
/*                                                                            */
/******************************************************************************/
#include <sys/queue.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

extern "C"
{
#include <pgm.h>
#include <wsq.h>
//#include <nfiq.h>
}
#include <netinet/in.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <m1io.h>
#include <fir.h>

// #include <sys/param.h>
// #include <sys/stat.h>
// #include <sys/types.h>

extern "C"
{
	int comp_nfiq(int *, float *, unsigned char *, const int, const int,
	    const int, const int);
}

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

class fir_general_header
{
	private :
		struct finger_image_record header;

	public :
		fir_general_header();
		void set_record_length(unsigned int len);
		unsigned int get_record_length();
		void write(FILE *) const;
};

class fir_view
{
	friend class fir;
	private :
		unsigned char *compressed_data;
		unsigned int compressed_data_size;
		unsigned char *uncompressed_data;
		unsigned int uncompressed_data_size;

		// the fir view's header; Table 4 INCITS 381
		struct finger_image_view_record fivr;

	public :
		fir_view(const unsigned char *uncompressed_raster_data,
		    const unsigned int w, const unsigned int h, 
		    const unsigned int d, const unsigned int resolution,
		    const unsigned int which_finger, const unsigned int type);

		void write(FILE *) const;
};

class fir
{
	// private :
	public :
		fir_general_header  gh;
		vector<fir_view *>  views;

	public :
		fir() {}
		void add_view(
		    const unsigned char *uncompressed_raster_data,
		    const unsigned int w,
		    const unsigned int h,
		    const unsigned int d,
		    const unsigned int resolution,
		    const unsigned int finger_position,
		    const unsigned int type);

		void write(const string &) const;
};

void
fir::write(const string &filename) const
{
	FILE *fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) {
		cerr << "failed write fopen on " << filename << endl;
		exit(2);
	}

	gh.write(fp);
	for (unsigned int i = 0; i < views.size(); i++)
		views[i]->write(fp);

	fclose(fp);
}

void
fir_general_header::set_record_length(unsigned int len)
{
	header.record_length = len;
}

unsigned int 
fir_general_header::get_record_length()
{
	return header.record_length;
}

void
fir_general_header::write(FILE *fp) const
{
	unsigned char c2[2] = {0, 0};
	unsigned short sval;

	OWRITE(header.format_id, sizeof(char), FORMAT_ID_LEN, fp);
	OWRITE(header.spec_version, sizeof(char), SPEC_VERSION_LEN, fp);

	// INICTS 381 is perverse in that the length field is 6 bytes.
	// This program internally uses a four byte uint up until it
	// has to be written.  This is implemented by writing two 0 bytes,
	// then writing the record length.
	OWRITE(c2, sizeof(char), 2, fp);
	LWRITE(&header.record_length, fp);

	SWRITE(&header.product_identifier_owner, fp);
        SWRITE(&header.product_identifier_type, fp);
	sval = header.compliance << HDR_COMPLIANCE_SHIFT;
	sval += header.scanner_id;
	SWRITE(&sval, fp);
	SWRITE(&header.image_acquisition_level, fp);
	CWRITE(&header.num_fingers_or_palm_images, fp);
	CWRITE(&header.scale_units, fp);
	SWRITE(&header.x_scan_resolution, fp);
	SWRITE(&header.y_scan_resolution, fp);
	SWRITE(&header.x_image_resolution, fp);
	SWRITE(&header.y_image_resolution, fp);
	CWRITE(&header.pixel_depth, fp);
	CWRITE(&header.image_compression_algorithm, fp);
	SWRITE(&header.reserved, fp);

	return;

	err_out:
		cerr << "failed write on one of the fields of the general header" << endl;
	exit(2);
}

fir_view::fir_view(
	const unsigned char *uncompressed_raster_data,
	const unsigned int w, const unsigned int h, const unsigned int d, 
	const unsigned int resolution,
	const unsigned int which_finger, const unsigned int type)
{
	fivr.finger_palm_position = which_finger;
	fivr.impression_type = type;
	fivr.horizontal_line_length = w;
	fivr.vertical_line_length = h;
	fivr.reserved = 0;

	// check for acceptable input
	if (d != 8) {
		cerr << "fir_view constructed with depth " << d
		    << " bits: only 8 is supported" << endl;
		exit(3);
	}

	// make private copy of uncompressed_data
	const unsigned int n = (unsigned int)w * (unsigned int)h;
	uncompressed_data = new unsigned char [n];
	for (unsigned int i = 0 ; i < n ; i++ )
		uncompressed_data[i] = uncompressed_raster_data[i];

	// do the quality computation
	{
		int nfiq;
		float confidence;
		const int err = 
		    comp_nfiq(&nfiq, &confidence, uncompressed_data, w, h, d,
			resolution);
		if (err) {
			cerr << "comp_nfiq returned " << err <<
			    " i.e. non-zero" << endl;
			exit(3);
		}
			fivr.quality = nfiq;
	}

	// WSQ compress the input data
	{
		int c = 0;
		// const float r_bitrate = 0.75;  // craig says this is 15:1
		const float r_bitrate = 0.57;  // patrick guess at 20:1
		const int err = wsq_encode_mem(&compressed_data, &c, r_bitrate,
		    uncompressed_data, (int)w, (int)h, (int)d, resolution, NULL);
		compressed_data_size = (unsigned int)c;

		if (err) {
			cerr << "wsq_encode_mem returned " << err <<
			    " i.e. non-zero" << endl;
			exit(3);
		}
	}

	fivr.length = compressed_data_size + FIVR_HEADER_LENGTH;
	fivr.reserved = 0;
}


void
fir_view::write(FILE *fp) const
{
	LWRITE(&fivr.length, fp);
	CWRITE(&fivr.finger_palm_position, fp);
	CWRITE(&fivr.count_of_views, fp);
	CWRITE(&fivr.view_number, fp);
	CWRITE(&fivr.quality, fp);
	CWRITE(&fivr.impression_type, fp);
	SWRITE(&fivr.horizontal_line_length, fp);
	SWRITE(&fivr.vertical_line_length, fp);
	CWRITE(&fivr.reserved, fp);
	
	fwrite(compressed_data, sizeof(unsigned char), compressed_data_size, fp);
	return;

err_out:
	cerr << "failed write on one of the fields of the general header" << endl;
	exit(2);
}

void
fir::add_view(const unsigned char *uncompressed_raster_data,
         const unsigned int w,
         const unsigned int h,
         const unsigned int d,
         const unsigned int resolution,
         const unsigned int finger_position,
         const unsigned int type)
{

	views.push_back(new fir_view(uncompressed_raster_data, w, h, d, 
	    resolution, finger_position, type));

	// INCITS 381 has the weird property that each "view" records
	// the total number of views.  So when one view is added all others'
	// must be incremented
	const unsigned int n = views.size();
	for (unsigned int i = 0; i < n; i++) {
		views[i]->fivr.count_of_views = n;
		views[i]->fivr.view_number    = i;
	}

	// keep the total volume of data up to data also
	gh.set_record_length(FIR_HEADER_LENGTH);
	for (unsigned int i = 0; i < n; i++)
		gh.set_record_length(gh.get_record_length() +
		    views[i]->fivr.length);
}


/******************************************************************************/
/* Initialize the header information within the FIR                           */
/******************************************************************************/
fir_general_header::fir_general_header()
{

	strcpy(header.format_id, FORMAT_ID);
	strcpy(header.spec_version, SPEC_VERSION);

	header.record_length = 0;
	header.product_identifier_owner = 0x00;
	header.product_identifier_owner = 0x0F;
					// NIST (owner of transforming app).
	header.scanner_id = 0;		// should be a crossmatch value
	header.compliance = 0;
	header.image_acquisition_level = 31;
	header.num_fingers_or_palm_images = 1;
	header.scale_units = 0x01;
	header.x_scan_resolution = 500;
	header.y_scan_resolution = 500;
	header.x_image_resolution = 500;
	header.y_image_resolution = 500;
	header.pixel_depth = 0x08;                // 8 bits
	header.image_compression_algorithm = 2;   // WSQ = 2
	header.reserved = 0;

	cerr << "done with default fir_general_header constructor" << endl;
}

void
usage()
{
	cerr << "usage:" << endl;
	cerr << "\tpgm2fir [-p] inputimage.pgm outputimage.381" << endl;
	cerr << "\t\twhere -p option specifies finger position; default is 2.\n" << endl;
	exit(1);
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
void
get_options(int argc, char *argv[], unsigned int &fingerposition, 
    string &pgmfile, string &m1file)
{
	char ch;
	struct stat sb;

	while ((ch = getopt(argc, argv, "p:")) != -1) {
		switch (ch) {
			case 'p' :
			{
				std::istringstream s(optarg);
				s >> fingerposition;
			}
			break;

		   case '?':
		   default:
			usage();
			break;
		}
	}
	if (argc - optind < 2)
		usage();

	pgmfile = argv[optind++];
	m1file  = argv[optind++];
	if (stat(m1file.c_str(), &sb) == 0) {
		cerr << "File " << m1file.c_str() << " exists, remove it first." << endl;
		exit(-1);
	}
  
	return;
}

unsigned
char *readpgmfile(const string &fn, unsigned int *width, unsigned int *height,
    unsigned int *depth)
{
	FILE *fp = fopen(fn.c_str(), "rb");
	if (fp == NULL) {
		cerr << "File " << optarg << " exists, remove it first" << endl;
		exit(-1);
	}

	int w, h;  gray d;  // d is max value
	gray **imagerows = pgm_readpgm(fp, &w, &h, &d);
	fclose(fp);

	// concatenate all h row-allocated chunks
	// into one contiguous chunk
	unsigned char *imageblock = 
	    (unsigned char *)malloc(w*h*sizeof(unsigned char));
	unsigned int n = 0;
	for (int r = 0 ; r < h ; r++) {
		const gray *row = imagerows[r];
		for (int c = 0 ; c < w ; c++)
			imageblock[n++] = row[c];
	}

	pgm_freearray(imagerows, h);
	*width  = (unsigned int)w;
	*height = (unsigned int)h;

	if (1 + (unsigned int)d == (1 <<  8))
		*depth =  8;
	else
		if (1 + (unsigned int)d == (1 << 16))
			*depth = 16;
		else {
			cerr << "pgm file maxval " << d << 
			    " implies a non-standard depth" << endl;
			exit(4);
		}

	return imageblock;
}

int debug = 0;  // global variable presnt in wsq or jpegl libraries

int
main(int argc, char *argv[])
{
	pgm_init(&argc, argv); /* why these get passed in? */

	const unsigned int ppi = 500;
	const unsigned int impression_type = 0;

	string pgmfile, m1file;
	unsigned int finger_position = 2;
	get_options(argc, argv, finger_position, pgmfile, m1file);

	unsigned int width, height, depth;
	const unsigned char *image =
	    readpgmfile(pgmfile, &width, &height, &depth);

	fir x;
	cerr << "done fir" << endl;
	x.add_view(image, (unsigned int)width, (unsigned int)height,
	    (unsigned int)depth, ppi, finger_position, impression_type);
	cerr << "done add" << endl;
	x.write(m1file);
	cerr << "done out" << endl;

	return(0);
}
