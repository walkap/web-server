#ifndef WEB_SERVER_USER_AGENT_PARSING_H
#define WEB_SERVER_USER_AGENT_PARSING_H

#include "../utils.h"
#include "src/trie/51Degrees.h"
#include "src/pattern/51Degrees.h"


#define UA_PARSING_DEBUG 0
#define DATA_PATH "../ua_parsing/src/51Degrees-LiteV3.4.trie";
#define PROPERTIES "ScreenPixelsWidth,ScreenPixelsHeight,PlatformName,IsMobile"
#define UNKNOWN "Unknown"

int get_screen_size_ua(const char *ua_str, int *size);

#endif //WEB_SERVER_USER_AGENT_PARSING_H