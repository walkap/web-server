#include "image_processing.h"

/**
 * This function resize an image besides check the aspect ratio and return a proper image
 * with new dimensions as requested
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param width - image's width
 * @param height - image's height
 * @return return 0 if success
 */
int resize_image(MagickBooleanType status, MagickWand *magick_wand, size_t height, size_t width) {

    //This is used to maintain ratio and avoid distortion
    float_t aspect_ratio = ( float_t ) MagickGetImageWidth( magick_wand ) / MagickGetImageHeight( magick_wand );
    float_t new_ratio = ( float_t ) width / height;

    //Check if the ratio it is the same as the values requested
    if(aspect_ratio != new_ratio ){
        height = (size_t) (width / aspect_ratio);
    }

    //TODO we should check either if the image size requested it is not too short
    //Resize the image
    MagickResetIterator( magick_wand );
    while ( MagickNextImage( magick_wand ) != MagickFalse ){
        status = MagickResizeImage( magick_wand, width, height, UndefinedFilter );
        if(status == MagickFalse){
            ThrowWandException(magick_wand);
        }
    }
    return(0);
}

/**
 * This function compress an image based on the quality parameter
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param quality - quality requested for the image we want to compress
 * @return returns 0 if success
 */
int compress_image(MagickBooleanType status, MagickWand *magick_wand, float_t quality){
    //Compress the image
    status = MagickSetImageCompressionQuality( magick_wand, (size_t) quality *100);
    if( status == MagickFalse ){
        ThrowWandException( magick_wand );
    }
    return(0);
}

/**
 * This function write an image on the file system
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param newfilename - the new name we want to associate to the processed image
 * @return returns 0 if success
 */
int write_image(MagickBooleanType status, MagickWand *magick_wand, char *newfilename){

    char *current_directory = current_dir();
    char *image_folder = concat(current_directory, "/images/");
    char *dest = concat(image_folder, newfilename);
    //TODO we should think about the possibility to divide images processed in folders based on the image size
    status = MagickWriteImages( magick_wand, dest, MagickTrue );
    if ( status == MagickFalse ){
        ThrowWandException( magick_wand );
    }
    free(dest);
    free(image_folder);
    return(0);
}


/**
 * This function resize an image as the name suggest
 * @param source - Image's filename before the resizing
 * @param destination - Image's filename after resizing
 * @param width - Image's widht we want to achieve
 * @param height - Image's width we want to achieve
 * @param quality - Image's quality compression, set -1 if it does't required
 */
int process_image(char *source, char *destination, size_t width, size_t height, float_t quality){

    MagickBooleanType status;
    MagickWand *magick_wand;

    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    //TODO we should set another source for images instead of the project's root
    status = MagickReadImage( magick_wand, source );
    if ( status == MagickFalse ){
        ThrowWandException( magick_wand );
    }

    //Image resize
    resize_image(MagickFalse, magick_wand, height, width);

    //Image quality compression
    compress_image(status, magick_wand, quality);

    //Write the image then destroy it.
    write_image(status, magick_wand, destination);
    DestroyMagickWand( magick_wand );
    MagickWandTerminus();
    return(0);
}

//TODO this is just to try the process_image function, should be deleted as soon as we test it
int main(){
    process_image("scarpe.jpg", "bbb3.jpg", 600, 800, 0.5);
}