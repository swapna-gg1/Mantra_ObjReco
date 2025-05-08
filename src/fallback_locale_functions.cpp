#include <cstdlib>

extern "C" {

// XC32 workaround: define a dummy locale_t type
typedef void* locale_t;

// Fallback for strtoll_l
long long strtoll_l(const char* str, char** endptr, int base, locale_t) {
    return std::strtoll(str, endptr, base);
}

// Fallback for strtoull_l
unsigned long long strtoull_l(const char* str, char** endptr, int base, locale_t) {
    return std::strtoull(str, endptr, base);
}

// Fallback for strtod_l
double strtod_l(const char* str, char** endptr, locale_t) {
    return std::strtod(str, endptr);
}

// Fallback for strtof_l
float strtof_l(const char* str, char** endptr, locale_t) {
    return std::strtof(str, endptr);
}

} // extern "C"
