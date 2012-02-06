/***************************************************************************
                                 window.cpp
                             -------------------
    begin                : Thu Nov 25 2003
    copyright            : (C) 2003 - 2012 by Roland Riegel
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

#include "window.h"

using namespace std;

Window::WindowStreamBuf::WindowStreamBuf(Window& window)
    : m_window(window)
{
}

Window::WindowStreamBuf::~WindowStreamBuf()
{
}

streamsize Window::WindowStreamBuf::xsputn(const char_type* str, streamsize n)
{
    m_window.print(string(str, n));
    return n;
}

Window::WindowStreamBuf::int_type Window::WindowStreamBuf::overflow(int_type c)
{
    if(c == traits_type::eof())
        return c;

    m_window.print((char) c);
    return c;
}

Window::WindowStream::WindowStream(Window& window)
    : basic_ostream<char>(new WindowStreamBuf(window))
{
}

Window::WindowStream::~WindowStream()
{
    delete rdbuf();
}

Window::Window()
 : m_visible(false), m_window(0), m_stream(*this)
{
}

Window::~Window()
{
    hide();
}

// create window and display it
void Window::show(int x, int y, int width, int height)
{
    if(m_window)
        return;
    
    m_window = newwin(height, width, y, x);
    clear();
    refresh();
    
    m_visible = true;
}

// hide window and destroy it
void Window::hide()
{
    if(!m_window)
        return;
    
    clear();
    refresh();
    delwin(m_window);
    
    m_window = 0;
    
    m_visible = false;
}

// is the window currently visible?
bool Window::isVisible()
{
    return m_visible;
}

// refresh window
void Window::refresh()
{
    if(m_window)
        wrefresh(m_window);
}

// clear the content of the window
void Window::clear()
{
    if(m_window)
        wclear(m_window);
}

// move and resize window
void Window::resize(int x, int y, int width, int height)
{
    if(!m_window)
        return;

    wresize(m_window, height, width);
    mvwin(m_window, y, x);
}

// return current window width
int Window::getWidth()
{
    if(!m_window)
        return 0;

    int width, height;
    getmaxyx(m_window, height, width);

    return width;
}

// return current window height
int Window::getHeight()
{
    if(!m_window)
        return 0;

    int width, height;
    getmaxyx(m_window, height, width);

    return height;
}

// return current distance to left screen edge
int Window::getLeft()
{
    if(!m_window)
        return 0;

    int x, y;
    getbegyx(m_window, y, x);

    return x;
}

// return current distance to top screen edge
int Window::getTop()
{
    if(!m_window)
        return 0;

    int x, y;
    getbegyx(m_window, y, x);

    return y;
}

// return current cursor position on the x-axis
int Window::getX()
{
    if(!m_window)
        return 0;

    int x, y;
    getyx(m_window, y, x);

    return x;
}

// return current cursor position on the y-axis
int Window::getY()
{
    if(!m_window)
        return 0;

    int x, y;
    getyx(m_window, y, x);

    return y;
}

// set current cursor x position
void Window::setX(int new_x)
{
    if(!m_window)
        return;

    wmove(m_window, getY(), new_x);
}

// set current cursor y position
void Window::setY(int new_y)
{
    if(!m_window)
        return;

    wmove(m_window, new_y, getX());
}

// set current cursor position
void Window::setXY(int new_x, int new_y)
{
    if(!m_window)
        return;

    wmove(m_window, new_y, new_x);
}

// print some text to the window
void Window::print(const string& text, int new_x, int new_y)
{
    if(!m_window)
        return;
    if(new_x <= -1)
        new_x = getX();
    if(new_y <= -1)
        new_y = getY();
    
    mvwaddstr(m_window, new_y, new_x, text.c_str());
}

// print a char to the window
void Window::print(char text, int new_x, int new_y)
{
    if(!m_window)
        return;
    if(new_x <= -1)
        new_x = getX();
    if(new_y <= -1)
        new_y = getY();
    
    mvwaddch(m_window, new_y, new_x, text);
}

// print via stream to window
Window::WindowStream& Window::print(int x, int y)
{
    if(x <= -1)
        x = getX();
    if(y <= -1)
        y = getY();
    setXY(x, y);

    return m_stream;
}

SubWindow::SubWindow(Window* parent)
 : Window(), m_parent(parent)
{
}

SubWindow::~SubWindow()
{
    hide();
}

// return parent window
Window* SubWindow::getParent()
{
    return m_parent;
}

// create window and display it
void SubWindow::show(int x, int y, int width, int height)
{
    if(m_window)
        return;
    
    m_window = derwin(m_parent->m_window, height, width, y, x);
    clear();
    refresh();
    
    m_visible = true;
}

