
#include "libc.h"

extern char _heap_start[];
extern char _heap_end[];

static char* heap_pointer = _heap_start;
static const char* heap_end = _heap_end;

static void put_char(char c, int* count);
static void put_string(const char* s, int* count);
static void put_int(int x, int *count);

int putchar(int c){
    int count = 0;
    put_char((char)c, &count);

    return count;
}

int puts(const char* s){
    int count = 0;
    put_string(s, &count);

    return count;
}

static void put_char(char c, int* count){
    *UART_ADDR  = c;
    (*count)++;
}

static void put_string(const char* s, int* count){
    while(*s){
        put_char(*s, count);
        s++;
    }
}

static void put_int(int x, int *count){
    
    char buf[12];
    int size = 0;

    unsigned int copy = (unsigned int)x;

    if(x < 0){
        copy = (unsigned int)(-(x + 1)) + 1;
    } else if(copy == 0){
        put_char('0', count);
        return;
    } 

    while(copy){
        buf[size++] = ((copy % 10) + '0');
        copy /= 10;
    }

    if(x < 0)
        put_char('-', count);
    
    for(int i = size - 1; i >= 0 ; i--){
        put_char(buf[i], count);
    }

}

int printf(const char* args, ...){
    va_list ap;
    va_start(ap, args);

    int count = 0;
    while(*args){

        if(*args == '%'){
            
            args++;
            switch(*args){
                case 'c': 
                    put_char((char)va_arg(ap, int), &count);     
                    break;
                case 's': 
                    put_string(va_arg(ap, const char*), &count); 
                    break;
                case 'd':
                    put_int(va_arg(ap, int), &count);
                    break;
                case '%':
                    put_char('%', &count);
                    break;
                default:
                    put_char('%', &count);
                    put_char(*args, &count);
                    break;
            } 
        }
        else{
            put_char(*args, &count);
        }
        args++;
    }

    
    va_end(ap);
    return count;
}

int snprintf(char* buf, unsigned int size, const char* fmt, ...){
    // TODO
    return 0;
}

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

void *malloc(unsigned int size){

    char *p = heap_pointer;
    
    size = (size + 7u) & ~7u; // 8 byte alignment
    
    if((heap_pointer + size) > heap_end)
        return NULL;
    
    heap_pointer += size;
    
    return p;
}

void free(void *p){
    // TODO
    (void)p;
}