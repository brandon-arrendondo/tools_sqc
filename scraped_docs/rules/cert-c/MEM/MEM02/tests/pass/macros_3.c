#define MALLOC_ARRAY(number, type) \
    ((type *)malloc((number) * sizeof(type)))