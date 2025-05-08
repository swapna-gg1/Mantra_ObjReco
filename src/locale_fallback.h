#ifndef LOCALE_FALLBACK_H
#define LOCALE_FALLBACK_H

typedef int locale_t; // Dummy type to satisfy the signature

inline long long strtoll_l(const char* str, char** endptr, int base, locale_t) {
    return strtoll(str, endptr, base);
}

inline unsigned long long strtoull_l(const char* str, char** endptr, int base, locale_t) {
    return strtoull(str, endptr, base);
}

inline double strtod_l(const char* str, char** endptr, locale_t) {
    return strtod(str, endptr);
}

inline float strtof_l(const char* str, char** endptr, locale_t) {
    return strtof(str, endptr);
}

#endif // LOCALE_FALLBACK_H
