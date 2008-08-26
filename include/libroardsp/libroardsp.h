//libroardsp.h:

#ifndef _LIBROARDSP_H_
#define _LIBROARDSP_H_

#include <roaraudio.h>

// defines
#define ROARDSP_MAX_FILTERS_PER_CHAIN 8

#define ROARDSP_FILTER_NONE           0
#define ROARDSP_FILTER_AMP            1
#define ROARDSP_FILTER_LOWP           2
#define ROARDSP_FILTER_HIGHP          3

// types:

struct roardsp_filter {
 int    channels;
 int    bits;
 void * inst;
 int (*calc  )(struct roardsp_filter * filter, void * data, size_t samples);
 int (*uninit)(struct roardsp_filter * filter);
};

struct roardsp_filterchain {
 int filters;
 struct roardsp_filter * filter[ROARDSP_MAX_FILTERS_PER_CHAIN];
};

struct roardsp_lowp {
 uint32_t freq; // in mHz (0Hz..4MHz)
 uint16_t a, b;
 int32_t  old[ROAR_MAX_CHANNELS];
};

// funcs:
int    roardsp_filter_str2id(char * str);
char * roardsp_filter_id2str(int id);
int    roardsp_filter_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int    roardsp_filter_uninit(struct roardsp_filter * filter);
int    roardsp_filter_calc  (struct roardsp_filter * filter, void * data, size_t len);

int roardsp_fchain_init  (struct roardsp_filterchain * chain);
int roardsp_fchain_uninit(struct roardsp_filterchain * chain);
int roardsp_fchain_add   (struct roardsp_filterchain * chain, struct roardsp_filter * filter);
int roardsp_fchain_calc  (struct roardsp_filterchain * chain, void * data, size_t len);

#endif

//ll
