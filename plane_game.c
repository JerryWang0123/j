#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch() on Windows
#include <windows.h> // For Sleep
#include <ctype.h> // For toupper

#define WIDTH 9
#define HEIGHT 24
#define PLANE_CHAR '^'
#define ROCK_CHAR 'O'
#define BULLET_CHAR '|'

typedef struct {
    int x, y;
} Entity;

Entity plane;
Entity* rocks;
Entity* bullets;
int num_rocks = 0;
int num_bullets = 0;
int max_rocks = 10;
int max_bullets = 10;
int score = 0;
int high_score = 0;

void cls() {
    system("cls");
}

void locate(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void hidecursor() {
    printf("\033[?25l");
}

void reset_game() {
    srand(time(NULL));
    plane.x = 3;
    plane.y = HEIGHT - 2;
    rocks = (Entity*)malloc(max_rocks * sizeof(Entity));
    bullets = (Entity*)malloc(max_bullets * sizeof(Entity));
    num_rocks = 0;
    num_bullets = 0;
    score = 5;
}

void draw() {
    cls(); // Clear screen using rlutil
    // Draw plane
    locate(plane.x, plane.y);
    printf("%c", PLANE_CHAR);
    // Draw rocks
    for (int i = 0; i < num_rocks; i++) {
        locate(rocks[i].x, rocks[i].y);
        printf("%c", ROCK_CHAR);
    }
    // Draw bullets
    for (int i = 0; i < num_bullets; i++) {
        locate(bullets[i].x, bullets[i].y);
        printf("%c", BULLET_CHAR);
        if (bullets[i].y + 1 <= HEIGHT) {
            locate(bullets[i].x, bullets[i].y + 1);
            printf("%c", BULLET_CHAR);
        }
    }
    // Draw score
    locate(1, 1);
    printf("Score: %d", score);
}

void update() {
    static int rock_frame = 0;
    rock_frame++;
    int rock_speed = 2 - score / 20;
    if (rock_speed < 1) rock_speed = 1;
    // Move rocks down
    if (rock_frame % rock_speed == 0) {
        for (int i = 0; i < num_rocks; i++) {
            rocks[i].y++;
            if (rocks[i].y >= HEIGHT) {
                // Remove rock and deduct 2 points
                for (int j = i; j < num_rocks - 1; j++) {
                    rocks[j] = rocks[j + 1];
                }
                num_rocks--;
                score -= 2;
                if (score < 0) score = 0;
                i--;
            }
        }
    }
    // Move bullets up
    for (int i = 0; i < num_bullets; i++) {
        bullets[i].y--;
        if (bullets[i].y <= 0) {
            // Remove bullet
            for (int j = i; j < num_bullets - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            num_bullets--;
            i--;
        }
    }
    // Check collisions
    // Bullet vs Rock
    for (int i = 0; i < num_bullets; i++) {
        for (int j = 0; j < num_rocks; j++) {
            if (bullets[i].x == rocks[j].x && (bullets[i].y == rocks[j].y || (bullets[i].y + 1 == rocks[j].y && bullets[i].y + 1 <= HEIGHT))) {
                // Remove both
                for (int k = i; k < num_bullets - 1; k++) {
                    bullets[k] = bullets[k + 1];
                }
                num_bullets--;
                for (int k = j; k < num_rocks - 1; k++) {
                    rocks[k] = rocks[k + 1];
                }
                num_rocks--;
                score += 4;
                i--; // Adjust index
                break;
            }
        }
    }
    // Rock vs Plane
    for (int i = 0; i < num_rocks; i++) {
        if (rocks[i].x == plane.x && rocks[i].y == plane.y) {
            // Game over
            if (score > high_score) high_score = score;
            locate(WIDTH / 2 - 5, HEIGHT / 2);
            printf("Game Over! High Score: %d", high_score);
            locate(WIDTH / 2 - 5, HEIGHT / 2 + 1);
            printf("Press R to restart");
            // Wait for R to restart
            while (1) {
                if (_kbhit()) {
                    char key = _getch();
                    if (toupper(key) == 'R') {
                        reset_game();
                        break;
                    }
                }
                Sleep(10); // Small delay
            }
        }
    }
    // Add new rock randomly
    if (rand() % 10 == 0 && num_rocks < max_rocks) {
        rocks[num_rocks].x = ((rand() % 5) * 2) + 1;
        rocks[num_rocks].y = 1;
        num_rocks++;
    }
    // Check if score <= 0
    if (score <= 0) {
        if (score > high_score) high_score = score; // 0
        locate(WIDTH / 2 - 5, HEIGHT / 2);
        printf("Game Over! High Score: %d", high_score);
        locate(WIDTH / 2 - 5, HEIGHT / 2 + 1);
        printf("Press R to restart");
        // Wait for R to restart
        while (1) {
            if (_kbhit()) {
                char key = _getch();
                if (toupper(key) == 'R') {
                    reset_game();
                    break;
                }
            }
            Sleep(10); // Small delay
        }
    }
}

void handle_input() {
    if (_kbhit()) {
        char key = _getch();
        if (key == 'a' || key == 'A') {
            if (plane.x > 1) plane.x -= 2;
        } else if (key == 'd' || key == 'D') {
            if (plane.x < 9) plane.x += 2;
        } else if (key == 'r' || key == 'R') {
            reset_game();
        } else if (key == ' ') {
            // Shoot bullet
            if (score > 0) {
                score--;
                if (num_bullets < max_bullets) {
                    bullets[num_bullets].x = plane.x;
                    bullets[num_bullets].y = plane.y - 1;
                    num_bullets++;
                }
            }
        }
    }
}

void reset_game();

int main() {
    printf("請切換到英文輸入法，按任意鍵開始遊戲...\n");
    _getch();
    reset_game();
    hidecursor(); // Hide cursor using rlutil
    while (1) {
        handle_input();
        update();
        draw();
        Sleep(50); // Delay for game speed, Windows specific
    }
    free(rocks);
    free(bullets);
    return 0;
}