#ifndef _C_FABRIK_
#define _C_FABRIK_

#include <stdlib.h>

// vector.h
typedef struct Vector Vector;

typedef struct joint_t
{
    float x;
    float y;
    float distToNext;
    // TODO: limiting angles?
} joint_t;

typedef struct limb_t
{
    // vec of joint_t
    Vector *limbs;
} limb_t;

// create a new limb.
// after limb is no longer used, destroy with destroyLimb
limb_t newLimb(size_t initJointNum);
// TODO: calcDist so we dont have to input every dist manually
// free allocated memories for limb
void destroyLimb(limb_t *limb);

// returns the number of joints in a limb
size_t jointLen(limb_t *limb);

// get n-th joint from a limb
joint_t *getJoint(limb_t *limb, size_t index);
// get first joint in a limb
joint_t *headJoint(limb_t *limb);
// get last joint in a limb
joint_t *tailJoint(limb_t *limb);

// make the limb reach for a point
void reach(limb_t *limb, float targetX, float targetY, size_t iterNum);
// pull the start of the limb to a target
void pullHead(limb_t *limb, float targetX, float targetY);
// pull the end of the limb to a target
void pullTail(limb_t *limb, float targetX, float targetY);

// add a joint to a limb
void addJoint(limb_t *limb, joint_t joint);

#endif