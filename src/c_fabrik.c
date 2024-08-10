#include <c_fabrik.h>

#include <stdio.h>
#include <stdlib.h>

#include <vector.h>

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

joint_t getJoint(limb_t *limb, size_t index)
{
    return *(joint_t*)vector_get(limb->limbs, index);
}

void addJoint(limb_t *limb, joint_t joint)
{
    vector_push_back(limb->limbs, &joint);
}
