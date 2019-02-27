#!/bin/sh

# check args
if [ $# -ne 1 ]; then
    echo "Usage: $0 <build configure file>"
    exit 1
fi

if [ ! -f $1 ]; then
    echo "Invalid filename $1"
    exit 2
fi

# load compile configure
. $1

# global
THREAD_COUNT=`cat /proc/cpuinfo| grep "processor"| wc -l`

WORK_PATH="${PWD}"
APP_PATH="${WORK_PATH}/app"
THIRD_PATH="${WORK_PATH}/app/third"
LIBRARIRS_PATH="${WORK_PATH}/libraries"

compile_openh264()
{
	echo ">>>>> compile_openh264"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/openh264" ]; then
		rm -rf "${THIRD_PATH}/openh264"
	fi
	mkdir -p "openh264"
	cd "${LIBRARIRS_PATH}/openh264"
	
	DES_DIR="${THIRD_PATH}/openh264"
	
	make -j${THREAD_COUNT}
	echo ">>>>> install openh264"
	make DES_DIR=${PREFIX} PREFIX="${THIRD_PATH}/openh264" install
	
	unset PREFIX
}

compile_fdk_aac()
{
	echo ">>>>> compile_fdk_aac"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/fdk-aac" ]; then
		rm -rf "${THIRD_PATH}/fdk-aac"
	fi
	mkdir -p "fdk-aac"
	cd "${LIBRARIRS_PATH}/fdk-aac"
	
	PREFIX="${THIRD_PATH}/fdk-aac"
	COMFIGURE_CMD="--prefix=${PREFIX}"
	
	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install fdk-aac"
	make install
	
	unset PREFIX
	unset COMFIGURE_CMD
}

compile_xcb_proto()
{
	echo ">>>>> compile_xcb_proto"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/libxcb" ]; then
		rm -rf "${THIRD_PATH}/xcb_proto"
	fi
	mkdir -p "xcb_proto"
	cd "${LIBRARIRS_PATH}/xcb-proto"	
	
	PREFIX="${THIRD_PATH}/xcb_proto"
	COMFIGURE_CMD="--prefix=${PREFIX}"
	
	autoreconf
	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install xcb_proto"
	make install
	
	unset PREFIX
	unset COMFIGURE_CMD
}

compile_libxcb()
{
	compile_xcb_proto
	
	echo ">>>>> compile_libxcb"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/libxcb" ]; then
		rm -rf "${THIRD_PATH}/libxcb"
	fi
	mkdir -p "libxcb"
	cd "${LIBRARIRS_PATH}/libxcb"
	
	PREFIX="${THIRD_PATH}/libxcb"
	XPROTO_LDPATH="${THIRD_PATH}/xcb_proto/lib/pkgconfig"
	COMFIGURE_CMD="--prefix=${PREFIX} PKG_CONFIG_PATH=${XPROTO_LDPATH}"
	
	autoscan
	aclocal
	automake –add-missing
	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install openh264"
	make install
	
	unset PREFIX
	unset COMFIGURE_CMD
}

compile_alsa()
{
	echo ">>>>> compile_alsa"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/alsa" ]; then
		rm -rf "${THIRD_PATH}/alsa"
	fi
	mkdir -p "alsa"
	cd "${LIBRARIRS_PATH}/alsa"
	pwd
	PREFIX="${THIRD_PATH}/alsa"
	COMFIGURE_CMD="--prefix=${PREFIX} --enable-static=yes --enable-shared=no"
	
	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install alsa"
	make install
	
	unset PREFIX
	unset COMFIGURE_CMD
}

# plist compiler
compile_libxml2()
{
	echo ">>>>> compile_libxml2"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/libxml2" ]; then
		rm -rf "${THIRD_PATH}/libxml2"
	fi
	mkdir -p "libxml2"
	cd "${LIBRARIRS_PATH}/libxml2"
	
	PREFIX="${THIRD_PATH}/libxml2"
	CFLAGS="-I/usr/include/python2.7"
	COMFIGURE_CMD="--prefix=${PREFIX} CFLAGS=${CFLAGS}"
	
	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install libxml2"
	make install
	
	unset CFLAGS
	unset PREFIX
	unset COMFIGURE_CMD
}

compile_plist()
{
	compile_libxml2
	echo ">>>>> compile_libplist"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/libplist" ]; then
		rm -rf "${THIRD_PATH}/libplist"
	fi
	mkdir -p "libplist"
	cd "${LIBRARIRS_PATH}/libplist"
	pwd
	rm -rf "CMakeCache.txt"
	
	cmake .  -DCMAKE_INSTALL_PREFIX=""
	make -j${THREAD_COUNT}
	echo ">>>>> install libplist"
	make DESTDIR="${THIRD_PATH}/libplist" PREFIX= install
	
	unset PREFIX
	unset COMFIGURE_CMD
}


compile_ffmpeg()
{
	#compile_libxcb
	compile_openh264
	#compile_fdk_aac
	#compile_alsa
	
	echo ">>>>> compile_ffmpeg"
	cd $THIRD_PATH
	if [ -d "${THIRD_PATH}/ffmpeg" ]; then
		rm -rf "${THIRD_PATH}/ffmpeg"
	fi
	mkdir -p "ffmpeg"
	cd "${LIBRARIRS_PATH}/ffmpeg"
	
	PREFIX="${THIRD_PATH}/ffmpeg"
	FDK_AAC_LDPATH="${THIRD_PATH}/fdk-aac/lib/pkgconfig"
	H264_LDPATH="${THIRD_PATH}/openh264/lib/pkgconfig"
	XCB_LDPATH="${THIRD_PATH}/libxcb/lib/pkgconfig"
	ALSA_LDPATH="${THIRD_PATH}/alsa/lib/pkgconfig"
	PKG_CONFIG_DIR="${FDK_AAC_LDPATH}:${H264_LDPATH}:${XCB_LDPATH}:${ALSA_LDPATH}"
	export PKG_CONFIG_PATH=${PKG_CONFIG_DIR}
	echo ${PKG_CONFIG_DIR}
	COMFIGURE_CMD="--prefix=${PREFIX} --enable-libfdk-aac --enable-libopenh264 --enable-libxcb --enable-libxcb-shm --enable-libxcb-xfixes
		--enable-libxcb-shape --enable-gpl --enable-nonfree --disable-zlib --disable-bzlib --disable-iconv --disable-avfilter
		--pkg-config=/usr/bin/pkg-config  --arch=${ARCH} --cpu=${CPU} --target-os=${Platform} --disable-doc --disable-htmlpages
		--disable-manpages --disable-podpages --disable-txtpages --disable-postproc --disable-swresample --disable-swscale
		--disable-ffmpeg --disable-ffplay --disable-ffprobe --disable-programs --disable-xlib --disable-sdl2 --disable-bzlib"

	./configure ${COMFIGURE_CMD}
	
	make -j${THREAD_COUNT}
	echo ">>>>> install ffmpeg"
	make install
	
	unset PREFIX
	unset COMFIGURE_CMD
}

compile_app()
{
	echo ">>>>>>compiler third env"
	#compile_plist
	compile_ffmpeg
	echo ">>>>>>compiler finish"
	
	cd ${WORK_PATH}
	
	rm "CMakeCache.txt"
	cmake . \
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
		-DCMAKE_C_FLAGS="${CMAKE_C_FLAGS}" \
        -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}" \
		-DARCH=${ARCH} \
		
	make -j4
	
}

compile_app
