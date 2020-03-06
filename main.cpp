#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/lexical_cast.hpp>

//#define ONLINE_JUDGE

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

class Rect
{
public:
    Rect(const Point & top_left, const Point & bottom_right);

public:
    bool contains(const Point & point) const;

private:
    Point top_left_;
    Point bottom_right_;
};

struct screen_t
{
    std::vector<Rect> regions;
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

Rect::Rect(const Point & top_left, const Point & bottom_right)
    : top_left_(top_left)
    , bottom_right_(bottom_right)
{
    BOOST_ASSERT(top_left_.x() < bottom_right_.x());
    BOOST_ASSERT(top_left_.y() < bottom_right_.y());
}

bool Rect::contains(const Point & point) const
{
    return top_left_.x() <= point.x() && point.x() <= bottom_right_.x() &&
           top_left_.y() <= point.y() && point.y() <= bottom_right_.y();
}

Point read_point(std::istream & is)
{
    int x, y;
    is >> x >> y;
    return Point(x, y);
}

Rect read_region(std::istream & is)
{
    auto left_top = read_point(is);
    auto bottom_right = read_point(is);
    return Rect(left_top, bottom_right);
}

void add_region(const Rect & region, screen_t & screen)
{
    for (int i = screen.icons.size() - 1; i >= 0; --i)
    {
        if (region.contains(screen.icons[i]))
        {
            screen.icons.erase(screen.icons.begin() + i);
        }
    }

    screen.regions.push_back(region);
}

void add_icon(const Point & icon, screen_t & screen)
{
    for (auto & region : screen.regions)
    {
        if (region.contains(icon))
        {
            return;
        }
    }

    screen.icons.push_back(icon);
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
            add_region(read_region(is), screen);
            break;
        case 'I':
            add_icon(read_point(is), screen);
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

TEST(MouseClicks, uDebug)
{
    std::istringstream iss(
        "I 0 0\n"
        "I 0 100\n"
        "I 100 0\n"
        "R 80 80 120 120\n"
        "I 100 100\n"
        "M 50 50\n"
        "#\n"
    );
    std::ostringstream oss;

    mouse_clicks(iss, oss);
    EXPECT_EQ(
        "  1  2  3\n"
        , oss.str());
}
#endif
