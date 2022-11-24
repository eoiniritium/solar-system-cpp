#include "raylib.h"
#include <string>
#include <cmath>


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

M convertToRealMovementValues(double scale, M pixelDistance) {
    return (scale) * pixelDistance;
}

M GetDistance(double scale, M x1, M y1, M x2, M y2) {
    M dx = fabs(x2 - x1);
    M dy = fabs(y2 - y1);
    M r = sqrt((dy * dy) + (dx * dx));

    return r;
}

N GetGravitationalForce(M r, KG m1, KG m2) {
    
    N force = G * ((m1 * m2)/ (r*r));

    return force;
}

Angle GetDirectionFrom_1_to_2_(M x1, M y1, M x2, M y2) {
    M dx = fabs(x2 - x1);
    M dy = fabs(y2 - y1);
    //printf("dx: %f dy: %f\n", dx, dy);

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
    
    //printf("Is right: %d IsBelow: %d\n", isRight, isBelow);

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

    std::cout << "ANGLE CALCULATION ERROR!" << std::endl;

    return 0.0f;
}


ForceVector splitVector(float direction, Magnitude magnitude) {
    // From positive X
    /*
            y
            |
      -x<---|--->x
            |
           -y

    +-----------x
    |
    |
    |
    |
    y
    */

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

class Body {
    private:
    // Mechanics
    M x, y;
    MS_1 vx, vy;
    MS_2 ax, ay;
    N Fx, Fy;
    KG mass;

    // Drawing
    Color col;
    float radius;
    std::string label;


    public:
    Body(std::string label, M xLocation, M yLocation, MS_1 uvx, MS_1 uvy, KG mass, float radius, Color colour) {
        this->x = xLocation;
        this->y = yLocation;
        this->vx = uvx;
        this->vy = uvy;
        this->mass = mass;
        this->col = colour;
        this->radius = radius;
        this->label = label;
    }

    std::string getLabel() {
        return label;
    }

    void simulate(double timeMultiplier, double scale, N effectiveForceX, N effectiveForceY) { // Grym Cydeffaith
        double dt = GetFrameTime();

        Fx = effectiveForceX;
        Fy = effectiveForceY;

        ax = Fx / mass;
        ay = Fy / mass;
        printf("\tFx: %fms^-1 Fy: %fms^-1 AX: %fms^-2 AY: %fms^-2\n", vx, vy, ax, ay);

        //v = u + at
        vx += ay * dt;
        vy += ay * dt;

        x += (vx*dt * timeMultiplier)/scale;
        y += (vy*dt * timeMultiplier)/scale;
    }

    void draw(bool drawLabel, bool drawDiagnostic) {
        if(drawLabel) this->drawLabel();
        if(drawDiagnostic) {
            drawResultantForce();
            drawResultantAcceleration();
            drawResultantVelocity();
            //printf("[%s] X: %f Y: %f R: %f\n", label.c_str(), x, y, radius);
        }

        DrawCircle(x, y, radius, col);    
    }

    KG getMass() {
        return mass;
    }

    M getX() {
        return x;
    }

    M getY() {
        return y;
    }

    void setLocation(M x, M y) {
        this->x = x;
        this->y = y;
    }

    private:

    void drawResultantForce() {
        DrawLine(x, y, x + Fx, y + Fy, WHITE);
    }

    void drawResultantAcceleration() {
        DrawLine(x, y, x + ax, y + ay, RED);
    }

    void drawResultantVelocity() {
        DrawLine(x, y, x + vx, y + vy, BLUE);
    }

    void drawLabel() {
       DrawText(label.c_str(), x + radius, y + radius, 12.0f, col); 
    }

    void applyForceSplit(N fx, N fy) { // Reset forces after draw
        this->Fx = fx;
        this->Fy = fy;
    }

    public:
    void drawLineAngle(Angle angle, float magnitude) {
        DrawLine(x, y, x + magnitude * cos(angle), y + magnitude * sin(angle), BLUE);
    }
};