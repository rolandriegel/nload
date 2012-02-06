/***************************************************************************
                                opt_window.h
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

#ifndef OPT_WINDOW_H
#define OPT_WINDOW_H

#include "window.h"
#include "form_field.h"

#include <map>
#include <string>

class OptWindow : public Window, public Form::Slots
{
    public:
        OptWindow();
        ~OptWindow();
        
        void show(int x, int y, int width, int height);
        void hide();
        
        void refresh();
        
        void processKey(int request);
        void slot_fieldChanged(Field* field);
        
    private:
        SubWindow m_subWindow;
        Form m_form;

        std::map<Field*, std::string> m_labels;
        std::map<Field*, std::string> m_fields;
};

#endif

