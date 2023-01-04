#include <iostream>
#include <cmath>

using namespace std;

class Exception
{
    string message;

public:
    Exception(): message("Empty exception") {}
    Exception(const char* message): message(message) {}

    void print() const
    {
        cout << message << "\n";
    }
};

class Point
{

public:
    float x = 0, y = 0;
    Point() {}
    Point(float x, float y): x(x), y(y) {} 

    friend ostream& operator <<(ostream& os, const Point& p);
};

ostream& operator<<(ostream& os, const Point& p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

class Vector2D
{

protected:
    Point p1, p2;

public:
    Vector2D(){}
    Vector2D(Point p1, Point p2): p1(p1), p2(p2){}

    float getLength()
    {
        float a = p2.x - p1.x;
        float b = p2.y - p2.y;

        return sqrt(a*a + b*b);
    }
};

class Hit : public Vector2D
{
    //FIXME:
    float force = 1;

public:
    Hit() {}
    Hit(Point p1, Point p2): Vector2D(p1, p2) {}
    Hit(Point p1, Point p2, float force): Vector2D(p1, p2), force(force)
    {
        if(force < 1 || 10 < force)
        {
            throw Exception("Invalid force value");
        }
    }

};

class Ball
{
    Point position;
    float diameter = 0;

public:
    Ball(){}
    Ball(Point position , float diameter): position(position), diameter(diameter) {}

    Point getPosition() const
    {
        return position;
    }

    float getDiameter() const
    {
        return diameter;
    }

    void hit(Hit h)
    {

    }

    friend ostream& operator <<(ostream& os, const Ball& b);
};

ostream& operator<<(ostream& os, const Ball& b)
{
    os << "Ball Center: " << b.getPosition() << "\nBall diameter: " << b.getDiameter() << "\n";
    return os;
}

class Rectangle
{

protected:
    //долу вляво, долу вдясно, горе вдясно, горе вляво
    Point points[4];

public:
    Rectangle() {}
    Rectangle(Point p1, Point p2, Point p3, Point p4): points{p1,p2,p3,p4} {}
    Rectangle(Point p1, float width, float height):
    points{p1, Point(p1.x + width, p1.y), Point(p1.x + width, p1.y + height), Point(p1.x, p1.y + height)} {}

    bool contains(Point p)
    {
        return points[0].x <= p.x && p.x <= points[2].x && points[0].y <= p.y && p.y <= points[2].y;
    }
};

class EmptyBoard : public Rectangle
{
    
public:
    EmptyBoard() {}
    EmptyBoard(Point p1, Point p2, Point p3, Point p4): Rectangle(p1,p2,p3,p4)
    {
        float width = Vector2D(p1, p2).getLength();
        float height = Vector2D(p1, p4).getLength();

        // 1/2        
        if(width * 2 != height && height * 2 != width)
        {
            throw Exception("Invalid board coordinates");
        }
    } 

    EmptyBoard(Point p1, float width, float height): Rectangle(p1, width, height)
    {
        // 1/2  
        if(width * 2 != height && height * 2 != width)
        {
            throw Exception("Invalid board coordinates");
        }  
    } 
};

class Board : public EmptyBoard
{
    Ball ball;

public:

    Board() {}
    Board(Point p1, Point p2, Point p3, Point p4, Ball ball) : EmptyBoard(p1, p2, p3, p4), ball(ball)
    {
        // FIXME: ball is thicker
        if(!contains(ball.getPosition()))
        {
            throw Exception("Invalid ball position");
        }
    }
    Board(Point p1, float width, float height, Ball ball) : EmptyBoard(p1, width, height), ball(ball)
    {
        // FIXME: ball is thicker
        if(!contains(ball.getPosition()))
        {
            throw Exception("Invalid ball position");
        }
    }

    Ball getBall()
    {
        return ball;
    }

    friend ostream& operator <<(ostream& os, const Board& b);
};

ostream& operator<<(ostream& os, const Board& b)
{
    os << "Field points:\n";
    for (size_t i = 0; i < 4; i++)
    {
        os << b.points[i] << "\n";
    }
    os << b.ball;
    return os;
}

void initBoard(Board& board)
{
    system("clear");
    cout << "Enter ball position and diameter\n";
    float x, y, diameter;
    cin >> x >> y >> diameter;
    Ball ball(Point(x,y), diameter);

    int commandNumber = 0;
    cout << "1. Simple\n2. Complex\n";
    cin >> commandNumber;

    switch (commandNumber)
    {
    case 1:
        float x, y, width, height;
        cin >> x >> y >> width >> height;
        board = Board(Point(x, y), width, height, ball);
        break;
    case 2:
        float x1, y1, x2, y2, x3, y3, x4, y4;
        cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
        board = Board(Point(x1, y1), Point(x2, y2), Point(x3, y3), Point(x4, y4), ball);
        break;
    default:
        throw Exception("Invalid command");
    }
}

void initHit(Hit& hit)
{
    
}

void printMainMenu()
{
    system("clear");
    cout << "1. Initialize\n2. Change\n3. Info\n4. Hit\n5. Commands\n6. Exit\n";
}


// sled kato izleze gledash simetralnoto
// za po-golqmi topcheta namalqsh poleto s radiusa che da e tochka
int main(int argc, char const *argv[])
{
    bool running = true;
    int commandNumber = 0;
    
    try
    {
        Board board;
        Hit hit;   
        char c; 

        while(running)
        {
            printMainMenu();
            cin >> commandNumber;

            switch (commandNumber)
            {
            case 1:
                initBoard(board);
                break;
            case 2:
                /* code */
                break;
            case 3:
                system("clear");
                cout << board;

                cin.get(c);
                break;
            case 4:
                initHit(hit);
                break;
            case 5:
                /* code */
                break;
            case 6:
                running = false;
                break;
            
            default:
                throw Exception("Invalid command");
            }
        }
    }
    catch(const Exception& e)
    {
        e.print();
    }

    return 0;
}
