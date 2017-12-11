#include "includes.h"
#include "node.h"
#include "settings.h"
#include "utils.h"

cv::Mat image(HEIGHT, WIDTH, CV_8UC3, (cv::Scalar)BACKGROUND_COLOR);

int main() {
  Shape shape(
      "sphere.vox",
      {{'f', 360}, {'e', 200}, {'d', 100}, {'i', 150}, {'h', 225}, {'g', 250}});

  // Light cam(cv::Vec3f(WIDTH / 2, HEIGHT / 2, 10.0f), 10000.0f);

  shape.translate(WIDTH / 2, HEIGHT / 2, 0.0f);
  shape.scale(3.f, 3.f, 3.f);

  /* Initial draw */
  render_shape(image, shape);

  /* Render loop */
  while (1) {
    int c = cv::waitKey() & 0xFF;

    if (c == 32 || c == 255) { // Space or Alt+x, X
      break;
    }

    bool needs_redraw = false;

    // Translate
    if (c == 82 || c == 119) { // Upper arrow || W
      shape.translate(0, -INTENSITY, 0);
      needs_redraw = true;
    }
    if (c == 81 || c == 97) { // Left arrow || A
      shape.translate(-INTENSITY, 0, 0);
      needs_redraw = true;
    }
    if (c == 83 || c == 100) { // Right arrow || D
      shape.translate(INTENSITY, 0, 0);
      needs_redraw = true;
    }
    if (c == 84 || c == 115) { // Down arrow || S
      shape.translate(0, INTENSITY, 0);
      needs_redraw = true;
    }
    if (c == 61) { //              +
      shape.translate(0, 0, INTENSITY);
      needs_redraw = true;
    }
    if (c == 45) { //              -
      shape.translate(0, 0, -INTENSITY);
      needs_redraw = true;
    }
    /*


    */
    // Rotate
    if (c == 177) { // 1   +X
      shape.rotate(INTENSITY, cv::Vec3f(1.0f, 0, 0));
      needs_redraw = true;
    }
    if (c == 178) { // 2   -X
      shape.rotate(INTENSITY, cv::Vec3f(-1.0f, 0, 0));
      needs_redraw = true;
    }
    if (c == 180) { // 4   +Y
      shape.rotate(INTENSITY, cv::Vec3f(0, 1.0f, 0));
      needs_redraw = true;
    }
    if (c == 181) { // 5   -Y
      shape.rotate(INTENSITY, cv::Vec3f(0, -1.0f, 0));
      needs_redraw = true;
    }
    if (c == 183) { // 7   +Z
      shape.rotate(INTENSITY, cv::Vec3f(0, 0, 1.0f));
      needs_redraw = true;
    }
    if (c == 184) { // 8   -Z
      shape.rotate(INTENSITY, cv::Vec3f(0, 0, -1.0f));
      needs_redraw = true;
    }

    // Scale
    if (c == 93) { // ]   +X
      shape.scale(1.0f + INTENSITY * 0.1f, 1.0f, 1.0f);
      needs_redraw = true;
    }
    if (c == 91) { // [   -X
      shape.scale(1.0f - INTENSITY * 0.1f, 1.0f, 1.0f);
      needs_redraw = true;
    }
    if (c == 39) { // :   +Y
      shape.scale(1.0f, 1.0f + INTENSITY * 0.1f, 1.0f);
      needs_redraw = true;
    }
    if (c == 59) { // ;   -Y
      shape.scale(1.0f, 1.0f - INTENSITY * 0.1f, 1.0f);
      needs_redraw = true;
    }
    if (c == 47) { // /   +Z
      shape.scale(1.0f, 1.0f, 1.0f + INTENSITY * 0.1f);
      needs_redraw = true;
    }
    if (c == 46) { // .   -Z
      shape.scale(1.0f, 1.0f, 1.0f - INTENSITY * 0.1f);
      needs_redraw = true;
    }

    if (needs_redraw) {
      render_shape(image, shape);
    }

    if (VERBOSITY >= 3) {
      std::cout << c << " was pressed" << std::endl;
    }
  }
  /* -------- */

  /* Print file */
  std::vector<int> settings;
  settings.push_back(cv::IMWRITE_JPEG_QUALITY);
  settings.push_back(95);

  cv::imwrite("file.jpg", image, settings);
  /* -------- */
}
