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
/* This program uses the ANSI/INCITS Face Recognition Format library to       */
/* print the contents of a file containing face records.                      */
/*                                                                            */
/* This program will use the value of the environment variable FRF_VIEWER     */
/* as the name of the program to execute for viewing images.                  */
/*                                                                            */
/******************************************************************************/

/* Needed by the GNU C libraries for Posix and other extensions */
#define _XOPEN_SOURCE	1

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/queue.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <biomdimacro.h>
#include <frf.h>

char *fn_prefix;

void
view_images(struct facial_block *fb)
{
	struct facial_data_block *fdb;
	char tempfn[MAXPATHLEN];
	FILE *fp;
	int i;
	char *viewer;
	char *ext;
	int first = true;

	i = 1;
	TAILQ_FOREACH(fdb, &fb->facial_data, list) {

		// Create a temp file containing the image data
		switch (fdb->image_data_type) {
			case FRF_IMAGE_DATA_TYPE_JPEG:
			case FRF_IMAGE_DATA_TYPE_JPEG2000:
				ext = "jpg";
				break;
			default:
				ext = "unk";
				break;
		}
		sprintf(tempfn, "%s-image%02d.%s", fn_prefix, i, ext);
		i++;
		if ((fp = fopen(tempfn, "wb")) < 0) {
			perror("Could not create temp image file.");
			return;
		}
		else {
			if (fwrite(fdb->image_data, 1, fdb->image_len, fp) <
				fdb->image_len) {
					perror("Could not write image file");
					remove(tempfn);
					return;
			}
			fclose(fp);
		}
		// launch the viewer
		viewer = getenv("FRF_VIEWER");
		if ((viewer == NULL) && first) {
			fprintf(stdout,
				"FRF_VIEWER not set; image files saved, but view disabled.\n");
			first = false;
		} else
			if (fork() == 0) 		// child process
				if (execlp(viewer, viewer, tempfn, 0) < 0) {
					perror("Could not execute viewer.");
					fprintf(stderr, 
					    "Is '%s' in your path?\n", viewer);
					remove(tempfn);
					exit (EXIT_FAILURE);
				}
	}
}

int main(int argc, char *argv[])
{
	char *usage = "usage: prfrf [-v] [-i] <datafile>\n"
			"\t -v Validate the record\n"
			"\t -i Load the images using app given by FRF_VIEWER"
			"\t    (will also save images to file)";
	FILE *fp;
	struct stat sb;
	struct facial_block *fb;
	int vflag = 0;
	int iflag = 0;
	int ch;
	int total_length;
	int ret;

	if ((argc < 2) || (argc > 4)) {
		printf("%s\n", usage);
		exit(EXIT_FAILURE);
	}

	while ((ch = getopt(argc, argv, "iv")) != -1) {
		switch (ch) {
			case 'i' :
				iflag = 1;
				break;
			case 'v' :
				vflag = 1;
				break;
			default :
				fprintf(stderr, "%s\n", usage);
				exit(EXIT_FAILURE);
			break;
		}
	}

	if (argv[optind] == NULL) {
		fprintf(stderr, "%s\n", usage);
		exit(EXIT_FAILURE);
	}

	fp = fopen(argv[optind], "rb");
	if (fp == NULL) {
		fprintf(stderr, "open of %s failed: %s\n",
			argv[optind], strerror(errno));
		exit(EXIT_FAILURE);
	}
	fn_prefix = argv[optind];

	if (fstat(fileno(fp), &sb) < 0) {
		fprintf(stdout, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	if (new_fb(&fb) < 0) {
		fprintf(stderr, "could not allocate Facial Block\n");
		exit(EXIT_FAILURE);
	}

	total_length = 0;
	ret = READ_ERROR;	/* In case of zero length file */
	while (total_length < sb.st_size) {

		ret = read_fb(fp, fb);
		if (ret != READ_OK)
			break;
		total_length += fb->record_length;

		// Optionally launch the external image viewer
		if (iflag) {
			view_images(fb);
		}

		// Optionally validate the FB
		if (vflag) {
			if (validate_fb(fb) != VALIDATE_OK) {
			    fprintf(stdout, "Facial Image Record is invalid.\n");
			} else {
			    fprintf(stdout, "Facial Image Record is valid.\n");
			}
		}
		// Dump the entire FB
		fprintf(stdout, "------------ Record Contents ------------\n");
		print_fb(stdout, fb);

		// Free the entire FB
		free_fb(fb);

		if (new_fb(&fb) < 0) {
			fprintf(stderr, "could not allocate FB\n");
			exit(EXIT_FAILURE);
		}
	}
	if (ret != READ_OK) {
		fprintf(stderr, "Could not read entire record; Contents:\n");
		print_fb(stderr, fb);
		free_fb(fb);
		exit (EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
