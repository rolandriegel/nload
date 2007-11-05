/***************************************************************************
                                opt_window.h
                             -------------------
    begin                : Thu Jan 17 2002
    copyright            : (C) 2002 - 2007 by Roland Riegel
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
#include <vector>

class OptionBase;

class OptWindow : public Window, public Form::Slots
{
    public:
        
        OptWindow();
        ~OptWindow();
        
        void show(int, int, int, int);
        void hide();
        
        void refresh();
        
        void processKey(int);
        void slot_fieldChanged(Field*);
        
        std::vector<OptionBase *>& options();
        
    private:
        
        int page();
        int countPages();
        
        std::vector<OptionBase *> m_options;
        
        SubWindow m_sub_window;
        Form m_form;
        
};

#endif
