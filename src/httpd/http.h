#ifndef HTTP_H
#define HTTP_H 1

#include "hook.h"

/* Buffer Sizes */

#define HTTPBUFSIZE	1024
#define HTTPREADBUFSIZE	8192
#define HTTP_PAGE_AGE	1440
#define HTTP_TIMEOUT	60
#define HTTP_CLEANOUT	3600

/* Internal Defines */

#define MAX_METHODS	9

#define HTTP_PAGE_HEAD	"<html>\r\n<head>\r\n<title>%s</title>\r\n</head>\r\n<body>\r\n"
#define HTTP_PAGE_FOOT	"\r\n</body>\r\n</html>\r\n"
#define HTTP_LINEBREAK	"<br>\r\n"
#define HTTP_TABLE_ROW	"<td width=\"%d%%\">%s</td>"

#define H_POST		"POST"
#define H_GET		"GET"

#ifndef HTTP_ALLOW
#define HTTP_ALLOW 	1
#define HTTP_DENY 	0
#endif

/* HTTP Replies */

#define HTTP_CONTINUE		100
#define HTTP_SWITCH_PROTO	101
#define HTTP_OK			200
#define HTTP_CREATED		201
#define HTTP_ACCEPTED		202
#define HTTP_NON_AUTHORATIVE	203
#define HTTP_NO_CONTENT		204
#define HTTP_RESET_CONTENT	205
#define HTTP_PART_CONTENT	206
#define HTTP_MULTICHOICE	300
#define HTTP_MOVED_PERM		301
#define HTTP_FOUND		302
#define HTTP_SEE_OTHER		303
#define HTTP_NOT_MODIFIED	304
#define HTTP_USE_PROXY		305
#define HTTP_TEMP_REDIRECT	307
#define HTTP_BAD_REQUEST	400
#define HTTP_UNAUTHORITED	401	/* ? TODO */
#define HTTP_PAYMENT_REQ	402
#define HTTP_FORBIDDEN		403	/* TODO */
#define HTTP_NOT_FOUND		404
#define HTTP_METHOD_NOT_ALLOW	405
#define HTTP_NOT_ACCEPTABLE	406
#define HTTP_PROXY_AUTH_REQ	407
#define HTTP_REQUEST_TIMEOUT	408
#define HTTP_CONFLICT		409
#define HTTP_GONE		410
#define HTTP_LENGTH_REQUIRED	411
#define HTTP_PRECOND_FAILED	412
#define HTTP_REQ_ENTITY_LARGE	413
#define HTTP_REQ_URI_LARGE	414
#define HTTP_UNSUPPORTED_MEDIA	415
#define HTTP_EXPECTATION_FAILED	417
#define HTTP_INTERNAL_ERROR	500	/* TODO */
#define HTTP_NOT_IMPLEMENTED	501
#define HTTP_BAD_GATEWAY	502
#define HTTP_SERVICE_UNAVAIL	503	/* TODO */
#define HTTP_GATEWAY_TIMEOUT	504
#define HTTP_VERSION_NOT_SUPP	505

/* HTTP Connection Information Flags: ->httpflags */

#define HFLAGS_HEADER_READ      	0x001	/* We already parsed this clients headers */
#define HFLAGS_GZIP			0x002	/* This client can use gzipped information */
#define HFLAGS_EXPECTATION_FAILED	0x004 	/* We do not meed clients expectations */
#define HFLAGS_AUTHENTICATED		0x008	/* This client did password authentication */
#define HFLAGS_PARSE_WAIT		0x010	/* This client did not send everything */
#define HFLAGS_EMPTY_HOST		0x020	/* The client is not authenticated yet */
#define HFLAGS_COOKIE_READ		0x040	/* The client had a cookie sent */
#define HFLAGS_COOKIE_NONE		0x080	/* The client does not have a cookie */

/* Internal Structures */

struct hmethod {
    char *name;
    int parse;
    int (*func) (struct Client *, char *, char *);
};

extern char *herrortab[];
extern int httpd_shutdown;
extern int httpd_exited;
extern struct Listener *ListenerPollList;

/* Configuration functions */

extern void *httpd_main(void *);

extern void configure_httpd(void);
extern void configure_httpd_errors(void);

extern char *httpdate(time_t);

/* Read functions */

extern int regain_httpd_listener(struct hook_data *);
extern int read_http_packet_hook(struct hook_data *);
extern void read_http_packet(int, void *);
extern void http_dopacket(aClient *, char *, size_t);
extern void parse_client_header(aClient *, char *, char *, int, int (*)(aClient *, char *, char *));

/* Send functions */

extern int send_http_message(aClient *, char *, int);
extern void sendto_http(aClient *, char *, ...);
extern void vsendto_http(aClient *, char *, va_list);
extern void send_http_queued(int, void *);
extern void send_http_status(aClient *, int);
extern void send_http_header(aClient *, char *, int);
extern void send_http_cookie(aClient *, char *, int);

/* Html Generation functions */

extern char *enervate_html_object(char *, size_t, char *, int, char *, ...);
extern char *enervate_html_object_begin(char *, size_t, char *, int, char *, ...);
extern inline int hex_to_int(char c);
extern inline char *url_decode(char *inchr, int il, char *outchr);

/* Internal Management functions */

extern int exit_httpd_client(aClient *);
extern void remove_exited_httpd_clients(void *);
extern void remove_remaining_httpd_clients(void *);
extern void setup_http_signals(void);

/* Method related functions */

extern int http_none(aClient *, char *, char *);
extern int http_get(aClient *, char *, char *);
extern int http_post(aClient *, char *, char *);
extern int http_mna(aClient *, char *, char *);

extern int http_none(aClient *, char *, char *);
extern int http_accept_encoding(aClient *, char *, char *);
extern int http_age(aClient *, char *, char *);
extern int http_allow(aClient *, char *, char *);
extern int http_cache_control(aClient *, char *, char *);
extern int http_connection(aClient *, char *, char *);
extern int http_content_language(aClient *, char *, char *);
extern int http_content_length(aClient *, char *, char *);
extern int http_content_location(aClient *, char *, char *);
extern int http_content_type(aClient *, char *, char *);
extern int http_cookie(aClient *, char *, char *);
extern int http_date(aClient *, char *, char *);
extern int http_expect(aClient *, char *, char *);
extern int http_expires(aClient *, char *, char *);
extern int http_last_modified(aClient *, char *, char *);
extern int http_location(aClient *, char *, char *);
extern int http_retry_after(aClient *, char *, char *);
extern int http_server(aClient *, char *, char *);
extern int http_set_cookie(aClient *, char *, char *);
extern int http_host_header(aClient *, char *, char *);

#endif
