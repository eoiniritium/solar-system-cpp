#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"
#include "gui.hpp"

const int ScreenWidth  = 1280;
const int ScreenHeight = 720;
double scale = 1000000.0f; // 1px = 1,000,000m
double timeMultiplier = 1;
float mouseWheel;

double framecount = 0; // Double so don'r run into overflow
double secondsElapsed = 0;

bool labels = true;
bool diagnostics = true;
bool isPaused = true;
bool reset = false;
bool addBody = false;
bool removeBodies = false;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(ScreenWidth, ScreenHeight, "Solar System");

    std::vector<Body> bodies;
    
    Vector2 MP = GetMousePosition();

    // Camera settings
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    float cameraZoom = 1.0f;
    Vector2 cameraTarget = {ScreenWidth/2, ScreenHeight/2};
    camera.target = MP;
    camera.offset = GetMousePosition();
    int cameraSpeed = 5;
    float zoomIncrement = 0.0125f;

    
    

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

    AddBodyDialog newBodyDialog(bodies, scale, addBody, ScreenWidth, ScreenHeight);

    Button removeAllBodies("Remove all", removeBodies, ScreenWidth - 110, ScreenHeight - 50, 100, 40, RED, 16.0f);

    

    while(!WindowShouldClose()) {
        // Misc
        MP = GetMousePosition();
        timeMultiplier = slider.getValue();
        timeElapsedString = "Time elapsed: " + removeTrailingCharacters(std::to_string(roundDecimalPlaces(secondsElapsed / 86400, 3)), '0') + " days"; // divide by 86400 for Seconds -> Days
        timeElapsed.updateText(timeElapsedString);
        double dT = GetFrameTime() * timeMultiplier;

        if (!IsWindowFocused()) {
            isPaused = true;
        }

        if(addBody){
            isPaused = true;
        }

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

        // Camera control
        if(!addBody) {
            if(IsKeyDown(KEY_RIGHT)) { cameraTarget.x += cameraSpeed; }
            if(IsKeyDown(KEY_LEFT )) { cameraTarget.x -= cameraSpeed; }
            if(IsKeyDown(KEY_UP   )) { cameraTarget.y -= cameraSpeed; }
            if(IsKeyDown(KEY_DOWN )) { cameraTarget.y += cameraSpeed; }

            
            if(IsKeyDown(KEY_X    )) { cameraZoom += zoomIncrement; } // Zoom in
            if(IsKeyDown(KEY_Z    )) { cameraZoom -= zoomIncrement; } // Zoom out

            if(cameraZoom < zoomIncrement) { cameraZoom = zoomIncrement; } // Make zoom < 0 impossible

            camera.target = cameraTarget;
            camera.zoom   = cameraZoom;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            if(!addBody) { BeginMode2D(camera); }
                for(size_t i = 0; i < bodies.size(); ++i) {
                    bodies[i].draw(labels, diagnostics);
                }
            if(!addBody) { EndMode2D(); }
            

            if(!addBody) { // Draw camera infomation
                int rectWidth = 600;
                int rectHeight = 80;
                int xStart = (ScreenWidth - rectWidth)/2;

                DrawRectangle(xStart , 0, rectWidth, rectHeight, WHITE);
                DrawRectangle(xStart+2, 0, rectWidth-4, rectHeight-2, BLACK);

                int controlsWidth = 100;
                DrawRectangle(xStart + rectWidth - controlsWidth, 0, rectWidth - controlsWidth, rectHeight, WHITE);
                DrawRectangle(xStart + rectWidth - controlsWidth + 2, 0, rectWidth - controlsWidth - 4, rectHeight-2, BLACK);
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

            if (!addBody) {
                addBodyButton.draw(MP);
            }

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
                newBodyDialog.draw();
            }

            if(removeBodies) {
                bodies.clear();
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