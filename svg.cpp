#include "svg.h"
namespace svg {

std::ostream &operator<<(std::ostream &os, const StrokeLineCap line_cap) {
  switch (line_cap) {
    case StrokeLineCap::BUTT:
      return os << "butt"s;
      break;
    case StrokeLineCap::ROUND:
      return os << "round"s;
      break;
    case StrokeLineCap::SQUARE:
      return os << "square"s;
      break;
  }
  return os;
}
std::ostream &operator<<(std::ostream &os, const StrokeLineJoin line_join) {
  switch (line_join) {
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

// ---------- Object ------------------

void Object::Render(const RenderContext &context) const {
  context.RenderIndent();

  // Делегируем вывод тега своим подклассам
  RenderObject(context);

  context.out << std::endl;
}

// ---------- Circle ------------------

Circle &Circle::SetCenter(Point center) {
  center_ = std::move(center);
  return *this;
}

Circle &Circle::SetRadius(double radius) {
  radius_ = std::move(radius);
  return *this;
}

void Circle::RenderObject(const RenderContext &context) const {
  auto &out = context.out;
  out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
  out << "r=\""sv << radius_ << "\""sv;
  RenderAttributes(out);
  out << " />"sv;
}

// Polyline

Polyline &Polyline::AddPoint(Point point) {
  points_.push_back(point);
  return *this;
};

void Polyline::RenderObject(const RenderContext &context) const {
  auto &out = context.out;
  out << "<polyline points=\""sv;
  bool is_first = true;
  for (const auto &point : points_) {
    if (!is_first) {
      out << " "sv;
    }
    out << point.x << "," << point.y;
    is_first = false;
  }
  out << "\"";
  RenderAttributes(out);
  out << " />"sv;
};

// Text

// Задаёт координаты опорной точки (атрибуты x и y)
Text &Text::SetPosition(Point pos) {
  position_ = pos;
  return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text &Text::SetOffset(Point offset) {
  offset_ = offset;
  return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text &Text::SetFontSize(uint32_t size) {
  font_size_ = size;
  return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text &Text::SetFontFamily(std::string font_family) {
  font_family_ = std::move(font_family);
  return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text &Text::SetFontWeight(std::string font_weight) {
  font_weight_ = std::move(font_weight);
  return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text &Text::SetData(std::string data) {
  ScreenData(data);
  data_ = std::move(data);
  return *this;
}

void Text::RenderObject(const RenderContext &context) const {
  using namespace std::literals;
  auto &out = context.out;
  // <text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana"
  // font-weight="bold">Hello C++</text>
  out << "<text"sv;
  RenderAttributes(out);
  out << " x=\""sv << position_.x << "\""sv;
  out << " y=\""sv << position_.y << "\""sv;
  out << " dx=\""sv << offset_.x << "\""sv;
  out << " dy=\""sv << offset_.y << "\""sv;
  out << " font-size=\""sv << font_size_ << "\""sv;
  if (!font_family_.empty()) {
    out << " font-family=\""sv << font_family_ << "\""sv;
  }
  if (!font_weight_.empty()) {
    out << " font-weight=\""sv << font_weight_ << "\""sv;
  }
  out << " >"sv;
  out << data_;
  out << "</text>";
};

void Text::ScreenCharacter(std::string &str, char c, std::string_view value) {
  size_t c_pos = str.find_first_of(c);
  if (c_pos != std::string::npos) {
    while (c_pos != std::string::npos) {
      std::string new_str =
          str.substr(0, c_pos) + value.data() + str.substr(c_pos + 1);
      str = new_str;
      c_pos = str.find_first_of(c, c_pos + 1);
    }
  }
}

void Text::ScreenData(std::string &str) {
  ScreenCharacter(str, '&', "&amp;"s);
  ScreenCharacter(str, '\"', "&quot;"s);
  ScreenCharacter(str, '<', "&lt;"s);
  ScreenCharacter(str, '>', "&gt;"s);
  ScreenCharacter(str, '\'', "&apos;"s);
}

// Document

void Document::AddPtr(std::unique_ptr<Object> &&obj) {
  objects_.push_back(std::move(obj));
}

// Выводит в ostream svg-представление документа
void Document::Render(std::ostream &out) const {
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv
      << std::endl;
  for (const auto &object : objects_) {
    object->Render(out);
  }
  out << "</svg>"sv << std::endl;
}
}  // namespace svg
