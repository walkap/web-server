#ifndef WEB_SERVER_USER_AGENT_PARSING_H
#define WEB_SERVER_USER_AGENT_PARSING_H

#include "../utils.h"
#include "src/trie/51Degrees.h"

#define UA_PARSING_DEBUG 0
#define DATA_PATH "../ua_parsing/src/51Degrees-LiteV3.4.trie";
#define PROPERTIES "ScreenPixelsWidth,ScreenPixelsHeight,PlatformName,IsMobile"
#define UNKNOWN "Unknown"

int get_screen_size_ua(const char *ua_str, int *info);

//TODO this is just for testing should be deleted as soon as we can
/*const char* mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
                               "AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
                               "Safari/9537.53");*/

#endif //WEB_SERVER_USER_AGENT_PARSING_H
