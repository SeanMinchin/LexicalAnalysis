#ifndef PA3_H
#define PA3_H

#include <utility>
#include <memory>
#include <vector>
#include <set>

// implement custom stack class using linked list framework
template <typename T>
class Stack {
private:
    // create Node class that only the Stack class has access to
    template <typename U>
    class Node {
    public:
        std::shared_ptr<Node<U>> next;
        U data;
        explicit Node(U data): next(nullptr), data(data) {}
    };
    std::shared_ptr<Node<T>> top;
public:
    explicit Stack(): top(nullptr) {}
    // destruct stack by popping the top element until the stack is empty
    ~Stack() {
        while(!empty()) {
            pop();
        }
    }
    // if top element of stack is null pointer, then stack is empty
    bool empty() const {
        return top == nullptr;
    }
    void push(T value) {
        std::shared_ptr<Node<T>> temp = std::make_shared<Node<T>>(value);
        if(empty()) {
            top = temp;
        } else {
            temp->next = top;
            top = temp;
        }
    }
    // remove the top element or throw empty stack exception
    void pop() {
        if(empty()) {
            throw std::runtime_error{"Stack is empty"};
        }
        top = top->next;
    }
    // return a constant reference to the top element of the stack or throw empty stack exception
    const T& peek() const {
        if(empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top->data;
    }
};

// create struct of token types
// syntax errors may be repeated so make vector, legals types should only be printed once so push to sets
struct Tokens {
    std::set<std::string> keywords;
    std::set<std::string> identifiers;
    std::set<std::string> constants;
    std::set<std::string> operators;
    std::set<std::string> delimiters;
    std::vector<std::string> syntaxErrors;
};

class InputParser {
private:
    Stack<std::string> stack;
    bool isInRangeInclusive(const char& c, char lowerBound, char upperBound);
    bool isLowerCaseLetter(const char& c);
    bool isDigit(const char& c);
    bool isTokenForStack(const std::string& token);
    std::vector<std::string> formattedVector;
public:
    int getNestedLoopDepth();
    void analyzeInput(Tokens& tokens);
    void setFormattedVector(const std::vector<std::string>& vector);
    bool isKeyword(const std::string& token);
    bool isOperator(const std::string& token);
    bool isConstant(const std::string& token);
    bool isDelimiter(const std::string& token);
    bool isIdentifier(const std::string& token);
};

#endif //PA3_H