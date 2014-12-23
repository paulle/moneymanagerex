/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#pragma once

#include "util.h"
#include <wx/webview.h>
#include <wx/webviewfshandler.h>
//----------------------------------------------------------------------------

class mmListCtrl: public wxListCtrl
{
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(mmListCtrl);
public:
    mmListCtrl(){}
    mmListCtrl(wxWindow *parent, wxWindowID winid);
    virtual ~mmListCtrl();
    int mmGetId(int defID);
    long g_selected_row;
    int g_selected_col;
    bool g_asc;
public:
    wxListItemAttr *attr1_, *attr2_; // style1
    std::map <int, int> m_col_id; //mmListCtrl item <column no, ID>
public:
    virtual wxListItemAttr* OnGetItemAttr(long row) const;
    wxString BuildPage(const wxString &title) const;
private:
    bool GetDefaultData(int winid, wxString& json);
    void OnMouseRightClick(wxMouseEvent& event);
    /* Headers Right Click*/
    void OnColRightClick(wxListEvent& event);
    void OnColClick(wxListEvent& event);
    void OnHeaderHide(wxCommandEvent& event);
    void OnHeaderReset(wxCommandEvent& event);
    void OnListLeftClick(wxMouseEvent& event);

    void mmCreateColumns();
    wxString m_json;
    wxString m_key;
};

//----------------------------------------------------------------------------
class mmPanelBase : public wxPanel
{
public:
    mmPanelBase() {}
    virtual ~mmPanelBase() {}

public:
    virtual wxString BuildPage() const { return "TBD"; }
    virtual void PrintPage();
    void windowsFreezeThaw()
    {
#ifdef __WXGTK__
        return;
#endif

        if (this->IsFrozen())
            this->Thaw();
        else
            this->Freeze();
    }
public:
    virtual void sortTable() = 0;
protected:
    mmListCtrl *m_listCtrl;
};
