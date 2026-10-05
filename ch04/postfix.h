// The two stack applications Chapter 4 walks through. Both are here rather
// than in the text alone so that the traces printed in the chapter are
// checked by the test suite.
#ifndef CH04_POSTFIX_H
#define CH04_POSTFIX_H

#include "Stack.h"

#include <sstream>
#include <string>

// Are the brackets balanced and correctly nested? Every opener is pushed and
// every closer must match the most recent unmatched opener, which is exactly
// what the top of a stack holds.
inline bool balanced(const std::string& text) {
    Stack<char> open;
    for (const char c : text) {
        if (c == '(' || c == '[' || c == '{') {
            open.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            // a closer, nothing open
            if (open.is_empty()) return false;
            const char want =
                (c == ')') ? '(' : (c == ']') ? '[' : '{';
            // the wrong opener
            if (open.pop() != want) return false;
        }
    }
    return open.is_empty();    // nothing left unclosed
}

// Evaluate a postfix expression of single integers and + - * /. Operands are
// pushed; an operator pops its two arguments and pushes the result.
inline int evaluate_postfix(const std::string& expression) {
    Stack<int> operands;
    std::istringstream in(expression);
    std::string token;
    while (in >> token) {
        const bool is_operator = token == "+" || token == "-"
                              || token == "*" || token == "/";
        if (is_operator) {
            if (operands.size() < 2)
                throw std::out_of_range("malformed expression");
            // the second operand came off first
            const int right = operands.pop();
            const int left  = operands.pop();
            if (token == "+") operands.push(left + right);
            if (token == "-") operands.push(left - right);
            if (token == "*") operands.push(left * right);
            if (token == "/") operands.push(left / right);
        } else {
            operands.push(std::stoi(token));
        }
    }
    if (operands.size() != 1) throw std::out_of_range("malformed expression");
    return operands.pop();
}

#endif
