//
//  main.cpp
//  Reverse Polish Calculator
//
//  Created by Gary Powell on 7/1/19.
//  Copyright © 2019 Guest User. All rights reserved.
//

#include <algorithm> // advance
#include <cassert>
#include <cctype> // isdigit
#include <exception>
#include <iostream>
#include <string>
#include <stack>

class Token {
    bool is_number {false};
    union {
        int  number {0};
        char token;
    } parsed;
    size_t numberOfCharsToAdvance {0};
    
public:
    constexpr size_t getNumberOfCharsToAdvance() const noexcept {
        return numberOfCharsToAdvance;
    }
    
    void incrementNumberOfCharsToAdvance() noexcept {
        ++numberOfCharsToAdvance;
    }
    
    constexpr bool isNumber() const noexcept {
        return is_number;
    }
    
    constexpr int getNumber() const {
        return parsed.number;
    }
    
    void setNumber(int val) noexcept {
        is_number = true;
        parsed.number = val;
    }
    
    constexpr char getToken() const noexcept {
        return parsed.token;
    }
    
    void setToken(char token) noexcept {
        is_number = false;
        parsed.token = token;
    }
};

int binaryCompute(int lhs, int rhs, char token) {
    switch (token) {
    case '+':
        return lhs + rhs;
    case '-':
        return lhs - rhs;
    case '*':
        return lhs * rhs;
    case '/':
        return lhs / rhs;
    }
    return 0;
}

int unaryCompute(int rhs, char token) {
    switch (token) {
    case '-':
        return -rhs;
    }
    return rhs;
}

using IntStack = std::stack<int>;

// extremely simple parser. We don't handle "negative signed vals. Or error inputs."
Token getNextToken(std::string::const_iterator inputIter, std::string::const_iterator endIter)
{
    using std::isdigit;
    using std::stoi;
    Token result;
    std::string numberStr;
    
    while (inputIter != endIter) {
        char c = *inputIter;
        ++inputIter;
        if (isdigit(c)) {
            numberStr += c;
            result.incrementNumberOfCharsToAdvance();
        } else {
            switch (c) {
                case '+': case '-': case '*': case '/':
                    if (numberStr.empty()) {
                        result.setToken(c);
                        result.incrementNumberOfCharsToAdvance();
                    }
                    goto done;
                case ',':
                    result.incrementNumberOfCharsToAdvance();
                    goto done;
            }
        }
    }
done:
    
    if (!numberStr.empty())
    {
        result.setNumber(stoi(numberStr));
    }
    
    return result;
}

int calculate(std::string input)
{
    using std::advance;
    int result (0);
    IntStack inputStack;
    auto iterInput(input.begin());
    
    // the first token must be a number;
    auto token = getNextToken(input.begin(), input.end());
    if (token.isNumber() ) {
        result = token.getNumber();
        advance(iterInput, token.getNumberOfCharsToAdvance());
    } else {
        throw std::runtime_error("First token must be a number");
    }
    
    while(iterInput != input.end()) {
        token = getNextToken(iterInput, input.end());
        if (token.isNumber()) {
            inputStack.push(token.getNumber());
        }
        else {
            if (inputStack.empty()) {
                result = unaryCompute(result, token.getToken());
            } else {
                auto inputStackVal {inputStack.top()};
                inputStack.pop();
                result = binaryCompute(result, inputStackVal, token.getToken());
            }
        }
        advance(iterInput, token.getNumberOfCharsToAdvance());
        
    }
    return result;
}

int main(int argc, const char * argv[]) {
    
    using std::string;
    using std::cout;
    using std::endl;
    using std::pair;
    pair<string,int> inputs[] {{("2,3+"),5},
                               {("1-"), -1},
        {("1"),1},
        {("10,20*"),200},
        {("10,5/"),2},
        {("10,4-"),6},
        {("2,3+5+"),10},
        {("7,14,7-+"),14}};
    
    for (auto input: inputs) {
        try {
            auto result = calculate(input.first);
            cout << input.first << " = " << result << endl;
            assert(result == input.second);
        
        } catch (std::exception const &e) {
            cout << "Calculator failure exception = \"" << e.what() << "\"" << endl;
        }
    }
    
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
