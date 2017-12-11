#pragma once

class Node;
class Shape;
class Light;

#include "includes.h"
#include "settings.h"
#include "utils.h"

using std::to_string;

class Node {
public:
  Node(int width = 1, int height = 1, int depth = 1,
       cv::Point3f center = cv::Point3f())
      : width(width), height(height), depth(depth), center(center) {
    position = cv::Point3f();
    rotation = cv::Vec3f();
    scaling = cv::Vec3f(1.0f, 1.0f, 1.0f);
    matx = cv::Matx44f::eye();
  }

  void translate(float x, float y, float z) {
    reset_rotation();
    reset_scale();
    matx = matx * utils::translate(x, y, z);
    restore_scale();
    restore_rotation();

    position += cv::Point3f(x, y, z);
  }
  void translate(const cv::Point3f &p) { translate(p.x, p.y, p.z); }

  void scale(float x, float y, float z) {
    reset_transform();
    reset_rotation();
    reset_scale();
    matx = matx * utils::scale(x, y, z);
    restore_scale();
    restore_rotation();
    restore_transform();

    cv::Vec3f new_scale =
        cv::Vec3f(utils::get_at(matx, 0, 0), utils::get_at(matx, 1, 1),
                  utils::get_at(matx, 2, 2));

    scaling = new_scale;
  }
  void scale(const cv::Vec3f &v) { scale(v.val[0], v.val[1], v.val[2]); }

  void rotate(float angle, cv::Vec3f axis) {
    auto old_matx = matx;

    reset_transform();
    reset_scale();
    matx = matx * utils::rotate(angle, axis);
    restore_scale();
    restore_transform();

    rotation.val[0] += axis.val[0] * angle;
    rotation.val[1] += axis.val[1] * angle;
    rotation.val[2] += axis.val[2] * angle;
  }
  void rotate(cv::Vec3f values) { rotate(1.0f, values); }

  int get_width() const { return width; }
  int get_height() const { return height; }
  int get_depth() const { return depth; }
  cv::Matx44f get_matx() const { return matx; }
  cv::Point3f get_pos() const { return position; }
  cv::Vec3f get_rot() const { return rotation; }
  cv::Vec3f get_sc() const { return scaling; }
  std::string get_dims() const {
    return utils::curlify({"width", to_string(width).c_str(), "height",
                           to_string(height).c_str(), "depth",
                           to_string(depth).c_str()});
  }

protected:
  cv::Matx44f reset_transform() {
    auto move = utils::translate(-position);
    return matx * move;
  }

  cv::Matx44f restore_transform() {
    auto move = utils::translate(position);
    return matx * move;
  }

  cv::Matx44f reset_rotation() {
    auto rotate_x = utils::rotate(-rotation.val[0], cv::Vec3f(1.0f, 0, 0));
    auto rotate_y = utils::rotate(-rotation.val[1], cv::Vec3f(0, 1.0f, 0));
    auto rotate_z = utils::rotate(-rotation.val[2], cv::Vec3f(0, 0, 1.0f));
    return matx * rotate_x * rotate_y * rotate_z;
  }

  cv::Matx44f restore_rotation() {
    auto rotate_x = utils::rotate(rotation.val[0], cv::Vec3f(1.0f, 0, 0));
    auto rotate_y = utils::rotate(rotation.val[1], cv::Vec3f(0, 1.0f, 0));
    auto rotate_z = utils::rotate(rotation.val[2], cv::Vec3f(0, 0, 1.0f));
    return matx * rotate_z * rotate_y * rotate_x;
  }

  cv::Matx44f reset_scale() {
    auto scale = utils::scale(scaling);
    return matx * scale.inv();
  }

  cv::Matx44f restore_scale() {
    auto scale = utils::scale(scaling);
    return matx * scale;
  }

  int width;
  int height;
  int depth;
  cv::Point3f center;
  cv::Matx44f matx;
  cv::Point3f position;
  cv::Vec3f rotation;
  cv::Vec3f scaling;
};

typedef std::map<char, int> color_pairs;
class Shape : public Node {
public:
  explicit Shape(const char *filename, const color_pairs &colors = {{'0', 360}})
      : Node(), color_groups(colors) {

    utils::Timer::start_measure("Shape loading");
    std::ifstream shapefile(filename);
    std::string buffer(
        (std::istreambuf_iterator<char>(shapefile)),
        std::istreambuf_iterator<char>()); // Buffer is raw file content

    shapefile.close();

    if (VERBOSITY >= 3) {
      std::cout << "\nShape buffer size is " << buffer.size() << '\n';
    }

    { // Get cube dimensions
      int i = 0;
      for (; i < (int)buffer.size() && buffer[i] != '\n'; i++) {
      }
      width = height = depth = i;
      center = cv::Point3f(width / 2, height / 2, depth / 2);
    }

    std::string working_buffer;

    {
      for (int i = 0; i < buffer.size(); ++i) {
        auto ch = buffer[i];
        if (ch == '\n') {
          continue;
        }

        working_buffer.push_back(ch);
      }
    }

    std::string unknown_chars = "";
    std::string char_groups = "";

    // Handle all points and save them with half width/height/depth offset
    for (int z = 0; z < depth; ++z) {
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          char ch = working_buffer.at(x + y * height + z * width * height);
          if (ch != ' ') {
            auto known_char = char_groups.find(ch);
            if (known_char == std::string::npos) {
              char_groups += ch;

              if (color_groups.find(ch) == color_groups.end()) {
                unknown_chars += ch;
                std::cout << "Unknown char " << ch << '\n';
                exit(1);
              }
            }

            vertices.save(x - width / 2, y - height / 2, z - depth / 2,
                          color_groups.find(ch)->second);
          }
        }
      }
    }

    if (char_groups.size() > color_groups.size()) {
      std::cout << "Expected " << char_groups.size() << " color groups "
                << color_groups.size() << " given. Used '" << char_groups
                << "'. Unknown chars are: '" << unknown_chars << "'" << '\n';
      exit(1);
    }

    if (VERBOSITY >= 2) {
      std::cout << "Shape " << filename << " loaded with " << get_dims()
                << " Dimensions" << std::endl;

      std::cout << "At position " << utils::curlify(get_pos()) << "\n"
                << std::endl;
    }
    utils::Timer::end_measure();
  }

  std::string as_string() const {
    std::string res = "Shape:\n\t";
    res.append("\n\t");

    res.append(get_dims());
    res.append("\n\t");

    res.append("Center: ");
    res.append(utils::curlify(center));
    res.append("\n\t");

    res.append("Position: ");
    res.append(utils::curlify(get_pos()));
    // res.append("   (GLOBAL): ");
    // res.append(utils::curlify(get_pos_global()));
    res.append("\n\t");

    res.append("Rotation: ");
    res.append(utils::curlify(get_rot()));
    res.append("\n\t");

    res.append("Scale: ");
    res.append(utils::curlify(get_sc()));
    res.append("\n\t");

    if (VERBOSITY >= 3) {
      res.append("Shape has ");
      res.append(to_string(vertices.size()));
      res.append(" vertices");
      res.append("\n\t");
    }

    return res;
  }

  bool at(int x, int y, int z) const {
    if ((x < 0) || (x > width) || (y < 0) || (y > height) || (z < 0) ||
        (z > depth)) { // Check if out of bounds
      return false;
    }

    return vertices.has(x, y, z);
  }

  utils::point_storage get_vertices() const { return vertices; }

private:
  utils::point_storage vertices;
  color_pairs color_groups;
};

class Light : public Node {
public:
  Light(cv::Point3f position, float light_distance = 100.0f)
      : Node(1, 1, 1, cv::Point3f()), light_distance(light_distance) {
    translate(position);
  }
  // cv::Vec3f dir;
  // cv::Point3f pos;
  // float fov;

  void scale() = delete;
  float get_light_distance() const { return light_distance; }

private:
  float light_distance;
};

static const float ZBUFFER_DIVIDER = 100000.0f;

void render_shape(cv::Mat &im, const Shape &shape) { //, const Light &cam) {
  utils::Timer::start_measure("Clearing screen");
  for (int y = 0; y < HEIGHT; ++y) { // Fill screen default color
    for (int x = 0; x < WIDTH; ++x) {
      *(im.ptr<cv::Vec3b>(y, x)) = BACKGROUND_COLOR;
    }
  }
  utils::Timer::end_measure();

  cv::Mat_<float> z_buffer(HEIGHT, WIDTH, -1.0f);

  // Transform each vertex according to its shape matrix
  utils::Timer::start_measure("Transforming shape vertices");
  auto vertcs = shape.get_vertices();
  auto new_vex = vertcs.get_all();

  for (unsigned v = 0; v < new_vex.size(); ++v) {
    auto vertex = new_vex[v];
    auto vertex_as_vector = utils::p2v(vertex);
    auto new_vertex =
        shape.get_matx() * // utils::perspective(20.0, 20.0, -1, 5) *
        vertex_as_vector;
    auto homogeneous_coord = new_vertex.val[3];

    float x = new_vertex.val[0] / homogeneous_coord,
          y = new_vertex.val[1] / homogeneous_coord,
          z = new_vertex.val[2] / homogeneous_coord;

    int z_buffer_x_index = (int)x;
    int z_buffer_y_index = (int)y;

    if (z_buffer_x_index < 0 || z_buffer_x_index >= WIDTH) {
      continue;
    }

    if (z_buffer_y_index < 0 || z_buffer_y_index >= HEIGHT) {
      continue;
    }

    auto z_val = z /= ZBUFFER_DIVIDER;
    auto z_buf_val = z_buffer.at<float>(z_buffer_x_index, z_buffer_y_index);

    if (utils::in_range<int>(x, 0, WIDTH) &&
        utils::in_range<int>(y, 0, HEIGHT)) {
      if (z_val > z_buf_val) {
        z_buffer.at<float>(z_buffer_x_index, z_buffer_y_index) = z_val;
      } else {
        // Skip if this point behind another
        continue;
      }
    } else {
      // Skip out of bounds iteration
      continue;
    }

    // float distance_to = cv::norm(cam.get_pos() - cv::Point3f(x, y, z));
    float color_intensity;

    // if (distance_to <= 0.0f) {
    //   color_intensity = 1.0f;
    // } else if (distance_to >= cam.get_light_distance()) {
    //   color_intensity = 0.0f;
    // } else {
    //   color_intensity = utils::clamp((cam.get_light_distance() -
    //   distance_to),
    //                                  0.0f, cam.get_light_distance()) /
    //                     cam.get_light_distance();
    // }

    color_intensity = 1.0f; // Override
    // std::cout << "ci " << color_intensity << '\n';

    const int &hue = vertcs.get_color(v);

    // utils::Timer::start_measure("Splat drawing");
    // Check if is not out of bounds
    for (int sh = y - 2; sh <= y + 2; ++sh) {
      for (int sw = x - 2; sw <= x + 2; ++sw) {

        if (utils::in_range<int>(sw, 0, WIDTH) &&
            utils::in_range<int>(sh, 0, HEIGHT)) {

          im.at<cv::Vec3b>(sh, sw) = utils::HSVtoBGR(
              cv::Vec3f(hue, 100 * color_intensity, 100 * color_intensity));
        }
      }
      // utils::hsl2bgr(cv::Vec3b(360, 100, color_intensity * 255));
    }
    // utils::Timer::end_measure();

    //   if (VERBOSITY >= 4) {
    //     std::cout << "Previous: " << vertex << " next: " << end_point <<
    //     '\n';
    //   }
  }
  utils::Timer::end_measure();

  cv::imshow(MAIN_WINDOW_NAME, im);
}
