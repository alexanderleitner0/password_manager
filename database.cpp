#include "database.hpp"
#include <stdexcept>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <vector>
#include <cstring>

namespace {
std::string deriveKey(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.data()), password.size(), hash);
    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}
std::string toHex(const unsigned char* data, size_t len) {
    static const char hex[] = "0123456789abcdef";
    std::string out;
    for (size_t i = 0; i < len; ++i) {
        out += hex[(data[i] >> 4) & 0xF];
        out += hex[data[i] & 0xF];
    }
    return out;
}
std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.data()), password.size(), hash);
    return toHex(hash, SHA256_DIGEST_LENGTH);
}
}

PasswordDB::PasswordDB(std::string filepath, const std::string& masterPassword)
    : db_(filepath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE), key_(deriveKey(masterPassword))
{
    try
    {
        db_.exec("PRAGMA foreign_keys = ON");
        db_.exec("CREATE TABLE IF NOT EXISTS PwdEntry (id INTEGER PRIMARY KEY, title TEXT, username TEXT, password TEXT, url TEXT, notes TEXT);");
        db_.exec("CREATE TABLE IF NOT EXISTS Meta (key TEXT PRIMARY KEY, value TEXT);");
        std::cout << "Tables created successfully if they did not exist." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        throw;
    }
}


bool PasswordDB::isFirstRun() {
    try {
        SQLite::Statement query(db_, "SELECT value FROM Meta WHERE key = 'master_hash';");
        if (query.executeStep()) {
            return false;
        }
        return true;
    } catch (...) {
        return true;
    }
}

bool PasswordDB::setMasterPasswordHash(const std::string& hash) {
    try {
        SQLite::Statement insert(db_, "INSERT INTO Meta (key, value) VALUES ('master_hash', ?);");
        insert.bind(1, hash);
        insert.exec();
        return true;
    } catch (...) {
        return false;
    }
}

bool PasswordDB::checkMasterPasswordHash(const std::string& hash) {
    try {
        SQLite::Statement query(db_, "SELECT value FROM Meta WHERE key = 'master_hash';");
        if (query.executeStep()) {
            std::string stored = query.getColumn(0).getString();
            return stored == hash;
        }
        return false;
    } catch (...) {
        return false;
    }
}

std::string PasswordDB::encrypt(const std::string& plaintext)
{

    std::vector<unsigned char> iv(EVP_MAX_IV_LENGTH);
    RAND_bytes(iv.data(), iv.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, ciphertext_len = 0;
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key_.data()), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    std::string result(reinterpret_cast<char*>(iv.data()), iv.size());
    result += std::string(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
    return result;
}

std::string PasswordDB::decrypt(const std::string& ciphertext)
{
    if (ciphertext.size() < EVP_MAX_IV_LENGTH) return "";
    const unsigned char* iv = reinterpret_cast<const unsigned char*>(ciphertext.data());
    const unsigned char* enc_data = reinterpret_cast<const unsigned char*>(ciphertext.data() + EVP_MAX_IV_LENGTH);
    int enc_len = ciphertext.size() - EVP_MAX_IV_LENGTH;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> plaintext(enc_len + EVP_MAX_BLOCK_LENGTH);
    int len = 0, plaintext_len = 0;
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key_.data()), iv);
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, enc_data, enc_len);
    plaintext_len = len;
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}

bool PasswordDB::addPassword(const PwdEntry &entry)
{
    try
    {
        SQLite::Statement insert(db_, "INSERT INTO PwdEntry (title, username, password, url, notes) VALUES (?, ?, ?, ?, ?);");
        insert.bind(1, entry.title);
        insert.bind(2, entry.username);
        insert.bind(3, encrypt(entry.password));
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
        update.bind(3, encrypt(entry.password));
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
            entry.password = decrypt(select.getColumn(3).getString());
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
            entry.password = decrypt(select.getColumn(3).getString());
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