#define _USE_MATH_DEFINES
#include "svg.h" // Объявления классов библиотеки должны быть расположены в файле svg.h

#include <cmath>
#include <sstream>

using namespace std::literals;
using namespace svg;

namespace shapes
{
    class Star : public svg::Drawable
    {
    public:
        Star(svg::Point center, double outer_radius, double inner_radius, int num_rays) : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays) {};

        void Draw(ObjectContainer &container) const override
        {
            using namespace svg;
            Polyline polyline;
            for (int i = 0; i <= num_rays_; ++i)
            {
                double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
                polyline.AddPoint({center_.x + outer_radius_ * sin(angle), center_.y - outer_radius_ * cos(angle)});
                if (i == num_rays_)
                {
                    break;
                }
                angle += M_PI / num_rays_;
                polyline.AddPoint({center_.x + inner_radius_ * sin(angle), center_.y - inner_radius_ * cos(angle)});
            }
            container.Add(polyline);
        };

    private:
        svg::Point center_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;
    };

    class Triangle : public svg::Drawable
    {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1), p2_(p2), p3_(p3)
        {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer &container) const override
        {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }

    private:
        svg::Point p1_, p2_, p3_;
    };

};

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays)
{
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i)
    {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays)
        {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}

int main()
{
    Document doc;
    doc.Add(Circle().SetCenter({20, 20}).SetRadius(10));
    doc.Add(Text()
                .SetFontFamily("Verdana"s)
                .SetPosition({35, 20})
                .SetOffset({0, 6})
                .SetFontSize(12)
                .SetFontWeight("bold"s)
                .SetData("Hello C++"s));
    doc.Add(CreateStar({20, 50}, 10, 5, 5));
    doc.Render(std::cout);
}