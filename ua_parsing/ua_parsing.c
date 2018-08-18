#include "ua_parsing.h"

//Global provider
static fiftyoneDegreesProvider provider;

int init_provider();
const char *get_property(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *offsets, char *property);

/**
 * This function get the device information about the screen size
 * @param ua_str The user agent string should be passed
 * @return Return a pointer to an array of string element 1 is the width size
 * element 2 is the height size
 */
const char **get_info(const char* ua_str){
    //Values we want to return
    const char *width, *height;
    //Allocate enough mem for hte two dimensions
    const char **info = malloc(sizeof(char *) * 2);
    //Init provider
    init_provider();
    //Data set
    fiftyoneDegreesDataSet *dataSet = provider.active->dataSet;
    //init the offset
    fiftyoneDegreesDeviceOffsets *offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
    //set the offset to 1, the numbers of record in the array
    offsets->size = 1;
    //Sets the offsets structure passed to the method for the User-Agent provided (or use this provider.active->dataSet)
    fiftyoneDegreesSetDeviceOffset(dataSet, ua_str, 0, offsets->firstOffset);
    //Get device size
    width = get_property(dataSet, offsets, "ScreenPixelsWidth");
    height = get_property(dataSet, offsets, "ScreenPixelsHeight");
    //Initialize the element of the two values array
    info[0] = width;
    info[1] = height;
    //free memory, offset and data set
    fiftyoneDegreesFreeDeviceOffsets(offsets);
    return info;
}

/**
 * Init the global provider with properties
 * @param properties
 * @return return 0 if init is ok
 */
int init_provider() {
    const char *fileName = DATA_PATH;
    const char *properties = PROPERTIES;
    //Init provider with property string
    fiftyoneDegreesDataSetInitStatus status =
            //Initialises the provider using the file provided and a string of properties.
            fiftyoneDegreesInitProviderWithPropertyString(fileName, &provider, properties);
    //Check the init status and then return
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        reportDatasetInitStatus(status, fileName);
        fgetc(stdin);
        return 1;
    }
    return 0;
}

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
    //const char *property = "ScreenPixelsWidth";
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

//TODO this is just for testing should be deleted as soon as we can
int main(int argc, char *argv[]) {
    const char **value = get_info(mobileUserAgent);
    printf("ciao %s", value[0]);
}