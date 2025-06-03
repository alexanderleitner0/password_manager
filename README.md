# Password Manager

## Project Description

A secure password manager in C++ with a GUI (wxWidgets) that stores passwords encrypted in an SQLite database.  
Features:
- Master password protection
- AES-256 encryption (OpenSSL)
- Store, view, edit, and delete passwords

---

## Build Instructions

### Requirements

- CMake ≥ 3.18
- C++20 compatible compiler
- Dependencies:
  - wxWidgets
  - SQLiteCpp
  - OpenSSL
  - Catch2 (for tests only)

### Build

```sh
git clone <repo-url>
mkdir build && cd build
cmake ..
make -j8
```

### Run

```sh
cd build
./passwordManager
```

### Run Tests

```sh
./pwManagerTest
```

---

## User Manual

1. On first start, choose and remember a master password!
2. Passwords can be added, viewed, edited, and deleted via the GUI.
3. All passwords are stored encrypted.
4. On every next start, you must enter the master password to access your data.



---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE).
