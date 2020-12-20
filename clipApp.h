/***************************************************************
 * Name:      clipApp.h
 * Purpose:   Defines Application Class
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#ifndef clipAPP_H
#define clipAPP_H

#include <wx/app.h>

class clipApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // clipAPP_H
