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
    bodies.push_back(Body("Earth", ScreenWidth/2, ScreenHeight/2,       0, 0, 5.972e24 , 10.0f, GREEN)); // Earth
    bodies.push_back(Body("Moon", 500, 500, 100, 0, 7.3476e22, 10.0f, WHITE)); // Moon

    double framecount = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            printf("Frame: %g\n", ++framecount);
            for(size_t i = 0; i < bodies.size(); ++i) {
                N Fx = 0;
                N Fy = 0;
                for(size_t j = 0; j < bodies.size(); j++) {
                    //Body comp = bodies[i];
                    if (i == j) continue;

                    //N currX = convertToRealMovementValues(scale, bodies[i].getX());
                    //N currY = convertToRealMovementValues(scale, bodies[i].getY());

                    //N pairX = convertToRealMovementValues(scale, bodies[j].getX());
                    //N pairY = convertToRealMovementValues(scale, bodies[j].getY());

                    N currX = bodies[i].getX();
                    N currY = bodies[i].getY();

                    N pairX = bodies[j].getX();
                    N pairY = bodies[j].getY();

                    KG currMass = bodies[i].getMass();
                    KG pairMass = bodies[j].getMass();


                    Angle dir = GetDirectionFrom_1_to_2_(currX, currY, pairX, pairY);
                    M distance = GetDistance(scale, currX, currY, pairX, pairY);
                    N gravForce = GetGravitationalForce(distance, currMass, pairMass);
                    ForceVector FVEC = splitVector(dir, gravForce);
                    Fx += FVEC.x;
                    Fy += FVEC.y;
                    printf("[%5s] Dir: %f Fg: %fN\t", bodies[i].getLabel().c_str(), dir * RAD2DEG, gravForce);
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