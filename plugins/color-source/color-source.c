#include <obs-module.h>

struct color_source {
	uint32_t color;

	uint32_t width;
	uint32_t height;

	obs_source_t *src;
};

static const char *color_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("ColorSource");
}

static void color_source_update(void *data, obs_data_t *settings)
{
	struct color_source *context = data;
	uint32_t color = (uint32_t)obs_data_get_int(settings, "color");
	uint32_t width = (uint32_t)obs_data_get_int(settings, "width");
	uint32_t height = (uint32_t)obs_data_get_int(settings, "height");

	context->color = color;
	context->width = width;
	context->height = height;
}

static void *color_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(source);

	struct color_source *srcdata = bzalloc(sizeof(struct color_source));
	srcdata->src = source;

	obs_data_set_default_int(settings, "color", 0xFFFFFFFF);
	obs_data_set_default_int(settings, "width", 800);
	obs_data_set_default_int(settings, "height", 600);

	color_source_update(srcdata, settings);

	return srcdata;
}

static void color_source_destroy(void *data)
{
	bfree(data);
}

static obs_properties_t *color_source_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();

	obs_properties_add_color(props, "color",
		obs_module_text("Color"));

	obs_properties_add_int(props, "width",
		obs_module_text("Width"), 0, 4096, 1);

	obs_properties_add_int(props, "height",
		obs_module_text("Height"), 0, 4096, 1);

	return props;
}

static void color_source_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);

	struct color_source *context = data;

	float cx = context->width;
	float cy = context->height;

	gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
	gs_eparam_t    *color = gs_effect_get_param_by_name(solid, "color");
	gs_technique_t *tech  = gs_effect_get_technique(solid, "Solid");

	struct vec4 colorVal;
	vec4_from_rgba(&colorVal, context->color);
	gs_effect_set_vec4(color, &colorVal);

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);
	gs_matrix_push();
	gs_matrix_identity();
	gs_matrix_scale3f(cx, cy, 1.0f);

	gs_draw(GS_TRISTRIP, 0, 0);

	gs_matrix_pop();
	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

static uint32_t color_source_getwidth(void *data)
{
	struct color_source *context = data;
	return context->width;
}

static uint32_t color_source_getheight(void *data)
{
	struct color_source *context = data;
	return context->height;
}

static void color_source_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "color", 0xFFFFFFFF);
}

static struct obs_source_info color_source_info = {
	.id             = "color_source",
	.type           = OBS_SOURCE_TYPE_INPUT,
	.output_flags   = OBS_SOURCE_VIDEO,
	.create         = color_source_create,
	.destroy        = color_source_destroy,
	.update         = color_source_update,
	.get_name       = color_source_get_name,
	.get_defaults   = color_source_defaults,
	.get_width      = color_source_getwidth,
	.get_height     = color_source_getheight,
	.video_render   = color_source_render,
	.get_properties = color_source_properties
};

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("color-source", "en-US")

bool obs_module_load(void)
{
	obs_register_source(&color_source_info);
	return true;
}