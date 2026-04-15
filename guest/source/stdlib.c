#include "stdlib.h"

extern char _heap_start[];
extern char _heap_end[];

static char* heap_pointer = _heap_start;
static const char* heap_end = _heap_end;


// TODO (Refine Memory Operation)
void *malloc(unsigned int size){

    char *p = heap_pointer;
    
    size = (size + 7u) & ~7u; // 8 byte alignment
    
    if((heap_pointer + size) > heap_end)
        return NULL;
    
    heap_pointer += size;
    
    return p;
}

void* calloc(unsigned int count, unsigned int size){

    char* p = heap_pointer;

    int amount = count * size;
    amount = (amount + 7u) & ~7u;

    if(heap_pointer + amount > heap_end){
        return NULL;
    } 

    for(int i = 0; i < amount; i++){
        *(heap_pointer + i) = 0x00;
    }

    heap_pointer += amount;

    return p;
}

void* realloc(void* ptr, unsigned int size){
    if(size == 0){
        free(ptr);
        return NULL;
    }

    if(ptr == NULL)
        return malloc(size);

    char* p = malloc(size);
    if(p == NULL)
        return NULL;

    // TODO
    // char* ptr_c = (char*)ptr;
    // for(int i = 0; i < size; i++){
    //     p[i] = ptr_c[i];
    // }

    return p;
}

void free(void *p){
    // TODO
    (void)p;
}

void exit(int code){
    // TODO
    (void)code;
    while (1) {
    }
}

int system(const char* command){
    // TODO
    return -1;
}

int remove(const char* path){
    (void)path;
    // TODO 
    return -1;
}

int rename(const char* oldpath, const char* newpath){

    // TODO
    return -1;
}

int atoi(const char* s){

    int ret = 0;

    int size = 0;
    while(s[size] && ('0' <= s[size] && s[size] <= '9')){

        ret *= 10;
        ret += (int)(s[size] - '0');
        size++;
    }

    return ret;
}

int abs(int x){

    if(x < 0)
        return -x;
    else 
        return x;
}

char *getenv(const char *name){
    (void)name;
    return NULL;
}

double atof(const char* s){
    double a = 0;

    int size = 0;
    int point = -1;

    while(s[size]){
        if(s[size] == '.'){
            point = size;
            size++;
            continue;
        }

        if(s[size] < '0' || s[size] > '9'){
            break;
        }

        a *= 10;
        a += (int)(s[size] - '0');
        size++;
    }

    if(point != -1){
        for(int i = 0; i < size - point - 1; i++){
            a *= 0.1;
        }
    }

    return a;
    
}