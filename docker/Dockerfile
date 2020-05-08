FROM frolvlad/alpine-glibc:latest as build-stage

# dlib
ARG RUNTIME_DEPS='libpng libjpeg-turbo giflib openblas libx11'
ARG BUILD_DEPS='wget unzip cmake build-base linux-headers libpng-dev libjpeg-turbo-dev giflib-dev openblas-dev libx11-dev'
ARG LIB_PREFIX='/usr/local'
ARG VERSION=19.18

ENV DLIB_VERSION=${VERSION} \
    LIB_PREFIX=${LIB_PREFIX} \
    DLIB_INCLUDE_DIR='$LIB_PREFIX/include' \
    DLIB_LIB_DIR='$LIB_PREFIX/lib'

RUN echo "Dlib: ${DLIB_VERSION}" \
    && rm -rf /usr/local/lib && ln -s /usr/local/lib64 /usr/local/lib \
    && apk add -u --no-cache $RUNTIME_DEPS \
    && apk add -u --no-cache --virtual .build-dependencies $BUILD_DEPS 

RUN wget -q https://github.com/davisking/dlib/archive/v${DLIB_VERSION}.zip -O dlib.zip 

RUN dlib_cmake_flags="-D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=$LIB_PREFIX \
    -D DLIB_NO_GUI_SUPPORT=OFF \
    -D DLIB_USE_BLAS=ON \
    -D DLIB_GIF_SUPPORT=ON \
    -D DLIB_PNG_SUPPORT=ON \
    -D DLIB_JPEG_SUPPORT=ON \
    -D DLIB_USE_CUDA=OFF" 

RUN unzip -qq dlib.zip \
    && mv dlib-${DLIB_VERSION} dlib \
    && rm dlib.zip \
    && cd dlib \
    && mkdir -p build \
    && cd build \
    && cmake $dlib_cmake_flags .. \
    && make -j $(getconf _NPROCESSORS_ONLN) \
    && cd /dlib/build \
    && make install \
    && cp /dlib/dlib/*.txt $LIB_PREFIX/include/dlib/ \
    && cd / \
    && rm -rf /dlib 

RUN /usr/glibc-compat/sbin/ldconfig \
    && apk del .build-dependencies \
    && rm -rf /var/cache/apk/* /usr/share/man /usr/local/share/man /tmp/*

# opencv
RUN apk add --no-cache ca-certificates
# required to install opencv
RUN apk add --no-cache \
    git \
    build-base \
    musl-dev \
    alpine-sdk \
    cmake clang \
    clang-dev \
    make \
    gcc \
    g++ \
    libc-dev \
    linux-headers \
    ffmpeg-libs \
    ffmpeg-dev

# install opencv
#RUN mkdir /tmp/
WORKDIR /tmp/
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/3.4.1.zip
RUN unzip opencv.zip
RUN mkdir /tmp/opencv-3.4.1/build

WORKDIR /tmp/opencv-3.4.1/build/
RUN cmake -D CMAKE_BUILD_TYPE=RELEASE  \ 
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    # No examples
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D INSTALL_C_EXAMPLES=OFF \
    # added --
    -D WITH_IPP=OFF \
    -D WITH_1394=OFF \
    -D WITH_LIBV4L=OFF \
    -D WITH_V4l=OFF \
    -D WITH_TBB=OFF \
    -D WITH_FFMPEG=ON  \
    -D WITH_GPHOTO2=OFF \
    -D WITH_GSTREAMER=OFF \
    # added this as well
    -D BUILD_SHARED_LIB=OFF \
    #added --   
    -D BUILD_DOCS=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_opencv_java=OFF \
    -D BUILD_opencv_python=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_calib3d=OFF \
    -D BUILD_opencv_flann=OFF \
    -D BUILD_opencv_photo=OFF \
    -D BUILD_opencv_shape=OFF \
    -D BUILD_opencv_features2d=OFF \
    -D BUILD_opencv_stitching=OFF \
    -D BUILD_opencv_superres=OFF \
    -D BUILD_opencv_videostab=OFF \
    -D BUILD_opencv_video=OFF \
    -D BUILD_opencv_python_bindings_generator=OFF \ 
    -D BUILD_opencv_java_bindings_generator=OFF \
    -D BUILD_opencv_dnn=OFF \
    -D BUILD_ANDROID_EXAMPLES=OFF ..
RUN make -j $(getconf _NPROCESSORS_ONLN) && make install
RUN rm -rf /tmp/opencv

# install superlu
WORKDIR /tmp/
RUN apk add gfortran
RUN wget -q https://github.com/xiaoyeli/superlu/archive/master.zip  -O superlu.zip
RUN unzip superlu.zip
# BUILD_SHARED_LIBS necessary to avoid R_X86_64_PC32 error
WORKDIR /tmp/superlu-master
RUN mkdir build && cd build && cmake -D BUILD_SHARED_LIBS=1 .. && make && make install

# install armadillo 
WORKDIR /tmp/
RUN apk add -u --no-cache --virtual .build-dependencies cmake build-base
RUN wget -q http://sourceforge.net/projects/arma/files/armadillo-9.700.2.tar.xz  -O armadillo-9.700.2.tar.xz
RUN tar -xvf armadillo-9.700.2.tar.xz
WORKDIR /tmp/armadillo-9.700.2
RUN cmake -DCMAKE_BUILD_TYPE=Release . && make && make install
    
# required for libX11
RUN apk update && apk add -u --no-cache libx11-dev giflib-dev libpng-dev libjpeg-turbo-dev openblas-dev 
WORKDIR /tmp/
RUN wget -q https://github.com/jibietr/image-enhancement/archive/master.zip -O  image-enhance.zip
RUN unzip image-enhance.zip
WORKDIR /tmp/image-enhancement-master 
RUN cmake . && make -j

RUN ldd image-enhancer | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'mkdir -p $(dirname deps%); cp % deps%;'

# runtime stage
FROM alpine:3.11

# copy necessary libraries
RUN mkdir /tmp/image-enhancement
COPY --from=build-stage /tmp/image-enhancement-master/deps /tmp/image-enhancement/deps
COPY --from=build-stage /tmp/image-enhancement-master/image-enhancer /tmp/image-enhancement/image-enhancer


ENV LD_LIBRARY_PATH /tmp/image-enhancement/deps/lib/:/tmp/image-enhancement/deps/usr/lib/:/tmp/image-enhancement/deps/usr/local/lib64/

CMD exec /bin/sh -c "trap : TERM INT; sleep 360000000 & wait"
