################################################################################
#
#   Builder environnement
#
################################################################################

FROM centos as builder

RUN yum update -y && \
    yum upgrade -y

RUN yum group install -y "Development Tools" && \
    yum install -y cmake git

COPY ./OpenCV /tmp/NFIQ2/OpenCV
RUN mkdir /tmp/NFIQ2/libOpenCV && \
	cd /tmp/NFIQ2/libOpenCV && \
	cmake -D CMAKE_MAKE_PROGRAM=make ../OpenCV && \
	make opencv_core opencv_ts opencv_imgproc opencv_highgui opencv_flann \
	     opencv_features2d opencv_calib3d opencv_ml opencv_video opencv_objdetect \
	     opencv_contrib opencv_nonfree opencv_gpu opencv_photo opencv_stitching \
	     opencv_videostab

COPY . /tmp/NFIQ2
WORKDIR /tmp/NFIQ2
RUN mkdir -p /usr/local/man/man1
RUN make
RUN make install

################################################################################
#
#   Running environnement
#
################################################################################

FROM centos

COPY --from=builder /tmp/NFIQ2/NFIQ2/NFIQ2Algorithm/bin/ /NFIQ2/NFIQ2/bin/
COPY --from=builder /tmp/NFIQ2/NFIQ2/NFIQ2Algorithm/lib/ /NFIQ2/NFIQ2/lib/
COPY --from=builder /tmp/NFIQ2/biomdi/common/lib/        /NFIQ2/biomdi/common/lib/
COPY --from=builder /tmp/NFIQ2/biomdi/fingerminutia/lib/ /NFIQ2/biomdi/fingerminutia/lib/
COPY --from=builder /tmp/NFIQ2/libOpenCV/lib/            /NFIQ2/libOpenCV/lib/

ENV LD_LIBRARY_PATH=/NFIQ2/libOpenCV/lib:/NFIQ2/biomdi/common/lib:/NFIQ2/biomdi/fingerminutia/lib:/NFIQ2/NFIQ2/lib/

RUN ln -s /NFIQ2/NFIQ2/bin/NFIQ2 /usr/bin/NFIQ2
