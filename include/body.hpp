#include "raylib.h"
#include <string>
#include <cmath>


#include <iostream>

typedef float N;    // Newtons
typedef float MS_1; // ms^-1
typedef float MS_2; // ms^-2
typedef float KG;   // KG
typedef float M;    // Meters
typedef float Angle;
typedef float VectorUnit;
typedef float Magnitude;

const double G = 6.67430e-11;

struct ForceVector {
    VectorUnit x;
    VectorUnit y;
};

N GetGravitationalForce(M x1, M y1, M x2, M y2, KG m1, KG m2) {
    
    //     G (m1*m2)
    // F = ---------
    //        r^2
    
    M dx = fabs(x2 - x1);
    M dy = fabs(y2 - y1);

    M r2 = (dy * dy) + (dx * dx);

    N force = G * ((m1 * m2)/ r2 );

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
    Magnitude x;
    Magnitude y;

    if (0<=direction && direction < PI/2) { // 0<=direction < π/2
        // +x, -y
        // No changes needed
        theta = direction;
        x = magnitude * cos(theta);
        y = -magnitude * sin(theta);
    }
    else if (PI/2 <= direction && direction < PI) { // π/2 <= direction < π
        // -x, -y
        theta = direction + PI;
        x = -magnitude * cos(theta);
        y = magnitude * sin(theta);

    }
    else if (PI <= direction && direction < ((3 * PI)/2)) { // π <= direction < 3π/2
        // -x, +y
        theta = direction - PI;
        x = -magnitude * cos(theta);
        y = magnitude * sin(theta);
    }
    else if (((3 * PI)/2) <= direction && direction < 2*PI) {
        // +x, +y
        theta = (2 * PI) - direction;
        x = magnitude * cos(theta);
        y = magnitude * sin(theta);
    }

    return {x, y};
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

    void simulate(N effectiveForceX, N effectiveForceY) { // Grym Cydeffaith
        Fx = effectiveForceX;
        Fy = effectiveForceY;
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
        DrawLine(x, y, x + Fx, y + Fy, RED);
    }

    void drawResultantAcceleration() {
        DrawLine(x, y, x + ax, y + ay, GREEN);
    }

    void drawResultantVelocity() {
        DrawLine(x, y, x + vx, y + vy, GREEN);
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