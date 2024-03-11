#ifndef __REQUESTS_H__
#define __REQUESTS_H__

#define MAX_HEADER_KEY_LEN 2048
#define MAX_HEADER_VALUE_LEN 8096
#define MAX_HEADERS 128
#define MAX_BODY_LEN 16384
#define BUFFER_LEN 4096
#define DOMAIN_MAX_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_PORT_LEN 6

struct http_header {
    char key[MAX_HEADER_KEY_LEN];
    char value[MAX_HEADER_VALUE_LEN];
};

typedef struct http_header http_header;

struct http_response {
    struct http_header headers[MAX_HEADERS];
    int status_code;
    char body[MAX_BODY_LEN];
};

typedef struct http_response http_response;

int http_get(const char* url, http_response* response);

#endif