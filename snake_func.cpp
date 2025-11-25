#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 10;

struct Point {
    int x, y;
    bool operator==(const Point& p) const {
        return x == p.x && y == p.y;
    }
};

deque<Point> snake;
Point food;
char direction = 'R';
bool gameOver = false;

int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return ch;
}

void spawnFood() {
    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
}

void init() {
    srand(time(0));
    snake.push_back({WIDTH/2, HEIGHT/2});
    spawnFood();
}

void input() {
    int key = kbhit();
    if (key == 'w' && direction != 'D') direction = 'U';
    if (key == 's' && direction != 'U') direction = 'D';
    if (key == 'a' && direction != 'R') direction = 'L';
    if (key == 'd' && direction != 'L') direction = 'R';
    if (key == 'q') gameOver = true;
}

void logic() {
    Point head = snake.front();
    
    if (direction == 'U') head.y--;
    if (direction == 'D') head.y++;
    if (direction == 'L') head.x--;
    if (direction == 'R') head.x++;
    
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
        gameOver = true;
        return;
    }
    
    for (auto& seg : snake) {
        if (head == seg) {
            gameOver = true;
            return;
        }
    }
    
    snake.push_front(head);
    
    if (head == food) {
        spawnFood();
    } else {
        snake.pop_back();
    }
}

void draw() {
    system("clear");
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Point p = {x, y};
            if (p == snake.front()) cout << "O";
            else if (find(snake.begin(), snake.end(), p) != snake.end()) cout << "o";
            else if (p == food) cout << "*";
            else cout << ".";
        }
        cout << endl;
    }
    cout << "Score: " << snake.size() - 1 << " | w/a/s/d - move, q - quit" << endl;
}

int main() {
    init();
    
    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(150000);
    }
    
    cout << "Game Over! Final Score: " << snake.size() - 1 << endl;
    return 0;
}
