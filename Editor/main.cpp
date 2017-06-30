#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char *argv[])
{
    // setup subsystems
    sf::RenderWindow window(sf::VideoMode(1280, 720), "UsagiEditor");
    window.setVerticalSyncEnabled(true);

    // activate the window
    window.setActive(true);

    sf::CircleShape shape(1), shape2(1);
    // set the shape color to green
    shape.setFillColor(sf::Color(100, 250, 50));
    shape2.setFillColor(sf::Color(50, 250, 50));

    float xpos = 0, ypos = 0;
    float xpos2 = 0, ypos2 = 0;

    // enter the loop
    // run the program as long as the window is open
    while(window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while(window.pollEvent(event))
        {
            // "close requested" event: we close the window
            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }
                case sf::Event::Resized:
                {
                    std::cout << "new width: " << event.size.width << std::endl;
                    std::cout << "new height: " << event.size.height << std::endl;
                    break;
                }
                case sf::Event::TextEntered:
                {
                    if(event.text.unicode)
                        std::wcout << "character typed: " << static_cast<wchar_t>(event.text.unicode) << std::endl;
                    break;
                }
                case sf::Event::MouseWheelScrolled:
                {
                    if(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                        std::cout << "wheel type: vertical" << std::endl;
                    else if(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
                        std::cout << "wheel type: horizontal" << std::endl;
                    else std::cout << "wheel type: unknown" << std::endl;
                    std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
                    std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
                    std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
                    break;
                }
                case sf::Event::MouseMoved:
                {
                    //std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
                    //std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
                    break;
                }
                case sf::Event::MouseEntered:
                {
                    std::cout << "the mouse cursor has entered the window" << std::endl;
                    break;
                }
                case sf::Event::MouseLeft:
                {
                    std::cout << "the mouse cursor has left the window" << std::endl;
                    break;
                }
                case sf::Event::KeyPressed:
                {
                    break;
                }
                case sf::Event::JoystickConnected:
                {
                    std::cout << "joystick connected: " << event.joystickConnect.joystickId << std::endl;
                    break;
                }
                case sf::Event::JoystickDisconnected:
                {
                    std::cout << "joystick disconnected: " << event.joystickConnect.joystickId << std::endl;
                    break;
                }
                case sf::Event::JoystickButtonPressed:
                {
                    std::cout << "joystick button pressed!" << std::endl;
                    std::cout << "joystick id: " << event.joystickButton.joystickId << std::endl;
                    std::cout << "button: " << event.joystickButton.button << std::endl;
                    break;
                }
                case sf::Event::JoystickMoved:
                {
                    switch(event.joystickMove.axis)
                    {
                        case sf::Joystick::X:
                        {
                            xpos = event.joystickMove.position;
                            break;
                        }
                        case sf::Joystick::Y:
                        {
                            ypos = event.joystickMove.position;
                            break;
                        }
                        case sf::Joystick::PovX:
                        {
                            xpos2 = event.joystickMove.position;
                            break;
                        }
                        case sf::Joystick::PovY:
                        {
                            ypos2 = event.joystickMove.position;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    break;
                }

            }
        }

        // clear the window with black color
        //window.clear(sf::Color::Black);


        // draw everything here...
        shape.setPosition(xpos + 150, ypos + 150);
        shape2.setPosition(xpos2 + 450, ypos2 + 150);
        window.draw(shape);
        window.draw(shape2);

        // end the current frame
        window.display();
    }

    // teardown
    return 0;
}
