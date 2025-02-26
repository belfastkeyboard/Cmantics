#pragma once


typedef struct INI INI;


INI *parse_ini(const char *filepath);

void destroy_ini(INI **ini);


const char *get_value(INI *ini,
                      const char *header,
                      const char *key);
