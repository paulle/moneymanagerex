/*******************************************************
Copyright (C) 2006 Madhan Kanagavel
Copyright (C) 2014 Nikolay

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

#include "mmpanelbase.h"
#include "constants.h"
#include "util.h"
#include <algorithm>
#include "model/Model_Setting.h"

static int COL_MAX = 16;
static const wxString translate_me[] =
{
    wxTRANSLATE("Initial Value"),
    wxTRANSLATE("Current Value")
};

static const char *ASSETS_LIST_JSON = R"({
"0": {"id": 0, "name":" ", "format":2, "width":24},
"1": {"id": 1,"name":"ID", "format":1, "width":0},
"2": {"id": 2,"name":"Name", "format":0, "width":150},
"3": {"id": 3,"name":"Date", "format":1, "width":100},
"4": {"id": 4,"name":"Type", "format":0, "width":100},
"5": {"id": 5,"name":"Initial Value", "format":1, "width":100},
"6": {"id": 6,"name":"Current Value", "format":1, "width":100},
"7": {"id": 7,"name":"Notes", "format":0, "width":450},
"key": "ASSET_COL_SETTINGS",
"sort_col": 3,
"sort_order": 1
})";

static const char *BUDGET_LIST_JSON = R"({
"0": {"id": 0, "name":" ", "format":2, "width":24},
"1": {"id": 1, "name":"Category", "format":0, "width":150},
"2": {"id": 2, "name":"Sub Category", "format":0, "width":150},
"3": {"id": 3, "name":"Frequency", "format":0, "width":150},
"4": {"id": 4, "name":"Amount", "format":1, "width":80},
"5": {"id": 5, "name":"Estimated", "format":1, "width":80},
"6": {"id": 6, "name":"Actual", "format":1, "width":80},
"key": "BUDGET_COL_SETTINGS",
"sort_col": 1,
"sort_order": 0
})";

static const char *CHECKING_LIST_JSON = R"({
"0": {"id": 0, "name":" ", "format":2, "width":24},
"1": {"id": 1,"name":"ID", "format":1, "width":0},
"2": {"id": 2,"name":"Date", "format":1, "width":100},
"3": {"id": 3,"name":"Number", "format":1, "width":85},
"4": {"id": 4,"name":"Payee", "format":0, "width":165},
"5": {"id": 5,"name":"Status", "format":0, "width":65},
"6": {"id": 6,"name":"Category", "format":0, "width":200},
"7": {"id": 7,"name":"Withdrawal", "format":1, "width":100},
"8": {"id": 8,"name":"Deposit", "format":1, "width":100},
"9": {"id": 9,"name":"Balance", "format":1, "width":100},
"10": {"id": 10,"name":"Notes", "format":0, "width":450},
"key": "CHECKING_COL_SETTINGS",
"sort_col": 2,
"sort_order": 0
})";

static const char *STOCK_LIST_JSON = R"({
"0": {"id": 0, "name":" ", "format":2, "width":24},
"1": {"id": 1,"name":"ID", "format":1, "width":0},
"2": {"id": 2,"name":"Purchase Date", "format":1, "width":110},
"3": {"id": 3,"name":"Share Name", "format":0, "width":120},
"4": {"id": 4,"name":"Share Symbol", "format":0, "width":110},
"5": {"id": 5,"name":"Number of Shares", "format":1, "width":95},
"6": {"id": 6,"name":"Unit Price", "format":1, "width":95},
"7": {"id": 7,"name":"Total Value", "format":1, "width":95},
"8": {"id": 8,"name":"Gain/Loss", "format":1, "width":0},
"9": {"id": 9,"name":"Curr. unit price", "format":1, "width":0},
"10": {"id": 910,"name":"Curr. total value", "format":1, "width":0},
"11": {"id": 11,"name":"Price Date", "format":1, "width":0},
"12": {"id": 12,"name":"Commission", "format":1, "width":0},
"13": {"id": 13,"name":"Notes", "format":0, "width":450},
"key": "STOCK_COL_SETTINGS",
"sort_col": 2,
"sort_order": 0
})";

static const char *BILLS_LIST_JSON = R"({
"0": {"id": 0, "name":" ", "format":2, "width":24},
"1": {"id": 1,"name":"ID", "format":1, "width":0},
"2": {"id": 2,"name":"Next Due Date", "format":1, "width":95},
"3": {"id": 3,"name":"Account", "format":0, "width":120},
"4": {"id": 4,"name":"Payee", "format":0, "width":110},
"5": {"id": 5,"name":"Status", "format":0, "width":0},
"6": {"id": 6,"name":"Category", "format":0, "width":0},
"7": {"id": 7,"name":"Type", "format":0, "width":95},
"8": {"id": 8,"name":"Amount", "format":1, "width":95},
"9": {"id": 9,"name":"Frequency", "format":0, "width":95},
"10": {"id": 10,"name":"Remaining Days", "format":0, "width":140},
"11": {"id": 11,"name":"Repetitions", "format":0, "width":0},
"12": {"id": 12,"name":"Autorepeat", "format":0, "width":0},
"13": {"id": 13,"name":"Number", "format":1, "width":0},
"14": {"id": 14,"name":"Notes", "format":0, "width":450},
"key": "BILLS_COL_SETTINGS",
"sort_col": 6,
"sort_order": 0
})";

class wxListItemAttr;

wxBEGIN_EVENT_TABLE(mmListCtrl, wxListCtrl)
    EVT_LIST_COL_CLICK(wxID_ANY, mmListCtrl::OnColClick)
    EVT_LIST_COL_RIGHT_CLICK(wxID_ANY, mmListCtrl::OnColRightClick)
    EVT_MENU(MENU_HEADER_HIDE, mmListCtrl::OnHeaderHide)
    EVT_MENU(MENU_HEADER_RESET, mmListCtrl::OnHeaderReset)
wxEND_EVENT_TABLE()

mmListCtrl::mmListCtrl(wxWindow *parent, wxWindowID winid)
    : wxListCtrl(parent, winid
        , wxDefaultPosition, wxDefaultSize
        , wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL)
    , attr1_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont))
    , attr2_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont))
    , g_selected_row(-1)
    , g_selected_col(0)
    , g_asc(true)
    , m_json(wxEmptyString)
{
    m_col_id.clear();
    if (GetDefaultData(winid, m_json)) {
        json::Object o = str2json_obj(m_json);
        m_key = wxString(json::String(o[L"key"]));
        m_json = Model_Setting::instance().GetStringSetting(m_key, m_json);
        o = str2json_obj(m_json);
        g_asc = json::Number(o[L"sort_order"]) > 0;
        g_selected_col = json::Number(o[L"sort_col"]);

        mmCreateColumns();
    }
}

bool mmListCtrl::GetDefaultData(int winid, wxString& json)
{
    switch (winid)
    {
    case mmID_ASSETS_LIST:
        json = ASSETS_LIST_JSON;
        break;
    case mmID_CHECKING_LIST:
        json = CHECKING_LIST_JSON;
        break;
    case mmID_STOCK_LIST:
        json = STOCK_LIST_JSON;
        break;
    case mmID_BILLS_LIST:
        json = BILLS_LIST_JSON;
        break;        
    case mmID_BUDGET_LIST:
        json = BUDGET_LIST_JSON;
        break;
    default:
        json = wxEmptyString;
    }
    return !json.empty();
}

mmListCtrl::~mmListCtrl()
{
    if (attr1_) delete attr1_;
    if (attr2_) delete attr2_;

    json::Object orig = str2json_obj(m_json);
    if (orig.Empty()) return;
    
    json::Object o;
    for (int i = 0; i < GetColumnCount(); i++) {
        json::Object d;
        wxListItem item;
        item.SetMask(wxLIST_MASK_FORMAT);
        int col_order = this->GetColumnOrder(i);
        if (GetColumn(i, item)){
            int id = json::Number(orig[std::to_wstring(i)][L"id"]);
            const auto name = orig[std::to_wstring(i)][L"name"];
            d[L"name"] = json::String(name);
            d[L"format"] = json::Number(item.GetAlign());
            d[L"width"] = json::Number(item.GetWidth());
            d[L"id"] = json::Number(id);
        }

        o[std::to_wstring(col_order)] = d;
    }
    if (!m_key.empty()) {
        o[L"key"] = json::String(m_key.ToStdWstring());
        o[L"sort_col"] = json::Number(g_selected_col);
        o[L"sort_order"] = json::Number(g_asc);
        m_json = json_obj2str(o);
        Model_Setting::instance().Set(m_key, m_json);
    }
}

wxListItemAttr* mmListCtrl::OnGetItemAttr(long row) const
{
    return (row % 2) ? attr2_ : attr1_;
}

int mmListCtrl::mmGetId(int defID)
{
    if (0 < m_col_id.size() && (int)m_col_id.size() >= GetColumnCount())
        return m_col_id[defID];
    return defID;
}

void mmListCtrl::mmCreateColumns()
{
    json::Object o = str2json_obj(m_json);

    for (int i = 0; i < COL_MAX; i++)
    {
        json::Object d = json::Object(o[std::to_wstring(i)]);
        if (d.Empty()) break;
        int id = json::Number(d[L"id"]);
        m_col_id[i] = id;
    }

    for (const auto& i : m_col_id)
    {
        json::Object d = json::Object(o[std::to_wstring(i.first)]);
        if (d.Empty()) break;
        const wxString& name = wxGetTranslation(wxString(json::String(d[L"name"])));
        int format = json::Number(d[L"format"]);
        int width = json::Number(d[L"width"]);

        wxListItem item;
        item.SetText(wxString::Format("%i|%s", i.second, name));
        item.SetWidth(width);
        item.SetAlign((wxListColumnFormat)format);
        item.SetId(i.second);
        this->InsertColumn(i.first, item);
    }
}

void mmListCtrl::OnColClick(wxListEvent& event)
{
    event.Skip();
}

void mmListCtrl::OnColRightClick(wxListEvent& event)
{
    g_selected_col = event.GetColumn();
    if (0 > g_selected_col || g_selected_col >= this->GetColumnCount()) return;
    wxMenu menu;
    menu.Append(MENU_HEADER_HIDE, _("Hide column"));
    menu.Append(MENU_HEADER_RESET, _("Reset columns size"));
    //TODO: Column size to header width
    PopupMenu(&menu);
    this->SetFocus();
}

void mmListCtrl::OnHeaderHide(wxCommandEvent& /*event*/)
{
    this->SetColumnWidth(g_selected_col, 0);
}

void mmListCtrl::OnHeaderReset(wxCommandEvent& event)
{
    if (GetDefaultData(this->GetId(), m_json)) {
        this->DeleteAllColumns();
        Model_Setting::instance().Set(m_key, m_json);
        mmCreateColumns();
    }
    else
    {
        for (int i = 0; i < GetColumnCount(); i++)
            SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    }
}

wxString mmListCtrl::BuildPage(const wxString &title) const
{
    wxString text;
    text << "<html>" << wxTextFile::GetEOL();
    text << "<head>" << wxTextFile::GetEOL();
    text << "<title>" << title << "</title>" << wxTextFile::GetEOL();
    text << "</head>" << wxTextFile::GetEOL();
    text << "<body>" << wxTextFile::GetEOL();
    text << "<table ";
    if ((GetWindowStyle() & wxLC_HRULES) ||
        (GetWindowStyle() & wxLC_VRULES))
        text << "border=1";
    else
        text << "border=0";
    text << " cellpadding=4 cellspacing=0 >" << wxTextFile::GetEOL();

    text << "<tr>" << wxTextFile::GetEOL();
    for (int c = 0; c < GetColumnCount(); c++)
    {
        wxListItem col;
        col.SetMask(wxLIST_MASK_TEXT);
        GetColumn(c, col);
        text << "<th><i>" << col.GetText() << "</i></th>" << wxTextFile::GetEOL();
    }
    text << "</tr>" << wxTextFile::GetEOL();

    for (int i = 0; i < GetItemCount(); i++)
    {
        text << "<tr>" << wxTextFile::GetEOL();
        for (int col = 0; col < GetColumnCount(); col++)
        {
            text << "<td>" << wxListCtrl::GetItemText(i, col) << "</td>" << wxTextFile::GetEOL();
        }
        text << "</tr>" << wxTextFile::GetEOL();
    }
    text << "</table>" << wxTextFile::GetEOL();
    text << "</body>" << wxTextFile::GetEOL();
    text << "</html>" << wxTextFile::GetEOL();

    return text;
}

//*--------------------------------------------------------------------*//
void mmPanelBase::PrintPage()
{
    wxWebView * htmlWindow = wxWebView::New(this, wxID_ANY);
    htmlWindow->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
    htmlWindow->SetPage(BuildPage(), "");
    htmlWindow->Print();
}
