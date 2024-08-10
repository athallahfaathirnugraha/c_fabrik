#include <c_fabrik.h>
#include <stdio.h>

int main()
{
    // create limb
    limb_t limb = newLimb(3);

    // add joints
    for (size_t i = 1; i <= 3; i++) {
        joint_t joint = {
            .x = i,
            .y = i
        };

        addJoint(&limb, joint);
    }

    // print joints
    for (size_t i = 0; i < 3; i++) {
        joint_t joint = getJoint(&limb, i);
        printf("joint %zu: %f, %f\n", i, joint.x, joint.y);
    }

    // destroy limb
    destroyLimb(&limb);

    return 0;
}