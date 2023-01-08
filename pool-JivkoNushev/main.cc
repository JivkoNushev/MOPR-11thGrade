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
    Line(Point p1, Point p2): p1(p1), p2(p2) {}
    Line(const Line& line): p1(line.p1), p2(line.p2) {}


    // взима симетричната отсечка на удадра от стената
    static Line findSymmetricalLine(Line line, Line through) 
    {
        // изразяваме правата чрез A, B, C 
        float a = through.p2.y - through.p1.y;
        float b = through.p1.x - through.p2.x;
        float c = a * through.p1.x + b * through.p1.y;

        // намираме симетричната точка на крайната точка на удара спрямо стената на масата
        Point symmetrical((b * b * line.p2.x - a * a * line.p2.x - 2 * a * b * line.p2.y - 2 * a * c) / (a * a + b * b), 
        (a * a * line.p2.y - b * b * line.p2.y - 2 * a * b * line.p2.x - 2 * b * c) / (a * a + b * b));

        // създаваме отсечка с начало точката на докосване и край симетричната точка
        Line bounce(line.p1, symmetrical);

        return bounce;
    }

    // взима точката, където се докосват две прави, ако се докосват 
    static bool intersect(Point line_p1, Point line_p2, Point segment_p1, Point segment_p2, float &x, float &y) 
    {
        // изразяваме правите чрез A, B, C 
        float a1 = line_p2.y - line_p1.y;
        float b1 = line_p1.x - line_p2.x;
        float c1 = line_p2.x * line_p1.y - line_p1.x * line_p2.y;

        float a2 = segment_p2.y - segment_p1.y;
        float b2 = segment_p1.x - segment_p2.x;
        float c2 = segment_p2.x * segment_p1.y - segment_p1.x * segment_p2.y;

        // изчисляваме детерминанта
        float det = a1 * b2 - a2 * b1;

        // ако детерминантата е 0 значи са успоредни
        if(0 == det)
            return false;

        // намираме къде се пресичат
        x = (b2 * c1 - b1 * c2) / det;
        y = (a1 * c2 - a2 * c1) / det;

        return true;
    }

    // връща дали една права съдържа точка
    bool containsPoint(Point p) 
    {
        float m = (p2.y - p1.y) / (p2.x - p1.x);
        float b = p1.y - m * p1.x;

        return p.y == m * p.x + b;
    }

    // взима големината на отсечката
    float getLength()
    {
        float a = p2.x - p1.x;
        float b = p2.y - p1.y;
        return sqrt(a*a + b*b);
    }

};

class Hit : public Line
{
public:
    float force = 1;

    Hit() {}
    Hit(Point p1, Point p2): Line(p1, p2) {}
    Hit(Point p1, Point p2, float force): Line(p1, p2), force(force)
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
    Ball(Point position, float diameter): position(position), diameter(diameter), startingPosition(position) {}

    Point getPosition() const
    {
        return position;
    }

    Point getStartingPosition() const
    {
        return startingPosition;
    }

    void setPosition(Point position)
    {
        this->position = position;
    }

    float getDiameter() const
    {
        return diameter;
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

public:
    //долу вляво, долу вдясно, горе вдясно, горе вляво
    Point points[4];

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
        float width = Line(p1, p2).getLength();
        float height = Line(p1, p4).getLength();

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

    void setBall(Ball ball)
    {
        this->ball = ball;
    }

    Ball& getBall()
    {
        return ball;
    }

    void restart()
    {
        ball.setPosition(ball.getStartingPosition());
    }

    void hit(Hit& h)
    { 
        // позицията от удара
        float new_x = ball.getPosition().x + h.force * (h.p2.x - h.p1.x);
        float new_y = ball.getPosition().y + h.force * (h.p2.y - h.p1.y);

        Point last_p(h.p2);
        Point p(new_x, new_y);

        // променена маса, за да не се правят проверки за топката, а да се сравнява винаги с радиус 0 
        Rectangle r(points[0], getWidth() - ball.getDiameter(), getHeight() - ball.getDiameter());

        // права на удара
        Line l(last_p, p);

        // проверява дали е вкарана топката в някоя от дупките
        for(int i = 0; i < 4; i++)
        {
            if(l.containsPoint(r.points[i]))
            {
                restart();
                return;
            }
        }

        // стените на масата
        Line lines[4] = {Line(r.points[0], r.points[1]), Line(r.points[0], r.points[4]), Line(r.points[1], r.points[3]), Line(r.points[4], r.points[3])}; 

        Point intersection;
        while(!r.contains(p))
        {
            float x = 0, y = 0;

            // проверяваме всяка страна дали е ударена от топката
            for(int i = 0; i < 4; i++)
            {
                if(Line::intersect(last_p, p, lines[i].p1, lines[i].p2, x, y))
                {
                    // като намерим удар намираме симетричната права и отклоняваме посоката на топката
                    l = Line::findSymmetricalLine(Line(intersection, p), lines[i]);
                    break;
                }
            }

            // проверява дали е вкарана топката в някоя от дупките
            for(int i = 0; i < 4; i++)
            {
                if(l.containsPoint(r.points[i]))
                {
                    restart();
                    return;
                }
            }
            // променяме новите точки на удара
            Point last_p(intersection);
            p = (l.p2);
        }

        // задаваме новата позиция на топката
        ball.setPosition(p);
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
    while(true)
    {
        try
        {
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
                return;
            case 2:
                float x1, y1, x2, y2, x3, y3, x4, y4;
                cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
                board = Board(Point(x1, y1), Point(x2, y2), Point(x3, y3), Point(x4, y4), ball);
                return;
            default:
                cout << "Invalid command\n";
            }
        }
        catch(Exception e)
        {
            e.print();
        }
    }
}

void changeBall(Board& board)
{
    system("clear");
    while(true)
    {
        try
        {
            cout << "Enter ball position and diameter\n";
            float x, y, diameter;
            cin >> x >> y >> diameter;
            Ball ball(Point(x,y), diameter);

            board.setBall(ball);
            return;
        }
        catch(Exception e)
        {
            e.print();
        }
    }
}

void changeBoard(Board& board)
{
    system("clear");
    int commandNumber = 0;
    while(true)
    {
        cout << "1. Simple\n2. Complex\n";
        cin >> commandNumber;

        try
        {
            switch (commandNumber)
            {
            case 1:
                float x, y, width, height;
                cin >> x >> y >> width >> height;
                board = Board(Point(x, y), width, height, board.getBall());
                return;
            case 2:
                float x1, y1, x2, y2, x3, y3, x4, y4;
                cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
                board = Board(Point(x1, y1), Point(x2, y2), Point(x3, y3), Point(x4, y4), board.getBall());
                return;
            default:
                cout << "invalid command\n";
            }
        }
        catch(Exception e)
        {
            e.print();
        }
    }
    
}

void change(Board &board)
{
    system("clear");
    while(true)
    {
        cout << "1. Board\n2. Ball\n";
        int command = 0;
        cin >> command;

        switch(command)
        {
            case 1:
                changeBoard(board);
                return;
            case 2:
                changeBall(board);
                return;
            default:
                cout << "Wrong command\n";
        }
    }
}

void initHit(Board& board)
{
    while (true)
    {
        try
        {
            float x, y, size;
            cin >> x >> y >> size;
            Hit h(board.getBall().getPosition(), Point(x, y), size);
            board.hit(h);
            return;
        }
        catch(Exception e)
        {
            e.print();
        }
    }
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
    cout << "1. Initialize\n2. Change\n3. Info\n4. Hit\n5. Exit\n";
}

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
                change(board);
                break;
            case 3:
                printInfo(board);
                break;
            case 4:
                initHit(board);
                break;
            case 5:
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
