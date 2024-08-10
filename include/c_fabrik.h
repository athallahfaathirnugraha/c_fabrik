#ifndef _C_FABRIK_
#define _C_FABRIK_

#include <stdlib.h>

// vector.h
typedef struct Vector Vector;

typedef struct joint_t
{
    float x;
    float y;
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

// free allocated memories for limb
void destroyLimb(limb_t *limb);

// returns the number of joints in a limb
size_t jointLen(limb_t *limb);

// get n-th joint from a limb
joint_t getJoint(limb_t *limb, size_t index);

// add a joint to a limb
void addJoint(limb_t *limb, joint_t joint);

#endif