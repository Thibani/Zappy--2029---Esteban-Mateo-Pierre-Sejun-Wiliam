# Zappy Server

## Overview

The server is a single-process, single-thread TCP server written in C++20.  
It uses `poll()` for socket multiplexing and manages all game state for the Zappy world.

## Build

```bash
cd server
make           # builds zappy_server
make debug     # builds with -g3 -DDEBUG
make tests     # builds and runs unit tests
make clean     # removes object files
make fclean    # removes object files + binary
make re        # fclean + all
```

## Usage

```bash
./zappy_server -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq
```

| Option | Description | Required | Default |
|--------|-------------|----------|---------|
| `-p`   | Port number (1–65535) | ✅ | — |
| `-x`   | World width (> 0) | ✅ | — |
| `-y`   | World height (> 0) | ✅ | — |
| `-n`   | Team names (one or more, space-separated) | ✅ | — |
| `-c`   | Number of authorized clients per team | ✅ | — |
| `-f`   | Reciprocal of time unit (frequency) | ❌ | `100` |

**Note:** The team name `GRAPHIC` is reserved for the GUI client and cannot be used as a team name.

### Example

```bash
./zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 5 -f 100
```

Press **Ctrl+D** in the server terminal for a clean shutdown.

---

## Architecture

```
server/
├── include/
│   ├── exceptions/
│   │   └── ServerException.hpp   # Exception hierarchy
│   ├── server/
│   │   ├── Server.hpp            # poll() loop, socket management
│   │   ├── Client.hpp            # Per-client state, buffers, command queue
│   │   └── CommandHandler.hpp    # Command parsing and dispatch
│   └── utils/
│       ├── Args.hpp              # CLI argument parsing
│       └── Clock.hpp             # Monotonic time, action deadlines
└── src/
    ├── main.cpp
    ├── server/
    │   ├── Server.cpp
    │   ├── Client.cpp
    │   └── CommandHandler.cpp
    └── utils/
        ├── Args.cpp
        └── Clock.cpp
```

---

## Key Concepts

### Time units

Every action takes `action_cost / f` seconds.  
Use `Clock::deadline(actionCost, freq)` to compute when an action completes, and `Clock::hasPassed(tp)` to check it.

```cpp
// Example: Forward takes 7/f seconds
TimePoint doneAt = Clock::deadline(7, args.freq);
// Later...
if (Clock::hasPassed(doneAt))
    // apply the move
```

### Command queue

Each client can have up to **10 pending commands** buffered.  
Commands beyond 10 are dropped with a warning log.  
Commands are newline-terminated (`\n` or `\r\n`).

### Client authentication

The first message from any client determines its type:
- `GRAPHIC` → GUI client
- any valid team name → AI client

Unauthenticated clients cannot send game commands.

---

## Person responsibilities

| Person | Files owned |
|--------|-------------|
| **1(Esteban)** | `Server`, `Client`, `CommandHandler`, `Args`, `Clock` |
| **2(Sejun)** | `Game`, `Map`, `Player`, `Team`, `Egg` |
| **3(William)** | GUI protocol handlers inside `CommandHandler` |
| **4(Mateo)** | `gui/` entirely |
| **5(Pierre)** | `ai/` entirely |

---

## Wiring in Game (person 2 handoff)

When `Game` is ready, in `main.cpp`:

1. Replace the stub:
```cpp
// Remove this:
namespace Zappy { class Game {}; }

// Add this:
#include "game/Game.hpp"
```

2. In `CommandHandler.cpp`, replace all `// TODO: Game::xxx` stubs with real calls.

3. In `Server.cpp`, implement `_nearestDeadlineMs()` and `_processPendingActions()` using `Game`'s action deadline tracking.

---

## Testing

```bash
make tests
```

Tests cover:
- Arg parsing (valid and invalid inputs)
- Client command queue (partial reads, CRLF, max capacity)
- Client write buffer (push, consume, overflow)

Manual test with `nc`:
```bash
nc localhost 4242
# WELCOME
team1           # authenticate as AI
# 0
# 10 10
Forward         # send a command
# ok
GRAPHIC         # (in a second terminal) authenticate as GUI
```