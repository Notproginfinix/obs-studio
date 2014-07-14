#include <util/text-lookup.h>
#include <util/dstr.h>
#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("rtmp-services", "en-US")

extern struct obs_service_info rtmp_common_service;
extern struct obs_service_info rtmp_custom_service;

bool obs_module_load(uint32_t libobs_ver)
{
	obs_register_service(&rtmp_common_service);
	obs_register_service(&rtmp_custom_service);

	UNUSED_PARAMETER(libobs_ver);
	return true;
}

void obs_module_unload(void)
{
	OBS_MODULE_FREE_DEFAULT_LOCALE();
}
