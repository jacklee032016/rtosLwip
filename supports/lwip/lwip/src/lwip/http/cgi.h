
#ifndef CGI_H_INCLUDED
#define CGI_H_INCLUDED

typedef struct HttpCGI {
	unsigned type;          /* Strategy to find string in the CGI table. */
	const char *name;       /* Request string from GET request */
	http_handler_t handler; /* Callback to process the special request */
} HttpCGI;

http_handler_t cgi_search(const char *name, HttpCGI *table);

extern HttpCGI cgi_table[];

#endif /* CGI_H_INCLUDED */
