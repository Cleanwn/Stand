#pragma once

#include "fwInteriorLocation.hpp"
#include "vector.hpp"

class CLightSource
{
	PAD(0, 0x1C0);
};

struct Corona_t
{
	PAD(0, 0x30);
};

#define MAX_NUM_CORONAS_PER_JOB				200
#define MAX_NUM_LIGHTS_PER_JOB				80

#define DO_VEHICLE_LIGHTS_ASYNC_SCRATCH_SIZE	/* 45440 */ (MAX_NUM_CORONAS_PER_JOB*sizeof(Corona_t) + MAX_NUM_LIGHTS_PER_JOB*(sizeof(CLightSource)))

struct DoVehicleLightsAsyncData
{
	CVehicle** m_paVehicles;
	uint32_t* m_paLightFlags;
	rage::Vec4V* m_paAmbientVolumeParams;
	rage::fwInteriorLocation* m_paInteriorLocs;
	Corona_t* volatile* m_pCoronasCurrPtr;
	CLightSource* volatile* m_pLightSourcesCurrPtr;
	Corona_t* m_CoronasStartingPtr;
	CLightSource* m_LightSourcesStartingPtr;
	uint32_t m_MaxCoronas;
	uint32_t m_MaxLightSources;
	float m_timeCycleSpriteSize;
	float m_timeCycleSpriteBrightness;
	float m_timecycleVehicleIntensityScale;
};

class CoronaAndLightList
{
public:
	Corona_t* m_pCoronas;
	CLightSource* m_pLightSources;
	uint32_t m_coronaCount;
	uint32_t m_lightCount;
	uint32_t m_maxCoronas;
	uint32_t m_maxLights;
	float m_timeCycleSpriteSize;
	float m_timeCycleSpriteBrightness;
	float m_timecycleVehicleIntensityScale;

	void Init(Corona_t* pCoronas, CLightSource* pLightSources, uint32_t maxCoronas, uint32_t maxLights, float tcSpriteSize, float tcSpriteBrightness, float tcVehicleLightIntensity)
	{
		m_pCoronas = pCoronas;
		m_pLightSources = pLightSources;
		m_coronaCount = 0;
		m_lightCount = 0;
		m_maxCoronas = maxCoronas;
		m_maxLights = maxLights;
		m_timeCycleSpriteSize = tcSpriteSize;
		m_timeCycleSpriteBrightness = tcSpriteBrightness;
		m_timecycleVehicleIntensityScale = tcVehicleLightIntensity;
	}

	uint32_t GetCoronaCount() { return m_coronaCount; }
	uint32_t GetLightCount() { return m_lightCount; }

	float GetTCSpriteSize() { return m_timeCycleSpriteSize; }
	float GetTCSpriteBrightness() { return m_timeCycleSpriteBrightness; }
	float GetTCVehicleIntensityScale() { return m_timecycleVehicleIntensityScale; }
};
static_assert(offsetof(CoronaAndLightList, m_coronaCount) == 0x10);
static_assert(offsetof(CoronaAndLightList, m_maxCoronas) == 0x18);
