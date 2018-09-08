#ifndef WEB_SERVER_IMAGE_RESIZING_H
#define WEB_SERVER_IMAGE_RESIZING_H

#include "../utils.h"

#define IMAGE_PRO_DEBUG 1


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

unsigned char * process_image(char *source, size_t width, size_t height, float_t quality, size_t* size);

#endif //WEB_SERVER_IMAGE_RESIZING_H