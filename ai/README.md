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
│   ├── ai.py            # boucle FSM principale (ZappyAI)
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
                    food ≤ 20
         ┌─────────────────────────────┐
         ▼                             │
      FORAGE ──── food ≥ 20 ────► COLLECT ◄─── (défaut / après incantation)
                                   │    ▲
                  ressources OK    │    │ incantation terminée
                                   ▼    │
                              COORDINATE
                               │      │
               assez joueurs   │      │ reçoit CALL_LVn
                               ▼      ▼
                          INCANTATE  FOLLOW_LEADER
                                          │
                              dir == 0    │ (sur la case du leader)
                                          ▼
                                      COORDINATE
```

| État | Déclencheur | Comportement |
|------|-------------|--------------|
| `FORAGE` | `food ≤ FOOD_DANGER_THRESHOLD` (20) | cherche et ramasse de la nourriture |
| `COLLECT` | état par défaut | ramasse les pierres manquantes pour l'incantation ; fork périodique tous les 50 ticks (niveau ≥ 2) |
| `COORDINATE` | ressources complètes | broadcast `CALL_LVn`, attend les coéquipiers (timeout 200 ticks) |
| `INCANTATE` | assez de joueurs sur la case | pose les pierres (rôle leader) + lance l'incantation ; fork si nourriture suffisante après succès |
| `FOLLOW_LEADER` | reçoit `CALL_LVn` d'un leader | se déplace vers la source du broadcast ; abandonne après 60 ticks sans réponse |
| `IDLE` | niveau max atteint (lv8) | exploration aléatoire |

### Priorités de transition

À chaque tick, deux overrides globaux sont vérifiés **avant** le dispatch d'état :

1. **Food critique** (`food ≤ 20`) → bascule immédiatement en `FORAGE`, quel que soit l'état courant.
2. **Appel leader** (`CALL_LVn` reçu) → bascule immédiatement en `FOLLOW_LEADER` si pas déjà en train d'incanter.

---

## Protocole de coordination

Les IAs se coordonnent via `Broadcast` avec ces messages :

```
Leader   → "CALL_LV2"    # appelle les joueurs de niveau 2
Follower → "WHERE_LV2"   # demande la position du leader en marchant vers lui
Follower → "READY_LV2"   # je suis sur ta case (dir == 0)
Leader   → "START_LV2"   # tout le monde est là, on incante
```

Le leader comptabilise les coéquipiers de deux façons : les `READY_LVn` reçus par broadcast **et** les joueurs visibles sur sa case lors du `Look`. Il démarre dès que le total atteint le nombre requis.

---

## Fork automatique

L'IA fork dans deux situations :

- **Périodiquement** : tous les 50 ticks en état `COLLECT` (niveau ≥ 2), pour alimenter le pool de connexions disponibles.
- **Après incantation réussie** : si la nourriture est suffisante (`food ≥ FOOD_SAFE_THRESHOLD`), un œuf est pondu et un nouveau processus est lancé automatiquement.

Chaque fork spawne un sous-processus `main.py` avec les mêmes arguments `-p`, `-n`, `-h`.

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

## Constantes clés

| Constante | Valeur | Rôle |
|-----------|--------|------|
| `FOOD_DANGER_THRESHOLD` | 20 | Déclenche le mode `FORAGE` |
| `FOOD_SAFE_THRESHOLD` | 40 | Fin du mode `FORAGE` / seuil fork post-incantation |
| `FOOD_LIFE_UNITS` | 126 | Unités de vie par item nourriture |

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