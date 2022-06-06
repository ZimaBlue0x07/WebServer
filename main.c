#include "webserver.h"

int main(){

    simpleWebServer webserver = {
        .port = "8001",
        .run = run,
        .createWebSocket = createWebSocket
    };
    webserver.createWebSocket(&webserver);
    webserver.run(&webserver);

    return 0;
}

