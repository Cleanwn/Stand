#pragma once

enum eAnchorPoints
{
	// Note: if this changes, please remember to update the corresponding <enumdef>
	// in 'pedvariations.psc'.

	ANCHOR_NONE = -1,
	ANCHOR_HEAD = 0,
	ANCHOR_EYES,
	ANCHOR_EARS,
	ANCHOR_MOUTH,
	ANCHOR_LEFT_HAND,
	ANCHOR_RIGHT_HAND,
	ANCHOR_LEFT_WRIST,
	ANCHOR_RIGHT_WRIST,
	ANCHOR_HIP,
	ANCHOR_LEFT_FOOT,
	ANCHOR_RIGHT_FOOT,
	ANCHOR_PH_L_HAND,
	ANCHOR_PH_R_HAND,
	NUM_ANCHORS,
};

class CPedPropData;

class CPedPropInfo
{
};
