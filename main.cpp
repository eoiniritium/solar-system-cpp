#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"
#include "gui.hpp"

const int ScreenWidth  = 1280;
const int ScreenHeight = 720;
const double oScale = 400000000.0f;
double scale = oScale; // 1px = 400,000km = 400,000,000m
double zoomSpeed = 100000000.0f;
double timeMultiplier = 1;

double framecount = 0; // Double so don'r run into overflow
double secondsElapsed = 0;

// Camera
double cameraX     = ScreenWidth/2;
double cameraY     = ScreenHeight/2;
double cameraSpeed = 100.0f;

bool labels = true;
bool diagnostics = true;
bool isPaused = true;
bool reset = false;
bool addBody = false;
bool removeBodies = false;
bool showBodyManager = false;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(ScreenWidth, ScreenHeight, "Solar System");

    std::vector<Body> bodies;

    // UI elements
    Label sliderLabel("Time multiplier", 10, ScreenHeight - 60, 20.0f, WHITE);
    Slider slider(RED, 16.0f, 70000.0f, 10000000.0f, 70000.0f,  10, ScreenHeight - 40, 700, 30);

    Label labelsToggleLabel("Labels", ScreenWidth - 240, ScreenHeight - 50, 16.0f, WHITE);
    Toggle labelsToggle(labels, ScreenWidth - 240, ScreenHeight - 30, 50, 20, RED);

    Label diagnosticsToggleLabel("Diagnostics", ScreenWidth - 335, ScreenHeight - 50, 16.0f, WHITE);
    Toggle diagnosticsToggle(diagnostics, ScreenWidth - 335, ScreenHeight - 30, 80, 20, RED);

    Label pauseText("Paused", ScreenWidth - 180, ScreenHeight - 50, 16.0f, WHITE);
    Toggle pausedToggle(isPaused, ScreenWidth - 180, ScreenHeight - 30, 50, 20, RED);

    Button resetBodiesButton("Reset", reset, ScreenWidth - 110, ScreenHeight - 100, 100, 40, WHITE, 16.0f);

    Button addBodyButton("Add Body", addBody, ScreenWidth - 450, ScreenHeight - 50, 100, 40, WHITE, 16.0f);

    std::string timeElapsedString = "Time elapsed: 0 days";
    Label timeElapsed(timeElapsedString, 10, 10, 16.0f, WHITE);

    AddBodyDialog newBodyDialog(bodies, scale, addBody, ScreenWidth, ScreenHeight, cameraX, cameraY);

    Button removeAllBodies("Remove all", removeBodies, ScreenWidth - 110, ScreenHeight - 50, 100, 40, RED, 16.0f);

    Button showBodyManagerDialogButton("Manage Bodies", showBodyManager, ScreenWidth - 450, ScreenHeight - 100, 150, 40, WHITE, 16.0f);
    BodyManagerDialog bodyManager(bodies, showBodyManager, ScreenWidth, ScreenHeight, WHITE);

    while(!WindowShouldClose()) {
        // Misc
        Vector2 MP = GetMousePosition();
        timeMultiplier = slider.getValue();
        timeElapsedString = "Time elapsed: " + removeTrailingCharacters(std::to_string(roundDecimalPlaces(secondsElapsed / 86400, 3)), '0') + " days"; // divide by 86400 for Seconds -> Days
        timeElapsed.updateText(timeElapsedString);
        double deltaTime = GetFrameTime();
        double dT = deltaTime * timeMultiplier;

        // Camera control
        bool left, right, up, down, zoomIn, zoomOut;
        left  = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT );
        right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        up    = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP   );
        down  = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN );

        zoomIn  = IsKeyDown(KEY_X);
        zoomOut = IsKeyDown(KEY_Z);

        if(up) {
            cameraY += cameraSpeed * deltaTime;
        }
        if(down) {
            cameraY -= cameraSpeed * deltaTime;
        }
        if(right) {
            cameraX -= cameraSpeed * deltaTime;
        }
        if(left) {
            cameraX += cameraSpeed * deltaTime;
        }

        if(zoomIn) {
            scale -= zoomSpeed * deltaTime;
        }
        if(zoomOut) {
            scale += zoomSpeed * deltaTime;
        }

        printf("scale: %g%%\n", (scale*100)/oScale);

        //DrawLine(cameraX, 0, cameraX, ScreenHeight, BLUE);
        //DrawLine(0, cameraY, ScreenWidth, cameraY, BLUE);

        //printf("scale: %g\n", scale);


        if (!IsWindowFocused()) {
            isPaused = true;
        }

        if(addBody){
            isPaused = true;
        }

        // Physics
        if(!isPaused) {
            secondsElapsed += dT;

            // Force calculations
            for(size_t i = 0; i < bodies.size(); ++i) {
                N Fx = 0;
                N Fy = 0;

                M currX = bodies[i].getX();
                M currY = bodies[i].getY();

                KG currMass = bodies[i].getMass();

                for(size_t k = 0; k < bodies.size(); ++k) {
                    if (i == k) continue; // Can't compare with self

                    M pairX = bodies[k].getX();
                    M pairY = bodies[k].getY();

                    KG pairMass = bodies[k].getMass();

                    Angle dir = GetDirectionFrom_A_to_B(currX, currY, pairX, pairY); // Confirmed
                    M distance = GetDistance(currX, currY, pairX, pairY); // Confirmed
                    N gravForce = GetGravitationalForce(distance, currMass, pairMass); // Confirmed
                    ForceVector force = splitVector(dir, gravForce); // Confirmed
                    Fx += force.x;
                    Fy += force.y;
                }

                bodies[i].simulate(dT, Fx, Fy);
            }

            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].applyTranslations(dT);
            }
        }

        // Drawing
        BeginDrawing();
            ClearBackground(BLACK);

            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].draw(labels, diagnostics);
            }

            sliderLabel.draw();
            slider.draw(MP);

            labelsToggleLabel.draw();
            labelsToggle.draw(MP);

            diagnosticsToggleLabel.draw();
            diagnosticsToggle.draw(MP);

            pauseText.draw();
            pausedToggle.draw(MP);

            timeElapsed.draw();

            resetBodiesButton.draw(MP);

            removeAllBodies.draw(MP);

            // Diagnostic information
            if(diagnostics) {
                std::string frameText = "Frame #: " + std::to_string((int)++framecount);
                DrawText(frameText.c_str() , 10, 50, 20.0f, DARKGREEN);
                std::string dtText = "Delta T: " + std::to_string(GetFrameTime()) + "s";
                DrawText(dtText.c_str() , 10, 70, 20.0f, DARKGREEN);
                DrawFPS(10, 30);

                DrawText("Resultant Force", ScreenWidth - 200, 10, 16, GREEN);
                DrawText("Resultant Acceleration", ScreenWidth - 200, 30, 16, RED);
                DrawText("Resultant Velocity", ScreenWidth - 200, 50, 16, BLUE);
            }

            if (addBody) {
                if(newBodyDialog.draw()) {
                    bodyManager.updateButtons();
                }
            }
            else if(showBodyManager) {
                bodyManager.draw();
            }
            else {
                addBodyButton.draw(MP);
                showBodyManagerDialogButton.draw(MP);
            }

            if(removeBodies) {
                bodies.clear();
                bodyManager.updateButtons();
            }

        EndDrawing();

        if(reset) {
            reset = false;
            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].reset();
            }
            secondsElapsed = 0;
        }
    }

    return 0;
}