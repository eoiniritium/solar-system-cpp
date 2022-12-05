#pragma once

#include <raylib.h>
#include <cmath>
#include <string>
#include <vector>
#include <random>
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

bool isDigits(const std::string &str) {
    return str.find_first_not_of("0123456789.eE+-") == std::string::npos;
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

class ColourPicker {
    private:
    int x, y, width, height;
    Color *colourVar, colour;
    bool flag;
    int nW, nH;
    std::vector<Color> choices;

    public:
    ColourPicker(Color &colourVar, Color defaultColour, int x, int y, int width, int height, Color colour) {
        colourVar = defaultColour;
        this->colourVar = &colourVar;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->colour = colour;
        flag = false;

        nW = 300;
        nH = 90;
    }

    void draw(Vector2 mousePosition) {
        int mx = mousePosition.x;
        int my = mousePosition.y;

        double xBound = x + width;
        double yBound = y + height;

        bool inX = mx >= x && mx <= xBound;
        bool inY = my >= y && my <= yBound;

        bool inNX = mx >= x && mx <= x + nW;
        bool inNY = my >= y && my <= y + nH;

        bool isClick = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        bool justOpened = false; // Skip instant selection on first open

        // Control Expanding and Collapsing
        if(inX && inY && isClick && !flag) {
            flag = true;
            // Random
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen(rd()); // seed the generator
            std::uniform_int_distribution<> distr(0, 255); // define the range

            DrawRectangle(x, y, nW, nH, WHITE);
            unsigned char r, g, b;
            for(size_t i = 0; i < 18; ++i) {
                r = distr(gen);
                g = distr(gen);
                b = distr(gen);

                Color col = {r, g, b, 255};
                choices.push_back(col);
            }

            justOpened = true;
        }

        if(!(inNX && inNY) && isClick && flag) {
            flag = false;
            choices.clear();
        }


        if(!flag) { // Hidden
            DrawRectangle(x, y, width, height, colour);
            DrawRectangle(x+2, y+2, width-4, height-4, *colourVar);
        }
        else { // Expanded
            DrawRectangle(x, y, nW, nH, colour);
            DrawRectangle(x+2, y+2, nW-4, nH-4, BLACK);
            int xOffset;
            int yOffset;
            int c = 0;
            for(size_t i = 0; i < 3; i++) {
                yOffset = y+5 + i*30;
                for(size_t j = 0; j < 6; ++j) {
                    xOffset = x+5 + j*50;
                    DrawRectangle(xOffset, yOffset, 40, 20, colour); // Each brick is 40x20px
                    DrawRectangle(xOffset+2, yOffset+2, 36, 16, choices[c++]);
                }
            }
            
            if(isClick && !justOpened) {
                int i = std::floor((mx - x - 5)/50);
                if(my >= y+5 && my <= y + 25) { // First row
                    *colourVar = choices[i];
                    flag = false;
                    choices.clear();
                }
                else if(my >= y+35 && my <= y + 55) {
                    *colourVar = choices[6+i];
                    flag = false;
                    choices.clear();
                }
                else if(my >= y+65 && my <= y + 85) {
                    *colourVar = choices[12+i];
                    flag = false;
                    choices.clear();
                }
            }

            justOpened = false;
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
            if(keyCode == KEY_BACKSPACE && (int)(text->length()) > 0) {
                text->pop_back();
            }
            else if (keyChar && (int)(text->length()) < maxLength){
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
    double *scale;
    DialogState state;
    std::vector<Body> *bodies; // Pointer to body vector
    bool *dialogFlag;
    bool chooseLocationFlag;
    bool addBody;
    float *zoom;


    //Presets
    bool pSun;
    bool pEarth;
    bool pMoon;
    bool pMars;

    //Presets buttons
    Button *pSunButton;
    Button *pEarthButton;
    Button *pMoonButton;
    Button *pMarsButton;

    // Compare
    bool isCompare;
    M cmpPointX;
    M cmpPointY;
    std::string compareString;

    Entry *nameEntry;
    std::string bodyname;
    Button *chooseLocationButton;
    Entry *massEntry;
    std::string massString;
    Entry *xVelocityEntry;
    std::string xVelocityString;
    Entry *yVelocityEntry;
    std::string yVelocityString;

    ColourPicker* bodyColourPicker;

    Button *addBodyButton;

    Slider *radiusSlider;

    M bodyVirtualX;
    M bodyVirtualY;
    MS_1 bodyUVX, bodyUVY;
    KG bodyMass;
    Color bodyColour;
    double radius;

    // For this only
    std::string locationString;

    public:
    AddBodyDialog(std::vector<Body> &bodiesVector, double &scale, bool &dialogFlag, float &zoom, int screenWidth, int screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->width = 550;
        this->height = 600;
        this->borderThickness = 2;
        
        this->bodies = &bodiesVector; // Passed by referance
        this->dialogFlag = &dialogFlag; // Should this be open?
        this->scale = &scale;

        this->diagX = (screenWidth - width)/2 + borderThickness;
        this->diagY = (screenHeight - height)/2 + borderThickness; 
        this->state = DialogState::AddBody;

        this->addBody = false;

        this->nameEntry = new Entry(bodyname, 25, diagX + 5, diagY + 70, 300, 30, 16.0f, WHITE);
        this->chooseLocationButton = new Button("Choose Location", chooseLocationFlag, diagX + 5, diagY + 110, 200, 50, WHITE, 16.0f);
        this->massEntry = new Entry(massString, 20, diagX + 5, diagY + 190, 200, 30, 16.0f, WHITE);
        this->xVelocityEntry = new Entry(xVelocityString, 10, diagX + 5, diagY + 250, 200, 30, 16.0f, WHITE);
        this->yVelocityEntry = new Entry(yVelocityString, 10, diagX + 5, diagY + 310, 200, 30, 16.0f, WHITE);
        this->radiusSlider = new Slider(GREEN, 16.0f, 1.0f, 30.0f, 5.0f, diagX + 5, diagY + 370, 300, 30);
        this->bodyColourPicker = new ColourPicker(bodyColour, RED, diagX + 5, diagY + 430, 50, 30, WHITE);

        this->addBodyButton = new Button("Add", addBody, diagX + 5, diagY + height - 45, width-15, 35, WHITE, 18.0f);

        this->bodyVirtualX = 0;
        this->bodyVirtualY = 0;

        this->locationString = "X: 0Km\nY: 0Km";

        this->isCompare = false;

        this->zoom = &zoom;

        // Presets
        pSun = false;
        pEarth = false;
        pMoon = false;

        // Presets Buttons
        pSunButton   = new Button("Sun"  , pSun  , diagX + width - 120, diagY + 70 , 100, 30, WHITE, 16.0f); // 5px spacing
        pEarthButton = new Button("Earth", pEarth, diagX + width - 120, diagY + 105, 100, 30, WHITE, 16.0f);
        pMoonButton  = new Button("Moon" , pMoon , diagX + width - 120, diagY + 140, 100, 30, WHITE, 16.0f);
        pMarsButton  = new Button("Mars" , pMars , diagX + width - 120, diagY + 175, 100, 30, WHITE, 16.0f);
    }

    void draw(int xOffset, int yOffset) {
        Vector2 mousepos = GetMousePosition();
        double mx = mousepos.x;
        double my = mousepos.y;

        // Button checking
        if(chooseLocationFlag) {
            state = DialogState::ChooseLocation;
        }

        // Preset buttons
        if(pSun) {
            bodyname = "Sun";
            massString = "1.989e30";
            bodyColour = {235, 216, 52, 255};

        }
        else if(pEarth) {
            bodyname = "Earth";
            massString = "5.972e24";
            bodyColour = {52, 128, 235, 255};
        }
        else if(pMoon) {
            bodyname = "Moon";
            massString = "7.34767309e22";
            bodyColour = {150, 150, 150, 255};
        }
        else if(pMars) {
            bodyname = "Mars";
            massString = "6.39e23";
            bodyColour = {235, 168, 87, 255};
        }




        // Add body button
        if(addBody) {
            // validation
            std::string failText = "";

            if(bodyname != "") {

            } else {
                failText = "Body Name";
            }

            if (isDigits(massString) && massString != "") {
                bodyMass = std::stod(massString);
            }
            else {
                failText = "Mass";
            }

            if(isDigits(xVelocityString) && isDigits(yVelocityString) && xVelocityString != "" && yVelocityString != "") {
                bodyUVX = std::stod(xVelocityString);
                bodyUVY = std::stod(yVelocityString);
            }
            else {
                failText = "Velocity";
            }

            if(failText == "") {
                *dialogFlag = false;
                bodies->push_back(Body(bodyname, bodyVirtualX, bodyVirtualY, bodyUVX, bodyUVY, bodyMass, radius, bodyColour, scale));
                resetAllVariables();
            } else {
                // Error handling
            }
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
                DrawText("Body Name", diagX + 5, diagY + 50, 16.0f, WHITE);
                nameEntry->draw();

                // Body location
                chooseLocationButton->draw(mousepos);
                DrawText(locationString.c_str(), diagX + 220, diagY + 115, 16.0f, WHITE);

                // Mass
                DrawText("Mass", diagX + 5, diagY + 170, 16.0f, WHITE);
                massEntry->draw();
                DrawText("KG", diagX + 210, diagY + 197, 16.0f, WHITE);

                // X velocity
                DrawText("X velocity", diagX + 5, diagY + 230, 16.0f, WHITE);
                xVelocityEntry->draw();
                DrawText("M/S", diagX + 210, diagY + 257, 16.0f, WHITE);

                // Y velocity
                DrawText("Y velocity", diagX + 5, diagY + 290, 16.0f, WHITE);
                yVelocityEntry->draw();
                DrawText("M/S", diagX + 210, diagY + 317, 16.0f, WHITE);

                // Body radius
                DrawText("Body Radius", diagX + 5, diagY + 350, 16.0f, WHITE);
                radiusSlider->draw(mousepos);
                radius = radiusSlider->getValue();

                // Colour picker
                DrawText("Body Colour", diagX +5, diagY + 410, 16.0f, WHITE);
                bodyColourPicker->draw(mousepos);

                // Add Body Button
                addBodyButton->draw(mousepos);

                // Preset buttons
                DrawText("Presets", diagX + width - 120, diagY + 50, 16.0f, WHITE);
                pSunButton  ->draw(mousepos);
                pEarthButton->draw(mousepos);
                pMoonButton ->draw(mousepos);
                pMarsButton ->draw(mousepos);
            break;

            case ChooseLocation:
                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    bodyVirtualX = (mx) * (*scale);
                    bodyVirtualY = (my) * (*scale);

                    locationString = "X: " + removeTrailingCharacters(std::to_string(bodyVirtualX/1000), '0') + "0Km\nY: " + removeTrailingCharacters(std::to_string(bodyVirtualY/1000), '0') + "0Km";
                    state = AddBody;
                    isCompare = false;
                    chooseLocationFlag = false;
                }

                else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                    if(!isCompare) {
                        cmpPointX = mx * *scale; // Get virtual position
                        cmpPointY = my * *scale;
                    }

                    isCompare = !isCompare;
                }

                if(isCompare) { // Working (I think)
                    DrawLine((cmpPointX / (*scale)) + xOffset, (cmpPointY / (*scale)) + yOffset, mx, my, RED);
                    DrawCircle((cmpPointX / (*scale)) + xOffset, (cmpPointY / (*scale)) + yOffset, 3.0f, GREEN);
                    compareString = "Distance: " + removeTrailingCharacters(std::to_string(roundDecimalPlaces(GetDistance(cmpPointX + (xOffset * (*scale)), cmpPointY + (yOffset * (*scale)), mx * *scale, my * *scale)/1000, 1)), '0') + "KM";

                    DrawText("Right Click to remove point", mx + 10, my + 10, 16.0f, WHITE);
                    DrawText(compareString.c_str(), mx + 10, my + 26, 16.0f, WHITE);
                } else {
                    DrawText("Right Click to set distance refernce point", mx + 10, my + 10, 16.0f, WHITE);
                }

                DrawLine(mx, 0, mx, screenHeight, WHITE);
                DrawLine(0, my, screenWidth, my, WHITE);
                DrawText("Click to select location", mx + 10, my - 26, 16.0f, WHITE);
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