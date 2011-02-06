#include "str_buf.h"
#include "string.h"



// to be written
str_buf * str_buf_alloc(size_t size) {
    str_buf* buf;
	if ((buf = (str_buf *) calloc(size,sizeof(str_buf))) && 	((*buf).string = (char *) calloc(size, sizeof(char)))) {
		(*buf).len = 0;
		(*buf).size = size;
		return buf;
	}
	else {
		return NULL;
	}
}

str_buf * str_buf_alloc_str(const char * str){
	int len = strlen(str);
	str_buf * buf = str_buf_alloc(len);
	(*buf).len=len;
	for (int i = 0; i<len; i++) {
		((*buf).string)[i]=str[i];
		//*buf++; *str++;
	}
	return buf;
}

str_buf * str_buf_alloc_substr(const char * str, size_t str_len){
	str_buf * buf = str_buf_alloc(str_len);
	for (int i = 0; i<str_len; i++) {
		((*buf).string)[i]=str[i];
	}
	(*buf).len = str_len;
	return buf;
}

void str_buf_free(str_buf * buf){
	free((*buf).string);
	free(buf);
}

const char * str_buf_head(str_buf * buf){
	return (*buf).string;
}


size_t str_buf_len(str_buf * buf){
	return (*buf).len;
}
size_t str_buf_size(str_buf * buf){
	return (*buf).size;
}

str_buf * str_buf_concat(str_buf * buf1, str_buf * buf2){
	str_buf * res = str_buf_alloc((*buf1).len+(*buf2).len);
    int buf1_len=(*buf1).len;
    int buf2_len=(*buf2).len;
	for (int i=0; i<buf1_len; i++) {
		((*res).string)[i]=((*buf1).string)[i];
	}
	for (int i=0; i<buf2_len; i++) {
		((*res).string)[buf1_len+i]=((*buf2).string)[i];
	}
	(*res).len=buf1_len+buf2_len;
	return res;
}


bool str_buf_equals(str_buf * buf1, str_buf * buf2){
	if ((*buf1).len!=(*buf2).len) {
		return false;
	}
	for (int i=0; i<(*buf1).len; i++) {
		if ( ((*buf1).string)[i] != ((*buf2).string)[i]){
			return false;
		}
	}
	return true;
}


bool str_buf_put_buf(str_buf * buf1, str_buf * buf2){
	//put length of buf2 in variable
	int buf2_len;
	if (buf2==NULL){
		buf2_len=0;
	}
	else{
		buf2_len=(*buf2).len;
	}
	//put length of buf1 in variable
	int buf1_len;
	if (buf1==NULL){
		buf1_len=0;
	}
	else{
		buf1_len=(*buf1).len;
	}

	//return immediately if operation not possible
	if ((*buf1).size-buf1_len<buf2_len){
		return false;
	}
	//copy buf2 string at the end of buf1's string
	for(int i=0; i<buf2_len; i++) {
		((*buf1).string)[buf1_len+i]=((*buf2).string)[i];
	}
	(*buf1).len=buf1_len+buf2_len;
	return true;
}

bool str_buf_put_str(str_buf * buf, const char * str){
	str_buf * buf2=str_buf_alloc_str(str);
	return str_buf_put_buf(buf,buf2);
}

bool str_buf_put_substr(str_buf * buf, const char * str, size_t str_len){
	str_buf * buf2=str_buf_alloc_substr(str, str_len);
	return str_buf_put_buf(buf, buf2);
}

