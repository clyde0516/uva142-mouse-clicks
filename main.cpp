#include <assert.h>
#include <iostream>
#include <sstream>
#include <map>
#include <boost/lexical_cast.hpp>

#ifndef ONLINE_JUDGE
#include <gmock/gmock.h>
#endif


class Point
{
public:
    Point(int x, int y);

public:
    int x() const;
    int y() const;
    double distance(const Point & point) const;

private:
    int x_;
    int y_;
};

class Region
{
public:
    Region(int left, int top, int right, int bottom);

public:
    bool contains(const Point & point) const;

private:
    int left_;
    int top_;
    int right_;
    int bottom_;
};

struct screen_t
{
    std::vector<Region> regions;
    std::vector<Point> icons;
};


Point::Point(int x, int y)
    : x_(x)
    , y_(y)
{
}

int Point::x() const
{
    return x_;
}

int Point::y() const
{
    return y_;
}

double Point::distance(const Point & point) const
{
    return sqrt(pow(x() - point.x(), 2) + pow(y() - point.y(), 2));
}

Region::Region(int left, int top, int right, int bottom)
    : left_(left)
    , top_(top)
    , right_(right)
    , bottom_(bottom)
{
    BOOST_ASSERT(left < right);
    BOOST_ASSERT(top < bottom);
}

bool Region::contains(const Point & point) const
{
    return left_ <= point.x() && point.x() <= right_ &&
           top_ <= point.y() && point.y() <= bottom_;
}

Region read_region(std::istream & is)
{
    int left, top, right, bottom;
    is >> left >> top >> right >> bottom;
    return Region(left, top, right, bottom);
}

Point read_point(std::istream & is)
{
    int x, y;
    is >> x >> y;
    return Point(x, y);
}

void select_by_click(const screen_t screen, const Point & click, std::ostream & os)
{
    for (int i = screen.regions.size() - 1; i >= 0; --i)
    {
        if (screen.regions[i].contains(click))
        {
            os << char('A' + i) << std::endl;
            return;
        }
    }

    double shortest_distance = std::numeric_limits<double>().max();
    std::vector<size_t> icon_index;
    for (size_t i = 0; i < screen.icons.size(); ++i)
    {
        double distance = screen.icons[i].distance(click);
        if (distance < shortest_distance)
        {
            shortest_distance = distance;
            icon_index.clear();
            icon_index.push_back(i);
        }
        else if (distance == shortest_distance)
        {
            icon_index.push_back(i);
        }
    }

    for (auto index : icon_index)
    {
        os << std::setfill(' ') << std::setw(3) << (index + 1);
    }
    os << std::endl;
}

void mouse_clicks(std::istream & is, std::ostream & os)
{
    screen_t screen;

    while (true)
    {
        char type;
        is >> type;
        
        switch (type)
        {
        case 'R':
            screen.regions.push_back(read_region(is));
            break;
        case 'I':
            screen.icons.push_back(read_point(is));
            break;
        case 'M':
            select_by_click(screen, read_point(is), os);
            break;
        default:
            BOOST_ASSERT(type == '#');
            return;
            break;
        }
    }
}

int main(int argc, char ** argv)
{
#ifdef ONLINE_JUDGE
    mouse_clicks(std::cin, std::cout);
#else
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
#endif
}

#ifndef ONLINE_JUDGE
TEST(MouseClicks, Sample)
{
    std::istringstream iss(
        "I 216 28\n"
        "R 22 19 170 102\n"
        "I 40 150\n"
        "I 96 138\n"
        "I 36 193\n"
        "R 305 13 425 103\n"
        "I 191 184\n"
        "I 387 200\n"
        "R 266 63 370 140\n"
        "I 419 134\n"
        "I 170 102\n"
        "M 50 50\n"
        "M 236 30\n"
        "M 403 167\n"
        "M 330 83\n"
        "#\n"
    );
    std::ostringstream oss;

    mouse_clicks(iss, oss);
    EXPECT_EQ(
        "A\n"
        "  1\n"
        "  6  7\n"
        "C\n"
        , oss.str());
}
#endif
