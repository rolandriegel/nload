/***************************************************************************
                                opt_window.h
                             -------------------
    begin                : Thu Jan 17 2002
    copyright            : (C) 2002 by Roland Riegel
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

#include <vector>
#include <string>
using std::string;
using std::vector;

#include <curses.h>
#include <form.h>

#include "status.h"
#include "options.h"
#include "window.h"

class OptWindow : public Window
{
public:
	
	OptWindow();
	~OptWindow();
	
	void setFieldChangedFunc( void (*)( FORM * ) );
	
	void show( int, int, int, int );
	void hide();
	
	void refresh();
	
	void processRequest( int );
	void fieldChanged( FORM * );
	
	vector<OptionBase *>& options();
	
private:
	
	void createForm( int, int );
	void deleteForm();
	
	void createWindow( int, int, int, int );
	void deleteWindow();
	
	void createSubWindow( int, int, int, int );
	void deleteSubWindow();
	
	int page();
	int countPages();
	
	vector<OptionBase *> m_options;
	
	WINDOW *m_window;
	WINDOW *m_sub_window;
	FORM *m_form;
	FIELD **m_fields;
	
	void ( *m_fieldchangedfunc )( FORM* );
	
};

#endif
