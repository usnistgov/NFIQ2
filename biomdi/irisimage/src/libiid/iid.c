/*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain. NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*/

#include <sys/queue.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <biomdi.h>
#include <biomdimacro.h>
#include <iid_ext.h>
#include <iid.h>

char *
iid_code_to_str(int category, int code)
{
	switch(category) {
	case IID_CODE_CATEGORY_ORIENTATION :
		switch(code) {
			case IID_ORIENTATION_UNDEF : return("Undefined");
			case IID_ORIENTATION_BASE : return("Base");
			case IID_ORIENTATION_FLIPPED : return("Flipped");
			default : return("Invalid code");
		}
	case IID_CODE_CATEGORY_SCAN_TYPE :
		switch(code) {
			case IID_SCAN_TYPE_CORRECTED : return("Corrected");
			case IID_SCAN_TYPE_PROGRESSIVE : return("Progressive");
			case IID_SCAN_TYPE_INTERLACE_FRAME :
				return("Interlace Frame");
			case IID_SCAN_TYPE_INTERLACE_FIELD :
				return("Interlace Field");
			default : return("Invalid code");
		}
	case IID_CODE_CATEGORY_OCCLUSION :
		switch(code) {
			case IID_IROCC_UNDEF : return("Undefined");
			case IID_IROCC_PROCESSED : return("Processed");
			default : return("Invalid code");
		}
	case IID_CODE_CATEGORY_OCCLUSION_FILLING :
		switch(code) {
			case IID_IROCC_ZEROFILL : return ("Zero Fill");
			case IID_IROCC_UNITFILL : return ("Unit Fill");
			default : return ("Invalid code");
		}
	case IID_CODE_CATEGORY_IMAGE_FORMAT :
		switch(code) {
			case IID_IMAGEFORMAT_MONO_RAW : return ("Mono Raw");
			case IID_IMAGEFORMAT_RGB_RAW : return ("RGB Raw");
			case IID_IMAGEFORMAT_MONO_JPEG : return ("Mono JPEG");
			case IID_IMAGEFORMAT_RGB_JPEG : return ("RGB JPEG");
			case IID_IMAGEFORMAT_MONO_JPEG_LS :
			    return ("Mono JPEG LS");
			case IID_IMAGEFORMAT_RGB_JPEG_LS :
			    return ("RGB JPEG LS");
			case IID_IMAGEFORMAT_MONO_JPEG2000 :
			    return ("Mono JPEG 2000");
			case IID_IMAGEFORMAT_RGB_JPEG2000 :
			    return ("RGB JPEG 2000");
			default : return ("Invalid code");
		}
	case IID_CODE_CATEGORY_IMAGE_TRANSFORMATION :
		switch(code) {
			case IID_TRANS_UNDEF : return ("Undefined");
			case IID_TRANS_STD : return ("Standard");
			default : return ("Invalid code");
		}
	case IID_CODE_CATEGORY_EYE_POSITION :
		switch(code) {
			case IID_EYE_UNDEF : return ("Undefined");
			case IID_EYE_RIGHT : return ("Right Eye");
			case IID_EYE_LEFT : return ("Left Eye");
			default : return ("Invalid code");
		}
	case IID_CODE_CATEGORY_KIND_OF_IMAGERY :
		switch(code) {
			case IID_IMAGE_KIND_RECTLINEAR_NO_ROI_NO_CROPPING :
			    return ("Rectlinear w/o ROI masking, w/o cropping");
			case IID_IMAGE_KIND_RECTLINEAR_NO_ROI_CROPPING :
			    return ("Rectlinear w/o ROI masking, cropped");
			case IID_IMAGE_KIND_RECTLINEAR_MASKING_CROPPING :
			    return ("Rectlinear with masking and cropping");
			case IID_IMAGE_KIND_UNSEGMENTED_POLAR :
			    return ("Unsegmented polar");
			case IID_IMAGE_KIND_RECTILINEAR_UNSEGMENTED_POLAR :
			    return ("Rectlinear recon of unsegmented polar");
			default : return ("Invalid code");
		}
	default : return("Invalid category");
	}
}

/******************************************************************************/
/* Implement the interface for allocating and freeing iris image data blocks. */
/******************************************************************************/
int
new_iih(IIH **iih)
{
	IIH *liih;

	liih = (IIH *)malloc(sizeof(IIH));
	if (liih == NULL)
		return (-1);
	memset((void *)liih, 0, sizeof(IIH));
	*iih = liih;
	return (0);
}

void
free_iih(IIH *iih)
{
	if (iih->image_data != NULL)
		free(iih->image_data);
	if (iih->image_ancillary.pupil_iris_boundary_freeman_code_data.fcc
	    != NULL)
		free(iih->image_ancillary.pupil_iris_boundary_freeman_code_data.fcc);
	if (iih->image_ancillary.sclera_iris_boundary_freeman_code_data.fcc
	    != NULL)
		free(iih->image_ancillary.sclera_iris_boundary_freeman_code_data.fcc);
	free(iih);
}

int
new_ibsh(IBSH **ibsh)
{
	IBSH *libsh;

	libsh = (IBSH *)malloc(sizeof(IBSH));
	if (libsh == NULL)
		return (-1);
	memset((void *)libsh, 0, sizeof(IBSH));
	TAILQ_INIT(&libsh->image_headers);
	*ibsh = libsh;
	return (0);
}

void
free_ibsh(IBSH *ibsh)
{
	IIH *iih;

	while (!TAILQ_EMPTY(&ibsh->image_headers)) {
		iih = TAILQ_FIRST(&ibsh->image_headers);
		TAILQ_REMOVE(&ibsh->image_headers, iih, list);
		free_iih(iih);
	}
	free(ibsh);
}

int
new_iibdb(IIBDB **iibdb)
{
	IIBDB *liibdb;

	liibdb = (IIBDB *)malloc(sizeof(IIBDB));
	if (liibdb == NULL)
		return (-1);
	memset((void *)liibdb, 0, sizeof(IIBDB));
	*iibdb = liibdb;
	return (0);
}

void
free_iibdb(IIBDB *iibdb)
{
	if (iibdb->biometric_subtype_headers[0] != NULL)
		free_ibsh(iibdb->biometric_subtype_headers[0]);
	if (iibdb->biometric_subtype_headers[1] != NULL)
		free_ibsh(iibdb->biometric_subtype_headers[1]);
	free(iibdb);
}

void
add_iih_to_ibsh(IIH *iih, IBSH *ibsh)
{
	iih->ibsh = ibsh;
	TAILQ_INSERT_TAIL(&ibsh->image_headers, iih, list);
}

/******************************************************************************/
/* Implement the interface for reading/writing/verifying iris image data      */
/* blocks.                                                                    */
/******************************************************************************/
static int
internal_read_iih(FILE *fp, BDB *bdb, IIH *iih)
{
	int ret;

	LGET(&iih->image_length, fp, bdb);
	SGET(&iih->image_number, fp, bdb);
	CGET(&iih->image_quality, fp, bdb);
	SGET(&iih->quality_algo_vendor_id, fp, bdb);
	SGET(&iih->quality_algo_id, fp, bdb);
	SGET(&iih->rotation_angle, fp, bdb);
	SGET(&iih->rotation_uncertainty, fp, bdb);

	/* Read the extended data */
	if (fp != NULL)
		ret = read_roimask(fp, &iih->roi_mask);
	else
		ret = scan_roimask(bdb, &iih->roi_mask);
	if (ret != READ_OK)
		READ_ERR_OUT("ROI Mask");
	if (fp != NULL)
		ret = read_unsegpolar(fp, &iih->unsegmented_polar);
	else
		ret = scan_unsegpolar(bdb, &iih->unsegmented_polar);
	if (ret != READ_OK)
		READ_ERR_OUT("Unsegmented Polar");
	if (fp != NULL)
		ret = read_image_ancillary(fp, &iih->image_ancillary);
	else
		ret = scan_image_ancillary(bdb, &iih->image_ancillary);
	if (ret != READ_OK)
		READ_ERR_OUT("Image Ancillary Data");

	if (iih->image_length != 0) {
		iih->image_data = (uint8_t *)malloc(iih->image_length);
		if (iih->image_data == NULL)
			ALLOC_ERR_OUT("Image data buffer");
		OGET(iih->image_data, 1, iih->image_length, fp, bdb);
	}
	return (READ_OK);
eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_iih(FILE *fp, IIH *iih)
{
	return (internal_read_iih(fp, NULL, iih));
}

int
scan_iih(BDB *bdb, IIH *iih)
{
	return (internal_read_iih(NULL, bdb, iih));
}

static int
internal_read_ibsh(FILE *fp, BDB *bdb, IBSH *ibsh)
{
	int i;
	int ret;
	IIH *iih;

	CGET(&ibsh->eye_position, fp, bdb);
	SGET(&ibsh->num_images, fp, bdb);
	for (i = 0; i < ibsh->num_images; i++) {
		ret = new_iih(&iih);
		if (ret < 0)
			ALLOC_ERR_OUT("image header");
		if (fp != NULL)
			ret = read_iih(fp, iih);
		else
			ret = scan_iih(bdb, iih);
		if (ret != READ_OK)
			READ_ERR_OUT("Iris image header");
		add_iih_to_ibsh(iih, ibsh);
	}
	return (READ_OK);
eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_ibsh(FILE *fp, IBSH *ibsh)
{
	return (internal_read_ibsh(fp, NULL, ibsh));
}

int
scan_ibsh(BDB *bdb, IBSH *ibsh)
{
	return (internal_read_ibsh(NULL, bdb, ibsh));
}

static int
internal_read_iibdb(FILE *fp, BDB *bdb, IIBDB *iibdb)
{
	IBSH *ibsh;
	uint16_t sval;
	int i;
	int ret;
	IRH *hdr;

	/* Read the Iris record header */
	hdr = &iibdb->record_header;
	OGET(hdr->format_id, 1, IID_FORMAT_ID_LEN, fp, bdb);
	OGET(hdr->format_version, 1, IID_FORMAT_VERSION_LEN, fp, bdb);
	CGET(&hdr->kind_of_imagery, fp, bdb);
	LGET(&hdr->record_length, fp, bdb);
	SGET(&hdr->capture_device_id, fp, bdb);
	CGET(&hdr->num_eyes, fp, bdb);
	SGET(&hdr->record_header_length, fp, bdb);
	SGET(&sval, fp, bdb);

	hdr->horizontal_orientation = (sval & IID_HORIZONTAL_ORIENTATION_MASK)
	    >> IID_HORIZONTAL_ORIENTATION_SHIFT;
	hdr->vertical_orientation = (sval & IID_VERTICAL_ORIENTATION_MASK)
	    >> IID_VERTICAL_ORIENTATION_SHIFT;
	hdr->scan_type = (sval & IID_SCAN_TYPE_MASK) >> IID_SCAN_TYPE_SHIFT;
	hdr->iris_occlusions = (sval & IID_IRIS_OCCLUSIONS_MASK)
	    >> IID_IRIS_OCCLUSIONS_SHIFT;
	hdr->occlusion_filling = (sval & IID_OCCLUSION_FILLING_MASK)
	    >> IID_OCCLUSION_FILLING_SHIFT;

	SGET(&hdr->diameter, fp, bdb);
	SGET(&hdr->image_format, fp, bdb);
	SGET(&hdr->image_width, fp, bdb);
	SGET(&hdr->image_height, fp, bdb);
	CGET(&hdr->intensity_depth, fp, bdb);
	CGET(&hdr->image_transformation, fp, bdb);
	OGET(hdr->device_unique_id, 1, IID_DEVICE_UNIQUE_ID_LEN, fp, bdb);

	/* Read the image headers and image data */
	for (i = 0; i < iibdb->record_header.num_eyes; i++) {
		if (new_ibsh(&ibsh) < 0) 
			ALLOC_ERR_OUT("image biometric subtype header");
		ibsh->iibdb = iibdb;
		if (fp != NULL)
			ret = read_ibsh(fp, ibsh);
		else
			ret = scan_ibsh(bdb, ibsh);
		if (ret == READ_OK)
			iibdb->biometric_subtype_headers[i] = ibsh;
		else if (ret == READ_EOF)
			// XXX Handle a partial read?
			return (READ_EOF);
		else
			READ_ERR_OUT("Iris Biometric Subtype Header %d", i);
	}
	return (READ_OK);

eof_out:
	return (READ_EOF);
err_out:
	return (READ_ERROR);
}

int
read_iibdb(FILE *fp, IIBDB *iibdb)
{
	return (internal_read_iibdb(fp, NULL, iibdb));
}

int
scan_iibdb(BDB *bdb, IIBDB *iibdb)
{
	return (internal_read_iibdb(NULL, bdb, iibdb));
}

static int
internal_write_iih(FILE *fp, BDB *bdb, IIH *iih)
{
	int ret;

	LPUT(iih->image_length, fp, bdb);
	SPUT(iih->image_number, fp, bdb);
	CPUT(iih->image_quality, fp, bdb);
	SPUT(iih->quality_algo_vendor_id, fp, bdb);
	SPUT(iih->quality_algo_id, fp, bdb);
	SPUT(iih->rotation_angle, fp, bdb);
	SPUT(iih->rotation_uncertainty, fp, bdb);

	/* Read the extended data */
	if (fp != NULL)
		ret = write_roimask(fp, &iih->roi_mask);
	else
		ret = push_roimask(bdb, &iih->roi_mask);
	if (ret != WRITE_OK)
		WRITE_ERR_OUT("ROI Mask");
	if (fp != NULL)
		ret = write_unsegpolar(fp, &iih->unsegmented_polar);
	else
		ret = push_unsegpolar(bdb, &iih->unsegmented_polar);
	if (ret != WRITE_OK)
		WRITE_ERR_OUT("Unsegmented Polar");
	if (fp != NULL)
		ret = write_image_ancillary(fp, &iih->image_ancillary);
	else
		ret = push_image_ancillary(bdb, &iih->image_ancillary);
	if (ret != WRITE_OK)
		WRITE_ERR_OUT("Image Ancillary Data");

	if (iih->image_data != NULL)
		OPUT(iih->image_data, 1, iih->image_length, fp, bdb);
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_iih(FILE *fp, IIH *iih)
{
	return (internal_write_iih(fp, NULL, iih));
}

int
push_iih(BDB *bdb, IIH *iih)
{
	return (internal_write_iih(NULL, bdb, iih));
}

static int
internal_write_ibsh(FILE *fp, BDB *bdb, IBSH *ibsh)
{
	IIH *iih;
	int ret;

	CPUT(ibsh->eye_position, fp, bdb);
	SPUT(ibsh->num_images, fp, bdb);
	TAILQ_FOREACH(iih, &ibsh->image_headers, list) {
		if (fp != NULL)
			ret = write_iih(fp, iih);
		else
			ret = push_iih(bdb, iih);
		if (ret != WRITE_OK)
			WRITE_ERR_OUT("Iris Image Header");
	}
	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_ibsh(FILE *fp, IBSH *ibsh)
{
	return (internal_write_ibsh(fp, NULL, ibsh));
}

int
push_ibsh(BDB *bdb, IBSH *ibsh)
{
	return (internal_write_ibsh(NULL, bdb, ibsh));
}

static int
internal_write_iibdb(FILE *fp, BDB *bdb, IIBDB *iibdb)
{
	int i;
	int ret;
	uint16_t sval;
	IRH *hdr;

	/* Write the Iris record header */
	hdr = &iibdb->record_header;

	OPUT(hdr->format_id, 1, IID_FORMAT_ID_LEN, fp, bdb);
	OPUT(hdr->format_version, 1, IID_FORMAT_VERSION_LEN, fp, bdb);
	CPUT(hdr->kind_of_imagery, fp, bdb);
	LPUT(hdr->record_length, fp, bdb);
	SPUT(hdr->capture_device_id, fp, bdb);
	CPUT(hdr->num_eyes, fp, bdb);
	SPUT(hdr->record_header_length, fp, bdb);
	sval =
	      (hdr->horizontal_orientation << IID_HORIZONTAL_ORIENTATION_SHIFT)
	    | (hdr->vertical_orientation << IID_VERTICAL_ORIENTATION_SHIFT)
	    | (hdr->scan_type << IID_SCAN_TYPE_SHIFT)
	    | (hdr->iris_occlusions << IID_IRIS_OCCLUSIONS_SHIFT)
	    | (hdr->occlusion_filling << IID_OCCLUSION_FILLING_SHIFT);
	SPUT(sval, fp, bdb);
	SPUT(hdr->diameter, fp, bdb);
	SPUT(hdr->image_format, fp, bdb);
	SPUT(hdr->image_width, fp, bdb);
	SPUT(hdr->image_height, fp, bdb);
	CPUT(hdr->intensity_depth, fp, bdb);
	CPUT(hdr->image_transformation, fp, bdb);
	OPUT(hdr->device_unique_id, 1, IID_DEVICE_UNIQUE_ID_LEN, fp, bdb);

	/* Write the image headers and data */
	for (i = 0; i < iibdb->record_header.num_eyes; i++) {
		if (fp != NULL)
			ret = write_ibsh(fp,
			    iibdb->biometric_subtype_headers[i]);
		else
			ret = push_ibsh(bdb,
			    iibdb->biometric_subtype_headers[i]);
		if (ret != WRITE_OK)
			WRITE_ERR_OUT("Iris Biometric Subtype Header %d", i+1);
	}

	return (WRITE_OK);
err_out:
	return (WRITE_ERROR);
}

int
write_iibdb(FILE *fp, IIBDB *iibdb)
{
	return (internal_write_iibdb(fp, NULL, iibdb));
}

int
push_iibdb(BDB *bdb, IIBDB *iibdb)
{
	return (internal_write_iibdb(NULL, bdb, iibdb));
}

int
print_iih(FILE *fp, IIH *iih)
{
	int ret;

	FPRINTF(fp, "\tImage Length\t\t\t: %u\n", iih->image_length);
	FPRINTF(fp, "\tImage Number\t\t\t: %hu\n", iih->image_number);
	FPRINTF(fp, "\tImage Quality\t\t\t: %hhu\n", iih->image_quality);
	FPRINTF(fp, "\tQuality Algorithm Vendor ID\t: 0x%04hX\n",
	    iih->quality_algo_vendor_id);
	FPRINTF(fp, "\tQuality Algorithm ID\t\t: 0x%04hX\n",
	    iih->quality_algo_id);
	FPRINTF(fp, "\tRotation Angle\t\t\t: ");
	if (iih->rotation_angle == IID_ROT_ANGLE_UNDEF)
		FPRINTF(fp, "Undefined\n");
	else
		FPRINTF(fp, "%hu\n", iih->rotation_angle);
	FPRINTF(fp, "\tRotation Uncertaintity\t\t: ");
	if (iih->rotation_uncertainty == IID_ROT_UNCERTAIN_UNDEF)
		FPRINTF(fp, "Undefined\n");
	else
		FPRINTF(fp, "%hu\n", iih->rotation_uncertainty);

	/* Read the extended data */
	ret = print_roimask(fp, &iih->roi_mask);
	if (ret != PRINT_OK)
		ERRP("Can't print ROI Mask");
	ret = print_unsegpolar(fp, &iih->unsegmented_polar);
	if (ret != PRINT_OK)
		ERRP("Can't print Unsegmented Polar");
	ret = print_image_ancillary(fp, &iih->image_ancillary);
	if (ret != PRINT_OK)
		ERRP("Can't print Image Ancillary Data");

// XXX If an option flag is passed, save the image data to a file
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

int
print_ibsh(FILE *fp, IBSH *ibsh)
{
	int i;
	int ret;
	IIH *iih;

	FPRINTF(fp, "-----------------------------\n");
	FPRINTF(fp, "Iris Biometric Subtype Header\n");
	FPRINTF(fp, "-----------------------------\n");
	FPRINTF(fp, "Eye Position\t\t\t\t: 0x%02hhX (%s)\n",
	    ibsh->eye_position,
	    iid_code_to_str(IID_CODE_CATEGORY_EYE_POSITION,
		ibsh->eye_position));
	FPRINTF(fp, "Number of Images\t\t\t: %d\n", ibsh->num_images);
	i = 1;
	TAILQ_FOREACH(iih, &ibsh->image_headers, list) {
		FPRINTF(fp, "Iris Image Header %d:\n", i);
		ret = print_iih(fp, iih);
		if (ret != PRINT_OK)
			ERR_OUT("Could not print Iris Image Header %d", i);
	}
	FPRINTF(fp, "-----------------------------\n");
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

int
print_iibdb(FILE *fp, IIBDB *iibdb)
{
	int ret;
	int i;
	IRH *hdr;

	hdr = &iibdb->record_header;
	FPRINTF(fp, "Format ID\t\t\t: %s\nSpecification Version\t\t: %s\n",
	    hdr->format_id, hdr->format_version);
	FPRINTF(fp, "Kind of imagery\t\t\t: %hhu (%s)\n",
	    hdr->kind_of_imagery,
	    iid_code_to_str(IID_CODE_CATEGORY_KIND_OF_IMAGERY,
	    hdr->kind_of_imagery));
	FPRINTF(fp, "Record Length\t\t\t: %u\n",
	    hdr->record_length);
	FPRINTF(fp, "Capture Device ID\t\t: 0x%04hX\n",
	    hdr->capture_device_id);
	FPRINTF(fp, "Number of Eyes Imaged\t\t: %d\n", hdr->num_eyes);
	FPRINTF(fp, "Record Header Length\t\t: %d\n",
	    hdr->record_header_length);
	FPRINTF(fp, "Iris Image Properties:\n");
	FPRINTF(fp, "\tHorizontal Orientation\t: %hhu (%s)\n",
	    hdr->horizontal_orientation,
	    iid_code_to_str(IID_CODE_CATEGORY_ORIENTATION,
		hdr->horizontal_orientation));
	FPRINTF(fp, "\tVertical Orientation\t: %hhu (%s)\n",
	    hdr->vertical_orientation,
	    iid_code_to_str(IID_CODE_CATEGORY_ORIENTATION,
		hdr->vertical_orientation));
	FPRINTF(fp, "\tScan Type\t\t: %hhu (%s)\n",
	    hdr->scan_type,
	    iid_code_to_str(IID_CODE_CATEGORY_SCAN_TYPE,
		hdr->scan_type));
	FPRINTF(fp, "\tIris Occlusions\t\t: %hhu (%s)\n",
	    hdr->iris_occlusions,
	    iid_code_to_str(IID_CODE_CATEGORY_OCCLUSION,
		hdr->iris_occlusions));
	FPRINTF(fp, "\tOcclusion Filling\t: %hhu (%s)\n",
	    hdr->occlusion_filling,
	    iid_code_to_str(IID_CODE_CATEGORY_OCCLUSION_FILLING,
		hdr->occlusion_filling));
	FPRINTF(fp, "Iris Diameter\t\t\t: %hu\n",
	    hdr->diameter);
	FPRINTF(fp, "Image Format\t\t\t: 0x%04hX (%s)\n",
	    hdr->image_format,
	    iid_code_to_str(IID_CODE_CATEGORY_IMAGE_FORMAT,
		hdr->image_format));
	FPRINTF(fp, "Image Size\t\t\t: %hux%hu\n",
	    hdr->image_width, hdr->image_height);
	FPRINTF(fp, "Image Depth\t\t\t: %hhu\n",
	    hdr->intensity_depth);
	FPRINTF(fp, "Image Transformation\t\t: %hhu (%s)\n",
	    hdr->image_transformation,
	    iid_code_to_str(IID_CODE_CATEGORY_IMAGE_TRANSFORMATION,
		hdr->image_transformation));
	FPRINTF(fp, "Device Unique ID\t\t: ");
	if (hdr->device_unique_id[0] != 0)
		FPRINTF(fp, "%s\n", hdr->device_unique_id);
	else
		FPRINTF(fp, "Not present\n");

	/* Print the image headers */
	for (i = 0; i < hdr->num_eyes; i++) {
		/* Make sure we actually read the biometric subtype */
		if (iibdb->biometric_subtype_headers[i] != NULL) {
			ret = print_ibsh(fp,
			    iibdb->biometric_subtype_headers[i]);
			if (ret != PRINT_OK)
				ERRP("Could not print Iris Biometric "
				    "Subtype Header %d", i+1);
		} else {
			ERRP("Iris Biometric Subtype Header %d not read", i+1);
		}
	}
	FPRINTF(fp, "\n");
	return (PRINT_OK);
err_out:
	return (PRINT_ERROR);
}

/******************************************************************************/
/* Implementation of the higher level access routines.                        */
/******************************************************************************/
int
get_ibsh_count(IIBDB *iibdb)
{
	return (iibdb->record_header.num_eyes);
}

int
get_ibshs(IIBDB *iibdb, IBSH *ibshs[])
{
	int i;

	for (i = 0; i < iibdb->record_header.num_eyes; i++)
		ibshs[i] = iibdb->biometric_subtype_headers[i];

	return (iibdb->record_header.num_eyes);
}

int
get_iih_count(IBSH *ibsh)
{
	return (ibsh->num_images);
}

int
get_iihs(IBSH *ibsh, IIH *iihs[])
{
	int count = 0;

	IIH *iih;

	TAILQ_FOREACH(iih, &ibsh->image_headers, list) {
		iihs[count] = iih;
		count++;
	}
	return (count);
}

int
clone_iibdb(IIBDB *src, IIBDB **dst, int cloneimg)
{
	IIBDB *liibdb;
	IBSH *dstibsh;
	IIH *srciih, *dstiih;
	int ret;
	int i;

	liibdb = NULL;
	dstibsh = NULL;
	dstiih = NULL;
	
	ret = new_iibdb(&liibdb);
	if (ret != 0)
		ALLOC_ERR_RETURN("Cloned IIBDB");

	liibdb->record_header = src->record_header;
	for (i = 0; i < src->record_header.num_eyes; i++) {
		ret = new_ibsh(&dstibsh);
		if (ret != 0)
			ALLOC_ERR_OUT("Cloned IBSH");
		COPY_IBSH(src->biometric_subtype_headers[i], dstibsh);
		liibdb->biometric_subtype_headers[i] = dstibsh;
		dstibsh->iibdb = liibdb;
		TAILQ_FOREACH(srciih,
		    &src->biometric_subtype_headers[i]->image_headers, list) {
			ret = new_iih(&dstiih);
			if (ret != 0)
				ALLOC_ERR_OUT("Cloned IIH");
			COPY_IIH(srciih, dstiih);
			add_iih_to_ibsh(dstiih, dstibsh);
			if (cloneimg) {
				dstiih->image_data =
				    (uint8_t *)malloc(srciih->image_length);
				if (dstiih->image_data == NULL)
					ALLOC_ERR_OUT("Cloned image data");
				bcopy(srciih->image_data, dstiih->image_data,
				    srciih->image_length);
			} else {
				dstiih->image_data = NULL;
			}
		}
	}
	*dst = liibdb;
	return (0);
err_out:
	if (liibdb != NULL)
		free_iibdb(liibdb);
	return (-1);
}
