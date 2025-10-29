/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Visualizer code for viewing a RESULT
 *=============================================**/

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

#include "visualizer.h"
#include <SFML/Graphics.hpp>

//* Create a rectangle on a given SFML window 
void windowRect(sf::Vector2f pos, sf::Vector2f size, sf::Color c, int outline, sf::Color oc, sf::RenderWindow & win)
{
    if (
        pos.x > win.getSize().x ||
        pos.y > win.getSize().y ||
        pos.x+size.x < 0 ||
        pos.y+size.y < 0
    ) return;
    
    sf::RectangleShape rect;
    rect.setPosition(pos);
    rect.setSize(size);
    rect.setFillColor(c);
    rect.setOutlineThickness(outline);
    rect.setOutlineColor(oc);
    win.draw(rect);
}

//* Text Origin : anchor points for TEXT
enum TextOrigin {
    TL,
    TR,
    BL,
    BR,
    C
};
//* Create text on a given SFML window 
sf::Text windowText(std::string s, sf::Vector2f pos, int size, TextOrigin origin, sf::Font & f, sf::RenderWindow & win)
{
    sf::Text text;
    text.setFont(f);
    text.setString(s);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::Black);

    switch (origin)
    {
        case TextOrigin::TR:
        {
            text.setOrigin(
                int(text.getLocalBounds().width),
                0
            ); break;
        }
        case TextOrigin::BL:
        {
            text.setOrigin(
                0,
                int(text.getLocalBounds().height)
            ); break;
        }
        case TextOrigin::BR:
        {
            text.setOrigin(
                int(text.getLocalBounds().width),
                int(text.getLocalBounds().height)
            ); break;
        }
        case TextOrigin::C:
        {
            text.setOrigin(
                int(text.getLocalBounds().width/2),
                int(text.getLocalBounds().height/2)
            ); break;
        }
        default: break;
    }

    text.setPosition(pos);
    win.draw(text);

    return text;
}

//* Converts/Translates positions to screen positions given zoom, offset, etc 
sf::Vector2f toScreen(sf::Vector2f p, float PIVOT_X, float PIVOT_Y, float OFF_X, float OFF_Y, float ZOOM, bool toint)
{
    if (toint)
        return sf::Vector2f(
            int(int(p.x + OFF_X - PIVOT_X)*ZOOM + PIVOT_X),
            int(int(p.y + OFF_Y - PIVOT_Y)*ZOOM + PIVOT_Y)
        );
    return sf::Vector2f(
        int(p.x + OFF_X - PIVOT_X)*ZOOM + PIVOT_X,
        int(p.y + OFF_Y - PIVOT_Y)*ZOOM + PIVOT_Y
    );
}

void visualize(RESULT* result, int ww, int hh, std::string fontPath)
{
    // Window
    sf::RenderWindow window(sf::VideoMode(ww, hh), "Packed!");
    window.setFramerateLimit(60);   

    // Nav Variables
    int     OFF_X = 30;
    int     OFF_Y = 30;
    int     SPEED = 20;
    float   ZOOM = 1;
    bool    SHOW_IDS = true;
    bool    CONTROLS = true;

    // Font
    sf::Font font;
    font.loadFromFile(fontPath);

    bool draw = true;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view));
            }
            // Controls
            if (event.type == sf::Event::KeyPressed)
            {
                sf::Keyboard::Key k = event.key.code;
                if (k == sf::Keyboard::Up)
                    OFF_Y += int(SPEED/ZOOM);
                else if (k == sf::Keyboard::Down)
                    OFF_Y -= int(SPEED/ZOOM);
                else if (k == sf::Keyboard::Left)
                    OFF_X += int(SPEED/ZOOM);
                else if (k == sf::Keyboard::Right)
                    OFF_X -= int(SPEED/ZOOM);
                else if (k == sf::Keyboard::P)
                    ZOOM = std::max(0.01f, ZOOM + (ZOOM/20.f));
                else if (k == sf::Keyboard::M)
                    ZOOM = std::max(0.01f, ZOOM - (ZOOM/20.f));
                else if (k == sf::Keyboard::I)
                    SHOW_IDS = !SHOW_IDS;
                else if (k == sf::Keyboard::C)
                    CONTROLS = !CONTROLS;
                else if (k == sf::Keyboard::Q)
                    window.close();
            }

            draw = true;
        }

        if (draw)
        {
            draw = false;

            window.clear(sf::Color::White);
            //

            float cx = float(window.getSize().x)/2.f;
            float cy = float(window.getSize().y)/2.f;

            // Canvas/Frame Rectangle
            windowRect( toScreen(sf::Vector2f(0,0), cx, cy, OFF_X, OFF_Y, ZOOM, false),
                        sf::Vector2f(result->w*ZOOM-1, result->h*ZOOM-1),
                        sf::Color(0, 0, 0, 0), 
                        1, sf::Color::Black,
                        window);
            // Top-left Coordinates Text
            sf::Vector2f TOPL = toScreen(sf::Vector2f(0,0), cx, cy, OFF_X, OFF_Y, ZOOM, true);
            windowText( "(0, 0)",
                        TOPL + sf::Vector2f(0, -16),
                        14, TextOrigin::C, font, window);
            // Top-right Coordinates Text
            windowText( "(" + std::to_string(result->w) + ", 0)",
                        TOPL + sf::Vector2f(int(result->w*ZOOM), -16),
                        14, TextOrigin::C, font, window);
            // Bottom-left Coordinates Text
            windowText( "(0, " + std::to_string(result->h) + ")",
                        TOPL + sf::Vector2f(0, int(result->h*ZOOM)+14),
                        14, TextOrigin::C, font, window);
            // Bottom-right Coordinates Text
            windowText( "(" + std::to_string(result->w) + ", " + std::to_string(result->h) + ")",
                        TOPL + sf::Vector2f(int(result->w*ZOOM), int(result->h*ZOOM)+14),
                        14, TextOrigin::C, font, window);

            for (SHAPE* rectangle : *(result->rectangles))
            {
                float x = (rectangle->x + OFF_X - cx)*ZOOM + cx;
                float y = (rectangle->y + OFF_Y - cy)*ZOOM + cy;

                if (x > window.getSize().x || y > window.getSize().y)
                    continue;

                windowRect( sf::Vector2f(x, y),
                            sf::Vector2f(rectangle->w*ZOOM-1, rectangle->h*ZOOM-1),
                            sf::Color(215, 215, 215), 
                            1, sf::Color::Black,
                            window);

                // Draw Rectangle's Id
                if (SHOW_IDS)
                {
                    windowText( std::to_string(rectangle->id),
                                sf::Vector2f(int(x + rectangle->w*ZOOM/2),
                                            int(y + rectangle->h*ZOOM/2)),
                                12, TextOrigin::C, font, window);
                }
            }

            windowRect( sf::Vector2f(cx-5, cy-1),
                        sf::Vector2f(10, 2),
                        sf::Color(0, 0, 0, 150), 
                        0, sf::Color::Black,
                        window);
            windowRect( sf::Vector2f(cx-1, cy-5),
                        sf::Vector2f(2, 10),
                        sf::Color(0, 0, 0, 150), 
                        0, sf::Color::Black,
                        window);
            
            int wx = window.getSize().x-200,
                wy = window.getSize().y-20;

            // Placement info Text
            sf::Text info = windowText( "W = "+std::to_string(result->w) + "\nH = "+std::to_string(result->h) + "\nOptimal H = "+std::to_string(result->opt_h) + "\nH/Opt.H = "+std::to_string(float(result->h)/float(result->opt_h)) + "\nLoss = "+std::to_string(result->loss)+"%",
                                        sf::Vector2f(wx, wy),
                                        14, TextOrigin::BL, font, window);
            windowRect( sf::Vector2f(info.getGlobalBounds().left-5, info.getGlobalBounds().top-5),
                        sf::Vector2f(195, info.getGlobalBounds().height+10),
                        sf::Color(0, 0, 0, 0), 
                        1, sf::Color::Black,
                        window);

            // Controls Text
            if (CONTROLS)
            {
                sf::Text controls = windowText( "p - Zoom in\nm - Zoom out\narrows - Move\ni - Show/Hide id's\nc - Show/Hide controls\nq - Quit",
                                                sf::Vector2f(info.getGlobalBounds().left, info.getGlobalBounds().top-5-20),
                                                14, TextOrigin::BL, font, window);
                windowRect( sf::Vector2f(info.getGlobalBounds().left-5, controls.getGlobalBounds().top-5),
                            sf::Vector2f(195, controls.getGlobalBounds().height+10),
                            sf::Color(0, 0, 0, 0), 
                            1, sf::Color::Black,
                            window);
            }
            

            //
            window.display();
        }
    }
}