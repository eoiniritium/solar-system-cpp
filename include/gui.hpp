#pragma once

#include <raylib.h>
#include <cmath>
#include <string>

#include <iostream>


class Label {
    private:
    std::string text;
    int x, y;
    double fontsize;
    Color colour;

    public:
    Label(std::string text, int x, int y, double fontsize, Color colour) 
    {
        this->text = text;
        this->x = x;
        this->y = y;
        this->fontsize = fontsize;
        this->colour = colour;
    }

    void draw() {
        DrawText(text.c_str(), x, y, fontsize, colour);
    }

    void updateText(std::string newText) {
        this->text = newText;
    }

    std::string getString() {
        return this->text;
    }
};

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
    Slider(Color colour, double fontsize, double lowerBound, double upperBound, double initialValue, int x, int y, int width, int height) {
        this->colour = colour;
        this->fontsize = fontsize;
        this->lowerBound = lowerBound;
        this->upperBound = upperBound;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->value = initialValue;
        range = upperBound - lowerBound;
        w = width/(range + 1);
        positionX = (w * (value - lowerBound)) + x;
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

        value = ((positionX - x) / (w)) + lowerBound;

        // Draw Slider Grabber
        DrawRectangle(positionX, y, w, height, LIGHTGRAY);
        DrawRectangle(positionX + (w / 2)-1, y, 2, height, colour);

        DrawText(std::to_string(value).c_str(), x + width + 10, y + (height - fontsize)/2, fontsize, colour);
    }

    double getValue() {
        return this->value;
    }
};

class Toggle {
    private:
    bool value;
    int x, y;
    int width, height;
    Color colour;

    public:
    Toggle(bool defaultValue, int x, int y, int width, int height, Color colour) {
        this->value = defaultValue;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->colour = colour;
    }

    void draw(Vector2 mousePosition) {
        double mx = mousePosition.x;
        double my = mousePosition.y;

        DrawRectangle(x, y, width, height, colour);
        if(value) {
            DrawRectangle(x + (width/2)+2, y+2, (width/2)-4, height-4, WHITE);
        }
        else {
            DrawRectangle(x+2, y+2, (width/2)-4, height-4, WHITE);
        }

        bool isMouseDown = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        double xBound = x + width;
        double yBound = y + height;

        bool inX = mx >= x && mx <= xBound;
        bool inY = my >= y && my <= yBound;

        if (isMouseDown && inX && inY) {
            value = !value;
        }
    }

    bool getValue() {
        return value;
    }

    void updateValue(bool value) {
        this->value = value;
    }
};

class Button {
    private:
    std::string text;
    int x;
    int y;
    int width;
    int height;
    Color colour;
    float fontsize;
    bool value;

    public:
    Button(std::string text, int x, int y, int width, int height, Color colour, float fontsize) {
        this->text = text;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->fontsize = fontsize;
        this->colour = colour;
        value = false;
    }

    void draw(Vector2 mousePosition) {
        double mx = mousePosition.x;
        double my = mousePosition.y;

        double textPxLen = MeasureText(text.c_str(), fontsize);

        DrawRectangle(x, y, width, height, colour);

        bool isMouseDown = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        double xBound = x + width;
        double yBound = y + height;

        bool inX = mx >= x && mx <= xBound;
        bool inY = my >= y && my <= yBound;

        if (inX && inY) {
            DrawRectangle(x+2, y+2, width-4, height-4, GRAY); // Border radius of 2px
        } 
        else {
            DrawRectangle(x+2, y+2, width-4, height-4, BLACK); // Border radius of 2px
        }
        
        DrawText(text.c_str(), x + ((width - textPxLen)/2), y + (height - fontsize)/2, fontsize, colour);

        if (isMouseDown && inX && inY) {
            value = true;
        }
        else {
            value = false;
        }
    }

    bool getValue() {
        return value;
    }
};