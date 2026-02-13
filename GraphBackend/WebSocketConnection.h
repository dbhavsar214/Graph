
//#include <libwebsockets.h>
#include <string>

class WebSocketConnection {
public:
    //explicit WSConnection();
    //bool setConnection(int port);

private:
    struct lws_context* context_;
    struct lws* wsi_;
};