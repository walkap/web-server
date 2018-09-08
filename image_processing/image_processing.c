#include "image_processing.h"

/**
 * This function resize an image besides check the aspect ratio and return a proper image
 * with new dimensions as requested
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param width - image's width
 * @param height - image's height
 */
void resize_image(MagickBooleanType status, MagickWand *magick_wand, size_t height, size_t width) {
    //This is used to maintain ratio and avoid distortion
    float aspect_ratio = (float_t) MagickGetImageWidth(magick_wand) / MagickGetImageHeight(magick_wand);
    float new_ratio = (float_t) width / height;
    //Check if the ratio it is the same as the values requested
    if (aspect_ratio != new_ratio) {
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
 * @param height
 * @return - char * returns a pointer to the new name
 */
char *rename_file(const char *filename, size_t width, size_t height) {
    //Check if the image size is equal to zero
    if (width == 0 || height == 0) {
        printf("The image size is zero");
        exit(EXIT_FAILURE);
    }
    //Check if size is too big
    if (width > 9999 || height > 9999) {
        printf("The image size should be <= 9999");
        exit(EXIT_FAILURE);
    }
    //Initialize array char to contain the complete new filename and the format
    char *newfilename;
    newfilename = calloc(strlen(filename), (sizeof(char)));
    if(newfilename == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    //Copy the file name without the format into the array char
    strncpy(newfilename, filename, strlen(filename) - 4);
    printf("newfilename %s\n", newfilename);

    //Add the image size and formate to the new file name array char
    sprintf(newfilename + strlen(filename) - 4, "-%ix%i.jpg", (int) width, (int) height);
    printf("newfilename %s\n", newfilename);
    return newfilename;
}

/**
 * This function resize an image as the name suggest
 * @param source - Image's filename before the resizing
 * @param destination - Image's filename after resizing
 * @param width - Image's widht we want to achieve
 * @param height - Image's width we want to achieve
 * @param quality - Image's quality compression, set -1 if it doesn't required
 * @return A blob to the image file
 */

unsigned char * process_image(char *source, size_t width, size_t height, float_t quality, size_t* newsize) {
    //This rename the image as the formats wants image-widthxheight.jpg
    char *destination = rename_file(source, width, height);
    MagickBooleanType status;
    MagickWand *magick_wand;
    unsigned char * blob;
    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();

    char *path = malloc(sizeof(char *) * (strlen(IMAGE_DIR) + strlen(source) + 1));
    if(path == NULL){
        perror("Malloc");
        exit(EXIT_FAILURE);
    }
    sprintf(path, "%s%s", IMAGE_DIR, source);
    status = MagickReadImage(magick_wand, path);
    if (status == MagickFalse) {
        ThrowWandException(magick_wand);
    }
    //Image resize
    resize_image(MagickFalse, magick_wand, height, width);
    //Check if quality value was set
    if(quality != -1){
        //Image quality compression
        compress_image(status, magick_wand, quality);
    }

    //Get the blob to return
    blob = MagickGetImageBlob(magick_wand, newsize);
    //Write the image then destroy it.
    write_image(status, magick_wand, destination);
    free(destination);
    DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return blob;
}

#if IMAGE_PRO_DEBUG
//TODO this is just for testing should be deleted as soon as we can
int main() {
    //TODO we could use always strings as parameter they are simpler to transform to int or size_t and not vice versa
    process_image("wizard.jpg", 350, 400, 0.5);
}
#endif
