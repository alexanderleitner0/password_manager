#include "database.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdio> // for remove()

TEST_CASE("PasswordDB Creation and Master Password", "[creation][master]") {
    const std::string dbfile = "test_pw.db";
    remove(dbfile.c_str());
    PasswordDB db(dbfile, "testmaster");

    SECTION("First run is true for new DB") {
        CHECK(db.isFirstRun());
    }
    SECTION("Set and check master password hash") {
        CHECK(db.setMasterPasswordHash("hash123"));
        CHECK_FALSE(db.isFirstRun());
        CHECK(db.checkMasterPasswordHash("hash123"));
        CHECK_FALSE(db.checkMasterPasswordHash("wronghash"));
    }
}

TEST_CASE("PasswordDB Add/Get/Update/Delete", "[crud]") {
    const std::string dbfile = "test_pw.db";
    remove(dbfile.c_str());
    PasswordDB db(dbfile, "testmaster");
    db.setMasterPasswordHash("hash123");

    PwdEntry entry{0, "title", "user", "pw", "url", "notes"};
    SECTION("Add password") {
        CHECK(db.addPassword(entry));
        auto all = db.getAllPasswords();
        CHECK(all.size() == 1);
        CHECK(all[0].title == "title");
    }
    SECTION("Get password by id") {
        db.addPassword(entry);
        auto all = db.getAllPasswords();
        auto single = db.getPassword(all[0].id);
        CHECK(single.username == "user");
    }
    SECTION("Update password") {
        db.addPassword(entry);
        auto all = db.getAllPasswords();
        auto single = db.getPassword(all[0].id);
        single.username = "newuser";
        CHECK(db.updatePassword(single));
        auto updated = db.getPassword(single.id);
        CHECK(updated.username == "newuser");
    }
    SECTION("Delete password") {
        db.addPassword(entry);
        auto all = db.getAllPasswords();
        CHECK(db.deletePassword(all[0].id));
        CHECK(db.getAllPasswords().empty());
    }
}

TEST_CASE("PasswordDB Encryption/Decryption", "[encryption]") {
    const std::string dbfile = "test_pw.db";
    remove(dbfile.c_str());
    PasswordDB db1(dbfile, "master1");
    db1.setMasterPasswordHash("hash1");
    PwdEntry entry{0, "title", "user", "secretpw", "url", "notes"};
    CHECK(db1.addPassword(entry));

    SECTION("Correct master password decrypts") {
        PasswordDB db2(dbfile, "master1");
        auto all = db2.getAllPasswords();
        CHECK(all[0].password == "secretpw");
    }
    SECTION("Wrong master password fails to decrypt") {
        PasswordDB db3(dbfile, "wrongmaster");
        auto all = db3.getAllPasswords();
        CHECK(all[0].password != "secretpw");
    }
    remove(dbfile.c_str());
} 