#ifndef GAME_H
#define GAME_H

// Struct definitions
typedef struct {
    float x, y;
    float dx, dy;
    int in_motion;
} Ball;

typedef struct {
    float x, y;   // Position of the hoop
    float dx;     // Horizontal movement
    int width;    // Width of the hoop
    int height;   // Height of the hoop
} Hoop;

typedef struct {
    float x, y;
    int score;
    int shots_made;
    int shots_attempted;
} Player;

typedef struct {
    int running;
} GameState;

// Function declarations
Player init_player(float x, float y);
Ball init_ball(float x, float y);
Hoop init_hoop(float x, float y, int width, int height, float dx);
GameState init_game_state();

void draw_ball(const Ball *ball, int player_id);
void draw_hoop(const Hoop *hoop, int player_id);
void run_game(Player *player1, Player *player2, Ball *ball1, Ball *ball2, Hoop *hoop1, Hoop *hoop2, GameState *game, int difficulty);

#endif // GAME_H