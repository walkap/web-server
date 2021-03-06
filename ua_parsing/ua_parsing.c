#include "ua_parsing.h"
#include "../image_processing/image_processing.h"

/**
 * Get the right property based on the argument property passed could be for example "ScreenPixelsWidth" or others
 * @param dataSet The dataset already initialized
 * @param offsets
 * @param property The property we want to see
 * @return A char pointer to the value of the property requested
 */
const char *get_property(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *offsets, char *property){
    int32_t requiredPropertyIndex;
    const char *value = NULL;
    //Returns the index in the array of required properties for this name, or -1 if not found.
    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(dataSet, property);
    if (requiredPropertyIndex >= 0 && requiredPropertyIndex <
                                      //Returns the number of properties that have been loaded in the dataset.
                                      fiftyoneDegreesGetRequiredPropertiesCount(dataSet)) {
        //Returns a pointer to the value for the property based on the device offsets provided.
        value = fiftyoneDegreesGetValuePtrFromOffsets(dataSet, offsets, requiredPropertyIndex);
    }
    return value;
}

/**
 * Reports the status of the data file initialization.
 * @param status
 * @param fileName
 */
void reportDatasetInitStatus(fiftyoneDegreesDataSetInitStatus status, const char* fileName) {
    switch (status) {
        case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
            printf("Insufficient memory to load '%s'.", fileName);
            break;
        case DATA_SET_INIT_STATUS_CORRUPT_DATA:
            printf("Device data file '%s' is corrupted.", fileName);
            break;
        case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
            printf("Device data file '%s' is not correct version.", fileName);
            break;
        case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
            printf("Device data file '%s' not found.", fileName);
            break;
        default:
            printf("Device data file '%s' could not be loaded.", fileName);
            break;
    }
}

/**
 * Init the global provider with properties
 * @param properties
 * @return int
 */
int init_provider(fiftyoneDegreesProvider *provider) {
    const char *fileName = DATA_PATH;
    const char *properties = PROPERTIES;
    //Init provider with property string
    fiftyoneDegreesDataSetInitStatus status =
            //Initialises the provider using the file provided and a string of properties.
            fiftyoneDegreesInitProviderWithPropertyString(fileName, provider, properties);
    //Check the init status and then return
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        reportDatasetInitStatus(status, fileName);
        fgetc(stdin);
        return 1;
    }
    return 0;
}


/**
 * This function get the device information about the screen size
 * @param ua_str The user agent string should be passed
 * @param size is used to save information device as an array of strings
 * @return int
 */
int get_screen_size_ua(const char *ua_str, int *size) {

    char *width, *ptr;

    //Init provider
    fiftyoneDegreesProvider provider;

    init_provider(&provider);
    //Data set
    fiftyoneDegreesDataSet *dataSet = provider.active->dataSet;
    //init the offset
    fiftyoneDegreesDeviceOffsets *offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
    //set the offset to 1, the numbers of record in the array
    offsets->size = 1;
    //Sets the offsets structure passed to the method for the User-Agent provided (or use this provider.active->dataSet)
    fiftyoneDegreesSetDeviceOffset(dataSet, ua_str, 0, offsets->firstOffset);
    //Get device size
    width = (char *)get_property(dataSet, offsets, "ScreenPixelsWidth");

    //Check if device size are available, if not then return 1
    if(strcmp(width, UNKNOWN) == 0){
        puts("The device size in unknown!");
        //free memory, offset and data set
        fiftyoneDegreesProviderFree(&provider);

        return 0;
    }

    //Initialize the element of the two values array
    if(strtol(width, &ptr, 0) > 1280) {
        *size = 1280;
    }else{
        int w;
        char *pt;
        w =  (int) strtol(width, &pt, 0);
        exit_on_error(*pt != '\0', "error in strtol width");
        *size = w;
    }

    //free memory, offset and data set
    fiftyoneDegreesProviderFree(&provider);

    return 1;
}

#if UA_PARSING_DEBUG
int main(int argc, char *argv[]) {
    const char **info = malloc(sizeof(char *) * 2);
    int value = get_info("User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:61.0) Gecko/20100101 Firefox/61.0", info);
    //int value = get_screen_size_ua(mobileUserAgent, info);
    if(value == 0){
        printf("The image size is: %sx%s", info[0], info[1]);
    }
    return 0;
}
#endif