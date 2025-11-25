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
