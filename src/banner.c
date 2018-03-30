#define _GNU_SOURCE
#include <string.h>

#include "banner.h"

static const char *typemap_low[1024] = {
	[21] = "ftp",
	[22] = "ssh",
	[23] = "telnet",
	[80] = "http",
};

const char *banner_service_type(int port)
{
	if(port < 1024)
		return typemap_low[port];
	switch(port) {
		case 8080:
			return typemap_low[80];
		default:
			return NULL;
	}
}

const char *banner_get_query(int port, unsigned int *len)
{
	static const char ftp[] =
		"HELP\r\n"
		"FEAT\r\n"
	;
	static const char http[] =
		"GET / HTTP/1.0\r\n"
		"Accept: */*\r\n"
		"User-Agent: fi6s/0.1 (+https://github.com/sfan5/fi6s)\r\n"
		"\r\n"
	;

	switch(port) {
		case 21:
			*len = strlen(ftp);
			return ftp;
		case 22:
		case 23:
			*len = 0;
			return "";
		case 80:
		case 8080:
			*len = strlen(http);
			return http;
		default:
			return NULL;
	}
}

void banner_postprocess(int port, char *banner, unsigned int *len)
{
	switch(port) {
		case 22: {
			// cut off after identification string or first NUL
			char *end;
			end = (char*) memmem(banner, *len, "\r\n", 2);
			if(!end)
				end = (char*) memchr(banner, 0, *len);
			if(end)
				*len = end - banner;
			break;
		}
		case 80:
		case 8080: {
			// cut off after headers
			char *end = (char*) memmem(banner, *len, "\r\n\r\n", 4);
			if(!end)
				end = (char*) memmem(banner, *len, "\n\n", 2);
			if(end)
				*len = end - banner;
			break;
		}

		default:
			break; // do nothing
	}
}
