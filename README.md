# virtualfilesystem
Das Virtualfilesystem ist eine API Bibliothek die verschiedene Dateisysteme virtuell simuliert. Bei der Entwicklung wird besonders auf die Skalierbarkeit geachtet (siehe "Hinzufügen eines Dateisystems").

## Setup zum selbst hosten oder entwickeln
1. Klone das Github Projekt
   ```shell
   # Klone das Projekt mit https
   git clone https://github.com/Baspli98/virtualfilesystem.git
   ```
# Git und Branching-Strategie
In diesem Projekt arbeiten wir mit Feature-Branches.
Der "main"-Branch ist der Hauptzweig in diesem Projekt.

## Workflow
1. Lokale "main"-Branch aktualisieren mit:
    ```shell
    git pull origin main
    ```
2. Erstellen eines neuen Zweigs (Branch), um ein bestimmtes Feature zu implementieren
    ```shell
    git switch -c <branchname>
    ```
3. Mache Lokale Änderungen.
4. Füge die Änderungen zur Bühne hinzu:
    ```shell
    # FÜge alle Änderungen hinzu
    git add .
    # Füge nur spezifische Änderungen hinzu
    git add <path-to-file | pattern>
    ```
5. Führe Commit durch:
    ```shell
    git commit -m <"Eine Nachricht die, die Änderungen beschreibt">
    ```
6. Pushe den Commit in das Remote-Repository:
    ```shell
    # Niemals in main pushen
    git push origin <branchname>
    ```
7. Öffne einen Pull Request auf Github, um einen Merge in den "main"-Branch anzufragen.
8. Pull Request wird vom anderen Team Teilnehmer überprüft.
9. Sobald überprüft und für gut behandelt Merge diesen Branch mit dem "main"-Branch. 
10. Lösche den Feature-Branch und fange bei Schritt 1 erneut an.

## Nützliche Befehle
Folgende Befehle sind zum Arbeiten mit Git hilfreich:

```shell
# Gibt den Commit-Verlauf in kurzer ASCII-Form aus
git log --oneline --parents --graph
```
```shell
# Gibt den aktuellen Stand des Verzeichnis aus
git status 
```
```shell
# Gibt den aktuellen Zweig in dem man sich gerade befindet an
git branch
# Löscht bestimmten Branch (lokal)
git branch -d <branchname>
```

# Betriebssysteme
Dieses Projekt wurde im Rahmen des Moduls Betriebssysteme im Studiengang Mensch-Technik-Interaktion und Angewandte Informatik der Hochschule Ruhr West entwickelt.
