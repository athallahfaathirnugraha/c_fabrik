#include <c_fabrik.h>

#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WINSIZE 600
#define POINTRAD 5

typedef struct Palette
{
    Color bg;
    Color line;
} Palette;

int main()
{
    size_t iterNum = 10;
    bool shouldReach = false;
    bool ensureReach = false;

    minAngle_t defaultMinAngle = {
        .left = 0.75,
        .right = 0.75
    };

    // create limb
    limb_t limb = newLimb(3);

    InitWindow(WINSIZE, WINSIZE, "example");

    Palette colors = {
        .bg = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)),
        .line = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR))
    };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        bool inUi = false;

        {
            Rectangle clearBtn = { .x = 30, .y = 30, .width = 50, .height = 20 };
            Rectangle iterNumLabel = { .x = 30, .y = 65, .width = 170, .height = 10 };
            Rectangle iterNumSlider = { .x = 30, .y = 85, .width = 70, .height = 10 };
            Rectangle controlsLabel = { .x = 30, .y = 110, .width = 170, .height = 20 };
            Rectangle ensureReachBox = { .x = 30, .y = 145, .width = 10, .height = 10 };
            Rectangle ensureReachLabel = { .x = 50, .y = 145, .width = 170, .height = 10 };
            Rectangle angleLabel = { .x = 30, .y = 170, .width = 170, .height = 10};
            Rectangle leftAngleSlider = { .x = 30, .y = 190, .width = 50, .height = 10 };
            Rectangle rightAngleSlider = { .x = 110, .y = 190, .width = 50, .height = 10 };

            if (GuiButton(clearBtn, "clear")) {
                clearJoint(&limb);
                inUi = true;
            }

            GuiLabel(iterNumLabel, "number of iteration per frame:");

            float fIterNum = iterNum;

            if (GuiSlider(iterNumSlider, "0", "30", &fIterNum, 0, 30)) {
                iterNum = fIterNum;
                inUi = true;
            }

            GuiLabel(controlsLabel, "space - continue/pause\nright arrow - iter");

            GuiCheckBox(ensureReachBox, "", &ensureReach);

            GuiLabel(ensureReachLabel, "ensure reach");

            GuiLabel(angleLabel, "min angle");

            float leftMinAngle;

            if (GuiSlider(leftAngleSlider, "0", "pi", &leftMinAngle, 0, M_PI)) {
                defaultMinAngle.left = leftMinAngle;
                inUi = true;
            }

            float rightMinAngle;

            if (GuiSlider(rightAngleSlider, "0", "pi", &rightMinAngle, 0, M_PI)) {
                defaultMinAngle.right = rightMinAngle;
                inUi = true;
            }

            if (!inUi) {
                const size_t boundLen = 6;

                Rectangle rectList[boundLen] = {
                    clearBtn,
                    iterNumLabel,
                    iterNumSlider,
                    controlsLabel,
                    ensureReachBox,
                    ensureReachLabel
                };

                for (size_t i = 0; i < boundLen; i++) {
                    if (CheckCollisionPointRec(mousePos, rectList[i])) {
                        inUi = true;
                        break;
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inUi) {
            // add joint
            joint_t joint = {
                .x = mousePos.x,
                .y = mousePos.y,
                .minAngle = defaultMinAngle
            };

            addJoint(&limb, joint);
        }

        if (IsKeyPressed(KEY_SPACE)) shouldReach = !shouldReach;

        if (shouldReach) reach(&limb, mousePos.x, mousePos.y, iterNum, ensureReach);
        if (IsKeyPressed(KEY_RIGHT)) reach(&limb, mousePos.x, mousePos.y, 1, ensureReach);

        if (IsKeyPressed(KEY_A) && jointLen(&limb) >= 3) {
            printf("2nd joint left angle: %f\n", leftAngle(&limb, 1) * RAD2DEG);
            printf("2nd joint right angle: %f\n", rightAngle(&limb, 1) * RAD2DEG);
        }

        BeginDrawing();

        ClearBackground(colors.bg);

        for (size_t i = 0; i < jointLen(&limb); i++) {
            joint_t *a = getJoint(&limb, i);
            DrawCircle(a->x, a->y, POINTRAD, colors.line);

            if (i >= 1 && i < jointLen(&limb) - 1) {
                float angles[] = {
                    leftMidAngle(&limb, i),
                    leftAngle(&limb, i),
                    rightMidAngle(&limb, i),
                    rightAngle(&limb, i)
                };

                for (int i = 0; i < 4; i += 2) {
                    float posAngle = angles[i];

                    float dx = cos(posAngle);
                    float dy = -sin(posAngle); // flipped y axis (raylib)

                    float dist = 25.;

                    float angle = angles[i + 1] * RAD2DEG;

                    size_t strLen = snprintf(NULL, 0, "%.1f", angle);
                    char *str = malloc(strLen + 1);
                    snprintf(str, strLen + 1, "%.1f", angle);

                    Vector2 strSize = MeasureTextEx(GetFontDefault(), str, 10, 1.);

                    DrawText(
                        str,
                        a->x + dx * dist - strSize.x / 2.,
                        a->y + dy * dist - strSize.y / 2.,
                        10,
                        colors.line
                    );

                    free(str);
                }
            }

            if (i + 1 < jointLen(&limb)) {
                // draw line between a & b
                joint_t *b = getJoint(&limb, i + 1);
                DrawLine(a->x, a->y, b->x, b->y, colors.line);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    // destroy limb
    destroyLimb(&limb);

    return 0;
}