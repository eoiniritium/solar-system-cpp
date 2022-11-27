#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"
#include "gui.hpp"

const int ScreenWidth  = 1280;
const int ScreenHeight = 720;
const double scale = 10000000.0f; // 1px = 10,000km = 10,000,000m
double timeMultiplier = 1;

double framecount = 0; // Double so don'r run into overflow
double secondsElapsed = 0;

bool labels = true;
bool diagnostics = true;
bool isPaused = false;
bool reset = false;
bool addBody = false;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(ScreenWidth, ScreenHeight, "Solar System");

    
    std::vector<Body> bodies;
    bodies.push_back(Body("Earth A", 3500000000, 350000000, 0, 0, 5.972e24 , 10.0f, {68, 112, 105, 255}, scale)); // Earth
    bodies.push_back(Body("Moon" , 3500000000, 734400000, 1000, 0, 7.3476309e22, 10.0f, {148, 146, 142, 255}, scale)); // Moon
    bodies.push_back(Body("Earth B", 200 * scale, 350 * scale, 500,  -100, 5.972e24 , 10.0f, {68, 112, 105, 255}, scale)); // Earth
    //bodies.push_back(Body("Sun"  , 1000 * scale, 600 * scale, 0, 0, 1.989e30, 35, {252, 186, 3, 255}, scale)); // Sun

    // UI elements
    Label sliderLabel("Time multiplier", 10, ScreenHeight - 60, 20.0f, WHITE);
    Slider slider(RED, 16.0f, 70000.0f, 1000000.0f, 100000.0f,  10, ScreenHeight - 40, 700, 30);

    Label labelsToggleLabel("Labels", ScreenWidth - 240, ScreenHeight - 50, 16.0f, WHITE);
    Toggle labelsToggle(labels, ScreenWidth - 240, ScreenHeight - 30, 50, 20, RED);

    Label diagnosticsToggleLabel("Diagnostics", ScreenWidth - 335, ScreenHeight - 50, 16.0f, WHITE);
    Toggle diagnosticsToggle(diagnostics, ScreenWidth - 335, ScreenHeight - 30, 80, 20, RED);

    Label pauseText("Paused", ScreenWidth - 180, ScreenHeight - 50, 16.0f, WHITE);
    Toggle pausedToggle(isPaused, ScreenWidth - 180, ScreenHeight - 30, 50, 20, RED);
    
    Button resetBodiesButton("Reset", reset, ScreenWidth - 110, ScreenHeight - 50, 100, 40, WHITE, 16.0f);

    Button addBodyButton("Add Body", addBody, ScreenWidth - 450, ScreenHeight - 50, 100, 40, WHITE, 16.0f);

    std::string timeElapsedString = "Time elapsed: 0 days";
    Label timeElapsed(timeElapsedString, 10, 10, 16.0f, WHITE);

    AddBodyDialog newBodyDialog(bodies, scale, addBody, ScreenWidth, ScreenHeight);


    while(!WindowShouldClose()) {
        // Misc
        Vector2 MP = GetMousePosition();
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
                // Apply translations
                bodies[i].applyTranslations(dT);
            }
        }
        
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