#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "requests.h"

#define MATCHES_LEN 10
#define HTTP_INDEX 1
#define DOMAIN_INDEX 2
#define PORT_INDEX 5
#define PATH_INDEX 6

#define IP_V4_MAX_LEN 16

struct parsed_url {
    char domain[DOMAIN_MAX_LEN];
    char port[MAX_PORT_LEN];
    char path[MAX_PATH_LEN];
};

typedef struct parsed_url parsed_url;

regex_t http_url_regex;
const char* http_url_regex_pattern = "^(http://)?(([[:alnum:]]|\\.)+)(:([[:digit:]]{1,5}))?([-%&/[:alnum:]]+)$";

__attribute__((constructor)) void http_init() {
    int result = regcomp(&http_url_regex, http_url_regex_pattern, REG_EXTENDED);
    if (result != 0) {
        perror("Unable to compile regex");
        exit(255);
    }
}

__attribute__((destructor)) void http_cleanup() {
    regfree(&http_url_regex);
}

static void addr_to_string(struct sockaddr_in* addr, char addr_string[IP_V4_MAX_LEN]) {
    uint8_t* ipv4_addr = (uint8_t*) &(addr->sin_addr).s_addr;
    memset(addr_string, 0, IP_V4_MAX_LEN);
    sprintf(addr_string, "%u.%u.%u.%u", ipv4_addr[0], ipv4_addr[1], ipv4_addr[2], ipv4_addr[3]);
}

static int parse_url(const char* url, parsed_url* parsed) {
    regmatch_t matches[MATCHES_LEN];
    memset(parsed->domain, 0, sizeof(parsed->domain));
    memset(parsed->port, 0, sizeof(parsed->port));
    memset(parsed->path, 0, sizeof(parsed->path));
    int beginning;
    int end;
    int length;

    int regex_result = regexec(&http_url_regex, url, MATCHES_LEN, matches, 0);
    if (regex_result != 0) {
        perror("Provided url does not match http schema\n");
        return -1;
    }
    if (matches[HTTP_INDEX].rm_so != -1) {
        printf("Specified protocol is http\n");
    }
    if (matches[DOMAIN_INDEX].rm_so != -1) {
        beginning = matches[DOMAIN_INDEX].rm_so;
        end = matches[DOMAIN_INDEX].rm_eo;
        length = end - beginning;
        if (length > DOMAIN_MAX_LEN - 1) {
            perror("Specified domain too long\n");
            return -1;
        }
        memcpy(parsed->domain, url + beginning, length);
    }
    if (matches[PORT_INDEX].rm_so != -1) {
        beginning = matches[PORT_INDEX].rm_so;
        end = matches[PORT_INDEX].rm_eo;
        length = end - beginning;
        if (length > MAX_PORT_LEN - 1) {
            perror("Specified port too long\n");
            return -1;
        }
        memcpy(parsed->port, url + beginning, length);
    } else {
        strcpy(parsed->port, "80");
    }
    if (matches[PATH_INDEX].rm_so != -1) {
        beginning = matches[PATH_INDEX].rm_so;
        end = matches[PATH_INDEX].rm_eo;
        length = end - beginning;
        if (length > MAX_PATH_LEN - 1) {
            perror("Path too long\n");
            return -1;
        }
        memcpy(parsed->path, url + beginning, length);
    }
    return 0;
}

static int resolve_domain_name(parsed_url* parsed, struct sockaddr_in* result_addr) {
    struct addrinfo* lookup_result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int addr_lookup_result = getaddrinfo(parsed->domain, parsed->port, &hints, &lookup_result);
    if (addr_lookup_result != 0) {
        perror("Unable to resolve domain\n");
        return -1;
    }
    struct sockaddr_in* addr = (struct sockaddr_in*) lookup_result[0].ai_addr;
    char addr_string[IP_V4_MAX_LEN];
    addr_to_string(addr, addr_string);
    printf("%s\n", addr_string);
    memcpy(result_addr, addr, sizeof(struct sockaddr_in));
    freeaddrinfo(lookup_result);
    return 0;
}

int http_get(const char* url, http_response* response) {
    struct sockaddr_in addr;
    parsed_url parsed;
    parse_url(url, &parsed);
    if (resolve_domain_name(&parsed, &addr) != 0) {
        return -1;
    }
    return 0;
}
