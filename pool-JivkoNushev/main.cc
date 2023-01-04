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
    Point(const Point& p): x(p.x), y(p.y) {} 

    friend ostream& operator <<(ostream& os, const Point& p);
};

ostream& operator<<(ostream& os, const Point& p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

class Line
{
public:
    Point p1, p2;

    Line(){}
    Line(Point p1, Point p2): p1(p1), p2(p2){}
    Line(const Line& line): p1(line.p1), p2(line.p2){}

};

class Vector2D
{

public:
    Point p1, p2;

    Vector2D(){}
    Vector2D(Point p1, Point p2): p1(p1), p2(p2){}

    float getLength()
    {
        float a = p2.x - p1.x;
        float b = p2.y - p1.y;

        return sqrt(a*a + b*b);
    }

    static Point IntersectionPoint(Point line_p1, Point line_p2, Point segment_p1, Point segment_p2) 
    {
        // Calculate the coefficients of the line equation.
        float a = line_p2.y - line_p1.y;
        float b = line_p1.x - line_p2.x;
        float c = line_p2.x * line_p1.y - line_p1.x * line_p2.y;

        // Calculate the intersection point.
        Point intersection_p;
        intersection_p.x = (b * (segment_p1.y - segment_p2.y) - a * (segment_p1.x - segment_p2.x)) / (2 * a * (segment_p1.y - segment_p2.y) - 2 * b * (segment_p1.x - segment_p2.x));
        intersection_p.y = (a * (segment_p1.x - segment_p2.x) - b * (segment_p1.y - segment_p2.y)) / (2 * a * (segment_p1.x - segment_p2.x) - 2 * b * (segment_p1.y - segment_p2.y));
        return intersection_p;
    }

};

class Hit : public Vector2D
{
public:
    float force = 1;

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
    Point startingPosition;
    float diameter = 0;

public:
    Ball(){}
    Ball(Point position , float diameter): position(position), diameter(diameter), startingPosition(position) {}

    Point getPosition() const
    {
        return position;
    }

    void setPosition(Point position)
    {
        this->position = position;
    }

    float getDiameter() const
    {
        return diameter;
    }

    void hit(Hit& h)
    {
        float x = h.force * (h.p2.x - h.p1.x);
        float y = h.force * (h.p2.y - h.p1.y);

        position = Point(x + position.x, y + position.y);
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

    float getWidth()
    {
        float a = points[1].x - points[0].x;  
        float b = points[1].y - points[0].y;  

        return sqrt(a * a + b * b);
    }

    float getHeight()
    {
        float a = points[4].x - points[0].x;  
        float b = points[4].y - points[0].y;  

        return sqrt(a * a + b * b);     
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

        if(width * 2 != height && height * 2 != width)
        {
            throw Exception("Invalid board coordinates");
        }
    } 

    EmptyBoard(Point p1, float width, float height): Rectangle(p1, width, height)
    {
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
        Rectangle r(points[0], getWidth() - ball.getDiameter(), getHeight() - ball.getDiameter());
        if(!r.contains(ball.getPosition()))
        {
            throw Exception("Invalid ball position");
        }
    }
    Board(Point p1, float width, float height, Ball ball) : EmptyBoard(p1, width, height), ball(ball)
    {
        Rectangle r(points[0], getWidth() - ball.getDiameter(), getHeight() - ball.getDiameter());
        if(!r.contains(ball.getPosition()))
        {
            throw Exception("Invalid ball position");
        }
    }

    Ball& getBall()
    {
        return ball;
    }

    void hit(Hit& h)
    {
        float new_x = ball.getPosition().x + h.force * (h.p2.x - h.p1.x);
        float new_y = ball.getPosition().y + h.force * (h.p2.y - h.p1.y);

        Point p(new_x, new_y);
        Point last_p(h.p2);
        Rectangle r(points[0], getWidth() - ball.getDiameter(), getHeight() - ball.getDiameter());

        // TODO: check corners
        Line lines[4] = {Line(points[0], points[1]), Line(points[0], points[4]), Line(points[1], points[3]), Line(points[4], points[3])}; 
        Line l;
        while(!r.contains(p))
        {

            // try to find intersecton with all sides and 
            //if(lines[0].p1.x < p.x && p.x < lines[0].p2.x && p.y <= lines)
            Point intersection = Vector2D::IntersectionPoint();

            // TODO: check corners

            Point p(new_x, new_y);
            Point last_p(Point(new_x, new_y));
        }


        ball.setPosition(Point(new_x, new_y));
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

void initHit(Board& board)
{
    float x, y, size;
    cin >> x >> y >> size;
    Hit h(board.getBall().getPosition(), Point(x, y), size);
    board.hit(h);
}

void printInfo(Board& board)
{
    char c; 
    system("clear");
    cout << board;
    cout << "Press 1 + Enter to Exit\n";
    cin >> c;
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
                printInfo(board);
                break;
            case 4:
                initHit(board);
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
