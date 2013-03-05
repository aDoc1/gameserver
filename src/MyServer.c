#include "MyServer.h"

struct {
	char *ext;
	char *filetype;
} MIMETypes [] = {
	{"gif", "image/gif" },  
	{"jpg", "image/jpg" }, 
	{"jpeg","image/jpeg"},
	{"png", "image/png" },  
	{"ico", "image/ico" },  
	{"zip", "image/zip" },  
	{"gz",  "image/gz"  },  
	{"tar", "image/tar" },  
	{"htm", "text/html" },  
	{"html","text/html" }, 
	{"js", "application/javascript"}, 
	{0,0} };

char* handle_request(char * req) {
	char * rv; // This will be our return value
	char * res; // This will point to a location in rv where we can write the html to.
	char * ftoken; // this will be a token that we pull out of the 'path' variable indicating the function
	               // for example, if you go to http://localhost:8000/?f=fib&n=10, this will be "f=fib"
	char * atoken; // A token representing the argument to the function, i.e., "n=10"
	char * tmp; // used to point to where the arguments start
	char * html; // holds HTML page served up by the web server
	char * fstr; // holds content type
	long len;
	FILE *fp;
	int reqlen, extlen, i;
	
	//printf("Request = %s\n", req);

	reqlen=strlen(req);	
		
	for(i=4;i<reqlen;i++) { /* null terminate after the second space to ignore extra stuff */
		if(req[i] == ' ') { /* string is "GET URL " +lots of other stuff */
			req[i] = 0;
			break;
		}
	}
	
	//printf("Request = %s\n", req);

	if(strncmp(req,"GET /\0 ",6) == 0) /* convert no filename to index file */
	{
		strcpy(req,"GET /index.html");
	}
	
	
	/* work out the file type and check we support it */
	reqlen=strlen(req);
	fstr = 0;
	for(i=0;MIMETypes[i].ext != 0;i++) {
		extlen = strlen(MIMETypes[i].ext);
		if( !strncmp(&req[reqlen-extlen], MIMETypes[i].ext, extlen)) {
			fstr = MIMETypes[i].filetype;
			break;
		}
	}
	
	if((fp = fopen(&req[5],"r")) == NULL) {  /* open the file for reading */
		perror("Failed to open file\n");
		exit(3);
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET); /* lseek back to the file start ready for reading */
	
	
	html = (char *)malloc(len * sizeof(char));
	rv = (char *)malloc((len + 1000) * sizeof(char));
	sprintf(rv, "HTTP/1.1 200 OK\nServer: adweb\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n", len, fstr); /* Header + a blank line */
	while (fgets(html, len, fp) != NULL) {
		strcat(rv, html);		
	}
	
	sleep(1);
	fclose(fp);
	free(html);

	return rv;
}
