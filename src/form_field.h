/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef FORM_FIELD_H
#define FORM_FIELD_H

#include <config.h>
#include <curses.h>

#if defined(HAVE_FORM_H)
#include <form.h>
#elif defined(HAVE_NCURSES_FORM_H)
#include <ncurses/form.h>
#endif

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

