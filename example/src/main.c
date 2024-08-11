// TODO: customizable settings

#include <c_fabrik.h>
#include <raylib.h>
#include <stdio.h>

#define WINSIZE 600
#define POINTRAD 5

#define JOINTNUM 4
#define ITERNUM 10

int main()
{
    // create limb
    limb_t limb = newLimb(3);

    // add joints
    for (size_t i = 0; i < JOINTNUM; i++) {
        joint_t joint = {
            .x = (float)WINSIZE / 2 + i * 30,
            .y = (float)WINSIZE / 2 + i * 30,
        };

        addJoint(&limb, joint);
    }

    // print joints
    for (size_t i = 0; i < jointLen(&limb); i++) {
        joint_t *joint = getJoint(&limb, i);
        printf("joint %zu: %f, %f\n", i, joint->x, joint->y);
    }

    InitWindow(WINSIZE, WINSIZE, "example");

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        reach(&limb, mousePos.x, mousePos.y, ITERNUM);

        BeginDrawing();

        ClearBackground(BLACK);

        for (size_t i = 0; i < jointLen(&limb); i++) {
            joint_t *a = getJoint(&limb, i);
            DrawCircle(a->x, a->y, POINTRAD, WHITE);

            if (i + 1 < jointLen(&limb)) {
                // draw line between a & b
                joint_t *b = getJoint(&limb, i + 1);
                DrawLine(a->x, a->y, b->x, b->y, WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    // destroy limb
    destroyLimb(&limb);

    return 0;
}