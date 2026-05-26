#pragma once

#include <iostream>
#include <cstdint>
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "Input.hpp"

class wWindow{

    //X11 initalizaiton
    static inline Display* tempDisplay{};
    Display* display{};
    int screen{};
    Window win{};
    XEvent winEvent{};
    Atom wmDeleteMsg{};

    int wWidth{}, wHeight{};

    bool windowClose {};
    

    public:
    wWindow(std::string name, int width, int height, const char* displayVars){

        this->wWidth = width;
        this->wHeight = height;

        this->display = wWindow::tempDisplay; //connecting to x11 display server with nullptr as function argument so the current primary display be selected

        this->screen = DefaultScreen(this->display); //grabbing the handler of the primary display that the DE is currently using

        //creating a simple window(7th and 8th param is for border and last param is the intitial background color of the window)
        this->win = XCreateSimpleWindow(this->display, RootWindow(this->display, this->screen), 10, 10, this->wWidth, this->wHeight, 1, WhitePixel(this->display, this->screen), BlackPixel(this->display, this->screen));
        XStoreName(this->display, this->win, name.data()); //window title name

        //allowing the window to check for input the giving display, keyrpess mask checks for key press input and structure notify checks for resize and window move input
        //exposuremask checks if the window is minimized or hidden
        XSelectInput(this->display, this->win, ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
        XMapWindow(this->display, this->win); //makes the window visible on the current target display

        //custom delete msg to check close event if the x button is pressed so the window can close properly
        this->wmDeleteMsg = XInternAtom(this->display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(this->display, this->win, &this->wmDeleteMsg, 1);
    }
    void Destroy(){
        //call after destroying renderer if created
        XDestroyWindow(this->display, this->win); //destroys the window
        XCloseDisplay(this->display); //closes the connection with the display server
    }

    //event handling
    int EventPending(){
        return XPending(this->display); //returns the events pending that needs to be handled
    }

    void NextEvent(){
        XNextEvent(this->display, &this->winEvent); //reads the next event and stores it in winEvent struct
    }

    bool EventcheckClose(){
        if(this->winEvent.type == ClientMessage){
            if(static_cast<Atom>(this->winEvent.xclient.data.l[0]) == this->wmDeleteMsg){ //when we click the X button and it matches the clientmsg then return true and handle the event
                return true;
            }
        }

        return false;
    }

    void EventPolling(){ //event pollin
        while(this->EventPending() > 0){

            this->NextEvent(); //clearing the event in the event queue

            //checking if user clicks the X button
            this->windowClose = this->EventcheckClose();

            //if user presses a key, sets the keysym to true in static map in static input class
            if(this->winEvent.type == KeyPress){
                KeySym keysym { XLookupKeysym(&this->winEvent.xkey, 0) };
                Input::SetKey(keysym, true);
            }

            //if uses release a key, clears the next event in the event queue if the current event is same as the next event with a auto repeat filter and dumps the current event
            //to a temporary nextEvent xevent struct and sets the key to false in the static map in static input class
            else if(this->winEvent.type == KeyRelease){
                bool is_auto_repeat {false};

                if(this->EventPending() > 0){
                    XEvent nextEvent;
                    XPeekEvent(this->display, &nextEvent);

                    //checking if the main window event key is same as the next event in the event queue
                    if(nextEvent.type == KeyPress && nextEvent.xkey.keycode == this->winEvent.xkey.keycode && nextEvent.xkey.time == this->winEvent.xkey.time){
                        XNextEvent(this->display, &nextEvent);
                        is_auto_repeat = true;
                    }
                }

                //set the key to false in the static map in static input class if auto repeat filter is false
                if(!is_auto_repeat){
                    KeySym keysym { XLookupKeysym(&this->winEvent.xkey, 0) };
                    Input::SetKey(keysym, false);
                }

            }

            switch(this->winEvent.type){

                case ButtonPress: {
                    uint32_t btn { this->winEvent.xbutton.button };
                    Input::SetMouseButton(btn, true);
                    break;
                }

                case ButtonRelease: {
                    uint32_t btn { this->winEvent.xbutton.button };
                    Input::SetMouseButton(btn, false);
                    break;
                }

                case MotionNotify: {
                    float x { static_cast<float>(this->winEvent.xmotion.x) };
                    float y { static_cast<float>(this->winEvent.xmotion.y) };
                    Input::SetMousePos(x, y);
                    break;
                }
            }
        }
    }

    //must call this before calling other functions
    static inline bool LibInit(){
        wWindow::tempDisplay = XOpenDisplay(nullptr); //connecting to x11 display server with nullptr as function argument so the current primary display be selected
        if(!tempDisplay) return false;
        return true;
    }

    //setters and getters
    Display* getDisplay(){ return this->display; }
    int getScreen(){ return this->screen; }
    Window* getRawWindow() { return &this->win; }

    int getWidth() { return this->wWidth; }
    int getHeight() { return this->wHeight; }
    bool checkWindowClose(){ return this->windowClose; }

};

//FAHHHHHHHHHHHHHHHHHHHH 🥀😭