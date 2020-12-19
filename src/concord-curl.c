#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <libconcord.h>
#include "concord-common.h"

#include "hashtable.h"
#include "debug.h"


/* @todo create distinction between bot and user token */
struct curl_slist*
Concord_reqheader_init(concord_api_t *api)
{
    char auth[MAX_HEADER_LEN] = "Authorization: Bot "; 

    struct curl_slist *new_header = NULL;
    void *tmp; /* for checking potential errors */

    new_header = curl_slist_append(new_header,"X-RateLimit-Precision: millisecond");
    ASSERT_S(NULL != new_header, "Couldn't create request header");

    tmp = curl_slist_append(new_header, strcat(auth, api->token));
    ASSERT_S(NULL != tmp, "Couldn't create request header");

    tmp = curl_slist_append(new_header,"User-Agent: concord (http://github.com/LucasMull/concord, v0.0)");
    ASSERT_S(NULL != tmp, "Couldn't create request header");

    tmp = curl_slist_append(new_header,"Content-Type: application/json");
    ASSERT_S(NULL != tmp, "Couldn't create request header");

    return new_header;
}

/* this is a very crude http header parser, splits key/value pairs at ':' */
static size_t
_curl_header_cb(char *content, size_t size, size_t nmemb, void *p_userdata)
{
    size_t realsize = size * nmemb;
    struct dictionary_s *header = p_userdata;

    char *ptr;
    if (!(ptr = strchr(content, ':')))
        return realsize; /* couldn't find key/value pair */

    *ptr = '\0'; /* isolate key from value at ':' */
    
    char *key = content;

    if (!(ptr = strstr(ptr+1, "\r\n")))
        return realsize; /* couldn't find CRLF */

    *ptr = '\0'; /* remove CRLF from value */

    /* trim space from start of value string if necessary */
    int i=1; /* start from one position after ':' char */
    for ( ; isspace(content[strlen(content)+i]) ; ++i)
        continue;

    char *field = strdup(&content[strlen(content)+i]);
    ASSERT_S(NULL != field, "Out of memory");

    /* store key/value pair in a dictionary */
    void *res = dictionary_set(header, key, field, &free);
    ASSERT_S(res == field, "Couldn't fetch header content");

    return realsize; /* return value for curl internals */
}

/* get curl response body */
static size_t
_curl_body_cb(char *content, size_t size, size_t nmemb, void *p_userdata)
{
    size_t realsize = size * nmemb;
    struct concord_response_s *response_body = p_userdata;


    char *tmp = realloc(response_body->str, response_body->size + realsize + 1);
    ASSERT_S(NULL != tmp, "Out of memory");

    response_body->str = tmp;
    memcpy(response_body->str + response_body->size, content, realsize);
    response_body->size += realsize;
    response_body->str[response_body->size] = '\0';

    return realsize;
}


/* init easy handle with some default opt */
CURL*
Concord_conn_easy_init(concord_api_t *api, struct concord_conn_s *conn)
{
    CURL *new_easy_handle = curl_easy_init();
    ASSERT_S(NULL != new_easy_handle, "Out of memory");

    CURLcode ecode;
    /*
    D_ONLY( ecode = curl_easy_setopt(new_easy_handle, CURLOPT_VERBOSE, 2L) );
    D_ONLY( ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode)) );
    */
    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_HTTPHEADER, api->request_header);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_PRIVATE, conn);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    /* SET CURL_EASY CALLBACKS */
    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_WRITEFUNCTION, &_curl_body_cb);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_WRITEDATA, &conn->response_body);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_HEADERFUNCTION, &_curl_header_cb);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_HEADERDATA, api->header);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    return new_easy_handle;
}

/* init multi handle with some default opt */
CURLM*
Concord_api_multi_init(concord_api_t *api)
{
    CURLM *new_multi_handle = curl_multi_init();
    ASSERT_S(NULL != new_multi_handle, "Out of memory");

    CURLMcode mcode;
    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_SOCKETFUNCTION, &Concord_api_socket_cb);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_SOCKETDATA, api);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_TIMERFUNCTION, &Concord_api_timeout_cb);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_TIMERDATA, &api->timeout);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    return new_multi_handle;
}

/* init easy handle with some default opt */
CURL*
Concord_ws_easy_init(concord_ws_t *ws)
{
    /* missing on_binary, on_ping, on_pong */
    struct cws_callbacks cws_cbs = {
        .on_connect = &Concord_on_connect_cb,
        .on_text = &Concord_on_text_cb,
        .on_close = &Concord_on_close_cb,
        .data = ws,
    };

    CURL *new_easy_handle = cws_new(BASE_GATEWAY_URL, NULL, &cws_cbs);
    ASSERT_S(NULL != new_easy_handle, "Out of memory");

    CURLcode ecode;
    D_ONLY(ecode = curl_easy_setopt(new_easy_handle, CURLOPT_VERBOSE, 2L));
    D_ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    ecode = curl_easy_setopt(new_easy_handle, CURLOPT_FOLLOWLOCATION, 2L);
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));

    return new_easy_handle;
}

CURLM*
Concord_ws_multi_init(concord_ws_t *ws)
{
    CURLM *new_multi_handle = curl_multi_init();
    ASSERT_S(NULL != new_multi_handle, "Out of memory");

    CURLMcode mcode;
    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_SOCKETFUNCTION, &Concord_ws_socket_cb);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_SOCKETDATA, ws);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_TIMERFUNCTION, &Concord_ws_timeout_cb);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    mcode = curl_multi_setopt(new_multi_handle, CURLMOPT_TIMERDATA, &ws->timeout);
    ASSERT_S(CURLM_OK == mcode, curl_multi_strerror(mcode));

    return new_multi_handle;
}

void
Concord_conn_set_method(struct concord_conn_s *conn, enum http_method method)
{
    CURLcode ecode;
    switch (method){
    case DELETE:
        ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case GET:
        ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_HTTPGET, 1L);
        break;
    case POST:
        ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_POST, 1L);
        break;
    case PATCH:
        ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
        break;
    case PUT:
        ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_UPLOAD, 1L);
        break;
    default:
        ERROR("Unknown http_method\n\tCode: %d", method);
    }

    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));
}

void
Concord_conn_set_url(struct concord_conn_s *conn, char endpoint[])
{
    char base_url[MAX_URL_LEN] = BASE_API_URL;


    CURLcode ecode = curl_easy_setopt(conn->easy_handle, CURLOPT_URL, strcat(base_url, endpoint));
    ASSERT_S(CURLE_OK == ecode, curl_easy_strerror(ecode));
}
