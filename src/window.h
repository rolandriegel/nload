/***************************************************************************
                                  window.h
                             -------------------
    begin                : Thu Jul 04 2002
    copyright            : (C) 2002 - 2012 by Roland Riegel
    email                : feedback@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <streambuf>
#include <string>

#include <curses.h>
#undef clear
#undef erase
#undef refresh

class Form;

class Window
{
    public:
        class WindowStreamBuf : public std::basic_streambuf<char>
        {
            public:
                WindowStreamBuf(Window& window);
                ~WindowStreamBuf();

            protected:
                std::streamsize xsputn(const char_type* str, std::streamsize n);
                int_type overflow(int_type c = traits_type::eof());

            private:
                Window& m_window;
        };

        class WindowStream : public std::basic_ostream<char>
        {
            public:
                WindowStream(Window& window);
                ~WindowStream();
        };

        Window();
        virtual ~Window();
        
        // create window and display it
        virtual void show(int x, int y, int width, int height);
        
        // hide window and destroy it
        virtual void hide();
        
        // is the window currently visible?
        virtual bool isVisible();
        
        // refresh window
        virtual void refresh();
        
        // clear the content of the window
        virtual void clear();
        
        // move and resize window
        virtual void resize(int x, int y, int width, int height);
        
        // return current window width
        virtual int getWidth();
        
        // return current window height
        virtual int getHeight();
        
        // return current distance to left screen edge
        virtual int getLeft();
        
        // return current distance to top screen edge
        virtual int getTop();
        
        // return current cursor position on the x-axis
        virtual int getX();
        
        // return current cursor position on the y-axis
        virtual int getY();
        
        // set current cursor x position
        virtual void setX(int new_x);
        
        // set current cursor y position
        virtual void setY(int new_y);
        
        // set current cursor position
        virtual void setXY(int new_x, int new_y);
        
        // print some text to the window
        virtual void print(const std::string& text, int new_x = -1, int new_y = -1);
        
        // print a char to the window
        virtual void print(char text, int new_x = -1, int new_y = -1);

        // print via stream to the window
        virtual WindowStream& print(int x = -1, int y = -1);
        
    protected:
        friend class SubWindow;
        friend class Form;

        bool m_visible;
        WINDOW* m_window;

        WindowStream m_stream;
};

class SubWindow : public Window
{
    public:
        SubWindow( Window* parent );
        ~SubWindow();
        
        // return parent window
        Window* getParent();
        
        // create window and display it
        virtual void show(int x, int y, int width, int height);

    private:
        friend class Form;

        Window* m_parent;
};

#endif

