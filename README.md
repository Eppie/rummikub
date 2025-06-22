# Rummikub

This project is a C++ implementation of the tile-based game Rummikub. The goal is to be the first player to play all the tiles from their rack by forming them into sets of runs and groups.

## How to Build

The project uses a `Makefile` for building.

*   **Build the main game executable:**
    ```bash
    make all
    ```
    This will create an executable named `rummikub`.

*   **Build the test executable:**
    ```bash
    make test
    ```
    This will create an executable named `test`.

*   **Clean build files:**
    ```bash
    make clean
    ```
    This will remove the `rummikub` and `test` executables.

## How to Run

*   **Run the main program:**
    ```bash
    ./rummikub
    ```
    Currently, this program will generate a deck of tiles, shuffle it, draw a hand of 14 tiles for a single player, and then attempt to find and display any "runs" within that hand. It will also indicate if the identified runs are valid.

*   **Run the tests:**
    ```bash
    ./test
    ```
    This will execute a basic test case that verifies the `generateAllTiles()` function creates the correct number of tiles (104).

## Current Status

The project is in an early stage of development. Here's a summary of what's implemented and what's missing:

### Implemented Features:
*   **Tile Representation**: `Tile.hpp` and `Tile.cpp` define the tile structure (number and color) and basic operations.
*   **Deck Generation**: `utilities.hpp` provides `generateAllTiles()` to create a full Rummikub deck (104 tiles) and `drawHand()` to deal tiles to a player.
*   **Run Detection (Basic)**: `runs.hpp` includes `findRuns()` which attempts to identify possible runs from a set of tiles. `isValidRun()` checks the validity of a potential run.
*   **Group Validation**: `groups.hpp` includes `isValidGroup()` to check if a set of tiles forms a valid group.
*   **Basic Test**: A simple test in `test.cpp` verifies tile generation.
*   **Makefile**: Basic build targets for the main program and tests are present.

### Missing Features / Areas for Improvement:
*   **Group Detection**: The `findGroups()` function in `groups.hpp` is incomplete and marked with a `TODO`. It currently only separates tiles by number, not by valid group formation rules.
*   **Game Logic**: There is no core game logic implemented. This includes:
    *   Player turns
    *   Managing the game board (tiles played)
    *   Rules for initial meld
    *   Drawing tiles from the pool
    *   Manipulating existing sets on the table
    *   Winning conditions
*   **User Interface**: No user interface exists beyond basic console output of the initial hand and detected runs.
*   **Comprehensive Tests**: Current tests are minimal. More tests are needed for run/group detection, game rules, etc.
*   **Network Play**: The `Makefile` includes targets for `client` and `server` (`client.cpp`, `server.cpp`), suggesting an intention for networked multiplayer, but these files and the corresponding functionality are not implemented.
*   **Utility Function Generalization**: Some functions in `utilities.hpp` (e.g., color-specific filters like `onlyBlues`) are marked with a `TODO` to be generalized.

## TODO List

To get the game into a working and playable state, the following tasks need to be addressed:

1.  **Complete `findGroups()` Function**: Implement the logic in `groups.hpp` to correctly identify all possible groups from a set of tiles.
2.  **Implement Core Game Logic**:
    *   Develop a system for managing player turns.
    *   Create data structures to represent the game board (table).
    *   Implement rules for players placing their initial meld (e.g., 30 points).
    *   Allow players to draw tiles from the main pool.
    *   Implement logic for players to add tiles to existing sets or rearrange them according to Rummikub rules.
    *   Define and check for winning conditions (a player plays all their tiles).
3.  **Develop a User Interface (CLI)**:
    *   Display the player's hand clearly.
    *   Show the current state of the game board.
    *   Provide a way for users to input their moves (e.g., selecting tiles, forming sets, placing them on the board).
4.  **Add Comprehensive Tests**:
    *   Write unit tests for `findRuns` and `findGroups` with various edge cases.
    *   Test game rule implementations.
    *   Test player move validation.
5.  **Refactor and Improve Utilities**:
    *   Address the `TODO` in `utilities.hpp` to generalize color-specific filter functions.
6.  **(Optional) Implement Network Play**:
    *   Create `client.cpp` and `server.cpp`.
    *   Implement networking logic for multiplayer games.
7.  **(Optional) Enhance User Interface**:
    *   Consider a simple graphical interface if desired, or improve the command-line experience.
