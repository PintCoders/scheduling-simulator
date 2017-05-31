#pragma once
#include "reader.hh"
#include <fstream>

namespace scheduler_simulator {

class Options;

class TextReader: public Reader {
  public:
    TextReader (Options*);
    virtual ~TextReader () = default;

    virtual bool is_next() override;
    virtual std::vector<std::string> next() override;

  private:
    std::ifstream ifs;
};

}