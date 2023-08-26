#ifndef _DUCQ_WS_
#define _DUCQ_WS_

#include "ducq.h"


ducq_i *ducq_new_ws_client(const char *host, const char *port);
ducq_i *ducq_new_ws_connection(int fd);

ducq_state ducq_new_ws_upgrade_from_http(ducq_i **ws, int fd, char *http_header);



#endif // _DUCQ_WS_
