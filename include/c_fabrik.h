#ifndef _C_FABRIK_
#define _C_FABRIK_

#include <stdlib.h>
#include <stdbool.h>

// vector.h
typedef struct Vector Vector;

// angle between joints must be bigger than left/right
// in radians
typedef struct minAngle_t
{
    float left;
    float right;
} minAngle_t;

typedef struct joint_t
{
    float x;
    float y;
    float distToNext;
    minAngle_t minAngle;
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
joint_t *getJoint(limb_t *limb, size_t index);
// get first joint in a limb
joint_t *headJoint(limb_t *limb);
// get last joint in a limb
joint_t *tailJoint(limb_t *limb);

// left angle of a joint
// don't call on head and tail
// always returns a positive value
float leftAngle(limb_t *limb, size_t index);
// right angle of a joint
// don't call on head and tail
// always returns a positive value
float rightAngle(limb_t *limb, size_t index);

// return the middle of the left angle
float leftMidAngle(limb_t *limb, size_t index);
// return the middle of the right angle
float rightMidAngle(limb_t *limb, size_t index);

// returns true if left angle < min angle
// delta is the difference between the min angle and the left angle
bool shouldAdjustLeftAngle(limb_t *limb, size_t index, float *outDelta);
// returns true if right angle < min angle
// delta is the difference between the min angle and the right angle
bool shouldAdjustRightAngle(limb_t *limb, size_t index, float *outDelta);

// rotate one joint after startIndex (exclusive) to head
// startIndex will be the origin. the rest will be pulled
void rotateToHead(limb_t *limb, size_t startIndex, float rad);
// rotate one joint from startIndex (exclusive) to tail
// startIndex will be the origin. the rest will be pulled
void rotateToTail(limb_t *limb, size_t startIndex, float rad);

// make the limb reach for a point
// if ensureReach, will drag the head when target is unreachable
void reach(limb_t *limb, float targetX, float targetY, size_t iterNum, bool ensureReach);
// pull joint to target
void pullJoint(limb_t *limb, size_t jointIndex, size_t targetIndex);
// pull the start of the limb to a target
void pullHead(limb_t *limb, float targetX, float targetY);
// pull the end of the limb to a target
void pullTail(limb_t *limb, float targetX, float targetY);

// add a joint to a limb
void addJoint(limb_t *limb, joint_t joint);

// remove all joint in a limb
void clearJoint(limb_t *limb);

#endif
