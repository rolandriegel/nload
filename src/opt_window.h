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

