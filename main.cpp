#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"

const int ScreenWidth  = 1280;
const int ScreenHeight = 720;
const double scale = 100000000.0f; // 1px = 100,000km = 100,000,000m
const double timeMultiplier = 1;

bool labels = true;
bool diagnostics = true;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);
    InitWindow(ScreenWidth, ScreenHeight, "Solar System");

    std::vector<Body> bodies;
    bodies.push_back(Body("Earth", ScreenWidth/2, ScreenHeight/2,       0, 0, 5.972e7 , 10.0f, GREEN)); // Earth
    bodies.push_back(Body("Moon", 500, 500, 100, 0, 7.3476e7, 10.0f, WHITE)); // Moon
    //bodies.push_back(Body("Sun", 1000, 600, 0, 0, 1000.0f, 35, RED));

    double framecount = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            printf("Frame: %g\n", ++framecount);
            Vector2 MP = GetMousePosition();
            bodies[1].setLocation(MP.x, MP.y); // Moon
            for(size_t i = 0; i < bodies.size(); ++i) {
                N Fx = 0;
                N Fy = 0;
                for(size_t k = 0; k < bodies.size(); k++) {
                    if (i == k) continue; // Can't compare with self

                    N currX = bodies[i].getX();
                    N currY = bodies[i].getY();

                    N pairX = bodies[k].getX();
                    N pairY = bodies[k].getY();

                    KG currMass = bodies[k].getMass();
                    KG pairMass = bodies[k].getMass();


                    Angle dir = GetDirectionFrom_1_to_2_(currX, currY, pairX, pairY); // Confirmed
                    M distance = GetDistance(currX, currY, pairX, pairY); // Confirmed
                    N gravForce = GetGravitationalForce(distance, currMass, pairMass); // Problem
                    //N gravForce = 100;
                    ForceVector FVEC = splitVector(dir, gravForce);
                    Fx += FVEC.x;
                    Fy += FVEC.y;
                }
                bodies[i].simulate(timeMultiplier, scale, Fx, Fy);
            }

            printf("\n");

            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].draw(labels, diagnostics);
            }

            // INFO BOX
            DrawText("Resultant Force", ScreenWidth - 200, 10, 16, WHITE);
            DrawText("Resultant Acceleration", ScreenWidth - 200, 30, 16, RED);
            DrawText("Resultant Velocity", ScreenWidth - 200, 50, 16, BLUE);

        EndDrawing();
    }

    return 0;
}