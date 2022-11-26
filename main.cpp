#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"
#include "gui.hpp"

const int ScreenWidth  = 1280;
const int ScreenHeight = 720;
const double scale = 100000000.0f; // 1px = 100,000km = 100,000,000m
const double timeMultiplier = 1;

bool labels = true;
bool diagnostics = true;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(ScreenWidth, ScreenHeight, "Solar System");

    double framecount = 0; // Double so don'r run into overflow

    Label sliderLabel("Time multiplier", 10, ScreenHeight - 60, 20.0f, WHITE);
    Slider slider(RED, 16.0f, 0.01f, 10, 10, ScreenHeight - 40, 250, 30);

    std::vector<Body> bodies;
    bodies.push_back(Body("Earth", ScreenWidth/2, ScreenHeight/2, 0, 0, 5.972e7 , 10.0f, {68, 112, 105, 255})); // Earth
    bodies.push_back(Body("Moon", 500, 500, 100, 0, 7.3476e7, 10.0f, {148, 146, 142, 255})); // Moon
    bodies.push_back(Body("Sun", 1000, 600, 0, 0, 100e7, 35, {252, 186, 3, 255})); // Sun

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            Vector2 MP = GetMousePosition();
            bodies[1].setLocation(MP.x, MP.y); // Moon
            for(size_t i = 0; i < bodies.size(); ++i) {
                N Fx = 0;
                N Fy = 0;
                for(size_t k = 0; k < bodies.size(); ++k) {
                    if (i == k) continue; // Can't compare with self

                    M currX = bodies[i].getX();
                    M currY = bodies[i].getY();

                    M pairX = bodies[k].getX();
                    M pairY = bodies[k].getY();

                    KG currMass = bodies[i].getMass();
                    KG pairMass = bodies[k].getMass();


                    Angle dir = GetDirectionFrom_A_to_B(currX, currY, pairX, pairY); // Confirmed
                    M distance = GetDistance(currX, currY, pairX, pairY); // Confirmed
                    N gravForce = GetGravitationalForce(distance, currMass, pairMass); // Confirmed
                    ForceVector force = splitVector(dir, gravForce); // Confirmed
                    Fx += force.x;
                    Fy += force.y;
                }
                bodies[i].simulate(timeMultiplier, scale, Fx, Fy);
            }
            
            // Drawing
            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].draw(labels, diagnostics);
            }

            sliderLabel.draw();
            slider.draw(MP);
            
            

            // Diagnostic information
            if(diagnostics) {
                std::string frameText = "Frame #: " + std::to_string((int)++framecount);
                DrawText(frameText.c_str() , 10, 30, 20.0f, DARKGREEN);
                std::string dtText = "Delta T: " + std::to_string(GetFrameTime()) + "s";
                DrawText(dtText.c_str() , 10, 50, 20.0f, DARKGREEN);
                DrawFPS(10, 10);

                DrawText("Resultant Force", ScreenWidth - 200, 10, 16, GREEN);
                DrawText("Resultant Acceleration", ScreenWidth - 200, 30, 16, RED);
                DrawText("Resultant Velocity", ScreenWidth - 200, 50, 16, BLUE);
            }
        EndDrawing();
    }

    return 0;
}