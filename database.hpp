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
    PasswordDB(std::string filepath, const std::string& masterPassword);

    bool addPassword(const PwdEntry &entry);
    bool updatePassword(const PwdEntry &entry);
    bool deletePassword(int id);
    std::vector<PwdEntry> getAllPasswords();
    PwdEntry getPassword(int id);

    bool isFirstRun();
    bool setMasterPasswordHash(const std::string& hash);
    bool checkMasterPasswordHash(const std::string& hash);

private:
    SQLite::Database db_;
    std::string key_;
    std::string encrypt(const std::string& plaintext);
    std::string decrypt(const std::string& ciphertext);
};

#endif