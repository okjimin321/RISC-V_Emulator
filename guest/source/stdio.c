#include "stdio.h"
#include "mmio.h"
#include "string.h"

// TODO===========================================================
struct FILE {
    const unsigned char* data;
    unsigned int size;
    unsigned int pos;
    int used;
};

static FILE stdout_obj;
static FILE stderr_obj;
static FILE wad_file;

FILE* stdout = &stdout_obj;
FILE* stderr = &stderr_obj;
//=================================================================


static void put_char(char c, int* count);
static void put_string(const char* s, int* count);
static void put_int(int x, int *count);
static void put_uint_base(unsigned int x, unsigned int base, int uppercase, int* count);

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

static void put_uint_base(unsigned int x, unsigned int base, int uppercase, int* count){
    char buf[32];
    int size = 0;
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if(x == 0){
        put_char('0', count);
        return;
    }

    while(x){
        buf[size++] = digits[x % base];
        x /= base;
    }

    for(int i = size - 1; i >= 0; i--){
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
                case 'i':
                    put_int(va_arg(ap, int), &count);
                    break;
                case 'u':
                    put_uint_base(va_arg(ap, unsigned int), 10, 0, &count);
                    break;
                case 'x':
                    put_uint_base(va_arg(ap, unsigned int), 16, 0, &count);
                    break;
                case 'X':
                    put_uint_base(va_arg(ap, unsigned int), 16, 1, &count);
                    break;
                case 'p':
                    put_string("0x", &count);
                    put_uint_base((unsigned int)va_arg(ap, void*), 16, 0, &count);
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

static void put_char_buf(char* buf, char c, int* count, unsigned int size){
    if(*count == -1)
        return;
    
    if((int)(*count) > (int)size - 1){
        *count = -1;
        return;
    }

    if(size - 1 == *count){
        buf[*count] = '\0';
    }
    else{
        buf[*count]  = c;
    }

    (*count)++;
}

static void put_string_buf(char* buf, const char* s, int* count, unsigned int size){
    while(*s){
        put_char_buf(buf, *s, count, size);
        s++;
    }
}

static void put_int_buf(char* buffer, int x, int *count, unsigned int Size){
    
    char buf[12];
    int size = 0;

    unsigned int copy = (unsigned int)x;

    if(x < 0){
        copy = (unsigned int)(-(x + 1)) + 1;
    } else if(copy == 0){
        put_char_buf(buffer, '0', count, Size);
        return;
    } 

    while(copy){
        buf[size++] = ((copy % 10) + '0');
        copy /= 10;
    }

    if(x < 0)
        put_char_buf(buffer, '-', count, Size);
    
    for(int i = size - 1; i >= 0 ; i--){
        put_char_buf(buffer, buf[i], count, Size);
    }

}

static void put_unsigned_formatted_buf(
    char* buffer,
    unsigned int x,
    unsigned int base,
    int uppercase,
    int negative,
    int width,
    int precision,
    int zero_pad,
    int* count,
    unsigned int Size
){
    char digits_buf[32];
    int digit_count = 0;
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if(x == 0){
        digits_buf[digit_count++] = '0';
    } else {
        while(x){
            digits_buf[digit_count++] = digits[x % base];
            x /= base;
        }
    }

    int zero_count = 0;
    if(precision > digit_count){
        zero_count = precision - digit_count;
    } else if(zero_pad && precision < 0 && width > digit_count + negative){
        zero_count = width - digit_count - negative;
    }

    int total = digit_count + zero_count + negative;
    while(width > total){
        put_char_buf(buffer, ' ', count, Size);
        width--;
    }

    if(negative){
        put_char_buf(buffer, '-', count, Size);
    }

    while(zero_count-- > 0){
        put_char_buf(buffer, '0', count, Size);
    }

    for(int i = digit_count - 1; i >= 0; i--){
        put_char_buf(buffer, digits_buf[i], count, Size);
    }
}

static void put_signed_formatted_buf(
    char* buffer,
    int x,
    int width,
    int precision,
    int zero_pad,
    int* count,
    unsigned int Size
){
    unsigned int copy = (unsigned int)x;
    int negative = x < 0;

    if(negative){
        copy = (unsigned int)(-(x + 1)) + 1;
    }

    put_unsigned_formatted_buf(buffer, copy, 10, 0, negative,
                               width, precision, zero_pad, count, Size);
}

int snprintf(char* buf, unsigned int size, const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int count = vsnprintf(buf, size, fmt, ap);

    va_end(ap);
    return count;
}

int vsnprintf(char* buf, unsigned int size, const char* fmt, va_list ap){

    if(size == 0){
        return 0;
    }

    int count = 0;
    while(*fmt){

        if(*fmt == '%'){
            
            fmt++;
            int zero_pad = 0;
            int width = 0;
            int precision = -1;

            if(*fmt == '0'){
                zero_pad = 1;
                fmt++;
            }

            while('0' <= *fmt && *fmt <= '9'){
                width = width * 10 + (*fmt - '0');
                fmt++;
            }

            if(*fmt == '.'){
                fmt++;
                precision = 0;
                while('0' <= *fmt && *fmt <= '9'){
                    precision = precision * 10 + (*fmt - '0');
                    fmt++;
                }
            }

            switch(*fmt){
                case 'c': 
                    put_char_buf(buf, (char)va_arg(ap, int), &count, size);     
                    break;
                case 's': 
                    put_string_buf(buf, va_arg(ap, const char*), &count, size); 
                    break;
                case 'd':
                case 'i':
                    put_signed_formatted_buf(buf, va_arg(ap, int), width, precision,
                                             zero_pad, &count, size);
                    break;
                case 'u':
                    put_unsigned_formatted_buf(buf, va_arg(ap, unsigned int), 10, 0, 0,
                                               width, precision, zero_pad, &count, size);
                    break;
                case 'x':
                    put_unsigned_formatted_buf(buf, va_arg(ap, unsigned int), 16, 0, 0,
                                               width, precision, zero_pad, &count, size);
                    break;
                case 'X':
                    put_unsigned_formatted_buf(buf, va_arg(ap, unsigned int), 16, 1, 0,
                                               width, precision, zero_pad, &count, size);
                    break;
                case 'p':
                    put_string_buf(buf, "0x", &count, size);
                    put_unsigned_formatted_buf(buf, (unsigned int)va_arg(ap, void*), 16, 0, 0,
                                               width, precision, zero_pad, &count, size);
                    break;
                case '%':
                    put_char_buf(buf, '%', &count, size);
                    break;
                default:
                    put_char_buf(buf, '%', &count, size);
                    put_char_buf(buf, *fmt, &count, size);
                    break;
            } 
        }
        else{
            put_char_buf(buf, *fmt, &count, size);
        }
        fmt++;
    }

    if(count >= 0){
        
        int idx = count >= size ? size - 1 : count;
        buf[idx] = '\0';
    }
    
    return count;
}

int fprintf(FILE* stream, const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int count = vfprintf(stream, fmt, ap);

    va_end(ap);
    return count;
}

int vfprintf(FILE* stream, const char* fmt, va_list ap){
    (void)stream;

    int count = 0;
    while(*fmt){

        if(*fmt == '%'){
            
            fmt++;
            switch(*fmt){
                case 'c': 
                    put_char((char)va_arg(ap, int), &count);     
                    break;
                case 's': 
                    put_string(va_arg(ap, const char*), &count); 
                    break;
                case 'd':
                case 'i':
                    put_int(va_arg(ap, int), &count);
                    break;
                case 'u':
                    put_uint_base(va_arg(ap, unsigned int), 10, 0, &count);
                    break;
                case 'x':
                    put_uint_base(va_arg(ap, unsigned int), 16, 0, &count);
                    break;
                case 'X':
                    put_uint_base(va_arg(ap, unsigned int), 16, 1, &count);
                    break;
                case 'p':
                    put_string("0x", &count);
                    put_uint_base((unsigned int)va_arg(ap, void*), 16, 0, &count);
                    break;
                case '%':
                    put_char('%', &count);
                    break;
                default:
                    put_char('%', &count);
                    put_char(*fmt, &count);
                    break;
            } 
        }
        else{
            put_char(*fmt, &count);
        }
        fmt++;
    }

    return count;
}

int fflush(FILE* stream){
    // TODO
    return 0;
}

// TODO (Make file system)=================================================================
FILE* fopen(const char* path, const char* mode) {
    (void)mode;
    
    if (strstr(path, ".wad") == NULL && strstr(path, ".WAD") == NULL) {
        return NULL;
    }
    wad_file.data = WAD_ADDR;
    wad_file.size = WAD_SIZE;
    wad_file.pos = 0;
    wad_file.used = 1;

    return &wad_file;
}

int fclose(FILE* stream) {
    stream->used = 0;
    return 0;
}

unsigned int fread(void* ptr, unsigned int size, unsigned int nmemb, FILE* stream) {
    unsigned int bytes = size * nmemb;

    if (stream->pos + bytes > stream->size) {
        bytes = stream->size - stream->pos;
    }

    memcpy(ptr, stream->data + stream->pos, bytes);
    stream->pos += bytes;

    return bytes / size;
}

unsigned int fwrite(const void* ptr, unsigned int size, unsigned int nmemb, FILE* stream){
    return 0;
}

int fseek(FILE* stream, long offset, int whence) {
    unsigned int new_pos;

    if (whence == SEEK_SET) {
        new_pos = offset;
    } else if (whence == SEEK_CUR) {
        new_pos = stream->pos + offset;
    } else if (whence == SEEK_END) {
        new_pos = stream->size + offset;
    } else {
        return -1;
    }

    if (new_pos > stream->size) {
        return -1;
    }

    stream->pos = new_pos;
    return 0;
}


long ftell(FILE* stream) {
    return stream->pos;
}


// TODO (refine)
static int scan_is_space(char c){
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f';
}

static int scan_digit_value(char c){
    if('0' <= c && c <= '9'){
        return c - '0';
    }
    if('a' <= c && c <= 'f'){
        return c - 'a' + 10;
    }
    if('A' <= c && c <= 'F'){
        return c - 'A' + 10;
    }
    return -1;
}

static void scan_skip_space(const char** s){
    while(scan_is_space(**s)){
        (*s)++;
    }
}

static int scan_int(const char** s, int base, int* out){
    const char* p = *s;
    int sign = 1;
    unsigned int value = 0;
    int digits = 0;

    if(*p == '-'){
        sign = -1;
        p++;
    } else if(*p == '+'){
        p++;
    }

    if(base == 0){
        if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')){
            base = 16;
            p += 2;
        } else if(p[0] == '0'){
            base = 8;
        } else {
            base = 10;
        }
    } else if(base == 16 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')){
        p += 2;
    }

    while(1){
        int digit = scan_digit_value(*p);
        if(digit < 0 || digit >= base){
            break;
        }
        value = value * (unsigned int)base + (unsigned int)digit;
        digits++;
        p++;
    }

    if(digits == 0){
        return 0;
    }

    *out = sign < 0 ? -(int)value : (int)value;
    *s = p;
    return 1;
}

int sscanf(const char* str, const char* fmt, ...){
    va_list ap;
    int assigned = 0;

    va_start(ap, fmt);

    while(*fmt){
        if(scan_is_space(*fmt)){
            while(scan_is_space(*fmt)){
                fmt++;
            }
            scan_skip_space(&str);
            continue;
        }

        if(*fmt != '%'){
            if(*str != *fmt){
                break;
            }
            str++;
            fmt++;
            continue;
        }

        fmt++;

        if(*fmt == '%'){
            if(*str != '%'){
                break;
            }
            str++;
            fmt++;
            continue;
        }

        switch(*fmt){
            case 'd':
            case 'u':
            case 'x':
            case 'X':
            case 'o':
            case 'i': {
                int* out = va_arg(ap, int*);
                int base = 10;

                scan_skip_space(&str);

                if(*fmt == 'x' || *fmt == 'X'){
                    base = 16;
                } else if(*fmt == 'o'){
                    base = 8;
                } else if(*fmt == 'i'){
                    base = 0;
                }

                if(!scan_int(&str, base, out)){
                    va_end(ap);
                    return assigned;
                }

                assigned++;
                break;
            }
            case 'c': {
                char* out = va_arg(ap, char*);
                if(*str == '\0'){
                    va_end(ap);
                    return assigned;
                }
                *out = *str++;
                assigned++;
                break;
            }
            case 's': {
                char* out = va_arg(ap, char*);
                scan_skip_space(&str);
                if(*str == '\0'){
                    va_end(ap);
                    return assigned;
                }
                while(*str && !scan_is_space(*str)){
                    *out++ = *str++;
                }
                *out = '\0';
                assigned++;
                break;
            }
            default:
                va_end(ap);
                return assigned;
        }

        fmt++;
    }

    va_end(ap);
    return assigned;
}
