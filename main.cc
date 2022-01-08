
/**
 * c++ implementation
 * https://limpet.net/mbrubeck/2014/08/08/toy-layout-engine-1.html
 */

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

using namespace std;

namespace NodeType {

typedef map<string, string> AttrMap;

struct Text {
  string data;
};

struct Element {
  string tag_name;
  AttrMap attributes;
};

}  // namespace NodeType

// alias behind
typedef variant<NodeType::Element, NodeType::Text> nodetype_t;
using NodeType::AttrMap;

struct Node {
 public:
  Node(string data) : node_type(NodeType::Text(data)){};

  Node(string name, AttrMap attrs, vector<Node> children)
      : node_type(NodeType::Element{name, attrs}), children(children){};

  static Node text(string data) { return Node(data); }

  static Node elem(string name, AttrMap attrs, vector<Node> children) {
    return Node(name, attrs, children);
  }

 private:
  // 默认值
  vector<Node> children = {};
  nodetype_t node_type;
};

struct Parser {
  int pos;
  string input;

  char next_char() { return pos + 1 < input.size() ? input[pos + 1] : 0; }
  bool start_with(string s) { return input.substr(pos, s.size()) == s; }
  bool eof() { return pos >= input.size(); }
};

struct Visitor {
  void operator()(NodeType::Element){};
  void operator()(NodeType::Text){};
};

int main() {}