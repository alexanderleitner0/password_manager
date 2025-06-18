#include "app.hpp"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <openssl/sha.h>
#include <wx/clipbrd.h>

// Forward declaration for hashPassword helper
static std::string hashPassword(const std::string& password);

MainFrame::MainFrame(const wxString& masterPassword_)
    : wxFrame(nullptr, wxID_ANY, "Password Manager", wxDefaultPosition, wxSize(800, 600)),
      db("passwords.db", std::string(masterPassword_.mb_str())),
      masterPassword(masterPassword_)
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
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* formSizer = new wxBoxSizer(wxVERTICAL);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Title:"), 0, wxTOP | wxLEFT, 10);
    titleCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    formSizer->Add(titleCtrl, 0, wxALL, 10);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Username:"), 0, wxLEFT, 10);
    usernameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    formSizer->Add(usernameCtrl, 0, wxALL, 10);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Password:"), 0, wxLEFT | wxTOP, 10);

    wxBoxSizer* passwordRowSizer = new wxBoxSizer(wxHORIZONTAL);
    passwordCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1), wxTE_PASSWORD);
    passwordRowSizer->Add(passwordCtrl, 1, wxRIGHT, 5);
    wxButton* showButton = new wxButton(panel, wxID_ANY, "Show", wxDefaultPosition, wxSize(60, 26));
    passwordRowSizer->Add(showButton, 0);
    wxButton* copyButton = new wxButton(panel, wxID_ANY, "Copy", wxDefaultPosition, wxSize(60, 26));
    passwordRowSizer->Add(copyButton, 0);
    formSizer->Add(passwordRowSizer, 0, wxALL | wxEXPAND, 10);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "URL:"), 0, wxLEFT, 10);
    urlCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    formSizer->Add(urlCtrl, 0, wxALL, 10);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Notes:"), 0, wxLEFT, 10);
    notesCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 60), wxTE_MULTILINE);
    formSizer->Add(notesCtrl, 0, wxALL, 10);

    // Button Sizer
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    saveButton = new wxButton(panel, wxID_ANY, "Save");
    //saveButton->SetBackgroundColour(wxColour(0,255,0));
    buttonSizer->Add(saveButton, 0, wxALL, 5);
    deleteButton = new wxButton(panel, wxID_ANY, "Delete");
    buttonSizer->Add(deleteButton, 0, wxALL, 5);
    formSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    // ListCtrl for entries
    entryList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(400, 500), wxLC_REPORT | wxLC_SINGLE_SEL);
    entryList->InsertColumn(0, "Title", wxLIST_FORMAT_LEFT, 120);
    entryList->InsertColumn(1, "Username", wxLIST_FORMAT_LEFT, 120);
    entryList->InsertColumn(2, "URL", wxLIST_FORMAT_LEFT, 140);

    mainSizer->Add(formSizer, 0, wxALL | wxEXPAND, 10);
    mainSizer->Add(entryList, 0, wxALL | wxEXPAND, 10);
    panel->SetSizer(mainSizer);

    // Bind events
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    saveButton->Bind(wxEVT_BUTTON, &MainFrame::OnSave, this);
    deleteButton->Bind(wxEVT_BUTTON, &MainFrame::OnDelete, this);
    entryList->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnEntrySelected, this);
    copyButton->Bind(wxEVT_BUTTON, &MainFrame::OnCopyPasswordToClipboard, this);
    Centre();

    RefreshEntryList();
}

void MainFrame::OnExit([[maybe_unused]] wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnCopyPasswordToClipboard([[maybe_unused]] wxCommandEvent& event)
{
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(passwordCtrl->GetValue()));
        wxTheClipboard->Close();
        SetStatusText("Password copied to Cliboard");
    }
}

void MainFrame::OnShowPassword([[maybe_unused]] wxCommandEvent& event)
{
    
}


void MainFrame::OnSave([[maybe_unused]] wxCommandEvent& event)
{
    PwdEntry entry;
    entry.title = std::string(titleCtrl->GetValue().mb_str());
    entry.username = std::string(usernameCtrl->GetValue().mb_str());
    entry.password = std::string(passwordCtrl->GetValue().mb_str());
    entry.url = std::string(urlCtrl->GetValue().mb_str());
    entry.notes = std::string(notesCtrl->GetValue().mb_str());

    // Check required fields
    if (entry.title.empty() || entry.password.empty()) {
        SetStatusText("Title and password must not be empty!");
        return;
    }

    // Set master password hash if first run
    if (db.isFirstRun()) {
        std::string hash = hashPassword(std::string(masterPassword.mb_str()));
        if (!db.setMasterPasswordHash(hash)) {
            SetStatusText("Failed to set master password hash!");
            return;
        }
    }

    bool ok = false;

    // Use selectedId to update instead of comparing strings
    if (selectedId != -1) {
        entry.id = selectedId;
        ok = db.updatePassword(entry);
        SetStatusText(ok ? "Password entry updated!" : "Failed to update password entry!");
    } else {
        ok = db.addPassword(entry);
        SetStatusText(ok ? "Password entry saved!" : "Failed to save password entry!");
    }

    if (ok) {
        // Clear fields after save
        titleCtrl->SetValue("");
        usernameCtrl->SetValue("");
        passwordCtrl->SetValue("");
        urlCtrl->SetValue("");
        notesCtrl->SetValue("");
        selectedId = -1;
        RefreshEntryList();
    }
}


void MainFrame::OnDelete([[maybe_unused]] wxCommandEvent& event)
{
    if (selectedId == -1) {
        SetStatusText("No entry selected to delete.");
        return;
    }
    if (db.deletePassword(selectedId)) {
        SetStatusText("Password entry deleted!");
        titleCtrl->SetValue("");
        usernameCtrl->SetValue("");
        passwordCtrl->SetValue("");
        urlCtrl->SetValue("");
        notesCtrl->SetValue("");
        selectedId = -1;
        RefreshEntryList();
    } else {
        SetStatusText("Failed to delete password entry!");
    }
}

void MainFrame::OnEntrySelected(wxListEvent& event)
{
    long itemIndex = event.GetIndex();
    selectedId = static_cast<int>(entryList->GetItemData(itemIndex));
    PwdEntry entry = db.getPassword(selectedId);
    titleCtrl->SetValue(entry.title);
    usernameCtrl->SetValue(entry.username);
    passwordCtrl->SetValue(entry.password);
    urlCtrl->SetValue(entry.url);
    notesCtrl->SetValue(entry.notes);
}

void MainFrame::RefreshEntryList()
{
    entryList->DeleteAllItems();
    std::vector<PwdEntry> entries = db.getAllPasswords();
    for (const auto& entry : entries) {
        long idx = entryList->InsertItem(entryList->GetItemCount(), entry.title);
        entryList->SetItem(idx, 1, entry.username);
        entryList->SetItem(idx, 2, entry.url);
        entryList->SetItemData(idx, entry.id);
    }
}


static std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.data()), password.size(), hash);
    static const char hex[] = "0123456789abcdef";
    std::string out;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        out += hex[(hash[i] >> 4) & 0xF];
        out += hex[hash[i] & 0xF];
    }
    return out;
}

bool App::OnInit()
{
    // Prompt for master password
    wxTextEntryDialog dlg1(nullptr, "Enter master password:", "Master Password", "", wxOK | wxCANCEL | wxTE_PASSWORD);
    if (dlg1.ShowModal() != wxID_OK || dlg1.GetValue().IsEmpty()) {
        wxMessageBox("Master password cannot be empty!", "Error", wxOK | wxICON_ERROR);
        return false;
    }
    wxString enteredPassword = dlg1.GetValue();

    // Check if it's first run
    PasswordDB tempDB("passwords.db", std::string(enteredPassword.mb_str()));
    bool firstRun = tempDB.isFirstRun();

    // If first run, confirm password
    if (firstRun) {
        wxTextEntryDialog dlg2(nullptr, "Confirm master password:", "Confirm Password", "", wxOK | wxCANCEL | wxTE_PASSWORD);
        if (dlg2.ShowModal() != wxID_OK || dlg2.GetValue().IsEmpty()) {
            wxMessageBox("Master password confirmation cancelled or empty!", "Error", wxOK | wxICON_ERROR);
            return false;
        }
        if (dlg2.GetValue() != enteredPassword) {
            wxMessageBox("Passwords do not match!", "Error", wxOK | wxICON_ERROR);
            return false;
        }
    } else {
        // Validate password hash
        std::string hash = hashPassword(std::string(enteredPassword.mb_str()));
        if (!tempDB.checkMasterPasswordHash(hash)) {
            wxMessageBox("Wrong master password!", "Error", wxOK | wxICON_ERROR);
            return false;
        }
    }

    masterPassword = enteredPassword;

    MainFrame* frame = new MainFrame(masterPassword);
    frame->Show(true);
    return true;
}


wxIMPLEMENT_APP(App);
