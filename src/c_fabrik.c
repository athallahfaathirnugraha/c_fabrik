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

void getAngles(limb_t *limb, size_t index, float *outPrev, float *outNext)
{
    joint_t *joint = getJoint(limb, index);
    joint_t *prevJoint = getJoint(limb, index - 1);
    joint_t *nextJoint = getJoint(limb, index + 1);

    *outPrev = atan2f(joint->y - prevJoint->y, prevJoint->x - joint->x);
    *outNext = atan2f(joint->y - nextJoint->y, nextJoint->x - joint->x);
}

float leftAngle(limb_t *limb, size_t index)
{
    float prevAngle;
    float nextAngle;
    getAngles(limb, index, &prevAngle, &nextAngle);

    float res = prevAngle - nextAngle;
    while (res < 0) res += 2 * M_PI;

    return res;
}

float rightAngle(limb_t *limb, size_t index)
{
    float prevAngle;
    float nextAngle;
    getAngles(limb, index, &prevAngle, &nextAngle);

    float res = nextAngle - prevAngle;
    while (res < 0) res += 2 * M_PI;

    return res;
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

bool shouldAdjustLeftAngle(limb_t *limb, size_t index, float *outDelta)
{
    float minAngle = getJoint(limb, index)->minAngle.left;
    float delta = minAngle - leftAngle(limb, index);
    if (outDelta) *outDelta = delta;
    return delta > 0;
}

bool shouldAdjustRightAngle(limb_t *limb, size_t index, float *outDelta)
{
    float minAngle = getJoint(limb, index)->minAngle.right;
    float delta = minAngle - rightAngle(limb, index);
    if (outDelta) *outDelta = delta;
    return delta > 0;
}

void rotatePoint(
    float x,
    float y,
    float originX,
    float originY,
    float rad,
    float *outX,
    float *outY
) {
    float translatedX = x - originX;
    float translatedY = y - originY;

    float transRotatedX = translatedX * cos(rad) - translatedY * sin(-rad);
    float transRotatedY = translatedY * cos(rad) + translatedX * sin(-rad);

    float rotatedX = transRotatedX + originX;
    float rotatedY = transRotatedY + originY;

    if (outX) *outX = rotatedX;
    if (outY) *outY = rotatedY;
}

void rotateToHead(limb_t *limb, size_t startIndex, float rad)
{
    joint_t *originJoint = getJoint(limb, startIndex);

    for (int i = startIndex; i >= 0; i--) {
        joint_t *joint = getJoint(limb, i);
        
        rotatePoint(
            joint->x,
            joint->y,
            originJoint->x,
            originJoint->y,
            rad,
            &joint->x,
            &joint->y
        );
    }
}

void rotateToTail(limb_t *limb, size_t startIndex, float rad)
{
    joint_t *originJoint = getJoint(limb, startIndex);
    
    for (size_t i = startIndex; i < jointLen(limb); i++) {
        joint_t *joint = getJoint(limb, i);
        
        rotatePoint(
            joint->x,
            joint->y,
            originJoint->x,
            originJoint->y,
            rad,
            &joint->x,
            &joint->y
        );
    }
}

// ensures limb does not have a joint that goes past min angle
static void adjustAngle(limb_t *limb, bool ensureReach)
{
    float delta;
    
    if (ensureReach) {
        // start on tail
        for (int i = jointLen(limb) - 2; i >= 1; i--) {
            if (shouldAdjustLeftAngle(limb, i, &delta)) {
                rotateToHead(limb, i, delta / 2.);
            } else if (shouldAdjustRightAngle(limb, i, &delta)) {
                rotateToHead(limb, i, -delta / 2.);
            }
        }
    } else {
        // start on head
        for (size_t i = 1; i < jointLen(limb) - 1; i++) {
            if (shouldAdjustLeftAngle(limb, i, &delta)) {
                rotateToTail(limb, i, -delta / 2.);
            } else if (shouldAdjustRightAngle(limb, i, &delta)) {
                rotateToTail(limb, i, delta / 2.);
            }
        }
    }
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

    // ensure does not go past min angle
    adjustAngle(limb, ensureReach);
}

static void pullJoint(limb_t *limb, size_t jointIndex, size_t targetIndex)
{
    joint_t *joint = getJoint(limb, jointIndex);
    joint_t *target = getJoint(limb, targetIndex);

    float targetDist =
        (targetIndex < jointIndex) ?
        target->distToNext : joint->distToNext;
    
    float dist = fdist(joint->x, joint->y, target->x, target->y);
    float deltaDist = dist - targetDist;

    float xk = (target->x - joint->x) / dist;
    float yk = (target->y - joint->y) / dist;

    float deltaX = deltaDist * xk;
    float deltaY = deltaDist * yk;

    joint->x += deltaX;
    joint->y += deltaY;
}

void pullHead(limb_t *limb, float targetX, float targetY)
{
    // pull head
    joint_t *head = headJoint(limb);

    head->x = targetX;
    head->y = targetY;

    // pull rest of limb
    for (size_t i = 1; i < jointLen(limb); i++) {
        pullJoint(limb, i, i - 1);
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
        pullJoint(limb, i, i + 1);
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
