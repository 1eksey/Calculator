#include "Evaluator.h"

#include <cctype>
#include <cmath>
#include <vector>

#include "CalcError.h"

double Evaluator::evaluate() {
    double v = parseExpr();
    skipSpaces();
    if (pos_ != s_.size()) fail(std::string("unexpected '") + s_[pos_] + "'");
    if (!std::isfinite(v)) throw CalcError("result is out of range");
    return v;
}

void Evaluator::fail(const std::string& msg) const {
    throw CalcError(msg + " (position " + std::to_string(pos_ + 1) + ")");
}

void Evaluator::skipSpaces() {
    while (pos_ < s_.size() && std::isspace(static_cast<unsigned char>(s_[pos_]))) ++pos_;
}

char Evaluator::peek() {
    skipSpaces();
    return pos_ < s_.size() ? s_[pos_] : '\0';
}

void Evaluator::expect(char c) {
    if (peek() != c) fail(std::string("expected '") + c + "'");
    ++pos_;
}

double Evaluator::parseExpr() {
    double v = parseTerm();
    for (;;) {
        char c = peek();
        if (c == '+') { ++pos_; v += parseTerm(); }
        else if (c == '-') { ++pos_; v -= parseTerm(); }
        else return v;
    }
}

double Evaluator::parseTerm() {
    double v = parseUnary();
    for (;;) {
        char c = peek();
        if (c == '*') { ++pos_; v *= parseUnary(); }
        else if (c == '/') {
            ++pos_;
            double rhs = parseUnary();
            if (rhs == 0.0) throw CalcError("division by zero");
            v /= rhs;
        }
        else return v;
    }
}

double Evaluator::parseUnary() {
    char c = peek();
    if (c == '-') { ++pos_; return -parseUnary(); }
    if (c == '+') { ++pos_; return parseUnary(); }
    return parsePower();
}

double Evaluator::parsePower() {
    double lhs = parsePrimary();
    char c = peek();
    if (c != '\0' && plugins_.isOperator(c)) {
        ++pos_;
        double rhs = parseUnary();
        return plugins_.find(std::string(1, c))->call({ lhs, rhs });
    }
    return lhs;
}

double Evaluator::parsePrimary() {
    char c = peek();
    if (c == '\0') fail("unexpected end of expression");
    if (c == '(') {
        ++pos_;
        double v = parseExpr();
        expect(')');
        return v;
    }
    if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') return parseNumber();
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') return parseCall();
    fail(std::string("unexpected '") + c + "'");
}

double Evaluator::parseNumber() {
    const std::size_t start = pos_;
    auto digits = [&] {
        while (pos_ < s_.size() && std::isdigit(static_cast<unsigned char>(s_[pos_]))) ++pos_;
        };
    digits();
    if (pos_ < s_.size() && s_[pos_] == '.') { ++pos_; digits(); }
    if (pos_ < s_.size() && (s_[pos_] == 'e' || s_[pos_] == 'E')) {
        std::size_t p = pos_ + 1;
        if (p < s_.size() && (s_[p] == '+' || s_[p] == '-')) ++p;
        if (p < s_.size() && std::isdigit(static_cast<unsigned char>(s_[p]))) {
            pos_ = p;
            digits();
        }
    }
    const std::string text = s_.substr(start, pos_ - start);
    if (text == ".") { pos_ = start; fail("invalid number"); }
    try {
        return std::stod(text);
    }
    catch (const std::exception&) {
        pos_ = start;
        fail("invalid number '" + text + "'");
    }
}

double Evaluator::parseCall() {
    const std::size_t start = pos_;
    while (pos_ < s_.size() &&
        (std::isalnum(static_cast<unsigned char>(s_[pos_])) || s_[pos_] == '_'))
        ++pos_;
    const std::string name = s_.substr(start, pos_ - start);

    const Plugin* plugin = plugins_.find(name);
    if (!plugin) { pos_ = start; fail("unknown function '" + name + "'"); }

    expect('(');
    std::vector<double> args;
    if (peek() == ')') {
        ++pos_;
    }
    else {
        for (;;) {
            args.push_back(parseExpr());
            if (peek() == ',') { ++pos_; continue; }
            expect(')');
            break;
        }
    }
    if (static_cast<int>(args.size()) != plugin->arity())
        throw CalcError(name + ": expected " + std::to_string(plugin->arity()) +
            " argument(s), got " + std::to_string(args.size()));
    return plugin->call(args);
}