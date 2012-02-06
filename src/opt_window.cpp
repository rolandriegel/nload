/***************************************************************************
                               opt_window.cpp
                             -------------------
    begin                : Thu Jan 17 2002
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

#include "opt_window.h"
#include "setting.h"
#include "settingfilter.h"
#include "settingstore.h"
#include "stringutils.h"

#define BORDER_LEFT 1
#define BORDER_RIGHT 1
#define BORDER_TOP 2
#define BORDER_BOTTOM 1

using namespace std;

OptWindow::OptWindow()
    : Window(), Form::Slots(), m_subWindow(this), m_form(this)
{
}

OptWindow::~OptWindow()
{
    hide();
}

// create option window and display the current settings
void OptWindow::show(int x, int y, int width, int height)
{
    if(m_visible)
        hide();
    
    Window::show(x, y, width, height);
    m_subWindow.show(BORDER_LEFT, BORDER_TOP, width - BORDER_LEFT - BORDER_RIGHT, height - BORDER_TOP - BORDER_BOTTOM);
    
    const int field_width = m_subWindow.getWidth() / 2;
    int line = 0;

    map<string, Setting>& settings = SettingStore::getAll();

    for(map<string, Setting>::const_iterator itSetting = settings.begin(); itSetting != settings.end(); ++itSetting)
    {
        Field* label = new Field(0, line, field_width, 1);
        Field* field = new Field(field_width, line, field_width, 1);

        m_labels[label] = itSetting->first;
        m_fields[field] = itSetting->first;

        m_form.fields().push_back(label);
        m_form.fields().push_back(field);

        label->setEnabled(false);
        label->setText((itSetting->second.getDescription() + ":").c_str());
        label->setFirstOnPage(line == 0);

        field->setText(itSetting->second.getThroughFilter().c_str());

        const SettingFilterMap* mappingFilter = (SettingFilterMap*) itSetting->second.findFilterWithId("map");
        if(mappingFilter)
        {
            const map<string, string>& mapping = mappingFilter->getMap();

            vector<string> elements;
            for(map<string, string>::const_iterator itMapping = mapping.begin(); itMapping != mapping.end(); ++itMapping)
                elements.push_back(itMapping->second);

            field->setEnumField(elements);
            field->setFixed(true);
        }
        
        ++line;
        line %= m_subWindow.getHeight() < 1 ? 1 : m_subWindow.getHeight();
    }
    
    m_form.show(this, &m_subWindow);
    
    m_visible = true;
}

// this function is called when a form field changes
void OptWindow::slot_fieldChanged(Field* field)
{
    map<Field*, string>::const_iterator itField = m_fields.find(field);
    if(itField == m_fields.end())
        return;

    map<string, Setting>& settings = SettingStore::getAll();
    map<string, Setting>::iterator itSetting = settings.find(itField->second);
    if(itSetting == settings.end())
        return;

    itSetting->second.setThroughFilter(trim(field->getText()));
    field->setText(itSetting->second.getThroughFilter());
}

// hide window and destroy it
void OptWindow::hide()
{
    m_form.hide();
    m_form.fields().clear();
    m_subWindow.hide();
    Window::hide();

    for(map<Field*, string>::const_iterator itLabel = m_labels.begin(); itLabel != m_labels.end(); ++itLabel)
        delete itLabel->first;
    for(map<Field*, string>::const_iterator itField = m_fields.begin(); itField != m_fields.end(); ++itField)
        delete itField->first;

    m_labels.clear();
    m_fields.clear();
    
    m_visible = false;
}

// process key presses
void OptWindow::processKey(int request)
{
    if(m_visible)
    {
        switch(request)
        {
            case KEY_LEFT:
                request = REQ_PREV_CHAR;
                break;
            case KEY_RIGHT:
                request = REQ_NEXT_CHAR;
                break;
            case KEY_UP:
                request = REQ_PREV_FIELD;
                break;
            case KEY_DOWN:
            case KEY_ENTER:
            case '\n':
            case '\015':
                request = REQ_NEXT_FIELD;
                break;
            case KEY_DC:
                request = REQ_DEL_CHAR;
                break;
            case KEY_BACKSPACE:
                request = REQ_DEL_PREV;
                break;
            case KEY_PPAGE:
                request = REQ_PREV_CHOICE;
                break;
            case KEY_NPAGE:
            case '\t':
                request = REQ_NEXT_CHOICE;
                break;
            case KEY_HOME:
                request = REQ_BEG_LINE;
                break;
            case KEY_END:
                request = REQ_END_LINE;
                break;
            case '+':
                request = REQ_NEXT_PAGE;
                break;
            case '-':
                request = REQ_PREV_PAGE;
                break;
        }
        m_form.processKey(request);
        refresh();
    }
}

void OptWindow::refresh()
{
    print(0, 0) << "Options:" << endl
                << string(getWidth(), '=');
    
    string navigator = " <-- (-) page " + toString(m_form.getPage() + 1) + "/" + toString(m_form.getPageCount()) + " (+) --> ";
    print(navigator, getWidth() - navigator.length() - 1, 1);
    
    wrefresh(m_window);
    m_subWindow.refresh();
}

