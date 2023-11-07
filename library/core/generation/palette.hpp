#pragma once
#include "colours.h"
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