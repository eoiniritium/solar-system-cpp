#pragma once

#include <raylib.h>
#include <cmath>
#include <string>
#include <vector>
#include "body.hpp"

#include <iostream>

double roundDecimalPlaces(double value, int decimalPlaces) {
    const double multiplier = std::pow(10.0f, decimalPlaces);
    return std::ceil(value * multiplier) / multiplier;
}

std::string removeTrailingCharacters(std::string str, const char character) {
    std::string ret = str;
    ret.erase(ret.find_last_not_of(character) + 1, std::string::npos);
    
    return ret;
}

class Label {
    private:
    std::string text;
    int x, y;
    double fontsize;
    Color colour;

    public:
    Label(std::string text, int x, int y, double fontsize, Color colour) {
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
        this->range = upperBound - lowerBound;
        this->w = width/(range + 1);
        this->positionX = (w * (value - lowerBound)) + x;
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

        DrawText((removeTrailingCharacters(std::to_string(roundDecimalPlaces(value, 2)), '0')).c_str(), x + width + 10, y + (height - fontsize)/2, fontsize, colour);
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

    void updateValue(bool value) {
        this->value = value;
    }
};

class Entry {

};

enum DialogState {
    AddBody,
    ChooseLocation
};

class AddBodyDialog {
    private:
    int screenWidth;
    int screenHeight;
    int width;
    int height;
    int borderThickness;
    int diagX;
    int diagY;
    double scale;
    DialogState state;
    std::vector<Body> *bodies; // Pointer to body vector
    bool *flag;

    M bodyRealX, bodyVirtualX;
    M bodyRealY, bodyVirtualY;
    
    public:
    AddBodyDialog(std::vector<Body> &bodiesVector, double scale, bool &flag, int screenWidth, int screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->width = 500;
        this->height = 600;
        this->borderThickness = 2;

        this->bodies = &bodiesVector; // Passed by referance
        this->flag = &flag; // Address of flag

        this->diagX = (screenWidth - width)/2 + borderThickness;
        this->diagY = (screenHeight-height)/2 + borderThickness; 
        this->state = DialogState::AddBody;
    }

    void draw() {
        Vector2 mousepos = GetMousePosition();
        double mx = mousepos.x;
        double my = mousepos.y;
        switch(state) {
            case AddBody:
                // Dialog box
                DrawRectangle((screenWidth - width)/2, (screenHeight-height)/2, width, height, WHITE);
                DrawRectangle((screenWidth - width)/2 + borderThickness, (screenHeight-height)/2 + borderThickness, width - borderThickness * 2, height - borderThickness * 2, BLACK);
                
                // Title
                DrawText("Add Body", diagX + 5, diagY + 5, 30.0f, WHITE);

                // X button
                DrawRectangle(diagX + width - 50, diagY + 10, 30, 30, WHITE);
                DrawRectangle(diagX + width - 48, diagY + 12, 26, 26, BLACK);
                DrawText("X", diagX + width - 45, diagY + 13, 18.0f, WHITE);
                if(mx >= diagX + width - 50 && mx <= diagX + width - 20 && my >= diagY + 10 && my <= diagY + 40 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    std::cout << "HERE!!" << std::endl;
                    *flag = false;
                }

                break;
            case ChooseLocation:
                break;
        } 
    }
};