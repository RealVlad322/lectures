#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <algorithm>

using namespace std;

class Point {
public:
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& p) const {
        return x == p.x && y == p.y;
    }
};

class Input {
public:
    static int kbhit() {
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
};

class Snake {
private:
    deque<Point> body;
    char direction;
    
public:
    Snake(int x, int y) : direction('R') {
        body.push_back(Point(x, y));
    }
    
    void setDirection(char dir) {
        if (dir == 'U' && direction != 'D') direction = dir;
        if (dir == 'D' && direction != 'U') direction = dir;
        if (dir == 'L' && direction != 'R') direction = dir;
        if (dir == 'R' && direction != 'L') direction = dir;
    }
    
    void move() {
        Point head = body.front();
        
        if (direction == 'U') head.y--;
        if (direction == 'D') head.y++;
        if (direction == 'L') head.x--;
        if (direction == 'R') head.x++;
        
        body.push_front(head);
    }
    
    void removeTail() {
        body.pop_back();
    }
    
    void grow() {
        // Голова уже добавлена в move(), просто не удаляем хвост
    }
    
    Point getHead() const {
        return body.front();
    }
    
    bool checkSelfCollision() const {
        Point head = body.front();
        for (size_t i = 1; i < body.size(); i++) {
            if (head == body[i]) return true;
        }
        return false;
    }
    
    bool isOnPosition(const Point& p) const {
        return find(body.begin(), body.end(), p) != body.end();
    }
    
    int getLength() const {
        return body.size();
    }
    
    const deque<Point>& getBody() const {
        return body;
    }
};

class Field {
private:
    int width, height;
    Point food;
    
public:
    Field(int w, int h) : width(w), height(h) {
        srand(time(0));
        spawnFood();
    }
    
    void spawnFood() {
        food.x = rand() % width;
        food.y = rand() % height;
    }
    
    bool isOutOfBounds(const Point& p) const {
        return p.x < 0  p.x >= width  p.y < 0 || p.y >= height;
    }
    
    Point getFoodPosition() const {
        return food;
    }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

class Game {
private:
    Field field;
    Snake snake;
    bool gameOver;
    
public:
    Game(int width, int height) 
        : field(width, height), 
          snake(width/2, height/2), 
          gameOver(false) {}
    
    void handleInput() {
        int key = Input::kbhit();
        if (key == 'w') snake.setDirection('U');
        if (key == 's') snake.setDirection('D');
        if (key == 'a') snake.setDirection('L');
        if (key == 'd') snake.setDirection('R');
        if (key == 'q') gameOver = true;
    }
    
    void update() {
        snake.move();
        Point head = snake.getHead();
        
        if (field.isOutOfBounds(head) || snake.checkSelfCollision()) {
            gameOver = true;
            return;
        }
        
        if (head == field.getFoodPosition()) {
            snake.grow();
            field.spawnFood();
        } else {
            snake.removeTail();
        }
    }
    
    void render() {
        system("clear");
        
        for (int y = 0; y < field.getHeight(); y++) {
            for (int x = 0; x < field.getWidth(); x++) {
                Point p(x, y);
                if (p == snake.getHead()) cout << "O";
                else if (snake.isOnPosition(p)) cout << "o";
                else if (p == field.getFoodPosition()) cout << "*";
                else cout << ".";
            }
            cout << endl;
        }
        cout << "Score: " << snake.getLength() - 1 
             << " | w/a/s/d - move, q - quit" << endl;
    }
    
    bool isGameOver() const {
        return gameOver;
    }
    
    int getScore() const {
        return snake.getLength() - 1;
    }
    
    void run() {
        while (!isGameOver()) {
            render();
            handleInput();
            update();
            usleep(150000);
        }
        cout << "Game Over! Final Score: " << getScore() << endl;
    }
};

int main() {
    Game game(20, 10);
    game.run();
    return 0;
}
