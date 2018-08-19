#ifndef WEB_SERVER_IMAGE_RESIZING_H
#define WEB_SERVER_IMAGE_RESIZING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include "../utils.h"

int resize_image(MagickBooleanType status, MagickWand *magick_wand, size_t height, size_t width);
int compress_image(MagickBooleanType status, MagickWand *magick_wand, float_t quality);
int write_image(MagickBooleanType status, MagickWand *magick_wand, char *filename);
char *rename_file(const char *filename, size_t width, size_t height);
int process_image(char *source, size_t width, size_t height, float_t quality);

#define ThrowWandException(wand){ \
  char *description; \
 \
  ExceptionType \
    severity; \
 \
  description = MagickGetException( wand, &severity ); \
  (void) fprintf( stderr, "%s %s %lu %s\n", GetMagickModule(), description ); \
  description = (char *) MagickRelinquishMemory( description ); \
  exit( -1 ); \
}
#endif //WEB_SERVER_IMAGE_RESIZING_H