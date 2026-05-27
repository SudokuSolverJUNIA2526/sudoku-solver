# Convention de nommage — Projet C

## Fichiers

- Utiliser uniquement des lettres minuscules.
- Séparer les mots avec des underscores (`_`).
- Éviter les caractères spéciaux, espaces et accents.
- Extensions autorisées : `.c` et `.h`.

### Exemples

```c
network_manager.c
network_manager.h
player_controller.c
```

---

## Variables

- Utiliser le format `snake_case`.
- Les noms doivent être explicites et compréhensibles.
- Éviter les abréviations inutiles.

### Exemples

```c
int player_score;
float delta_time;
char *server_address;
```

---

## Fonctions

- Utiliser le format `snake_case`.
- Commencer par un verbe décrivant l’action.
- Préfixer si nécessaire par le module concerné.

### Exemples

```c
void init_renderer(void);
int load_texture(const char *path);
void network_send_packet(void);
```

---

## Enum

- Utiliser le format `PascalCase`.
- Le nom doit représenter une catégorie ou un ensemble d’états.
- Préfixer si nécessaire selon le module.

### Exemples

```c
typedef enum PlayerState
{
    ...
} PlayerState;

typedef enum NetworkStatus
{
    ...
} NetworkStatus;
```

---

## Valeurs d'enum

- Utiliser le format `SCREAMING_SNAKE_CASE`.
- Préfixer chaque valeur avec le nom de l’enum.

### Exemples

```c
PLAYER_STATE_IDLE,
PLAYER_STATE_RUNNING,
PLAYER_STATE_JUMPING,

NETWORK_STATUS_CONNECTED,
NETWORK_STATUS_DISCONNECTED,
```

---

## Struct

- Utiliser le format `PascalCase`.
- Le nom doit représenter clairement l’objet ou la donnée.

### Exemples

```c
struct PlayerData
{
    int health;
    int score;
};

struct NetworkPacket
{
    int id;
};
```

---

## Typedef

- Utiliser le format `PascalCase`.
- Les typedef de struct et enum doivent avoir le même nom que leur type.

### Exemples

```c
typedef struct PlayerData
{
    int health;
} PlayerData;

typedef enum GameState
{
    GAME_STATE_MENU,
    GAME_STATE_PLAYING
} GameState;
```

---

## Constantes

- Utiliser le format `SCREAMING_SNAKE_CASE`.
- Les constantes globales doivent être préfixées si nécessaire.

### Exemples

```c
#define MAX_PLAYERS 16
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

const float GRAVITY_FORCE = 9.81f;
```

