#include <stdio.h>

#include "requests.h"

int main(int argc, char** argv) {
    struct http_response response;
    http_get("http://localhost:8080/2018-06-01/runtime/invocation/next", &response);
    http_get("http://localhost:8080/2018-06-01/runtime/invocation/156cb537-e2d4-11e8-9b34-d36013741fb9/response", &response);
    http_get("http://google.com", &response);
    return 0;
}