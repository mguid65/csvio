#include <iostream>
#include <fstream>
#include <string>
#include <string_view>


class CSVLineState {
 public:
  enum Scope {
    LINE,
    QUOTE
  };

  CSVLineState(std::ifstream& infile) : m_infile(infile) {}

  std::string readline() {
    return getcsvline();
  }

 private:
  std::string getcsvline() {
    std::string result;

    char buf[1024]{0};
    char * buf_end = buf + 1000;
    char * pos = buf;

    while(m_infile.good()) {
      m_infile.get(*pos);
      if (m_state == LINE && pos[0] == '\n') {
        result.append(buf);
        break;
      } else if (m_state == LINE && pos[0] == '\"') {
        m_state = QUOTE;
      } else if (m_state == QUOTE && pos[0] == '\"') {
        m_state = LINE;
      } else if (m_state == QUOTE && pos[0] == EOF) {
        std::cerr << "Unexpected EOF\n" << '\n';
        break;
      } else if (m_state == LINE && pos[0] == EOF) {
        result += std::string(buf);
        break;
      }
      pos+=1;
      if(pos >= buf_end) {
        pos = buf;
        result += std::string(buf);
        std::fill(buf, buf + 1024, '\0');
      }
    }
    return result;
  }

  Scope m_state = LINE;
  std::ifstream& m_infile;
};


int main() {
  std::ifstream infile("testreadline.csv", std::ios::binary);
  CSVLineState state(infile);
  while(infile.good()) {
    std::string s = state.readline();
    std::cout << s << '\n';
  }
  return 0;
}



