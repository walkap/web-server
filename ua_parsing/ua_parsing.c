#include "ua_parsing.h"

static fiftyoneDegreesProvider provider;
const char* mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
                               "AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
                               "Safari/9537.53");
static void reportDatasetInitStatus(
        fiftyoneDegreesDataSetInitStatus status,
        const char* fileName);
const char* getPixelWidth(fiftyoneDegreesDataSet* dataSet, fiftyoneDegreesDeviceOffsets *offsets);

int init_provider(const char* properties){
    const char* fileName = "../ua_parsing/src/51Degrees-LiteV3.4.trie";
    //Init provider with property string
    fiftyoneDegreesDataSetInitStatus status =
            fiftyoneDegreesInitProviderWithPropertyString(
                    fileName, &provider, properties);
    //Check the init status
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        reportDatasetInitStatus(status, fileName);
        fgetc(stdin);
        return 1;
    }
    return 0;
}
void parse_ua(const char *ua_string, fiftyoneDegreesDataSet* dataSet){
    const char* pixelWidth;
    fiftyoneDegreesDeviceOffsets *offsets;
    offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
    offsets->size = 1;
    fiftyoneDegreesSetDeviceOffset(provider.active->dataSet, ua_string, 0, offsets->firstOffset);
    pixelWidth = getPixelWidth(dataSet, offsets);
    printf("The width is: %s\n", pixelWidth);
    fiftyoneDegreesFreeDeviceOffsets(offsets);
}

const char* getPixelWidth(fiftyoneDegreesDataSet* dataSet, fiftyoneDegreesDeviceOffsets *offsets){
    int32_t requiredPropertyIndex;
    const char* screenPixelWidth = NULL;
    const char* property = "ScreenPixelsWidth";
    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(dataSet, property);
    if (requiredPropertyIndex >= 0 &&
        requiredPropertyIndex <
        fiftyoneDegreesGetRequiredPropertiesCount(dataSet)) {
        screenPixelWidth = fiftyoneDegreesGetValuePtrFromOffsets(
                dataSet,
                offsets,
                requiredPropertyIndex);
    }
    return screenPixelWidth;
}

int main(int argc, char *argv[]){
    init_provider("ScreenPixelsWidth");
    parse_ua(mobileUserAgent,provider.active->dataSet);
}


/**
* Reports the status of the data file initialization.
*/
static void reportDatasetInitStatus(fiftyoneDegreesDataSetInitStatus status,
                                    const char* fileName) {
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