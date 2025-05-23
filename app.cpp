#include "app.hpp"
#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
    MainFrame() : wxFrame(nullptr, wxID_ANY, "Password Manager", wxDefaultPosition, wxSize(800, 600))
    {
        // Create a basic menu bar
        wxMenuBar *menuBar = new wxMenuBar;
        wxMenu *fileMenu = new wxMenu;
        fileMenu->Append(wxID_EXIT, "Exit\tAlt-X", "Quit this program");
        menuBar->Append(fileMenu, "File");
        SetMenuBar(menuBar);

        // Create a status bar
        CreateStatusBar();
        SetStatusText("Welcome to Password Manager!");

        // Bind events
        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
        Centre();
    }

private:
    void OnExit(wxCommandEvent& event)
    {
        Close(true);
    }
};

bool App::OnInit()
{
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(App);
