//main.c
#include "gfx.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Initialize game elements
    Player player1 = init_player(400, 500);
    Player player2 = init_player(600, 500);

    Ball ball1 = init_ball(player1.x, player1.y - 10);
    Ball ball2 = init_ball(player2.x, player2.y - 10);

    Hoop hoop1 = init_hoop(400, 150, 100, 10, 0);
    Hoop hoop2 = init_hoop(600, 150, 100, 10, 0);

    GameState game = init_game_state();

    // Initialize graphics window
    gfx_open(800, 600, "Basketball Game");
    gfx_color(255, 255, 255);

    // Display main menu
    gfx_text(300, 150, "Welcome to Basketball Game!");
    gfx_text(250, 200, "Press 1 for Easy, 2 for Medium, 3 for Hard");
    gfx_text(250, 250, "Press S for Single Player or M for Multiplayer");
    gfx_text(250, 300, "Press 'q' at any time to quit.");

    char choice;
    int difficulty = 0;
    int single_player = 0;

    while (difficulty == 0) {
        if (gfx_event_waiting()) {
            choice = gfx_wait();
            if (choice == '1') difficulty = 1;
            else if (choice == '2') difficulty = 2;
            else if (choice == '3') difficulty = 3;
            else if (choice == 'q') exit(0);
            else if (choice == 's') single_player = 1;
            else if (choice == 'm') single_player = 0;
        }
    }

    // If single-player, remove Player 2 and Hoop 2
    if (single_player) {
        player2.score = -1; // Disable Player 2 in run_game
    }

    // Run the game loop
    run_game(&player1, &player2, &ball1, &ball2, &hoop1, &hoop2, &game, difficulty);

    // Display game over screen
    gfx_clear_color(0, 0, 0);
    gfx_clear();
    gfx_color(255, 255, 255);
    gfx_text(350, 200, "Game Over!");

    char final_score[100];
    if (single_player) {
        sprintf(final_score, "Final Score: %d", player1.score);
        gfx_text(300, 250, final_score);
    } else {
        sprintf(final_score, "Player 1: %d, Player 2: %d", player1.score, player2.score);
        gfx_text(300, 250, final_score);
    }

    gfx_text(300, 300, "Press 'q' to quit.");

    while (1) {
        if (gfx_event_waiting() && gfx_wait() == 'q') break;
    }

    return 0;
}