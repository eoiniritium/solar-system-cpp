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
    if(ret[ret.length()-1] == '.') {
        ret += "0";
    }

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
        this->w = (width-4)/(range + 1);
        this->positionX = (w * (value - lowerBound)) + x;
    }

    void draw(Vector2 mouseposition) {
        double mx = mouseposition.x;
        double my = mouseposition.y;
        DrawRectangle(x, y, width, height, WHITE);
        DrawRectangle(x+2, y+2, width-4, height-4, {170, 176, 191, 255});

        bool isMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        double xBound = x + width;
        double yBound = y + height;

        bool inX = mx >= x + w/2 && mx <= xBound - w/2 -2;
        bool inY = my >= y && my <= yBound; 

        if(isMouseDown && inX && inY) {
            positionX = mx - w/2;
        }

        value = ((positionX - x) / (w)) + lowerBound;

        // Draw Slider Grabber
        DrawRectangle(positionX+2, y+2, w, height-4, LIGHTGRAY);
        DrawRectangle(positionX+2 + (w / 2)-1, y+2, 2, height-4, colour);

        DrawText((removeTrailingCharacters(std::to_string(roundDecimalPlaces(value, 2)), '0')).c_str(), x + width + 10, y + (height - fontsize)/2, fontsize, colour);
    }

    double getValue() {
        return this->value;
    }
};

class Toggle {
    private:
    bool *value;
    int x, y;
    int width, height;
    Color colour;

    public:
    Toggle(bool &flagVar, int x, int y, int width, int height, Color colour) {
        this->value = &flagVar;
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
        if(*value) {
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
            *value = !*value;
        }
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
    bool *value;

    public:
    Button(std::string text, bool &flagVar, int x, int y, int width, int height, Color colour, float fontsize) {
        this->text = text;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->fontsize = fontsize;
        this->colour = colour;
        value = &flagVar;
        *value = false; // For Safety. no UB
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
            *value = true;
        }
        else {
            *value = false;
        }
    }
};

class Entry {
    private:
    std::string *text;
    int index;
    Color colour;
    int x, y, width, height, maxLength;
    double fontsize;
    bool focused;
    public:
    Entry(std::string &stringvar, int maxLength, int x, int y, int width, int height, double fontsize, Color colour) {
        this->text = &stringvar;
        this->maxLength = maxLength;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->colour = colour;
        this->fontsize = fontsize;
        this->index = 0;
        this->focused = false;
    }

    void draw() {
        DrawRectangle(x, y, width, height, colour);

        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 mp = GetMousePosition();
            int mx = mp.x;
            int my = mp.y;

            if(mx >= x && mx <= x + width && my >= y && my <= y + height) {
                focused = true;
            } else {
                focused = false;
            }
        }

        if(focused) {
            DrawRectangle(x+2, y+2, width-4, height-4, GRAY);
        }
        else {
            DrawRectangle(x+2, y+2, width-4, height-4, BLACK);
        }

        // Text handling
        if(focused) {
            int keyCode = GetKeyPressed();
            int keyChar = GetCharPressed();

            if(keyCode == KEY_ENTER) {
                focused = false;
            }
            if(keyCode == KEY_BACKSPACE && text->length() > 0) {
                text->pop_back();
            }
            else if (keyChar && text->length() < maxLength){
                (*text) += keyChar;
            }
        }

        DrawText((*text).c_str(), this->x + 5, this->y + (height - fontsize)/2, fontsize, WHITE); // maybe just text?
    }
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
    bool *dialogFlag;
    bool chooseLocationFlag;

    Entry *nameEntry;
    std::string bodyname;
    Button *chooseLocationButton;
    Entry *massEntry;
    std::string massString;
    Entry *xVelocityEntry;
    std::string xVelocityString;
    Entry *yVelocityEntry;
    std::string yVelocityString;

    Slider *radiusSlider;
    

    M bodyVirtualX;
    M bodyVirtualY;
    MS_1 bodyUVX, bodyUVY;
    KG bodyMass;
    double radius;

    // For this only
    std::string locationString;

    public:
    AddBodyDialog(std::vector<Body> &bodiesVector, double scale, bool &dialogFlag, int screenWidth, int screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->width = 550;
        this->height = 600;
        this->borderThickness = 2;
        this->scale = scale;

        this->bodies = &bodiesVector; // Passed by referance
        this->dialogFlag = &dialogFlag; // Should this be open?

        this->diagX = (screenWidth - width)/2 + borderThickness;
        this->diagY = (screenHeight - height)/2 + borderThickness; 
        this->state = DialogState::AddBody;

        this->nameEntry = new Entry(bodyname, 25, diagX + 5, diagY + 70, 300, 30, 16.0f, WHITE);
        this->chooseLocationButton = new Button("Choose Location", chooseLocationFlag, diagX + 5, diagY + 110, 200, 50, WHITE, 16.0f);
        this->massEntry = new Entry(massString, 20, diagX + 5, diagY + 190, 200, 30, 16.0f, WHITE);
        this->xVelocityEntry = new Entry(xVelocityString, 10, diagX + 5, diagY + 250, 200, 30, 16.0f, WHITE);
        this->yVelocityEntry = new Entry(yVelocityString, 10, diagX + 5, diagY + 310, 200, 30, 16.0f, WHITE);
        this->radiusSlider = new Slider(GREEN, 16.0f, 1.0f, 30.0f, 5.0f, diagX + 5, diagY + 370, 300, 30);

        this->bodyVirtualX = 0;
        this->bodyVirtualY = 0;

        this->locationString = "X: 0 Y: 0";
    }

    void draw() {
        Vector2 mousepos = GetMousePosition();
        double mx = mousepos.x;
        double my = mousepos.y;

        // Button checking
        if(chooseLocationFlag) {
            state = DialogState::ChooseLocation;
        }

        switch(state) {
            case AddBody:
                // Dialog box
                DrawRectangle((screenWidth - width)/2, (screenHeight-height)/2, width, height, WHITE);
                DrawRectangle((screenWidth - width)/2 + borderThickness, (screenHeight-height)/2 + borderThickness, width - borderThickness * 2, height - borderThickness * 2, BLACK);

                // Title
                DrawText("Add Body", diagX + 5, diagY + 5, 30.0f, WHITE);

                // X button
                DrawRectangle(diagX + width - 50, diagY + 10, 30, 30, WHITE);
                if(mx >= diagX + width - 50 && mx <= diagX + width - 20 && my >= diagY + 10 && my <= diagY + 40) {
                    DrawRectangle(diagX + width - 48, diagY + 12, 26, 26, GRAY);
                    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                        resetAllVariables();
                        *dialogFlag = false; // Exits dialog
                    }
                }
                else {
                    DrawRectangle(diagX + width - 48, diagY + 12, 26, 26, BLACK);
                }
                DrawText("X", diagX + width - 45, diagY + 13, 18.0f, WHITE);
                
                // Body name
                DrawText("Body Name:", diagX + 5, diagY + 50, 16.0f, WHITE);
                nameEntry->draw();
                
                // Body location
                chooseLocationButton->draw(mousepos);
                DrawText(locationString.c_str(), diagX + 220, diagY + 127, 16.0f, WHITE);

                DrawText("Mass:", diagX + 5, diagY + 170, 16.0f, WHITE);
                massEntry->draw();
                DrawText("KG", diagX + 210, diagY + 197, 16.0f, WHITE);

                DrawText("X velocity:", diagX + 5, diagY + 230, 16.0f, WHITE);
                xVelocityEntry->draw();
                DrawText("M/S", diagX + 210, diagY + 257, 16.0f, WHITE);

                DrawText("Y velocity", diagX + 5, diagY + 290, 16.0f, WHITE);
                yVelocityEntry->draw();
                DrawText("M/S", diagX + 210, diagY + 317, 16.0f, WHITE);

                DrawText("Body Radius", diagX + 5, diagY + 350, 16.0f, WHITE);
                radiusSlider->draw(mousepos);
                radius = radiusSlider->getValue();


            break;

            case ChooseLocation:
                DrawLine(mx, 0, mx, screenHeight, WHITE);
                DrawLine(0, my, screenWidth, my, WHITE);
                DrawText("Click to select location", mx + 10, my + 10, 16.0f, WHITE);

                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    bodyVirtualX = mx * scale;
                    bodyVirtualY = my * scale;
                    locationString = "X: " + removeTrailingCharacters(std::to_string(bodyVirtualX), '0') + "0m Y: " + removeTrailingCharacters(std::to_string(bodyVirtualY), '0') + "0m";
                    state = AddBody;
                    chooseLocationFlag = false;
                }
            break;
        }
    }

    void resetAllVariables() {
        bodyVirtualX = 0;
        bodyVirtualY = 0;
        bodyUVX = 0;
        bodyUVY = 0;
        bodyMass = 0;
        bodyname = "";
        massString = "";
        xVelocityString = "";
        yVelocityString = "";
        locationString = "";
    }
};