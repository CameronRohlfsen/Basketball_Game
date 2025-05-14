// game.c
#include "gfx.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

// Initialize a player
Player init_player(float x, float y) {
    Player player;
    player.x = x;
    player.y = y;
    player.score = 0;
    player.shots_made = 0;
    player.shots_attempted = 0;
    return player;
}

// Initialize a ball
Ball init_ball(float x, float y) {
    Ball ball;
    ball.x = x;
    ball.y = y;
    ball.dx = 0.0;
    ball.dy = 0.0;
    ball.in_motion = 0;
    return ball;
}

// Initialize a hoop
Hoop init_hoop(float x, float y, int width, int height, float dx) {
    Hoop hoop;
    hoop.x = x;
    hoop.y = y;
    hoop.width = width;
    hoop.height = height;
    hoop.dx = dx;
    return hoop;
}

// Initialize game state
GameState init_game_state() {
    GameState game;
    game.running = 1;
    return game;
}

// Draw the basketball court
void draw_basketball_court() {
    gfx_color(240, 210, 150); // Tan background
    gfx_clear();
    gfx_color(38, 60, 120); // Court lines

    int arc_width = 400;
    int arc_x = 400;
    int arc_y = 400;

    gfx_line(arc_x - arc_width / 2, arc_y, arc_x + arc_width / 2, arc_y);

    for (int angle = 0; angle <= 180; angle++) {
        double rad = angle * M_PI / 180.0;
        int x1 = arc_x + arc_width / 2 * cos(rad);
        int y1 = arc_y + arc_width / 2 * sin(rad);
        int x2 = arc_x + arc_width / 2 * cos(rad + M_PI / 180.0);
        int y2 = arc_y + arc_width / 2 * sin(rad + M_PI / 180.0);
        gfx_line(x1, y1, x2, y2);
    }

    int y_top = 150;
    gfx_line(arc_x - arc_width / 2, arc_y, arc_x - arc_width / 2, y_top);
    gfx_line(arc_x + arc_width / 2, arc_y, arc_x + arc_width / 2, y_top);
}

// Draw a hoop
void draw_hoop(const Hoop *hoop, int player_id) {
    gfx_color(255, 255, 255); // Backboard
    int w = hoop->width, h = 40;
    int x = hoop->x, y = hoop->y;

    int left = x - w / 2, top = y - h - 10;
    int right = x + w / 2, bottom = top + h;

    gfx_line(left, top, right, top);
    gfx_line(left, bottom, right, bottom);
    gfx_line(left, top, left, bottom);
    gfx_line(right, top, right, bottom);

    // Inner rectangle
    if (player_id == 1) gfx_color(255, 165, 0);
    else if (player_id == 2) gfx_color(0, 0, 255);

    int inner_w = w / 3, inner_h = h / 2;
    int i_left = x - inner_w / 2;
    int i_top = top + (h - inner_h) / 2;
    int i_right = x + inner_w / 2;
    int i_bottom = i_top + inner_h;

    gfx_line(i_left, i_top, i_right, i_top);
    gfx_line(i_left, i_bottom, i_right, i_bottom);
    gfx_line(i_left, i_top, i_left, i_bottom);
    gfx_line(i_right, i_top, i_right, i_bottom);

    // Hoop
    gfx_color(255, 0, 0);
    int hoop_top = y, hoop_bottom = y + 10;
    gfx_line(x - w / 2, hoop_top, x + w / 2, hoop_top);
    gfx_line(x - w / 2, hoop_top, x - w / 2, hoop_bottom);
    gfx_line(x + w / 2, hoop_top, x + w / 2, hoop_bottom);

    // Net
    gfx_color(255, 255, 255);
    int lines = 6;
    for (int i = 0; i <= lines; i++) {
        int xs = x - w / 2 + i * (w / lines);
        int ys = hoop_top;
        int xe = x - w / 2 + (i + 1) * (w / lines);
        int ye = hoop_bottom;
        if (i < lines) gfx_line(xs, ys, xe, ye);

        xs = x + w / 2 - i * (w / lines);
        xe = x + w / 2 - (i + 1) * (w / lines);
        if (i < lines) gfx_line(xs, ys, xe, ye);
    }
}

// Draw the ball
void draw_ball(const Ball *ball, int player_id) {
    if (player_id == 1) gfx_color(255, 165, 0);
    else if (player_id == 2) gfx_color(0, 0, 255);
    gfx_circle(ball->x, ball->y, 10);
}

// Handle ball motion and scoring
void handle_ball(Ball *ball, Player *player, Hoop *hoop) {
    if (!ball->in_motion) return;

    ball->x += ball->dx;
    ball->y += ball->dy;
    ball->dy = -10;

    if (ball->y <= hoop->y + hoop->height && ball->y >= hoop->y &&
        ball->x >= hoop->x - hoop->width / 2 && ball->x <= hoop->x + hoop->width / 2) {
        player->score++;
        player->shots_made++;
        gfx_color(0, 255, 0);
        gfx_text(350, 200, "NICE SHOT!");
        gfx_flush();
        usleep(1000000);

        ball->in_motion = 0;
        ball->x = player->x;
        ball->y = player->y - 10;
    } else if (ball->y <= 0) {
        player->shots_attempted++;  // count the miss if not already
        gfx_color(255, 0, 0);
        gfx_text(350, 200, "MISS!");
        gfx_flush();
        usleep(1000000);

        ball->in_motion = 0;
        ball->x = player->x;
        ball->y = player->y - 10;
    } else if (ball->y > 600) {
        ball->in_motion = 0;
        ball->x = player->x;
        ball->y = player->y - 10;
    }
}


// Move the hoop side to side
void move_hoop(Hoop *hoop) {
    hoop->x += hoop->dx;
    if (hoop->x - hoop->width / 2 < 0 || hoop->x + hoop->width / 2 > 800)
        hoop->dx = -hoop->dx;
}

// Main game loop
void run_game(Player *p1, Player *p2, Ball *b1, Ball *b2, Hoop *h1, Hoop *h2, GameState *g, int difficulty) {
    char input;
    struct timeval start, current;
    int hoop_speed, ball_speed;

    if (difficulty == 1) { hoop_speed = 2; ball_speed = -15; }
    else if (difficulty == 2) { hoop_speed = 5; ball_speed = -12; }
    else { hoop_speed = 8; ball_speed = -10; }

    h1->dx = h2->dx = hoop_speed;
    gettimeofday(&start, NULL);

    while (g->running) {
        gettimeofday(&current, NULL);

        gfx_clear_color(240, 210, 150);
        gfx_clear();
        draw_basketball_court();

        draw_hoop(h1, 1);
        draw_ball(b1, 1);

        if (p2->score >= 0) {
            draw_hoop(h2, 2);
            draw_ball(b2, 2);
        }

        // Player 1 score
        gfx_color(255, 255, 255);
        gfx_text(650, 30, "Player 1 Score:");
        char score1[10];
        sprintf(score1, "%d", p1->score);
        gfx_text(750, 30, score1);

        double p1_pct = p1->shots_attempted ? (100.0 * p1->shots_made / p1->shots_attempted) : 0.0;
        char p1_pct_text[20];
        sprintf(p1_pct_text, "Pct: %.1f%%", p1_pct);
        gfx_text(750, 50, p1_pct_text);

        // Player 2 score
        if (p2->score >= 0) {
            gfx_text(50, 30, "Player 2 Score:");
            char score2[10];
            sprintf(score2, "%d", p2->score);
            gfx_text(150, 30, score2);

            double p2_pct = p2->shots_attempted ? (100.0 * p2->shots_made / p2->shots_attempted) : 0.0;
            char p2_pct_text[20];
            sprintf(p2_pct_text, "Pct: %.1f%%", p2_pct);
            gfx_text(150, 50, p2_pct_text);
        }

        // Input handling
        if (gfx_event_waiting()) {
            input = gfx_wait();
            if (input == 'q') g->running = 0;
            else if (input == 'w') { b1->in_motion = 1; b1->dy = ball_speed; p1->shots_attempted++; }
            else if (input == 'i' && p2->score >= 0) { b2->in_motion = 1; b2->dy = ball_speed; p2->shots_attempted++; }
            else if (input == 'a') { p1->x -= 10; b1->x = p1->x; }
            else if (input == 'd') { p1->x += 10; b1->x = p1->x; }
            else if (input == 'j' && p2->score >= 0) { p2->x -= 10; b2->x = p2->x; }
            else if (input == 'l' && p2->score >= 0) { p2->x += 10; b2->x = p2->x; }
        }

        // Game updates
        move_hoop(h1);
        if (p2->score >= 0) move_hoop(h2);
        handle_ball(b1, p1, h1);
        if (p2->score >= 0) handle_ball(b2, p2, h2);

        // Feedback for Player 1
        if (b1->in_motion && b1->y <= h1->y && fabs(b1->x - h1->x) < h1->width / 2) {
            p1->shots_made++;
            p1->score++;
            gfx_color(0, 255, 0);
            gfx_text(350, 200, "NICE SHOT!");
            gfx_flush();
            usleep(1000000);
            b1->in_motion = 0;
            b1->x = p1->x;
            b1->y = p1->y - 10;
        } else if (b1->in_motion && b1->y <= 0) {
            gfx_color(255, 0, 0);
            gfx_text(350, 200, "MISS!");
            gfx_flush();
            usleep(1000000);
            b1->in_motion = 0;
            b1->x = p1->x;
            b1->y = p1->y - 10;
        }

        // Feedback for Player 2
        if (b2->in_motion && b2->y <= h2->y && fabs(b2->x - h2->x) < h2->width / 2) {
            p2->shots_made++;
            p2->score++;
            gfx_color(0, 255, 0);
            gfx_text(350, 200, "NICE SHOT!");
            gfx_flush();
            usleep(1000000);
            b2->in_motion = 0;
            b2->x = p2->x;
            b2->y = p2->y - 10;
        } else if (b2->in_motion && b2->y <= 0) {
            gfx_color(255, 0, 0);
            gfx_text(350, 200, "MISS!");
            gfx_flush();
            usleep(1000000);
            b2->in_motion = 0;
            b2->x = p2->x;
            b2->y = p2->y - 10;
        }

        gfx_flush();
    }
}
