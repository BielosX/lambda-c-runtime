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

int http_get(const char* url, http_response* response) {
    regmatch_t matches[MATCHES_LEN];
    char domain[DOMAIN_MAX_LEN];
    char port[MAX_PORT_LEN];
    char path[MAX_PATH_LEN];
    int beginning;
    int end;
    int length;
    memset(domain, 0, sizeof(domain));
    memset(port, 0, sizeof(port));
    memset(path, 0, sizeof(path));
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
        memcpy(domain, url + beginning, end - beginning);
    }
    if (matches[PORT_INDEX].rm_so != -1) {
        beginning = matches[PORT_INDEX].rm_so;
        end = matches[PORT_INDEX].rm_eo;
        length = end - beginning;
        if (length > MAX_PORT_LEN - 1) {
            perror("Specified port too long\n");
            return -1;
        }
        memcpy(port, url + beginning, length);
    } else {
        strcpy(port, "80");
    }
    if (matches[PATH_INDEX].rm_so != -1) {
        int beginning = matches[PATH_INDEX].rm_so;
        int end = matches[PATH_INDEX].rm_eo;
        length = end - beginning;
        if (length > MAX_PATH_LEN - 1) {
            perror("Path too long\n");
            return -1;
        }
        memcpy(path, url + beginning, length);
    }
    struct addrinfo* lookup_result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int addr_lookup_result = getaddrinfo(domain, port, &hints, &lookup_result);
    if (addr_lookup_result != 0) {
        perror("Unable to resolve domain\n");
        return -1;
    }
    struct sockaddr_in* addr = (struct sockaddr_in*) lookup_result[0].ai_addr;
    uint8_t* ipv4_addr = (uint8_t*) &(addr->sin_addr).s_addr;
    printf("Domain resolved as %u.%u.%u.%u\n", ipv4_addr[0], ipv4_addr[1], ipv4_addr[2], ipv4_addr[3]);
    freeaddrinfo(lookup_result);
    return 0;
}
