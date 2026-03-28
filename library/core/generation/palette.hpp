#pragma once
#include "core/colors.h"
#include "gradient.hpp"
#include <string>

  struct Palette
  {
    Gradient gradient;
    RGBA primary;
    RGBA secondary;
    RGBA highlight;
    std::string name;
  };