# Passwort-Manager (C++ mit wxWidgets)

## 📜 **Überblick**

Ein sicherer Passwort-Manager, der Passwörter mit AES-256-Verschlüsselung speichert. Er bietet eine einfache Benutzeroberfläche, um Passwörter zu speichern, zu verwalten und sicher abzurufen. Zusätzlich unterstützt er die Passwort-Generierung und das Kopieren von Passwörtern in die Zwischenablage.

## ⚙️ **Funktionen**

### 1. **Master-Passwort-Schutz**

Der Passwort-Manager wird mit einem Master-Passwort entsperrt. Dieses wird verwendet, um alle gespeicherten Passwörter zu verschlüsseln und zu entschlüsseln.

### 2. **AES-256-Verschlüsselung**

Alle gespeicherten Passwörter werden mit AES-256 verschlüsselt, um maximale Sicherheit zu gewährleisten. Die Verschlüsselung verwendet Salt und Key-Derivation (PBKDF2), um die Sicherheit des Master-Passworts zu erhöhen.

### 3. **Passwort-Generierung**

Es kann ein zufälliges Passwort mit einer benutzerdefinierten Länge und den gewünschten Zeichentypen (Buchstaben, Zahlen, Sonderzeichen) generiert werden.

### 4. **Passwort-Management**

- **Speichern**: Passwörter werden mit einem Titel, Benutzername, URL und Anmerkungen gespeichert.
- **Verwalten**: Passwörter können angezeigt, bearbeitet und gelöscht werden.
- **Kopieren**: Kopiere ein Passwort mit einem Klick in die Zwischenablage.

### 5. **SQLite-Datenbank**

Passwörter werden in einer verschlüsselten SQLite-Datenbank gespeichert. So können die Daten sicher verwaltet und bei Bedarf abgerufen werden.

### 6. **Benutzeroberfläche (GUI)**

Die Benutzeroberfläche wird mit **wxWidgets** erstellt und bietet:

- Eine einfache und klare Liste aller gespeicherten Passwörter.
- Buttons für das Hinzufügen, Bearbeiten und Löschen von Passwörtern.
- Eine Suchfunktion zur schnellen Suche nach Passwörtern.

### 7. **Datenexport und -import**

Die Passwörter können in einem verschlüsselten Format exportiert und bei Bedarf wieder importiert werden.

## 🛠️ **Technologien**

- **C++** – Programmiersprache
- **wxWidgets** – GUI-Bibliothek
- **SQLite3** – Datenbank zur Speicherung der Passwörter
- **Crypto++ oder OpenSSL** – AES-Verschlüsselung
- **CMake** – Build-System
