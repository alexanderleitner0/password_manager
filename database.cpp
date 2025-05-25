#include "database.hpp"
#include <stdexcept>
#include <iostream>

PasswordDB::PasswordDB(std::string filepath) : db_(filepath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    try
    {
        db_.exec("PRAGMA foreign_keys = ON");
        db_.exec("CREATE TABLE IF NOT EXISTS PwdEntry (id INTEGER PRIMARY KEY, title TEXT, username TEXT, password TEXT, url TEXT, notes TEXT);");
        std::cout << "Tables created successfully if they did not exist." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        throw;
    }
}

bool PasswordDB::addPassword(const PwdEntry &entry)
{
    try
    {
        SQLite::Statement insert(db_, "INSERT INTO PwdEntry (title, username, password, url, notes) VALUES (?, ?, ?, ?, ?);");
        insert.bind(1, entry.title);
        insert.bind(2, entry.username);
        insert.bind(3, entry.password);
        insert.bind(4, entry.url);
        insert.bind(5, entry.notes);
        insert.exec();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool PasswordDB::updatePassword(const PwdEntry &entry)
{
    try
    {
        SQLite::Statement update(db_, "UPDATE PwdEntry SET title = ?, username = ?, password = ?, url = ?, notes = ? WHERE id = ?;");
        update.bind(1, entry.title);
        update.bind(2, entry.username);
        update.bind(3, entry.password);
        update.bind(4, entry.url);
        update.bind(5, entry.notes);
        update.bind(6, entry.id);
        update.exec();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool PasswordDB::deletePassword(int id)
{
    try
    {
        SQLite::Statement delStmt(db_, "DELETE FROM PwdEntry WHERE id = ?;");
        delStmt.bind(1, id);
        delStmt.exec();
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Delete error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<PwdEntry> PasswordDB::getAllPasswords()
{
    std::vector<PwdEntry> entries;
    try
    {
        SQLite::Statement select(db_, "SELECT id, title, username, password, url, notes FROM PwdEntry;");
        while (select.executeStep())
        {
            PwdEntry entry;
            entry.id = select.getColumn(0).getInt();
            entry.title = select.getColumn(1).getString();
            entry.username = select.getColumn(2).getString();
            entry.password = select.getColumn(3).getString();
            entry.url = select.getColumn(4).getString();
            entry.notes = select.getColumn(5).getString();
            entries.push_back(entry);
        }
        return entries;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return {};
    }
}

PwdEntry PasswordDB::getPassword(int id)
{
    PwdEntry entry;
    try
    {
        SQLite::Statement select(db_, "SELECT id, title, username, password, url, notes FROM PwdEntry WHERE id = ?;");
        select.bind(1, id);
        if (select.executeStep())
        {
            entry.id = select.getColumn(0).getInt();
            entry.title = select.getColumn(1).getString();
            entry.username = select.getColumn(2).getString();
            entry.password = select.getColumn(3).getString();
            entry.url = select.getColumn(4).getString();
            entry.notes = select.getColumn(5).getString();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "GetPassword error: " << e.what() << std::endl;
    }
    return entry;
}