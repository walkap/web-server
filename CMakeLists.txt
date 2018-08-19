cmake_minimum_required(VERSION 3.10)
project(web-server C)

include_directories(/usr/local/include/ImageMagick-7)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra -O2 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16")

set(TRIE ua_parsing/src/trie/51Degrees.c
        ua_parsing/src/trie/51Degrees.h cache/cache.h)

set(CITYHASH
        ua_parsing/src/cityhash/city.c
        ua_parsing/src/cityhash/city.h)

set(PATTERN ua_parsing/src/pattern/51Degrees.c
        ua_parsing/src/pattern/51Degrees.h
        ${CITYHASH})

set(THREADING
        ua_parsing/src/threading.h
        ua_parsing/src/threading.c)

find_package(ImageMagick COMPONENTS MagickWand)

add_executable(main
        server/server.c)

add_executable(ua
        ua_parsing/ua_parsing.c
        ${TRIE})

add_executable(adapt
        image_processing/image_processing.c
        image_processing/image_processing.h)

target_link_libraries(adapt ${ImageMagick_LIBRARIES})
