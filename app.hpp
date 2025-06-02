#ifndef APP_HPP
#define APP_HPP

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "database.hpp"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& masterPassword);

private:
    wxTextCtrl* titleCtrl;
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxTextCtrl* urlCtrl;
    wxTextCtrl* notesCtrl;
    wxButton* saveButton;
    wxButton* editButton;
    wxButton* deleteButton;
    wxListCtrl* entryList;
    PasswordDB db;
    int selectedId = -1;
    wxString masterPassword;

    void OnExit(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnEntrySelected(wxListEvent& event);
    void RefreshEntryList();
};

class App : public wxApp
{
public:
    bool OnInit();
    wxString masterPassword;
};

#endif