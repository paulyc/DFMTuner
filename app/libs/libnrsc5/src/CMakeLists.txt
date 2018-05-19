#add_compile_options (--std=gnu11 -O3 -Wall)
add_compile_options (-O3 -Wall)

#configure_file (config.h.in config.h)
include_directories ("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("../../../libs/include")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        libnrsc5

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
	acquire.c
    decode.c
    frame.c
    hdc_to_aac.c
    input.c
    main.c
    output.c
    pids.c
    sync.c

    firdecim_q15.c

    conv_dec.c

    reed-solomon.c
    galois.c

    log.c

    strndup.c
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
		log-lib

		# Specifies the name of the NDK library that
		# you want CMake to locate.
		log)

set(AO_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lib/libao.a)
set(FFTW3F_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lib/libfftw3f.a)
set(RTL_SDR_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lib/librtlsdr.a)
set(FAAD2_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lib/libfaad.a)
set(LIBUSB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lib/libusb1.0.a)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries ( # Specifies the target library.
    libnrsc5
		# Links the target library to the log library
		# included in the NDK.
		${log-lib}
    ${FAAD2_LIBRARY}
    ${AO_LIBRARY}
    ${FFTW3F_LIBRARY}
    ${RTL_SDR_LIBRARY}
		${LIBUSB_LIBRARY}
    m
)

#install (
#    TARGETS libnrsc
#    RUNTIME DESTINATION lib
#)
