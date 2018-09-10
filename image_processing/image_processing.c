#include "image_processing.h"

/**
 * This function resize an image besides check the aspect ratio and return a proper image
 * with new dimensions as requested
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param width - image's width
 * @param height - image's height
 */
void resize_image(MagickBooleanType status, MagickWand *magick_wand, size_t width, size_t height) {
    size_t original_width = MagickGetImageWidth(magick_wand);
    size_t original_height = MagickGetImageHeight(magick_wand);
    float aspect_ratio, new_aspect_ratio;

    //TODO this should not be replicated it should be in a external function, see also rename_image
    //TODO use standard sizes such as 800 480 320
    //This is used to maintain ratio and avoid distortion
    aspect_ratio = (float_t) original_width / original_height;
    new_aspect_ratio = (float_t) width / height;

    //Check if the ratio it is the same as the values requested
    if (aspect_ratio != new_aspect_ratio) {
        height = (size_t) (width / aspect_ratio);
    }

    //Resize the image
    MagickResetIterator(magick_wand);
    while (MagickNextImage(magick_wand) != MagickFalse) {
        status = MagickResizeImage(magick_wand, width, height, UndefinedFilter);
        if (status == MagickFalse) {
            ThrowWandException(magick_wand);
        }
    }
}

/**
 * This function compress an image based on the quality parameter
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param quality - quality requested for the image we want to compress
 */
void compress_image(MagickBooleanType status, MagickWand *magick_wand, float_t quality) {
    //Compress the image
    status = MagickSetImageCompressionQuality(magick_wand, (size_t) quality * 100);
    //Check the compressing status
    if (status == MagickFalse) {
        ThrowWandException(magick_wand);
    }
}

/**
 * This function write an image on the file system
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param filename - the new name we want to associate to the processed image
 */
void write_image(MagickBooleanType status, MagickWand *magick_wand, char *filename) {
    char *target = malloc(strlen(IMAGE_DIR) + strlen(filename) + 1);
    if(target == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    //Concatenate strings
    sprintf(target, "%s%s", IMAGE_DIR, filename);
    //Write the image on the file system
    status = MagickWriteImages(magick_wand, target, MagickTrue);
    //Check the writing status
    if (status == MagickFalse) {
        ThrowWandException(magick_wand);
    }
    //Free the memory allocated with malloc
    free(target);
}

/**
 * This function rename an image after the image size
 * @param filename - older image name
 * @param width
 * @return - char * returns a pointer to the new name
 */
char *rename_file(const char *filename, size_t width) {
    char *newfilename;
    //Initialize array char to contain the complete new filename and the format
    newfilename = calloc(strlen(filename), (sizeof(char)));
    if(newfilename == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    //Copy the file name without the format into the array char
    strncpy(newfilename, filename, strlen(filename) - 4);
    //Add the image size and formate to the new file name array char
    if( sprintf(newfilename + strlen(filename) - 4, "-%iw.jpg", (int) width) < 0){
        perror("sprintf");
        exit(EXIT_FAILURE);
    }
    return newfilename;
}

/**
 * This function resize an image as the name suggest
 * @param source - Image's filename before the resizing
 * @param destination - Image's filename after resizing
 * @param width - Image's widht we want to achieve
 * @param height - Image's width we want to achieve
 * @param quality - Image's quality compression, set -1 if it doesn't required
 * @param newsize - The size of the new blob
 * @return A blob to the image file
 */
unsigned char * process_image(char *source, size_t width, size_t height, float_t quality, size_t* newsize) {
    unsigned char * blob;
    char *path, *destination;
    size_t old_image_width;
    MagickBooleanType status;
    MagickWand *magick_wand;

    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();

    //Keep track of the original image width
    old_image_width = MagickGetImageWidth(magick_wand);

    //Allocate enough memory for image path
    path = malloc(sizeof(char *) * (strlen(IMAGE_DIR) + strlen(source) + 1));
    if(path == NULL){
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    //Concatenates relative path with image name
    sprintf(path, "%s%s", IMAGE_DIR, source);
    status = MagickReadImage(magick_wand, path);
    if (status == MagickFalse) {
        ThrowWandException(magick_wand);
    }
    free(path);

    //Check if quality value was set
    if(quality != -1){
        //Image quality compression
        compress_image(status, magick_wand, quality);
    }

    //Image resize only if the ua width is less than the original image
    if(width < old_image_width){
        resize_image(MagickFalse, magick_wand, width, height);
        //Name the new image
        destination = rename_file(source, width);
        //Write the image then destroy it.
        write_image(status, magick_wand, destination);
    }

    //Get the blob to return
    blob = MagickGetImageBlob(magick_wand, newsize);

    //Destroy the magick wand
    DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return blob;
}

#if IMAGE_PRO_DEBUG
int main() {
    size_t *imgsize = malloc(sizeof(size_t));
    //TODO we don't need height at all
    process_image("/wizard.jpg", 1280, 500, 0.5, imgsize);
}
#endif