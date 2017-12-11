#pragma once

#include "includes.h"
#include "node.h"
#include "settings.h"

using std::to_string;

namespace utils {

std::string curlify(std::initializer_list<const char *> args) {
  std::string str;

  if (args.size() % 2 != 0) {
    str.append("Curlify takes even count of arguments. ");
    str.append(std::to_string(args.size()));
    str.append(" given");
    return str;
  }

  str += "{ ";

  for (auto arg = args.begin() + 1;; arg += 2) {
    auto key = *(arg - 1);
    auto val = *arg;

    str += key;
    str += ": ";
    str += val;

    if ((arg - args.begin()) < args.size() - 1) {
      str += ", ";
    } else {
      break;
    }
  }

  str += " }";

  return str;
}

template <typename T> std::string curlify(const cv::Point3_<T> &p) {
  return curlify({"x", to_string(p.x).c_str(), "y", to_string(p.y).c_str(), "z",
                  to_string(p.z).c_str()});
}

template <typename T> std::string curlify(const cv::Vec<T, 3> &v) {
  return curlify({"x", to_string(v.val[0]).c_str(), "y",
                  to_string(v.val[1]).c_str(), "z",
                  to_string(v.val[2]).c_str()});
}

class Timer {
  Timer() {}
  ~Timer() {}
  static int time_measure_helper_1;
  static int time_measure_helper_2;
  static std::string command;
  static bool started;

public:
  static void start_measure(const char *cmd) {
    if (!TIME_MEASURE) {
      return;
    }

    if (started) {
      std::cout << "Err. Cannot start '" << cmd << "' because '" << command
                << "' is already running" << '\n';
      return;
    }

    started = true;
    command = cmd;
    time_measure_helper_1 = clock();
  }

  static void end_measure(bool print_result = true) {
    if (!TIME_MEASURE) {
      return;
    }

    if (!started) {
      std::cout << "Err. Cannot finish time measure before start" << '\n';
      return;
    }

    time_measure_helper_2 = clock();
    if (print_result) {
      auto took = (time_measure_helper_2 - time_measure_helper_1) /
                  double(CLOCKS_PER_SEC) * 1000; // ms
      std::string rating;

      if (took < 6.0) {
        rating = "\e[96mLIGHTNING FAST\e[39m";
      } else if (took >= 6.0 && took < 30.0) {
        rating = "\e[92mFAST\e[39m";
      } else if (took >= 30.0 && took < 100.0) {
        rating = "\e[93mMODERATE\e[39m";
      } else if (took >= 100.0 && took < 160.0) {
        rating = "\e[95mSLOW\e[39m";
      } else if (took >= 160.0 && took < 250.0) {
        rating = "\e[91mVERY SLOW\e[39m";
      } else {
        rating = "\e[31mBLOCKING\e[39m";
      }

      std::cout << "\n\t\e[32mTIME:\e[39m Execution of '" << command
                << "' took " << took << "ms. (" << rating << ")"
                << "\n\n";
    }

    time_measure_helper_1 = 0;
    time_measure_helper_2 = 0;
    command = "";
    started = false;
  }
};
int Timer::time_measure_helper_1 = 0;
int Timer::time_measure_helper_2 = 0;
std::string Timer::command = "";
bool Timer::started = false;
/*



*/
cv::Matx44f &set_at(cv::Matx44f &matx, const int row, const int column,
                    const float val) {
  auto tmp_mat = cv::Mat2f(matx);
  tmp_mat.at<float>(row, column) = val;
  matx = cv::Matx44f((float *)tmp_mat.clone().ptr());
}

float get_at(const cv::Matx44f &matx, const int row, const int column) {
  auto tmp_mat = cv::Mat2f(matx);
  return tmp_mat.at<float>(row, column);
}
template <typename T> T get_at(const cv::Vec<T, 2> &v, const unsigned row) {
  return v.val[row];
}
template <typename T> T get_at(const cv::Vec<T, 3> &v, const unsigned row) {
  return v.val[row];
}

cv::Matx44f translate(const float x, const float y, const float z) {
  auto matx = cv::Matx44f::eye();
  set_at(matx, 0, 3, x);
  set_at(matx, 1, 3, y);
  set_at(matx, 2, 3, z);
  return matx;
}
cv::Matx44f translate(const cv::Point3f &p) { return translate(p.x, p.y, p.z); }
template <typename T> cv::Matx44f translate(const cv::Vec<T, 3> &p) {
  return translate(p.val[0], p.val[1], p.val[2]);
}

cv::Matx44f scale(const float x, const float y, const float z) {
  auto matx = cv::Matx44f::eye();
  set_at(matx, 0, 0, x);
  set_at(matx, 1, 1, y);
  set_at(matx, 2, 2, z);
  return matx;
}
cv::Matx44f scale(const cv::Point3f &p) { return scale(p.x, p.y, p.z); }

enum RotateAxis { X, Y, Z };

cv::Matx44f rotate(const float grad, const RotateAxis axis) {
  auto matx = cv::Matx44f::eye();
  const float rad = grad * M_PI / 180.0f;

  switch (axis) {
  case RotateAxis::X:
    set_at(matx, 1, 1, cos(rad));
    set_at(matx, 1, 2, -sin(rad));
    set_at(matx, 2, 1, sin(rad));
    set_at(matx, 2, 2, cos(rad));
    break;

  case RotateAxis::Y:
    set_at(matx, 0, 0, cos(rad));
    set_at(matx, 0, 2, sin(rad));
    set_at(matx, 2, 0, -sin(rad));
    set_at(matx, 2, 2, cos(rad));
    break;

  case RotateAxis::Z:
    set_at(matx, 0, 0, cos(rad));
    set_at(matx, 0, 1, -sin(rad));
    set_at(matx, 1, 0, sin(rad));
    set_at(matx, 1, 1, cos(rad));
    break;
  }

  return matx;
}

cv::Matx44f rotate(const float grad, const cv::Vec3f vec) {
  auto matx = cv::Matx44f::eye();
  const float rad = grad * M_PI / 180.0f;

  auto cosval = (float)cos(rad);
  auto sinval = (float)sin(rad);
  auto rx = vec[0];
  auto ry = vec[1];
  auto rz = vec[2];

  set_at(matx, 0, 0, cosval + pow(rx, 2.0f) * (1 - cosval));
  set_at(matx, 0, 1, rx * ry * (1 - cosval) - rz * sinval);
  set_at(matx, 0, 2, rx * rz * (1 - cosval) + ry * sinval);

  set_at(matx, 1, 0, ry * rx * (1 - cosval) + rz * sinval);
  set_at(matx, 1, 1, cosval + pow(ry, 2.0f) * (1 - cosval));
  set_at(matx, 1, 2, ry * rz * (1 - cosval) - rx * sinval);

  set_at(matx, 2, 0, rz * rx * (1 - cosval) - ry * sinval);
  set_at(matx, 2, 1, rz * ry * (1 - cosval) + rx * sinval);
  set_at(matx, 2, 2, cosval + pow(rz, 2.0f) * (1 - cosval));

  return matx;
}

cv::Matx44f perspective(const float fovw, const float fovh, const float znear,
                        const float zfar) {
  return cv::Matx44f(cos(fovw / 2) / sin(fovw / 2), 0, 0, 0, //
                     0, cos(fovh / 2) / sin(fovh / 2), 0, 0, //
                     0, 0, (zfar / (zfar - znear)), 5,       //
                     0, 0, -(zfar / (zfar - znear)) * znear, 0);
}

std::vector<std::string> split(std::string str, std::string token) {
  std::vector<std::string> result;
  while (str.size()) {
    int index = str.find(token);
    if (index != std::string::npos) {
      result.push_back(str.substr(0, index));
      str = str.substr(index + token.size());
      if (str.size() == 0)
        result.push_back(str);
    } else {
      result.push_back(str);
      str = "";
    }
  }
  return result;
}

class point_storage {
  std::map<std::string, unsigned> stg;
  std::vector<std::string> stg_vec;
  static std::string stringify(int x, int y, int z) {
    std::string query = to_string(x) + "," + to_string(y) + "," + to_string(z);
    return query;
  }
  template <typename T> static std::string stringify(const cv::Point3_<T> &p) {
    return stringify(p.x, p.y, p.z);
  }

  static cv::Point3f unstringify(std::string str) {
    auto splitted = split(str, ",");
    auto res = cv::Point3f(std::stof(splitted[0]), std::stof(splitted[1]),
                           std::stof(splitted[2]));
    return res;
  }

public:
  point_storage() {}

  explicit point_storage(std::vector<cv::Point3f> pts) {
    for (auto p = pts.begin(); p != pts.end(); ++p) {
      std::string query = point_storage::stringify(*p);
      stg[query] = true;
      stg_vec.push_back(query);
    }
  }

  ~point_storage() {}

  void save(float x, float y, float z, const unsigned &color) {
    std::string query = point_storage::stringify(x, y, z);
    stg[query] = color;
    stg_vec.push_back(query);
  }

  void save(const cv::Point3f &p, const unsigned &color) {
    save(p.x, p.y, p.z, color);
  }

  bool has(float x, float y, float z) const {
    std::string query = point_storage::stringify(x, y, z);
    auto tmp = stg.find(query);
    return tmp != stg.end();
  }

  cv::Point3f get(float x, float y, float z) const {
    std::string query = point_storage::stringify(x, y, z);
    auto point_raw = stg.find(query)->first;
    return point_storage::unstringify(point_raw);
  }

  cv::Point3f get(int index) const {
    return point_storage::unstringify(stg_vec[index]);
  }

  unsigned get_color(float x, float y, float z) const {
    auto query = point_storage::stringify(x, y, z);
    return stg.find(query)->second;
  }
  unsigned get_color(int index) const {
    auto query = stg_vec[index];
    return stg.find(query)->second;
  }

  std::vector<cv::Point3f> get_all() const {
    std::vector<cv::Point3f> res;

    for (auto p = stg_vec.begin(); p != stg_vec.end(); ++p) {
      res.push_back(point_storage::unstringify(*p));
    }

    return res;
  }

  auto size() const { return stg.size(); }
}; // namespace utils

template <class T, class Compare>
constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
  return assert(!comp(hi, lo)), comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

cv::Vec3b HSVtoBGR(const cv::Vec3f &hsv) {
  cv::Mat_<cv::Vec3f> hsvMat(hsv);
  cv::Mat_<cv::Vec3f> bgrMat;

  cv::cvtColor(hsvMat, bgrMat, CV_HSV2BGR);

  bgrMat *= 255; // Upscale after conversion

  // Conversion to Vec3b is handled by OpenCV, no need to static_cast
  return bgrMat(0);
}

template <typename T> T clamp(T x, T min, T max) {
  if (x < min) {
    return min;
  } else if (x > max) {
    return max;
  }
  return x;
}

template <typename _T> cv::Vec<_T, 4> p2v(const cv::Point3_<_T> &p) {
  return cv::Vec<_T, 4>(p.x, p.y, p.z, 1.0f);
}

template <typename T> bool in_range(T val, T from, T to) {
  return ((val > from) && (val < to));
}

} // namespace utils
