#pragma once

#include "raylib.h"
#include <string>
#include <cmath>
#include <limits>


#include <iostream>


typedef double N;    // Newtons
typedef double MS_1; // ms^-1
typedef double MS_2; // ms^-2
typedef double KG;   // KG
typedef double M;    // Meters
typedef double Angle; // Radians
typedef double VectorUnit; // Vector
typedef double Magnitude; // Magnitude

const double G = 6.67430e-11;

struct ForceVector {
    VectorUnit x;
    VectorUnit y;
};

M GetDistance(M x1, M y1, M x2, M y2) { // Confirmed
    M dx = fabs(x2 - x1);
    M dy = fabs(y2 - y1);
    M r = sqrt((dy * dy) + (dx * dx));
    return r;
}

N GetGravitationalForce(M r, KG m1, KG m2) { // Confirmed?
    
    N force = (G * m1 * m2)/(r*r); // Gravitation Equation
    return force;
}


Angle GetDirectionFrom_A_to_B(M x1, M y1, M x2, M y2) { // Confirmed
    M dx = fabs(x2 - x1);
    M dy = fabs(y2 - y1);

    bool isRight = x2 >= x1; // is x2 right of x1
    bool isBelow = y2 >= y1; // is x2 below x1

    if(x1 == x2) {
        if(isBelow) {
            return (3*PI)/2;
        }
        else if(!isBelow) {
            return PI/2;
        }
    } 
    else if (y1 == y2) 
    {
        if(isRight) {
            return 0;
        }
        else if (!isRight) {
            return PI;
        }
    }

    Angle theta = atan(dy/dx);
    
    if(isRight && !isBelow) {
        return theta;
    }
    else if(!isRight && !isBelow) {
        return PI - theta;
    }
    else if(!isRight && isBelow) {
        return theta + PI;
    }
    else if(isRight && isBelow) {
        return (2 * PI) - theta;
    }

    return 0.0f;
}


ForceVector splitVector(Angle direction, Magnitude magnitude) { // Confirmed
    
    if(direction == 0) {
        return {magnitude, 0};
    }
    else if(direction == (PI/2)) {
        return {0, -magnitude};
    }
    else if(direction == PI) {
        return {-magnitude, 0};
    }
    else if(direction == ((3 * PI)/2)){
        return {0, magnitude};
    }

    Angle theta;
    Magnitude Mx = 0;
    Magnitude My = 0;

    if (0<=direction && direction < PI/2) { // 0<=direction < π/2
        // +x, -y
        // No changes needed
        theta = direction;
        Mx = magnitude * cos(theta);
        My = -magnitude * sin(theta);
    }
    else if (PI/2 <= direction && direction < PI) { // π/2 <= direction < π
        // -x, -y
        theta = direction + PI;
        Mx = -magnitude * cos(theta);
        My = magnitude * sin(theta);

    }
    else if (PI <= direction && direction < ((3 * PI)/2)) { // π <= direction < 3π/2
        // -x, +y
        theta = direction - PI;
        Mx = -magnitude * cos(theta);
        My = magnitude * sin(theta);
    }
    else if (((3 * PI)/2) <= direction && direction < 2*PI) {
        // +x, +y
        theta = (2 * PI) - direction;
        Mx = magnitude * cos(theta);
        My = magnitude * sin(theta);
    }

    return {Mx, My};
}

struct BodyInfo {
    M virtualX , virtualY;
    MS_1 vx, vy;
};

class Body {
    private:
    // Mechanics
    M virtualX, virtualY, x, y;
    MS_1 vx, vy;
    MS_2 ax, ay;
    N Fx, Fy;
    KG mass;
    double scale;

    BodyInfo original;

    // Drawing
    Color col;
    float radius;
    std::string label;


    public:
    Body(std::string label, M xLocation, M yLocation, MS_1 uvx, MS_1 uvy, KG mass, float radius, Color colour, double scale) {
        this->virtualX = xLocation;
        this->x = xLocation / scale;
        this->virtualY = yLocation;
        this->y = yLocation / scale;
        this->vx = uvx;
        this->vy = uvy;
        this->mass = mass;
        this->col = colour;
        this->radius = radius;
        this->label = label;
        this->scale = scale;

        this->original.virtualX = virtualX;
        this->original.virtualY = virtualY;
        this->original.vx = vx;
        this->original.vy = vy;

    }

    std::string getLabel() {
        return label;
    }

    void simulate(double time, N effectiveForceX, N effectiveForceY) { // Grym Cydeffaith
        double t = time;
        applyForceSplit(effectiveForceX, effectiveForceY);

        ax = (Fx) / (mass); // MATHS
        ay = (Fy) / (mass); // MATHS

        printf("ax: %g ay: %g\n", ax, ay);

        ////v = u + at
        vx = vx + (ax * t);
        vy = vy + (ay * t);
    }

    void applyTranslations(double time) {
        double t = time;

        virtualX += vx * t;
        virtualY += vy * t;
        x = virtualX / scale;
        y = virtualY / scale;
    }

    void draw(bool drawLabel, bool drawDiagnostic) {
        DrawCircle(x, y, radius, col);
        
        if(drawLabel) this->drawLabel();
        if(drawDiagnostic) {
            drawResultantForce();
            drawResultantAcceleration();
            drawResultantVelocity();
        }
    }

    KG getMass() {
        return mass;
    }

    M getX() {
        return virtualX;
    }

    M getY() {
        return virtualY;
    }

    void setLocation(M x, M y) {
        this->virtualX = x;
        this->virtualY = y;
    }

    void reset() {
        virtualX  = original.virtualX;
        virtualY  = original.virtualY;
        vx = original.vx;
        vy = original.vy;
        x = virtualX / scale;
        y = virtualY / scale;
        Fx = 0;
        Fy = 0;
    }

    private:
    void drawResultantForce() {
        //printf("P1(%g, %g) P2(%g, %g) MAXINT: %d\n",x, y, x + Fx, y + Fy, std::numeric_limits<int>::max());
        DrawLine(x, y, x + Fx/((scale*scale*5e5)), y + (Fy/(scale*scale*5e5)), GREEN); // THIS IS THE PROBLEM
    }

    void drawResultantVelocity() {
        DrawLine(x, y, x + vx, y + vy, BLUE);
    }

    void drawResultantAcceleration() {
        DrawLine(x, y, x + ax, y + ay, RED);
    }

    void drawLabel() {
        //printf("DrawText. Label: %s, x: %g, y: %g\n", label.c_str(), x + radius, y + radius);
        //printf("[%s] virtual (%g, %g) real (%g, %g)", label.c_str(), virtualX, virtualY, x, y);
        DrawText(label.c_str(), x + radius, y + radius, 12.0f, col); 
    }

    void applyForceSplit(N fx, N fy) { // Reset forces after draw
        this->Fx = fx;
        this->Fy = fy;
    }
};