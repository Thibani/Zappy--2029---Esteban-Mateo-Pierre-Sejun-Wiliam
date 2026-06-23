# Zappy AI

Client IA pour le projet Zappy — EPITECH 2025.

## Lancement rapide

```bash
make
./zappy_ai -p 4242 -n team1 -h localhost
```

## Build

```bash
make        # crée le venv + génère le wrapper zappy_ai
make re     # rebuild complet
make clean  # supprime les __pycache__
make fclean # supprime le wrapper et le venv
```

## Usage

```bash
./zappy_ai -p <port> -n <team> [-h <host>]
```

| Option | Description | Défaut |
|--------|-------------|--------|
| `-p`   | Port du serveur | — |
| `-n`   | Nom de l'équipe | — |
| `-h`   | Adresse du serveur | `localhost` |

## Tests

```bash
make tests
```

Lance les 3 suites (46 tests) avec output verbeux — chaque commande
envoyée et reçue est affichée.

Pour filtrer :

```bash
PYTHONPATH=src:tests python3 tests/run.py parser   # parser seulement
PYTHONPATH=src:tests python3 tests/run.py brain    # brain seulement
PYTHONPATH=src:tests python3 tests/run.py fsm      # intégration seulement
```

---

## Structure

```
ai/
├── src/
│   ├── main.py          # entry point, argparse
│   ├── zappyAI.py       # boucle FSM principale
│   ├── brain.py         # état mutable du joueur
│   ├── connection.py    # socket TCP + handshake
│   ├── parser.py        # parse les réponses serveur
│   ├── constants.py     # table d'incantation, coûts, seuils
│   └── fsm.py           # enum des états
└── tests/
    ├── run.py           # runner global
    ├── tests_runner.py  # framework verbose ✓/✗
    ├── mock_server.py   # faux serveur TCP scriptable
    ├── tests_parser.py  # 18 tests unitaires parser
    ├── tests_brain.py   # 21 tests unitaires brain + constantes
    └── tests_fsm.py     # 7 tests d'intégration mock server
```

---

## Machine à états (FSM)

```
                    food ≤ 10
         ┌─────────────────────────────┐
         ▼                             │
      FORAGE ──── food ≥ 20 ────► COLLECT ◄─── (défaut)
                                   │    ▲
                  ressources OK    │    │ incantation terminée
                                   ▼    │
                              COORDINATE
                               │      │
               assez joueurs   │      │ reçoit CALL_LVn
                               ▼      ▼
                          INCANTATE  FOLLOW_LEADER
```

| État | Déclencheur | Comportement |
|------|-------------|--------------|
| `FORAGE` | `food ≤ 10` (override global) | cherche et ramasse de la nourriture |
| `COLLECT` | état par défaut | ramasse les pierres manquantes pour l'incantation |
| `COORDINATE` | ressources complètes | broadcast `CALL_LVn`, attend les coéquipiers |
| `INCANTATE` | assez de joueurs sur la case | pose les pierres + lance l'incantation |
| `FOLLOW_LEADER` | reçoit `CALL_LVn` d'un leader | se déplace vers la source du broadcast |
| `IDLE` | niveau max atteint (lv8) | exploration aléatoire |

---

## Protocole de coordination

Les IAs se coordonnent via `Broadcast` avec ces messages :

```
Leader   → "CALL_LV2"    # appelle les joueurs de niveau 2
Follower → "READY_LV2"   # je suis sur ta case
Leader   → "START_LV2"   # tout le monde est là, on incante
```

---

## Table d'incantation

| Niveau | Joueurs | linemate | deraumere | sibur | mendiane | phiras | thystame |
|--------|---------|----------|-----------|-------|----------|--------|----------|
| 1 → 2  | 1       | 1        | 0         | 0     | 0        | 0      | 0        |
| 2 → 3  | 2       | 1        | 1         | 1     | 0        | 0      | 0        |
| 3 → 4  | 2       | 2        | 0         | 1     | 0        | 2      | 0        |
| 4 → 5  | 4       | 1        | 1         | 2     | 0        | 1      | 0        |
| 5 → 6  | 4       | 1        | 2         | 1     | 3        | 0      | 0        |
| 6 → 7  | 6       | 1        | 2         | 3     | 0        | 1      | 0        |
| 7 → 8  | 6       | 2        | 2         | 2     | 2        | 2      | 1        |

---

## Ajouter un test FSM

Le `MockServer` scripture les échanges commande par commande :

```python
from mock_server import MockServer, Expect, Push, Done

script = [
    Expect("look",      "[ food, , , ]"),  # attend "look", répond
    Expect("take food", "ok"),
    Push("message 2, CALL_LV3"),           # pousse une ligne sans attendre
    Done(),                                # ferme la connexion
]

with MockServer(script) as ms:
    # lance l'IA en thread ici
    assert ms.errors == []                 # vérifie que le script a été suivi
    assert ms.received == ["team1", "Look", "Take food"]
```