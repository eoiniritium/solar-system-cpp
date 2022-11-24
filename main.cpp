#include <iostream>
#include <vector>
#include "raylib.h"
#include "body.hpp"

const int ScreenWidth  = 800;
const int ScreenHeight = 800;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(360);
    InitWindow(ScreenHeight, ScreenWidth, "Solar System");

    std::vector<Body> bodies;
    bodies.push_back(Body("OBJ 1", 100, 100, 0, 0, 100000000, 10, GREEN));
    bodies.push_back(Body("OBJ 2", 500, 500, 0, 0, 100000000, 10.0f, WHITE));

    bool labels = true;
    bool diagnostics = true;

    while(!WindowShouldClose()) {
        Vector2 mp = GetMousePosition();
        bodies[0].setLocation(mp.x, mp.y);

        //Angle dir = GetDirectionFrom_1_to_2_(A.getX(), A.getY(), B.getX(), B.getY());
        
        //A.drawLineAngle(dir, 0.0f);
        
        BeginDrawing();
            ClearBackground(BLACK);
            for(size_t i = 0; i < bodies.size(); ++i) {
                N Fx = 0;
                N Fy = 0;
                for(size_t j = 0; j < bodies.size(); j++) {
                    //Body comp = bodies[i];
                    if (i == j) continue;

                    Angle dir = GetDirectionFrom_1_to_2_(bodies[i].getX(), bodies[i].getY(), bodies[j].getX(), bodies[j].getY());
                    N gravForce = GetGravitationalForce(bodies[i].getX(), bodies[i].getY(), bodies[j].getX(), bodies[j].getY(), bodies[i].getMass(), bodies[j].getMass());
                    ForceVector FVEC = splitVector(dir, gravForce);
                    Fx += FVEC.x;
                    Fy += FVEC.y;
                    printf("Dir: %f Fg: %f\t", dir * RAD2DEG, gravForce);
                }
                //printf("Fx: %f Fy: %f\n", Fx, Fy);
                printf("\n");


                bodies[i].simulate(Fx, Fy);
            }

            for(size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].draw(labels, diagnostics);
            }

        EndDrawing();
    }

    return 0;
}