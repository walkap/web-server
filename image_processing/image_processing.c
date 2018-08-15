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
 * @param filename - the new name we want to associate to the processed image
 * @return returns 0 if success
 */
int write_image(MagickBooleanType status, MagickWand *magick_wand, char *filename){

    char *current_directory = current_dir();
    char *image_folder = concat(current_directory, "/images/");
    char *destination = concat(image_folder, filename);
    //TODO we should think about the possibility to divide images processed in folders based on the image size
    status = MagickWriteImages( magick_wand, destination, MagickTrue );
    if ( status == MagickFalse ){
        ThrowWandException( magick_wand );
    }
    free(destination);
    free(image_folder);
    return(0);
}

/**
 * This function rename an image after the image size
 * @param filename - older image name
 * @param width
 * @param height
 * @return - char * returns a pointer to the new name
 */
char *rename_file(const char *filename, size_t width, size_t height){

    if(width == 0 || height == 00){
        printf("The image size is zero");
        exit(EXIT_FAILURE);
    }

    //Check if size is too big
    if(width > 9999 || height > 9999){
        printf("The image size should be <= 9999");
        exit(EXIT_FAILURE);
    }

    //Initialize array chat to contain the complete new filename and the format
    char *newfilename;

    newfilename = malloc(strlen(filename) + 11);

    //Copy the file name without the format into the array char
    strncpy(newfilename, filename, strlen(filename) - 4);

    //Add the image size and formate to the new file name array char
    sprintf(newfilename + strlen(filename) - 4, "-%dx%d.jpg", (int) width, (int) height);

    printf("%s\n", newfilename);

    return newfilename;
}

/**
 * This function resize an image as the name suggest
 * @param source - Image's filename before the resizing
 * @param destination - Image's filename after resizing
 * @param width - Image's widht we want to achieve
 * @param height - Image's width we want to achieve
 * @param quality - Image's quality compression, set -1 if it doesn't required
 */

//TODO the destination shouldn't be chosen but set from size
int process_image(char *source, char *destination, size_t width, size_t height, float_t quality){

    //This rename the image as the formats wants image-widthxheight.jpg
    char *newdestination = rename_file(destination, width, height);

    MagickBooleanType status;
    MagickWand *magick_wand;

    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    //TODO we should set another source for images instead of the project's root -> the source should be chosen by the caller
    status = MagickReadImage( magick_wand, source );
    if ( status == MagickFalse ){
        ThrowWandException( magick_wand );
    }

    //Image resize
    resize_image(MagickFalse, magick_wand, height, width);

    //Image quality compression
    compress_image(status, magick_wand, quality);

    //TODO need to concatenate widthxheight to filename and return the new filename,
    //Write the image then destroy it.
    write_image(status, magick_wand, newdestination);
    DestroyMagickWand( magick_wand );
    free(newdestination);
    MagickWandTerminus();
    return(0);
}

//TODO this is just to try the process_image function, should be deleted as soon as we test it
int main(){
    process_image("scarpe.jpg", "bbb3.jpg", 600, 800, 0.5);
}