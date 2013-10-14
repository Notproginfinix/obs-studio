#pragma once

#include "obs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct random_tex {
	texture_t texture;
	effect_t  whatever;
};

EXPORT struct random_tex *random_create(const char *settings, source_t source);
EXPORT void random_destroy(struct random_tex *rt);
EXPORT uint32_t random_get_output_flags(struct random_tex *rt);

EXPORT void random_video_render(struct random_tex *rt, source_t filter_target);

EXPORT int random_getwidth(struct random_tex *rt);
EXPORT int random_getheight(struct random_tex *rt);

#ifdef __cplusplus
}
#endif
