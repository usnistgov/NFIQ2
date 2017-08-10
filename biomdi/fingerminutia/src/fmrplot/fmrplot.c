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
/* This program will plot the minutiae information from several M1 records    */
/* over an image contained in the 'raw' bitmap file, creating a new image     */
/* file in JPEG format.  The first M1 file is used to retrieve the            */
/* dimensions of the image.                                                   */
/*                                                                            */
/* This program uses the GD Graphics Library, last available at               */
/* http://www.libgd.org and distributed in many ported packages.              */
/******************************************************************************/
#define _XOPEN_SOURCE   1
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <errno.h>
#include <fcntl.h>
#include <gd.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <fmr.h>

#define PLOTDIAM	8	// Minutiae circle plot diamter, in pixels
#define PLOTLENGTH	12	// Minutiae tail plot length, in pixels

#define MAXPLOTS	3
#define MAXTYPES	3

static int colors[MAXPLOTS][MAXTYPES];
static int infilecount = 0;	// tracks the number of input FMR files
static int view = 1;

static int COMCOLOR;	// Will be set later, the color for the minutiae center
static int BACKCOLOR;	// Will be set later, the color for the bacground

/******************************************************************************/
/* Print a how-to-use the program message.                                    */
/******************************************************************************/
void
usage()
{
	fprintf(stderr, 
		"usage:\n\tfmrplot -f <m1file> [-f <m1file> ...] [-i <imgfile>] -o <outfile> [-p] [-v <num>]\n"
		"\t\t -f:  Specifies the M1 input file(s)\n"
		"\t\t -i:  Specifies the input image file\n"
		"\t\t -o:  Specifies the output image file\n"
		"\t\t -p:  Output a PNG file instead of JPEG\n"
		"\t\t -v:  The finger view number\n");
}

/* Global option indicators */
static int p_opt;

/* Global file pointers */
FILE *fmr_fp[MAXPLOTS] = {NULL};	// the FMR (378-2004) input files
FILE *img_fp = NULL;	// for the input image file
FILE *out_fp = NULL;	// for the output image file

/******************************************************************************/
/* Close all open files.                                                      */
/******************************************************************************/
void
close_files()
{
	int i;
	for (i = 0; i < MAXPLOTS; i++)
		if (fmr_fp[i] != NULL)
			fclose(fmr_fp[i]);
	if (img_fp != NULL)
		fclose(img_fp);
	if (out_fp != NULL)
		fclose(out_fp);
}

/******************************************************************************/
/* Initialize the color table that will be used to plot the minutiae.         */
/******************************************************************************/
void
init_color_map()
{
	int BLUE = gdTrueColor(0, 0, 255);
	int BROWN = gdTrueColor(153, 102, 51);
	int CYAN = gdTrueColor(0, 255, 255);
	int GREEN = gdTrueColor(0, 255, 0);
	int MAGENTA = gdTrueColor(255, 0, 255);
	int ORANGE = gdTrueColor(255, 127, 0);
	int PURPLE = gdTrueColor(127, 0, 127);
	int RED = gdTrueColor(255, 0, 0);
	int YELLOW = gdTrueColor(255, 255, 0);
	int GRAY = gdTrueColor(220, 220, 220);
	colors[0][0] = RED;
	colors[0][1] = BLUE;
	colors[0][2] = GREEN;
	colors[1][0] = BROWN;
	colors[1][1] = CYAN;
	colors[1][2] = YELLOW;
	colors[2][0] = ORANGE;
	colors[2][1] = MAGENTA;
	colors[2][2] = PURPLE;

	COMCOLOR = RED;
	BACKCOLOR = GRAY;
}

/******************************************************************************/
/* Return the next available color map for drawing                            */
/******************************************************************************/
int *next_color_map()
{
	static int index = 0;
	return colors[index++];
}

/******************************************************************************/
/* Process the command line options, and set the global option indicators     */
/* based on those options.  This function will force an exit of the program   */
/* on error.                                                                  */
/******************************************************************************/
void
get_options(int argc, char *argv[])
{
	char ch;
	struct stat sb;
	int i_opt, f_opt, o_opt, v_opt;

	i_opt = f_opt = o_opt = v_opt = 0;
	while ((ch = getopt(argc, argv, "i:f:o:jpv:")) != -1) {
		switch (ch) {

		    case 'f':
			if (infilecount == MAXPLOTS) {
				fprintf(stderr, "Too many M1 input files");
				goto err_out;
			}

			if ((fmr_fp[infilecount] = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			infilecount++;
			f_opt = 1;
			break;

		    case 'i':	// Fingerprint image file
			if ((img_fp = fopen(optarg, "rb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			i_opt = 1;
			break;

		    case 'v':	// View number
			view = strtol(optarg, NULL, 10);
        		if (view == 0 && errno == EINVAL)
				usage();
			v_opt = 1;
			break;

		    case 'o':
			if (stat(optarg, &sb) == 0) {
		    	    fprintf(stderr,
				"File '%s' exists, remove it first.\n", optarg);
			    exit(EXIT_FAILURE);
			}
			if ((out_fp = fopen(optarg, "wb")) == NULL)
				OPEN_ERR_EXIT(optarg);
			o_opt = 1;
			break;
				
		    case 'p':	// PNG output file
			p_opt = 1;
			break;

		    case '?':
		    default:
			usage();
			break;
		}
	}

	if ((o_opt && f_opt) == 0) {
		usage();
		goto err_out;
	}
	return;

err_out:
	close_files();
	exit(EXIT_FAILURE);
}

/******************************************************************************/
/* Create a new raw image from an existing image (based on the image in a     */
/* file) or just a gray background, and the desired size of the image.        */
/******************************************************************************/
int
new_image(FILE *img_fp, int x_size, int y_size, gdImagePtr *img)
{
	int x, y;
	unsigned char **img_data;
	int color;
	int ret = -1;

	// Allocate memory for the image data that will be read from
	// a file: Y rows of X samples
	img_data = (unsigned char **)malloc(y_size * sizeof(char *));
	if (img_data == NULL)
		ALLOC_ERR_EXIT("memory for image data");
	if (img_fp != NULL) {
		for (y = 0; y < y_size; y++) {
			img_data[y] = (unsigned char *)
			    malloc(x_size * sizeof(char));
			if (img_data[y] == NULL)
				ALLOC_ERR_EXIT("memory for image data");
			if (fread(img_data[y], sizeof(unsigned char), x_size,
			    img_fp) != x_size)
				ERR_OUT("reading image file");
		}
	}
		
	*img = gdImageCreateTrueColor(x_size, y_size);
	if (*img == NULL)
		ERR_OUT("creating image");

	for (y = 0; y < y_size; y++)
		for (x = 0; x < x_size; x++) {
			if (img_fp != NULL) {
				color = gdTrueColor(img_data[y][x],
				    img_data[y][x], img_data[y][x]);
				gdImageSetPixel(*img, x, y, color);
			} else {
				gdImageSetPixel(*img, x, y, BACKCOLOR);
			}
		}

	ret = 0;
err_out:
	for (y = 0; y < y_size; y++)
		if (img_data[y] != NULL)
			free(img_data[y]);
	free(img_data);
	return (ret);
}

/******************************************************************************/
/* Plot the minutiae from a finger view mintuiae record onto the image        */
/* passed in as a parameter.                                                  */
/*                                                                            */
/******************************************************************************/
int
plot_minutiae(gdImagePtr img, struct finger_view_minutiae_record *fvmr)
{
	int count, i;
	int ret = -1;
	struct finger_minutiae_data **fmds;
	int *color_map;
	float fx, fy;
	int x, y;

	count = get_fmd_count(fvmr);
	if (count == 0)
		ERR_OUT("FVMR contains no minutiae");

	if (count < 0)
		ERR_OUT("Retrieving minutiae from FVMR");

	fmds = (struct finger_minutiae_data **) malloc(
		    count * sizeof(struct finger_minutiae_data **));
	if (fmds == NULL)
		ALLOC_ERR_OUT("memory for minutiae data");

	if (get_fmds(fvmr, fmds) != count)
		ERR_OUT("getting minutiae data");

	color_map = next_color_map();
	x = y = 0;
	for (i = 0; i < count; i++) {

		if (fmds[i]->type > MAXTYPES - 1)
			ERR_OUT("minutiae type value is invalid");

		// XXX Check that the point is inside the figure
		gdImageArc(img, fmds[i]->x_coord, fmds[i]->y_coord,
			   PLOTDIAM, PLOTDIAM, 0, 360,
			   color_map[fmds[i]->type]);

		// Plot the tail line segment
		// XXX Check that the point is inside the figure
		fx = fmds[i]->x_coord + 
		    (cos(((fmds[i]->angle * 2) / 180.0) * M_PI) * PLOTLENGTH);
		fy = fmds[i]->y_coord - 
		    (sin(((fmds[i]->angle * 2) / 180.0) * M_PI) * PLOTLENGTH);
		gdImageLine(img, fmds[i]->x_coord, fmds[i]->y_coord,
			    (int)fx, (int)fy, color_map[fmds[i]->type]);

	}
	/* Draw a cross at the center of minutiae mass */
	find_center_of_minutiae_mass(fmds, count, &x, &y);
	gdImageLine(img, x, y, x - PLOTLENGTH, y, COMCOLOR);
	gdImageLine(img, x, y, x + PLOTLENGTH, y, COMCOLOR);
	gdImageLine(img, x, y, x, y - PLOTLENGTH, COMCOLOR);
	gdImageLine(img, x, y, x, y + PLOTLENGTH, COMCOLOR);

	ret = 0;
err_out:
	if (fmds != NULL)
		free(fmds);
	return (ret);
}

int
main(int argc, char *argv[])
{
	struct finger_minutiae_record *fmrs[MAXPLOTS] = {NULL};
	struct finger_minutiae_record *fmr;
	struct finger_view_minutiae_record **fvmrs = NULL;
	gdImagePtr img;

	int rcount, i;
	int first = 1;

	get_options(argc, argv);

	init_color_map();

	for (i = 0; i < infilecount; i++) {
	
		// Allocate the FMR record in memory
		if (new_fmr(FMR_STD_ANSI, &fmrs[i]) < 0)
			ALLOC_ERR_EXIT("FMR");

		fmr = fmrs[i];

		// Read the FMR
		if (read_fmr(fmr_fp[i], fmr) != READ_OK) {
			fprintf(stderr, "Could not read FMR from file.\n");
			exit(EXIT_FAILURE);
		}

		if (first) {
			// Create a new image based on the dimensions from the
			// M1 record and the existing image file
			if (new_image(img_fp, fmr->x_image_size,
				      fmr->y_image_size, &img) != 0)
				ERR_OUT("Could not create new image");
			first = 0;
		}

		// Get all of the minutiae records
		rcount = get_fvmr_count(fmr);
		if (rcount == 0)
			ERR_OUT("there are no FVMRs in the FMR");
		if (rcount < view)
			ERR_OUT("View number greater than count in FMR");
		fvmrs = (struct finger_view_minutiae_record **) malloc(rcount * 
		    sizeof(struct finger_view_minutiae_record **));
		if (fvmrs == NULL)
			ALLOC_ERR_EXIT("FVMR Array");
		if (get_fvmrs(fmr, fvmrs) != rcount)
			ERR_OUT("getting FVMRs from FMR");
		if (plot_minutiae(img, fvmrs[view - 1]) != 0)
			ERR_OUT("plotting minutiae");
	}

	// Write out the new image file as a JPEG
	if (p_opt)
		gdImagePngEx(img, out_fp, 0);	// No compression
	else
		gdImageJpeg(img, out_fp, 95);

	// Free the entire FMR
	free_fmr(fmr);

	gdImageDestroy(img);

	close_files();

	exit(EXIT_SUCCESS);

err_out:
	if (fmr != NULL)
		free_fmr(fmr);

	if (fvmrs != NULL)
		free(fvmrs);

	close_files();

	exit(EXIT_FAILURE);
}
