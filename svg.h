#pragma once

#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <optional>
#include "algorithm"

namespace svg
{

  using namespace std::literals;

  using Color = std::string;

  inline const Color NoneColor{"none"};

  enum class StrokeLineCap
  {
    BUTT,
    ROUND,
    SQUARE,
  };

  enum class StrokeLineJoin
  {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
  };

  std::ostream &operator<<(std::ostream &os, StrokeLineCap line_cap)
  {
    switch (line_cap)
    {
    case StrokeLineCap::BUTT:
      os << "butt"s;
      break;
    case StrokeLineCap::ROUND:
      os << "round"s;
      break;
    case StrokeLineCap::SQUARE:
      os << "square"s;
      break;
    }
    return os;
  }

  std::ostream &operator<<(std::ostream &os, StrokeLineJoin line_join)
  {
    switch (line_join)
    {
    case StrokeLineJoin::ARCS:
      os << "arcs"s;
      break;
    case StrokeLineJoin::BEVEL:
      os << "bevel"s;
      break;
    case StrokeLineJoin::MITER:
      os << "miter"s;
      break;
    case StrokeLineJoin::MITER_CLIP:
      os << "miter-clip"s;
      break;
    case StrokeLineJoin::ROUND:
      os << "round"s;
      break;
    }
    return os;
  }

  struct Point
  {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
    double x = 0;
    double y = 0;
  };

  /*
   * Вспомогательная структура, хранящая контекст для вывода SVG-документа с
   * отступами. Хранит ссылку на поток вывода, текущее значение и шаг отступа при
   * выводе элемента
   */
  struct RenderContext
  {
    RenderContext(std::ostream &out) : out(out) {}

    RenderContext(std::ostream &out, int indent_step, int indent = 0)
        : out(out), indent_step(indent_step), indent(indent) {}

    RenderContext Indented() const
    {
      return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const
    {
      for (int i = 0; i < indent; ++i)
      {
        out.put(' ');
      }
    }

    std::ostream &out;
    int indent_step = 0;
    int indent = 0;
  };

  /*
   * Абстрактный базовый класс Object служит для унифицированного хранения
   * конкретных тегов SVG-документа
   * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
   */
  class Object
  {
  public:
    void Render(const RenderContext &context) const;

    virtual ~Object() = default;

  protected:
    virtual void RenderObject(const RenderContext &context) const = 0;
  };

  template <typename Owner>
  class PathProps
  {

  public:
    void SetFillColor(Color color)
    {
      fill_color_ = color;
      return AsOwner();
    }
    void SetStrokeColor(Color color)
    {
      stroke_color_ = color;
      return AsOwner();
    }
    void SetStrokeWidth(double width)
    {
      stroke_width_ = width;
      return AsOwner();
    }
    void SetStrokeLinecap(StrokeLineCap line_cap)
    {
      line_cap_ = line_cap;
      return AsOwner();
    }
    void SetStrokeLineJoin(StrokeLineJoin line_join)
    {
      line_join_ = line_join;
      return AsOwner();
    }

  protected:
    PathProps() = default;
    ~PathProps() = default;

    void RenderAttributes(std::ostream &out) const
    {
      if (fill_color_)
      {
        out << " fill=\""sv << *fill_color_ << "\""sv;
      }
      if (stroke_color_)
      {
        out << " stroke=\""sv << *stroke_color_ << "\""sv;
      }
      if (stroke_width_)
      {
        out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
      }
      if (line_cap_)
      {
        out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
      }
      if (line_join_)
      {
        out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
      }
    }

  private:
    Owner &AsOwner()
    {
      return static_cast<Owner &>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
  };

  /*
   * Класс Circle моделирует элемент <circle> для отображения круга
   * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
   */
  class Circle final : public Object
  {
  public:
    Circle &SetCenter(Point center);
    Circle &SetRadius(double radius);

  private:
    void RenderObject(const RenderContext &context) const override;

    Point center_ = {0.0, 0.0};
    double radius_ = 1.0;
  };

  /*
   * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
   * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
   */
  class Polyline final : public Object
  {
  public:
    // Добавляет очередную вершину к ломаной линии
    Polyline &AddPoint(Point point);

  private:
    void RenderObject(const RenderContext &context) const override;
    std::deque<Point> points_;
  };

  /*
   * Класс Text моделирует элемент <text> для отображения текста
   * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
   */
  class Text final : public Object
  {
  public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text &SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text &SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text &SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text &SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text &SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text &SetData(std::string data);

  private:
    void RenderObject(const RenderContext &context) const override;

    void ScreenCharacter(std::string &str, char c, std::string_view value);

    void ScreenData(std::string &str);

    Point position_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t font_size_ = 1u;
    std::string font_family_ = ""s;
    std::string font_weight_ = ""s;
    std::string data_ = ""s;
  };

  class ObjectContainer
  {
  public:
    // Добавляет в svg-документ объект-наследник svg::Object

    template <typename T>
    void Add(T obj)
    {
      AddPtr(std::make_unique<T>(std::move(obj)));
    }

    virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;

    virtual ~ObjectContainer() = default;
  };

  class Document : public ObjectContainer
  {
  public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object> &&obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream &out) const;

  private:
    std::deque<std::unique_ptr<Object>> objects_;
  };

  class Drawable
  {
  public:
    virtual void Draw(ObjectContainer &container) const = 0;

    virtual ~Drawable() = default;
  };

} // namespace svg
