#include "image_resizing.h"

/**
 * This function resize an image as the name suggest
 * @param filename - Image's filename before the resizing
 * @param newfilename - Image's filename after resizing
 * @param width - Image's widht we want to achieve
 * @param height - Image's width we want to achieve
 */
int resize_image(char *filename, char *newfilename, const size_t width, const size_t height){

#define ThrowWandException(wand) \
{ \
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

    MagickBooleanType status;
    MagickWand *magick_wand;

    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    status = MagickReadImage( magick_wand, filename );
    if ( status == MagickFalse )
    ThrowWandException( magick_wand );

    //Turn the images into a thumbnail sequence.
    MagickResetIterator( magick_wand );
    while ( MagickNextImage( magick_wand ) != MagickFalse ){
        MagickResizeImage( magick_wand, height, width, UndefinedFilter );
    }

    //Write the image then destroy it.
    status = MagickWriteImages( magick_wand, newfilename, MagickTrue );
    if ( status == MagickFalse )
    ThrowWandException( magick_wand );
    magick_wand = DestroyMagickWand( magick_wand );
    MagickWandTerminus();
    return(0);
}

//TODO this is just to try the resize_image function, should be deleted as soon as we test it
int main(){
    resize_image("aaa.jpg", "bbb.jpg", 10, 10);
}