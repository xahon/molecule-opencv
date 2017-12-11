#pragma once

#include "includes.h"

extern const size_t VERBOSITY = 4; // From 1 to 4. 4 - Highest, 1 - Lowest
extern const bool DEBUG = true;
extern const bool TIME_MEASURE = true;
extern const float INTENSITY = 15.f;
extern const int WIDTH = 800;
extern const int HEIGHT = 600;
extern double ASPECT_RATIO = (double)WIDTH / HEIGHT;
extern const char *MAIN_WINDOW_NAME = "Render";
extern const cv::Vec3b BACKGROUND_COLOR = cv::Vec3b(0, 0, 0);
extern const float ZNEAR = 10.0f;
extern const float ZFAR = -100.0f;
