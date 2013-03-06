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

void handle_request(char * req, int sockfd) {
	static char header[1024]; // holds the header text 
	char * content; // holds the content served up by the web server
	char * fstr; // holds content type
	long len;
	FILE *fp;
	int reqlen, extlen, i;

	reqlen = strlen(req);	
		
	for(i=4; i<reqlen; i++) { /* null terminate after the second space to ignore extra stuff */
		if(req[i] == ' ') {   /* string is "GET URL " + possibly other stuff */
			req[i] = 0;
			break;
		}
	}

	if(strncmp(req,"GET /\0 ",6) == 0) /* convert no filename to index file */
	{
		strcpy(req,"GET /index.html");
	}
	
	
	/* work out the file type and check we support it */
	reqlen = strlen(req);
	fstr = 0;
	for(i=0;MIMETypes[i].ext != 0;i++) {
		extlen = strlen(MIMETypes[i].ext);
		if( !strncmp(&req[reqlen-extlen], MIMETypes[i].ext, extlen)) {
			fstr = MIMETypes[i].filetype;
			break;
		}
	}
	
	if((fp = fopen(&req[5],"rb")) == NULL) {  /* open the file for reading */
		perror("Failed to open file\n");
		exit(3);
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET); /* fseek back to the file start ready for reading */
	content = (char *)malloc(len * sizeof(char));

	sprintf(header, "HTTP/1.1 200 OK\nServer: adweb\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n", len, fstr); /* Header + a blank line */
	write(sockfd, header, strlen(header));  /* write the header to the socket */
	
	if (fread(content, len, 1, fp) > 0) {  /* read the content from the file */
		write(sockfd, content, len);	/* write the content to the socket */
	}
	fclose(fp);
	free(content);
	
	close(sockfd);
}
