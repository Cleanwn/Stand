#pragma once

// NOTE: When applying forces to objects be sure to choose the correct type.
// If you are not sure please ask for advice.
// Generally, use FORCE and/or TORQUES when applying forces continuously (over multiple frames). e.g. simulating wind, rockets, etc.
// Use an IMPULSE or ANGULAR_IMPULSE when applying a one off kick to an object. E.g. a bullet impact, car crash, etc.
// This will ensure your forces are consistent when the frame rate varies.
enum eCommandApplyForceTypes
{
	APPLY_TYPE_FORCE = 0,					//continuous force applied every frame, will produce a constant acceleration independent of frame rate.
	APPLY_TYPE_IMPULSE,						//Caution a single impact applied at a point in time, will produce a step change in velocity. If applied continuously will produce an acceleration that IS dependant on frame rate and may also break the physics engine, i.e. please do
	APPLY_TYPE_EXTERNAL_FORCE,		//same as a normal force but applied as if it was an external push to the object, through the breaking system. This means that breakable objects will break apart due to the force you're applying. 	
	APPLY_TYPE_EXTERNAL_IMPULSE, 	//ditto, a nomal impulse plus breaking.			
	APPLY_TYPE_TORQUE,				// Angular equivalent of force, causes continuous angular acceleration independent of framerate
	APPLY_TYPE_ANGULAR_IMPULSE		// Angular equivalent of impulse, causes instantaneous change in angular velocity
};
