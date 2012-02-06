/***************************************************************************
                                form_field.h
                             -------------------
    begin                : Sat Jul 27 2002
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

#ifndef FORM_FIELD_H
#define FORM_FIELD_H

#include <curses.h>
#include <form.h>
#undef clear
#undef erase
#undef refresh

#include <string>
#include <vector>

class Window;
class SubWindow;
class Form;

class Field
{
    public:
        Field(int x, int y, int width, int height);
        ~Field();

        void setText(const std::string& text);
        std::string getText();
        
        void move(int x, int y);
        
        void setVisible(bool new_visible);
        bool isVisible();
        
        void setEnabled(bool new_enabled);
        bool isEnabled();
        
        void setIntegerField(int min, int max);
        void setEnumField(const std::vector<std::string>& elements);
        
        void setFixed(bool new_fixed);
        bool isFixed();
        
        void setFirstOnPage(bool new_newpage);
        bool isFirstOnPage();
        
        friend bool operator==(const Field& field1, const Field& field2);
        friend bool operator==(const Field& field1, const FIELD* field2);
        
    private:
        friend class Form;

        FIELD* m_field;
        std::vector<std::string> m_enumElements;
        const char** m_enumElementsArray;
};

class Form
{
    public:
        class Slots
        {
            public:
                Slots() {}
                virtual ~Slots() {}
                virtual void slot_fieldChanged(Field* field) {}
        };
        
        Form(Slots* slots = 0);
        ~Form();
        
        std::vector<Field *>& fields();
        
        void show(Window* main_window, SubWindow* sub_window);
        void hide();

        void processKey(int key);
        
        bool isVisible();
        
        int getPage();
        int getPageCount();
        
    private:
        Slots* m_slots;
        
        FORM* m_form;
        FIELD** m_curses_fields;
        std::vector<Field *> m_fields;

        bool m_visible;
        
        static void fieldChanged(FORM* form);
};

#endif

