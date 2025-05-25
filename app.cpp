#include "app.hpp"
#include <wx/wx.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Password Manager", wxDefaultPosition, wxSize(800, 600))
{
    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "Exit\tAlt-X", "Quit this program");
    menuBar->Append(fileMenu, "File");
    SetMenuBar(menuBar);

    // status bar
    CreateStatusBar();
    SetStatusText("Welcome to Password Manager!");

    // Panel and Sizer for input fields
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Title:"), 0, wxTOP | wxLEFT, 10);
    titleCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    sizer->Add(titleCtrl, 0, wxALL, 10);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Username:"), 0, wxLEFT, 10);
    usernameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    sizer->Add(usernameCtrl, 0, wxALL, 10);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Password:"), 0, wxLEFT, 10);
    passwordCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    sizer->Add(passwordCtrl, 0, wxALL, 10);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "URL:"), 0, wxLEFT, 10);
    urlCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    sizer->Add(urlCtrl, 0, wxALL, 10);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Notes:"), 0, wxLEFT, 10);
    notesCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 60), wxTE_MULTILINE);
    sizer->Add(notesCtrl, 0, wxALL, 10);

    panel->SetSizer(sizer);

    // Bind events
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Centre();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

bool App::OnInit()
{
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(App);
