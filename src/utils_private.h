#ifndef LIBCONCORD_UTILS_H_
#define LIBCONCORD_UTILS_H_

//#include <libconcord.h> << implicit

long long Utils_parse_ratelimit_header(struct dictionary_s *header, bool use_clock);

#endif
