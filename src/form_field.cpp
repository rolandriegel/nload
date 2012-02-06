/***************************************************************************
                               form_field.cpp
                             -------------------
    begin                : Tue Jul 30 2002
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

#include "form_field.h"
#include "window.h"

using namespace std;

Field::Field(int x, int y, int width, int height) : m_field(0)
{
    m_field = new_field(height, width, y, x, 0, 0);
    set_field_opts(m_field, field_opts(m_field) | O_BLANK | O_PASSOK);

    m_enumElementsArray = 0;
}

Field::~Field()
{
    setEnumField(vector<string>());
    free_field(m_field);
}

void Field::setText(const string& text)
{
    set_field_buffer(m_field, 0, text.c_str());
}

string Field::getText()
{
    return field_buffer(m_field, 0);
}

void Field::move(int x, int y)
{
    move_field(m_field, y, x);
}

void Field::setVisible(bool new_visible)
{
    set_field_opts(m_field, new_visible ? field_opts(m_field) | O_VISIBLE : field_opts(m_field) & ~O_VISIBLE);
}

bool Field::isVisible()
{
    return (field_opts(m_field) & O_VISIBLE) == O_VISIBLE;
}

void Field::setEnabled(bool new_enabled)
{
    set_field_opts(m_field, new_enabled ? field_opts(m_field) | O_ACTIVE : field_opts(m_field) & ~O_ACTIVE);
}

bool Field::isEnabled()
{
    return (field_opts(m_field) & O_ACTIVE) == O_ACTIVE;
}

void Field::setIntegerField(int min, int max)
{
    set_field_type(m_field, TYPE_INTEGER, 0, min, max);
}

void Field::setEnumField(const vector<string>& elements)
{
    if(elements.empty())
    {
        set_field_type(m_field, TYPE_ALNUM, 0);

        if(m_enumElementsArray)
            delete[] m_enumElementsArray;
        m_enumElementsArray = 0;

        m_enumElements.clear();

        return;
    }

    m_enumElements = elements;

    if(m_enumElementsArray)
        delete[] m_enumElementsArray;
    m_enumElementsArray = new const char*[m_enumElements.size() + 1];

    for(vector<string>::const_iterator itElement = m_enumElements.begin(); itElement != m_enumElements.end(); ++itElement)
        m_enumElementsArray[itElement - m_enumElements.begin()] = itElement->c_str();
    m_enumElementsArray[elements.size()] = 0;

    set_field_type(m_field, TYPE_ENUM, m_enumElementsArray, 0, 0);
}

void Field::setFixed(bool new_fixed)
{
    set_field_opts(m_field, new_fixed ? field_opts(m_field) & ~O_EDIT : field_opts(m_field) | O_EDIT);
}

bool Field::isFixed()
{
    return (field_opts(m_field) & O_EDIT) != O_EDIT;
}

void Field::setFirstOnPage(bool new_newpage)
{
    set_new_page(m_field, new_newpage);
}

bool Field::isFirstOnPage()
{
    return new_page(m_field);
}

bool operator==(const Field& field1, const Field& field2)
{
    return field1.m_field == field2.m_field;
}

bool operator==(const Field& field1, const FIELD* field2)
{
    return field1.m_field == field2;
}


Form::Form(Slots* slots)
    : m_slots(slots), m_form(0), m_visible(false)
{
}

Form::~Form()
{
}

vector<Field*>& Form::fields()
{
    return m_fields;
}
    
void Form::show(Window* main_window, SubWindow* sub_window)
{
    if(m_form)
        return;

    m_curses_fields = new FIELD* [ m_fields.size() ];
    vector<Field*>::const_iterator r = m_fields.begin(); 
    int i = 0;
    while(r != m_fields.end())
    {
        m_curses_fields[i] = (*r)->m_field;
        r++; i++;
    }
    
    m_curses_fields[m_fields.size()] = 0;
    m_form = new_form(m_curses_fields);
    
    if(!m_form)
        return;
    
    set_form_userptr(m_form, this);
    set_form_win(m_form, main_window->m_window);
    set_form_sub(m_form, sub_window->m_window);
    set_field_term(m_form, fieldChanged);
    
    post_form(m_form);
    
    m_visible = true;
}

void Form::hide()
{
    if(!m_form)
        return;

    unpost_form(m_form);
    free_form(m_form);
    m_form = 0;
    
    delete[] m_curses_fields;
    m_curses_fields = 0;
    
    m_visible = false;
}

bool Form::isVisible()
{
    return m_visible;
}

void Form::processKey(int key)
{
    if(m_form)
        form_driver(m_form, key);
}

int Form::getPage()
{
    if(!m_form)
        return 0;
    return form_page(m_form);
}

int Form::getPageCount()
{
    int pages = 0;
    for(vector<Field*>::const_iterator i = m_fields.begin(); i != m_fields.end(); i++)
    {
        if((*i)->isFirstOnPage())
            pages++;
    }
    return pages;
}

void Form::fieldChanged(FORM* form)
{
    Form* f = (Form*) form_userptr(form);
    if(!f)
        return;

    if(f->m_slots)
    {
        for(vector<Field*>::const_iterator i = f->m_fields.begin(); i != f->m_fields.end(); ++i)
            if((*i)->m_field == current_field(form))
                f->m_slots->slot_fieldChanged(*i);
    }
}

