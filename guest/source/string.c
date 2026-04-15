
#include "string.h"
#include "stdlib.h"

void *memset(void *dst, int value, unsigned int n){
    
    char* ptr = (char*)dst;
    for(unsigned int i = 0; i < n; i++){
        *ptr = (char)value;
        ptr++;
    }

    return dst;
}

void *memcpy(void *dst, const void *src, unsigned int n){
    
    char* dst_ptr = (char*)dst;
    const char* src_ptr = (const char*)src;

    for(unsigned int i = 0; i < n; i++){
        dst_ptr[i] = src_ptr[i];
    }

    return dst;
}

unsigned int strlen(const char *s){

    int count = 0;
    while(*(s + count)){
        count++;
    }
    return count;
}


int strcmp(const char* a, const char* b){

    int idx = 0;
    while(a[idx] && b[idx]){
        if(a[idx] < b[idx]){
            return -1;
        } else if(a[idx] > b[idx]){
            return 1;
        } 
        idx++;
    }
    return (int)a[idx] - (int)b[idx];
}

int strncmp(const char* a, const char* b, unsigned int n){
    
    unsigned int idx = 0;
    while(a[idx] && b[idx] && (idx < n)){
        if(a[idx] < b[idx]){
            return -1;
        } else if(a[idx] > b[idx]){
            return 1;
        } 
        idx++;
    }

    if(idx == n){
        return 0;
    }

    return (int)a[idx] - (int)b[idx];
}

int strcasecmp(const char* a, const char* b){

    int idx = 0;
    char cmp_a = 0;
    char cmp_b = 0;

    while(a[idx] && b[idx]){
        cmp_a = 'A' <= a[idx] && a[idx] <= 'Z'  ? a[idx] - 'A' + 'a' : a[idx];
        cmp_b = 'A' <= b[idx] && b[idx] <= 'Z'  ? b[idx] - 'A' + 'a' : b[idx];

        if(cmp_a != cmp_b){
            return (int)cmp_a - (int)cmp_b;
        }

        idx++;
    }

    cmp_a = 'A' <= a[idx] && a[idx] <= 'Z'  ? a[idx] - 'A' + 'a' : a[idx];
    cmp_b = 'A' <= b[idx] && b[idx] <= 'Z'  ? b[idx] - 'A' + 'a' : b[idx];

    return (int)cmp_a - (int)cmp_b;

}

int strncasecmp(const char* a, const char* b, unsigned int n){
    
    int idx = 0;
    char cmp_a = 0;
    char cmp_b = 0;

    while(idx < n && a[idx] && b[idx]){

        cmp_a = 'A' <= a[idx] && a[idx] <= 'Z'  ? a[idx] - 'A' + 'a' : a[idx];
        cmp_b = 'A' <= b[idx] && b[idx] <= 'Z'  ? b[idx] - 'A' + 'a' : b[idx];

        if(cmp_a != cmp_b){
            return (int)cmp_a - (int)cmp_b;
        }

        idx++;
    }

    if(n == idx){
        return 0;
    }

    cmp_a = 'A' <= a[idx] && a[idx] <= 'Z'  ? a[idx] - 'A' + 'a' : a[idx];
    cmp_b = 'A' <= b[idx] && b[idx] <= 'Z'  ? b[idx] - 'A' + 'a' : b[idx];

    return (int)cmp_a - (int)cmp_b;
}

char* strncpy(char* dst, const char* src, unsigned int n){
    
    int i = 0;
    for(; i < n && src[i]; i++){
        dst[i] = src[i];
    }

    for(; i < n; i++){
        dst[i] = '\0';
    }

    return dst;
}

char* strdup(const char* s){

    int size = 0;
    while(s[size]){
        size++;
    }

    // add '/0'
    size += 1;

    char* p = malloc(size);

    if(p == NULL){
        return NULL;
    }

    for(int i = 0; i < size; i++){
        p[i] = s[i];
    }

    return p;
}

char* strchr(const char* s, int c){
    
    int idx = 0;
    while(1){
        
        if(s[idx] == c){
            return s + idx;
        }
        if(s[idx] == '\0')
            break;
        idx++;
    }
    return NULL;
}

char* strrchr(const char* s, int c){
    
    int size = 0;
    while(s[size]){
        size++;
    }

    for(int i = size; i >= 0; i--){
        if(s[i] == c)
            return s + i;
    }
    return NULL;
}

char* strstr(const char* a, const char* b){

    if(b[0] == '\0'){
        return a;
    }

    int a_size = 0;
    while(a[a_size]){
        a_size++;
    }


    int b_size = 0;
    while(b[b_size]){
        b_size++;
    }

    char* ret = NULL;
    for(int i = 0; i <= a_size - b_size; i++){

        int isSame = 1;
        for(int j = 0; j < b_size; j++){
            if(a[i + j] != b[j]){
                isSame = 0;
                break;
            }
        }

        if(isSame){
            ret = a + i;
            break;
        }
    }

    return ret;
}

void *memmove(void *dst, const void *src, unsigned int n){

    // TODO (refine)
    char *p1 = (char*)dst;
    const char*p2 = (const char*)src;

    for(int i = n - 1; i >= 0; i++){
        p1[i] = p2[i];
    }

    return dst;
}

int memcmp(const void *a, const void *b, unsigned int n){
    const unsigned char *p1 = a;
    const unsigned char *p2 = b;

    for(int i = 0; i < n; i++){
        if(p1[i] != p2[i]){
            return p1[i] - p2[i];
        }
    }

    return 0;

}
