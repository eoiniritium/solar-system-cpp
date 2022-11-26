#include <raylib.h>
#include <cmath>
#include <string>

#include <iostream>

class Slider {
    private:
    double lowerBound;
    double upperBound;
    int x;
    int y;
    int width;
    int height;
    double fontsize;
    double value;
    int positionX;
    Color colour;
    double w;
    double range;

    public:
    Slider(Color colour, double fontsize, double lowerBound, double upperBound, int x, int y, int width, int height) {
        this->colour = colour;
        this->fontsize = fontsize;
        this->lowerBound = lowerBound;
        this->upperBound = upperBound;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        range = upperBound - lowerBound;
        w = width/(range + 1);
        positionX = x + (width - w) / 2;
    }

    void draw(Vector2 mouseposition) {
        double mx = mouseposition.x;
        double my = mouseposition.y;
        DrawRectangle(x, y, width, height, {170, 176, 191, 255});

        bool isMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        double xBound = x + width;
        double yBound = y + height;

        bool inX = mx >= x + w/2 && mx <= xBound - w/2;
        bool inY = my >= y && my <= yBound; 

        if(isMouseDown && inX && inY) {
            positionX = mx - w/2;
        }

        value = ((positionX - x) / (2 * w)) + lowerBound;

        // Draw Slider Grabber
        DrawRectangle(positionX, y, w, height, LIGHTGRAY);
        DrawRectangle(positionX + (w / 2)-1, y, 2, height, colour);

        DrawText(std::to_string(value).c_str(), x + width + 10, y + (height - fontsize)/2, fontsize, colour);
    }
};