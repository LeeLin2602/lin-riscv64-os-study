#include "printf.h"

volatile char *uart = (volatile char*) 0x10000000;

void putchar(char c) {
    *uart = c;
}

void print_hex(unsigned int value) {
    char *hex_digits = "0123456789abcdef";
    char buffer[9];  
    int index = 8;

    buffer[index--] = '\0'; 

   
    do {
        buffer[index--] = hex_digits[value & 0xF];
        value >>= 4;
    } while (value);

    
    char *ptr = &buffer[index + 1];
    while (*ptr) {
        putchar(*ptr++);
    }
}

void print_int(long long value) {
    if(value == 0) {
        putchar('0');
        return;
    }

    if(value < 0) {
        putchar('-');
        value = -value;
    }

    char buffer[20];
    int i = 0;

    while(value) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    while(i-- > 0) {
        putchar(buffer[i]);
    }
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    while(*format) {
        if(*format == '%') {
            format++;
            switch(*format) {
                case 's': {
                    char *str = va_arg(args, char*);
                    while (*str) {
                        putchar(*str++);
                    }
                    break;
                }
                case 'd': {
                    int value = va_arg(args, int);
                    print_int(value);
                    break;
                }
                case 'l': {
                    format++;
                    switch(*format) {
                        case 'l': {
                            format++;
                            switch(*format) {
                                case 'd': {
                                    long long value = va_arg(args, long long);
                                    print_int(value);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
                case 'x': {
                    int value = va_arg(args, int);
                    print_hex(value);
                    break;
                }
                default:
                    putchar(*format);
                    break;
            }
        } else {
            putchar(*format);
        }
        format++;
    }
    va_end(args);
}
