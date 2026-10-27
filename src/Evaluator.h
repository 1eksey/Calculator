#pragma once

#include <cstddef>
#include <string>
#include <utility>

#include "PluginManager.h"

// expr := term (('+'|'-') term)*
// term := unary (('*'|'/') unary)*
// unary := ('-'|'+') unary | power
// power := primary [ plugin_operator unary ] (right-associative)
// primary := number | '(' expr ')' | name '(' expr (',' expr)* ')'
class Evaluator {
public:
    Evaluator(const PluginManager& plugins, std::string text) : plugins_(plugins), s_(std::move(text)) { }

    double evaluate();

private:
    [[noreturn]] void fail(const std::string& msg) const;
    void skipSpaces();
    char peek();
    void expect(char c);

    double parseExpr();
    double parseTerm();
    double parseUnary();
    double parsePower();
    double parsePrimary();
    double parseNumber();
    double parseCall();

    const PluginManager& plugins_;
    std::string s_;
    std::size_t pos_ = 0;
};