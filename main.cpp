#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <vector>

#include "svg.h"  // Объявления классов библиотеки должны быть расположены в файле svg.h

using namespace std::literals;
using namespace svg;


namespace shapes {
class Star : public svg::Drawable {
 public:
  Star(svg::Point center, double outer_radius, double inner_radius,
       int num_rays)
      : center_(center),
        outer_radius_(outer_radius),
        inner_radius_(inner_radius),
        num_rays_(num_rays) {};

  void Draw(ObjectContainer& container) const override {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays_; ++i) {
      double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
      polyline.AddPoint({center_.x + outer_radius_ * sin(angle),
                         center_.y - outer_radius_ * cos(angle)});
      if (i == num_rays_) {
        break;
      }
      angle += M_PI / num_rays_;
      polyline.AddPoint({center_.x + inner_radius_ * sin(angle),
                         center_.y - inner_radius_ * cos(angle)});
    }
    container.Add(polyline);
  };

 private:
  svg::Point center_;
  double outer_radius_;
  double inner_radius_;
  int num_rays_;
};

class Triangle : public svg::Drawable {
 public:
  Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
      : p1_(p1), p2_(p2), p3_(p3) {}

  // Реализует метод Draw интерфейса svg::Drawable
  void Draw(svg::ObjectContainer& container) const override {
    container.Add(
        svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(
            p1_));
  }

 private:
  svg::Point p1_, p2_, p3_;
};

class Snowman : public Drawable {
 public:
  Snowman(Point head_center, double head_radius)
      : head_center_(head_center), head_radius_(head_radius) {}

  void Draw(svg::ObjectContainer& container) const override {
    container.Add(
        Circle()
            .SetCenter({head_center_.x, head_center_.y + head_radius_ * 5})
            .SetRadius(head_radius_ * 2.0));
    container.Add(
        Circle()
            .SetCenter({head_center_.x, head_center_.y + head_radius_ * 2})
            .SetRadius(head_radius_ * 1.5));
    container.Add(Circle().SetCenter(head_center_).SetRadius(head_radius_));
  }

 private:
  Point head_center_;
  double head_radius_;
};
};  // namespace shapes

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end,
                 svg::ObjectContainer& target) {
  for (auto it = begin; it != end; ++it) {
    (*it)->Draw(target);
  }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
  using namespace std;
  DrawPicture(begin(container), end(container), target);
}

int main() {
  using namespace svg;
  using namespace shapes;
  using namespace std;

  vector<unique_ptr<svg::Drawable>> picture;

  picture.emplace_back(
      make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
  // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом
  // 4
  picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
  // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
  picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

  svg::Document doc;
  // Так как документ реализует интерфейс ObjectContainer,
  // его можно передать в DrawPicture в качестве цели для рисования
  DrawPicture(picture, doc);

  // Выводим полученный документ в stdout
  doc.Render(cout);
}