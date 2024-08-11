#include <c_fabrik.h>

#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>

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
            Rectangle clearBtn = { .x = 30, .y = 30, .width = 70, .height = 30 };
            Rectangle iterNumSlider = { .x = 30, .y = 70, .width = 70, .height = 10 };

            if (GuiButton(clearBtn, "clear")) {
                clearJoint(&limb);
                inUi = true;
            }

            float fIterNum = iterNum;

            if (GuiSlider(iterNumSlider, "0", "30", &fIterNum, 0, 30)) {
                iterNum = fIterNum;
                inUi = true;
            }

            if (!inUi) {
                const size_t boundLen = 2;
                Rectangle rectList[boundLen] = { clearBtn, iterNumSlider };

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
                .y = mousePos.y
            };

            addJoint(&limb, joint);
        }

        reach(&limb, mousePos.x, mousePos.y, iterNum);

        BeginDrawing();

        ClearBackground(colors.bg);

        for (size_t i = 0; i < jointLen(&limb); i++) {
            joint_t *a = getJoint(&limb, i);
            DrawCircle(a->x, a->y, POINTRAD, colors.line);

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