#ifndef APP_HPP
#define APP_HPP

#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    wxTextCtrl* titleCtrl;
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxTextCtrl* urlCtrl;
    wxTextCtrl* notesCtrl;

    void OnExit(wxCommandEvent& event);
};

class App : public wxApp
{
public:
    bool OnInit();
};

#endif