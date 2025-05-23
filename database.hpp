#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

struct PwdEntry
{
    int id;
    std::string title;
    std::string username;
    std::string password;
    std::string url;
    std::string notes;
};

class PasswordDB
{
public:
    PasswordDB(std::string filepath);

    bool addPassword(const PwdEntry &entry);
    bool updatePassword(const PwdEntry &entry);
    bool deletePassword(int id);
    std::vector<PwdEntry> getAllPasswords();
    PwdEntry getPassword(int id);

private:
    SQLite::Database db_;
    bool initialized_;
};

#endif