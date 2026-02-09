#pragma once
#include "interfaces.hpp"
#include <vector>

class Chain {
private:
  ISource *source;
  std::vector<IConverter *> converters;
  ISink *sink;

public:
  Chain(ISource *source, ISink *sink) : source(source), sink(sink) {}

  Chain(ISource *source, IConverter *converter, ISink *sink)
      : source(source), converters(std::vector<IConverter *>({converter})),
        sink(sink) {}

  Chain(ISource *source, std::vector<IConverter *> converters, ISink *sink)
      : source(source), converters(converters), sink(sink) {}

  bool ready() {
    if (!source->ready())
      return false;

    for (auto &converter : converters) {
      if (!converter->ready())
        return false;
    }

    return sink->ready();
  }

  void process() {
    if (!ready())
      return;

    auto pixels = source->process();
    if (pixels.size() == 0)
      return;

    for (auto &converter : converters)
      pixels = converter->process(pixels);

    sink->process(pixels);
  }

  void initialize() {
    source->initialize();

    for (auto &converter : converters)
      converter->initialize();

    sink->initialize();
  }

  ISource *getSource() { return source; }
  ISink *getSink() { return sink; }
};
