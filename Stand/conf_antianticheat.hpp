#pragma once

#define HTTP_HOOK true
#define BLOCK_ALL_METRICS false
#define CLEAR_BONUS_ON_DL false
#ifdef STAND_DEBUG
#define PREVENT_PGSTREAMER_CRASH true
#define PREVENT_GBUFFER_CRASH true
#define PREVENT_GRCDEVICE_CRASH true
#else
#define PREVENT_PGSTREAMER_CRASH false
#define PREVENT_GBUFFER_CRASH false
#define PREVENT_GRCDEVICE_CRASH false
#endif
