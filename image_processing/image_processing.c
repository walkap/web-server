#include "image_processing.h"

/**
 * Get the screen size and adapt the width based on some standard values
 * @param width
 * @return The new width
 */
size_t set_width(size_t width) {
    if (width <= 320) {
        width = XSMALL_IMAGE;
    } else if (width > 320 && width <= 480) {
        width = SMALL_IMAGE;
    } else if (width > 480 && width <= 768) {
        width = MEDIUM_IMAGE;
    } else if (width > 768 && width <= 1024) {
        width = LARGE_IMAGE;
    } else {
        width = XLARGE_IMAGE;
    }
    return width;
}

/**
 * This function resize an image besides check the aspect ratio and return a proper image
 * with new dimensions as requested
 * @param status - MagickBooleanType
 * @param magick_wand - the magick wand object associated to the image we want to resize
 * @param width - image's width
 * @param height - image's height
 */
void resize_image(MagickBooleanType status, MagickWand *magick_wand, size_t width) {
    size_t original_width = MagickGetImageWidth(magick_wand);
    size_t original_height = MagickGetImageHeight(magick_wand);
    float aspect_ratio;
    size_t height;

    //This is used to maintain ratio and avoid distortion
    aspect_ratio = (float_t) original_width / original_height;
    height = (size_t) (width / aspect_ratio);
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
void compress_image(MagickBooleanType status, MagickWand *magick_wand, double quality) {
    //Compress the image
    const size_t compression =  (const size_t) (quality * 100);
    status = MagickSetImageCompressionQuality(magick_wand,compression);
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
    if (target == NULL) {
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
    char *new_filename;
    //Initialize array char to contain the complete new filename and the format
    new_filename = calloc(strlen(filename), (sizeof(char)));
    if (new_filename == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    //Copy the file name without the format into the array char
    strncpy(new_filename, filename, strlen(filename) - 4);
    //Add the image size and formate to the new file name array char
    if (sprintf(new_filename + strlen(filename) - 4, "-%iw.jpg", (int) width) < 0) {
        perror("sprintf");
        exit(EXIT_FAILURE);
    }
    return new_filename;
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
unsigned char *process_image(char *source, size_t width, double quality, size_t *newsize) {
    unsigned char *blob;
    char *path, *destination;
    MagickBooleanType status;
    MagickWand *magick_wand;

    //Read an image.
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    //Allocate enough memory for image path
    path = malloc(sizeof(char *) * (strlen(IMAGE_DIR) + strlen(source) + 1));
    if (path == NULL) {
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
    //if (quality != -1) {
        //Image quality compression
        compress_image(status, magick_wand, quality);
    //}
    //Set the right size based on some standard values
    width = set_width(width);
    //Resize the image with the new width
    resize_image(MagickFalse, magick_wand, width);
    //Name the new image
    destination = rename_file(source, width);
    //Write the image then destroy it.
    write_image(status, magick_wand, destination);

    //Get the blob to return
    blob = MagickGetImageBlob(magick_wand, newsize);

    //Destroy the magick wand
    // DestroyMagickWand(magick_wand); //TODO disable to avoid an error
    MagickWandTerminus();
    return blob;
}

#if IMAGE_PRO_DEBUG
int main() {
    size_t *imgsize = malloc(sizeof(size_t));
    process_image("/avengers.jpg", 767,0.01,imgsize);
}
#endif