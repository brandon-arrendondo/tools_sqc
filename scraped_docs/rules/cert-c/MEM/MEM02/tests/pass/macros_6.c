enum month { Jan, Feb, /* ... */ };
typedef enum month month;

typedef struct date date;
struct date {
  unsigned char dd;
  month mm;
  unsigned yy;
};

typedef struct string string;
struct string {
  size_t length;
  char text[];
};

date *d, *week, *fortnight;
string *name;

d = MALLOC(date);
week = MALLOC_ARRAY(7, date);
name = MALLOC_FLEX(string, 16, char);
fortnight = CALLOC(14, date);