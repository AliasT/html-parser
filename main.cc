
/**
 * c++ implementation
 * https://limpet.net/mbrubeck/2014/08/08/toy-layout-engine-1.html
 * 学一门语言语法的最好方法是写一个 parser
 *
 * Build: clang++ main.cc --std=c++20
 */

#include <assert.h>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
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

} // namespace NodeType

// alias behind
typedef variant<NodeType::Element, NodeType::Text> nodetype_t;
using NodeType::AttrMap;

struct Node {
public:
    Node(string data)
        : node_type(NodeType::Text { data }) {};

    Node(string name, AttrMap attrs, vector<Node> children)
        : node_type(NodeType::Element { name, attrs })
        , children(children) {};

    static Node text(string data) { return Node(data); }

    static Node elem(string name, AttrMap attrs, vector<Node> children) { return Node(name, attrs, children); }

private:
    // 默认值
    vector<Node> children = {};
    nodetype_t node_type;
};

struct Parser {

    int pos = 0;
    string input;

    char next_char() { return pos < input.size() ? input[pos] : 0; }

    bool start_with(string s) { return input.substr(pos, s.size()) == s; }

    bool eof() { return pos >= input.size(); }

    char consume_char()
    {
        char cur = input[pos];
        pos++;
        return cur;
    }

    string consume_while(function<bool(char)> const& f)
    {
        string result;
        while (!eof() && f(next_char())) {
            result += input[pos];
            pos++;
        }
        return result;
    }

    void consume_whitespace()
    {
        consume_while([](char c) { return isspace(c); });
    }

    string parse_tag_name()
    {
        return consume_while([](char c) { return isalnum(c); });
    }

    Node parse_node()
    {
        switch (next_char()) {
        case '<':
            return parse_element();
        default:
            return parse_text();
        }
    }

    Node parse_element()
    {
        // Opening tag
        assert(consume_char() == '<');
        string tag_name = parse_tag_name();
        AttrMap attrs = parse_attributes();
        assert(consume_char() == '>');

        // Contents
        auto children = parse_nodes();

        // Closing tag.
        assert(consume_char() == '<');
        assert(consume_char() == '/');
        // 前后标签名相同
        assert(parse_tag_name() == tag_name);
        assert(consume_char() == '>');

        return Node::elem(tag_name, attrs, children);
    }

    Node parse_text()
    {
        return Node::text(consume_while([](char c) { return c != '<'; }));
    }

    AttrMap parse_attributes()
    {
        AttrMap attributes {};
        while (1) {
            consume_whitespace();
            if (next_char() == '>') break;
            const auto [name, value] = parse_attr();
            attributes[name] = value;
        }
        return attributes;
    }

    tuple<string, string> parse_attr()
    {
        auto name = parse_tag_name();
        assert(consume_char() == '=');
        auto value = parse_attr_value();
        return { name, value };
    }

    string parse_attr_value()
    {
        auto open_quote = consume_char();
        // 单引号 ？
        assert(open_quote == '"' || open_quote == '\'');
        // 匹配到 open_quote 结束
        auto value = consume_while([open_quote](char c) { return c != open_quote; });
        assert(consume_char() == open_quote);

        return value;
    }

    vector<Node> parse_nodes()
    {
        vector<Node> nodes {};
        while (1) {
            consume_whitespace();
            if (eof() || start_with("</")) break;
            nodes.push_back(parse_node());
        }
        return nodes;
    }

    static Node parse(string source)
    {
        auto parser = Parser { .pos = 0, .input = source };
        auto nodes = parser.parse_nodes();
        if (nodes.size() == 1) {
            return nodes.at(0);
        } else {
            return Node::elem("html", AttrMap {}, nodes);
        }
    }
};

struct Visitor {
    void operator()(NodeType::Element) {};
    void operator()(NodeType::Text) {};
};

int main()
{
    auto root = Parser::parse("<html><body>Hello, world!</body></html>");
    cout << 1;
}