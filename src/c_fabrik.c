#include <c_fabrik.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <vector.h>

static float fdist(float ax, float ay, float bx, float by)
{
    return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

limb_t newLimb(size_t initJointNum)
{
    limb_t limb;

    limb.limbs = malloc(sizeof(Vector));
    vector_setup(limb.limbs, initJointNum, sizeof(joint_t));

    return limb;
}

void destroyLimb(limb_t *limb)
{
    vector_destroy(limb->limbs);
    free(limb->limbs);
}

size_t jointLen(limb_t *limb)
{
    return limb->limbs->size;
}

joint_t *getJoint(limb_t *limb, size_t index)
{
    return (joint_t*)vector_get(limb->limbs, index);
}

joint_t *headJoint(limb_t *limb)
{
    return getJoint(limb, 0);
}

joint_t *tailJoint(limb_t *limb)
{
    return getJoint(limb, jointLen(limb) - 1);
}

static void internalGetAngles(
    joint_t *prev,
    joint_t *curr,
    joint_t *next,
    float *outPrev,
    float *outNext
) {
    *outPrev = atan2f(curr->y - prev->y, prev->x - curr->x);
    *outNext = atan2f(curr->y - next->y, next->x - curr->x);
}

void getAngles(limb_t *limb, size_t index, float *outPrev, float *outNext)
{
    joint_t *joint = getJoint(limb, index);
    joint_t *prevJoint = getJoint(limb, index - 1);
    joint_t *nextJoint = getJoint(limb, index + 1);

    internalGetAngles(prevJoint, joint, nextJoint, outPrev, outNext);
}

static float internalLeftAngle(joint_t *prev, joint_t *curr, joint_t *next)
{
    float prevAngle, nextAngle;
    internalGetAngles(prev, curr, next, &prevAngle, &nextAngle);

    float res = prevAngle - nextAngle;
    while (res < 0) res += 2 * M_PI;

    return res;
}

float leftAngle(limb_t *limb, size_t index)
{
    joint_t *prev = getJoint(limb, index - 1);
    joint_t *curr = getJoint(limb, index);
    joint_t *next = getJoint(limb, index + 1);

    return internalLeftAngle(prev, curr, next);
}

static float internalRightAngle(joint_t *prev, joint_t *curr, joint_t *next)
{
    float prevAngle, nextAngle;
    internalGetAngles(prev, curr, next, &prevAngle, &nextAngle);

    float res = nextAngle - prevAngle;
    while (res < 0) res += 2 * M_PI;

    return res;
}

float rightAngle(limb_t *limb, size_t index)
{
    joint_t *prev = getJoint(limb, index - 1);
    joint_t *curr = getJoint(limb, index);
    joint_t *next = getJoint(limb, index + 1);

    return internalRightAngle(prev, curr, next);
}

float rightMidAngle(limb_t *limb, size_t index)
{
    float prevAngle, nextAngle;
    getAngles(limb, index, &prevAngle, &nextAngle);

    float res = prevAngle + (nextAngle - prevAngle) / 2.;
    if (res > nextAngle || res < prevAngle) res += M_PI;

    while (res > 2 * M_PI) res -= 2 * M_PI;
    while (res < 0) res += 2 * M_PI;

    return res;
}

float leftMidAngle(limb_t *limb, size_t index)
{
    float res = rightMidAngle(limb, index) + M_PI;

    while (res > 2 * M_PI) res -= 2 * M_PI;
    while (res < 0) res += 2 * M_PI;

    return res;
}

void reach(limb_t *limb, float targetX, float targetY, size_t iterNum, bool ensureReach)
{
    if (jointLen(limb) == 0) return;
    
    float headX = headJoint(limb)->x;
    float headY = headJoint(limb)->y;

    for (size_t i = 0; i < iterNum; i++) {
        pullTail(limb, targetX, targetY);
        pullHead(limb, headX, headY);
    }

    if (ensureReach) {
        pullTail(limb, targetX, targetY);
    }
}

static void pullJoint(joint_t *joint, joint_t *target, float targetDist)
{
    float dist = fdist(joint->x, joint->y, target->x, target->y);
    float deltaDist = dist - targetDist;

    float xk = (target->x - joint->x) / dist;
    float yk = (target->y - joint->y) / dist;

    float deltaX = deltaDist * xk;
    float deltaY = deltaDist * yk;

    joint->x += deltaX;
    joint->y += deltaY;

    // adjust for min angle
}

void pullHead(limb_t *limb, float targetX, float targetY)
{
    // pull head
    joint_t *head = headJoint(limb);

    head->x = targetX;
    head->y = targetY;

    // pull rest of limb
    for (size_t i = 1; i < jointLen(limb); i++) {
        joint_t *joint = getJoint(limb, i);
        joint_t *prevJoint = getJoint(limb, i - 1);

        pullJoint(joint, prevJoint, prevJoint->distToNext);
    }
}

void pullTail(limb_t *limb, float targetX, float targetY)
{
    // pull tail
    joint_t *tail = tailJoint(limb);

    tail->x = targetX;
    tail->y = targetY;
    
    // pull rest of limb
    for (int i = jointLen(limb) - 2; i >= 0; i--) {
        joint_t *joint = getJoint(limb, i);
        joint_t *nextJoint = getJoint(limb, i + 1);

        pullJoint(joint, nextJoint, joint->distToNext);
    }
}

void addJoint(limb_t *limb, joint_t joint)
{
    if (jointLen(limb) > 0) {
        joint_t *tail = tailJoint(limb);
        tail->distToNext = fdist(tail->x, tail->y, joint.x, joint.y);
    }
    
    vector_push_back(limb->limbs, &joint);
}

void clearJoint(limb_t *limb)
{
    vector_clear(limb->limbs);
}
