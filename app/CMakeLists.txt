# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories(
        libs/libusb/android
        libs/libusb/libusb
        libs/libnrsc5/faad2-prefix/src/faad2_external/include
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad
        libs/librtlsdr/include
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(
        # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
)

add_library(
        # Sets the name of the library.
        usb1.0

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        libs/libusb/libusb/core.c
        libs/libusb/libusb/descriptor.c
        libs/libusb/libusb/hotplug.c
        libs/libusb/libusb/io.c
        libs/libusb/libusb/sync.c
        libs/libusb/libusb/strerror.c
        libs/libusb/libusb/os/linux_usbfs.c
        libs/libusb/libusb/os/poll_posix.c
        libs/libusb/libusb/os/threads_posix.c
        libs/libusb/libusb/os/linux_netlink.c
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        usb1.0

        ${log-lib}
)

# Adds the output of the additional CMake build as a prebuilt static
# library and names it fftw3f.
add_library( fftw3f SHARED IMPORTED )
set_target_properties( fftw3f PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libfftw3f.so )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/libs/fftw-3.3.7/install/${ANDROID_ABI}/include )

add_library(
        rtlsdr

        SHARED

        libs/librtlsdr/src/librtlsdr.c
        libs/librtlsdr/src/tuner_e4k.c
        libs/librtlsdr/src/tuner_fc0012.c
        libs/librtlsdr/src/tuner_fc0013.c
        libs/librtlsdr/src/tuner_fc2580.c
        libs/librtlsdr/src/tuner_r82xx.c
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        rtlsdr

        ${log-lib}
        usb1.0
)

add_library(
        faad2

        SHARED

        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/bits.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/cfft.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/common.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/decoder.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/drc.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/drm_dec.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/error.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/filtbank.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/hcr.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/huffman.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ic_predict.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/is.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/lt_predict.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/mdct.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/mp4.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ms.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/output.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/pns.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ps_dec.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ps_syntax.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/pulse.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/rvlc.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_dct.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_dec.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_e_nf.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_fbt.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_hfadj.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_hfgen.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_huff.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_qmf.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_syntax.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/sbr_tf_grid.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/specrec.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ssr.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ssr_fb.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/ssr_ipqf.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/syntax.c
        libs/libnrsc5/faad2-prefix/src/faad2_external/libfaad/tns.c
)

target_link_libraries(
        faad2

        ${log-lib}
)

add_library(
        # Sets the name of the library.
        nrsc5

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        libs/libnrsc5/src/acquire.c
        libs/libnrsc5/src/decode.c
        libs/libnrsc5/src/frame.c
        libs/libnrsc5/src/hdc_to_aac.c
        libs/libnrsc5/src/input.c
        libs/libnrsc5/src/main.c
        libs/libnrsc5/src/output.c
        libs/libnrsc5/src/pids.c
        libs/libnrsc5/src/sync.c
        libs/libnrsc5/src/firdecim_q15.c
        libs/libnrsc5/src/conv_dec.c
        libs/libnrsc5/src/reed-solomon.c
        libs/libnrsc5/src/galois.c
        libs/libnrsc5/src/log.c
        libs/libnrsc5/src/strndup.c
)

target_link_libraries(
        nrsc5

        ${log-lib}
        usb1.0
        rtlsdr
        faad2
        fftw3f
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp
)


target_link_libraries(
        # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        usb1.0
        rtlsdr
        nrsc5
        faad2
)
