
#include "ctype.h"

int toupper(int c){
    if('a' <= c && c <= 'z'){
        return c - 'a' + 'A';
    }
    return c;
}

int tolower(int c){
    if('A' <= c && c <= 'Z'){
        return c -'A' + 'a';
    }
    return c;
}

int isspace(int c){
    if(c == ' ' || c == '\t' || c == '\n' ||
        c == '\r' || c == '\v' || c == '\f'){
        return 1;
    }
    return 0;
}

int isprint(int c){
    return (32 <= c && c < 127);
}

int isalpha(int c){
    if('a' <= c && c <= 'z'){
        return 2;
    } else if('A' <= c && c <= 'Z'){
        return 1;
    }
    return 0;
}

int isdigit(int c){
    return ('0' <= c && c <= '9');
}

int isalnum(int c){
    if('a' <= c && c <= 'z'){
        return 2;
    } else if('A' <= c && c <= 'Z'){
        return 1;
    } else if(('0' <= c && c <= '9')){
        return 1;
    }
    return 0;
}

int isupper(int c){
    return ('A' <= c && c <= 'Z');
}

int islower(int c){
    return ('a' <= c && c <= 'z');
}