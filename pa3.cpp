#include "pa3.h"
#include <iostream>
#include <fstream>

bool isDelimiterLeft(char c) {
    return c == '(';
}

bool isDelimiterRight(char c) {
    return c == ')' || c == ';' || c == ',';
}

std::vector<std::string> getInputFromFile(const std::string& fileName) {
    std::ifstream infile;
    infile.open(fileName);
    // if unable to open file, end program
    if(!infile) {
        std::cout << "Unable to open specified file";
        exit(-1);
    }

    // read the file line by line
    // push each line as a string into a vector
    std::string line;
    std::vector<std::string> text;
    while(std::getline(infile, line)) {
        text.push_back(line);
    }
    // close when empty and return vector of input file lines
    infile.close();
    return text;
}

// turn vector of input lines into vector of each individual token
// separate all the tokens by whitespace and split the string into a vector based on whitespace
std::vector<std::string> parse(const std::vector<std::string>& text) {
    std::vector<std::string> tempInput = text;

    // remove empty line strings from the input file
    for(unsigned int i = 0; i < tempInput.size(); ++i) {
        if(tempInput[i].empty()) {
            tempInput.erase(tempInput.begin() + i);
        }
    }

    // replace '++' operator with '&'
    for(std::string& token : tempInput) {
        for(unsigned int i = 0; i < token.length() - 1; ++i) {
            if(token.at(i) == '+' && token.at(i + 1) == '+') {
                token.replace(i, 2, "&");
                i = 0;
            }
        }
    }

    // add a space after '('
    for(std::string& token : tempInput) {
        for(unsigned int i = 0; i < token.length() - 1; ++i) {
            if(isDelimiterLeft(token.at(i)) && token.at(i + 1) != ' ') {
                token.insert(i + 1, " ");
                i = 0;
            }
        }
    }

    // add a space before ')' and ',' and ';'
    for(std::string& token : tempInput) {
        for(unsigned int i = 0; i < token.length(); ++i) {
            if(isDelimiterRight(token.at(i)) && token.at(i - 1) != ' ') {
                token.insert(i, " ");
                i = 0;
            }
        }
    }

    // add a space before and after '='
    for(std::string& token : tempInput) {
        for(unsigned int i = 0; i < token.length() - 1; ++i) {
            if(token.at(i) == '=' && token.at(i - 1) != ' ' && token.at(i + 1) != ' ') {
                token.replace(i, 1, " = ");
                i = 0;
            }
        }
    }

    // remove leading whitespace from lines
    for(std::string& token : tempInput) {
        unsigned int finalIndex = 0;
        while(finalIndex < token.length() && token.at(finalIndex) == ' ') {
            ++finalIndex;
        }
        token.replace(0, finalIndex, "");
    }

    // build string
    std::string tempString;
    for(const std::string& token : tempInput) {
        tempString.append(token).append(" ");
    }
    // trim final space at end of string
    tempString.replace(tempString.length(), 1, "");

    // insert each token separately onto the vector
    // as long as each character in the string of formatted input is not a space, keep appending the new string
    std::vector<std::string> formattedVector;
    std::string builder;
    for(const char& c : tempString) {
        if(c != ' ') {
            builder += c;
        } else {
            // check for garbage/new line/whitespace characters at end of tokens and trim them off
            char tempChar = builder.at(builder.size() - 1);
            if(!(tempChar >= 33 && tempChar <= 126)) {
                builder.pop_back();
            }
            // when a space is reached, push the string onto the vector
            formattedVector.push_back(builder);
            // clear the string builder
            builder = "";
        }
    }
    // add final string to end of vector signifying end of text
    formattedVector.emplace_back("end_file");
    return formattedVector;
}

void InputParser::setFormattedVector(const std::vector<std::string>& vector) {
    this->formattedVector = vector;
}

// determine if character is in ASCII range
bool InputParser::isInRangeInclusive(const char& c, char lowerBound, char upperBound) {
    return c >= lowerBound && c <= upperBound;
}

bool InputParser::isLowerCaseLetter(const char& c) {
    return isInRangeInclusive(c, 'a', 'z');
}

bool InputParser::isDigit(const char& c) {
    return isInRangeInclusive(c, '0', '9');
}

bool InputParser::isKeyword(const std::string &token) {
    return token == "FOR" || token == "BEGIN" || token == "END";
}

bool InputParser::isOperator(const std::string &token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "=" || token == "&";
}

bool InputParser::isDelimiter(const std::string &token) {
    return token == "," || token == ";" || token == "(" || token == ")";
}

// identifiers should be strings of all lowercase letters
bool InputParser::isIdentifier(const std::string &token) {
    for(const char& c : token) {
        if(!isLowerCaseLetter(c)) {
            return false;
        }
    }
    return true;
}

// constants should be numbers
bool InputParser::isConstant(const std::string &token) {
    for(const char& c : token) {
        if(!isDigit(c)) {
            return false;
        }
    }
    return true;
}

// replace all legal beginnings of for loops 'FOR ( ) BEGIN' with 'START' by modifying substrings
// internally represent the start of a for loop with START
int InputParser::getNestedLoopDepth() {
    Stack<std::string> tempStack;
    std::vector<std::string> tempVector;

    // create a single string with all the keywords and parentheses delimiters
    for(const std::string& token : formattedVector) {
        if(isKeyword(token)|| token == "(" || token == ")") {
            tempVector.push_back(token);
        }
    }
    std::string tempString;
    for(const std::string& token : tempVector) {
        tempString.append(token);
    }

    // case 1: replace all instances of "END" with " END "
    // case 2: replace all instances of "FOR()BEGIN" with " START "
    std::string str1 = "END";
    std::string str2 = "FOR()BEGIN";
    std::string replace1 = " END ";
    std::string replace2 = " START ";
    std::string::size_type n1 = 0;
    std::string::size_type n2 = 0;
    while((n1 = tempString.find(str1, n1)) != std::string::npos) {
        tempString.replace(n1, str1.size(), replace1);
        n1 += replace1.size();
    }
    while((n2 = tempString.find(str2, n2)) != std::string::npos) {
        tempString.replace(n2, str2.size(), replace2);
        n2 += replace2.size();
    }

    // add START and END to stack
    // build tokens by concatenating characters until whitespace is reached
    std::string builder;
    for(char c : tempString) {
        if(c != ' ') {
            builder += c;
        } else {
            if(builder == "START" || builder == "END") {
                tempStack.push(builder);
            }
            // clear string builder once a complete token string is pushed to stack
            builder = "";
        }
    }

    // stack now contains either START or END
    // every time an END is encountered, increment the current depth
    // compare current depth to max depth to check if max depth should be changed
    // if START is encountered, decrement the current depth
    int currentDepth = 0;
    int highestDepth = 0;
    while(!tempStack.empty()) {
        if(tempStack.peek() == "END") {
            ++currentDepth;
        } else if(tempStack.peek() == "START") {
            --currentDepth;
        }
        if(currentDepth > highestDepth) {
            highestDepth = currentDepth;
        }
        tempStack.pop();
    }

    // if the calculated max depth is less than 0 that means the real depth is 0
    int maxDepth = highestDepth - currentDepth;
    if(maxDepth < 0) {
        return 0;
    }
    // if current depth is a negative number actual depth is absolute value of that
    if(currentDepth < 0) {
        return 0 - currentDepth;
    }
    return maxDepth;
};

// get name of text file from user
std::string defineTextFileName() {
    std::string fileName;
    std::cout << "Please enter the name of the input file:" << std::endl;
    std::cin >> fileName;
    std::cout << std::endl;
    if(fileName.empty()) {
        std::cout << "Error: file name is empty" << std::endl;
        exit(-1);
    }
    return fileName;
}

// parse vector of each token by comparing delimiters and keywords to their expected previous token
// push delimiters and keywords to stack and peek to see if user code is in order
void InputParser::analyzeInput(Tokens& tokens) {
    int extraLeftParentheses = 0;
    int extraRightParentheses = 0;
    int leftParenthesesCount = 0;
    int rightParenthesesCount = 0;
    for(const std::string& token : formattedVector) {
        if(token == "BEGIN") {
            break;
        } else if(token == "(") {
            ++leftParenthesesCount;
        } else if(token == ")") {
            ++rightParenthesesCount;
        }
    }
    if(leftParenthesesCount > rightParenthesesCount) {
        extraLeftParentheses = leftParenthesesCount - rightParenthesesCount;
    } else if(rightParenthesesCount > leftParenthesesCount) {
        extraRightParentheses = rightParenthesesCount - leftParenthesesCount;
    }

    int forKeywordCount = 0;
    int endKeywordCount = 0;
    // traverse through the input tokens
    for(const std::string& token : formattedVector) {
        // add specific tokens to their corresponding sets
        if(isIdentifier(token)) {
            tokens.identifiers.insert(token);
        } else if(isConstant(token)) {
            tokens.constants.insert(token);
        } else if(isOperator(token)) {
            // make sure to add '++' instead of its internal representation of '&'
            if(token == "&") {
                tokens.operators.insert("++");
            } else {
                tokens.operators.insert(token);
            }
        } else if(isDelimiter(token) || isKeyword(token)) {
            if(token == ";" || token == ",") {
                tokens.delimiters.insert(token);
            } else if(isKeyword(token)) {
                tokens.keywords.insert(token);
            }

            // first check if the current token is the expected token
            // if so, push current token to stack
            // check if current token is a duplicate of previous: "( (" or ") )"  or "BEGIN BEGIN" or "FOR FOR"
            // if so, add duplicate token to syntax errors and don't push to stack
            // check if token is neither expected nor a duplicate
            // if so, push previous expected token to stack and push current token to stack
            if(token == "FOR") {
                stack.push(token);
                ++forKeywordCount;
            } else if(token == "(") {
                if(!stack.empty() && stack.peek() == "FOR") {
                    stack.push(token);
                } else {
                    stack.push("FOR");
                    stack.push(token);
                }
            } else if(token == ")") {
                if(!stack.empty() && stack.peek() == "(") {
                    stack.push(token);
                } else {
                    stack.push("(");
                    stack.push(token);
                }
            } else if(token == "BEGIN") {
                if(!stack.empty() && stack.peek() == ")") {
                    stack.push(token);
                } else if(stack.peek() == "(") {
                    tokens.syntaxErrors.emplace_back("BEGIN");
                } else {
                    stack.push(")");
                    stack.push(token);
                }
            } else if(token == "END") {
                ++endKeywordCount;
                if(!stack.empty() && (stack.peek() == "BEGIN" || stack.peek() == "END")) {
                    stack.push(token);
                } else {
                    stack.push("BEGIN");
                    stack.push(token);
                    tokens.syntaxErrors.emplace_back("BEGIN");
                }
            }

            // if something is missing, push it to the stack based on the expected token
            // determine expected token by peeking the stack to see the last token
        } else {
            if(stack.peek() == "FOR") {
                stack.push("(");
            } else if(stack.peek() == "(") {
                stack.push(")");
            } else if(stack.peek() == ")") {
                stack.push("BEGIN");
                tokens.syntaxErrors.emplace_back("BEGIN");
            }
            // ignore token signifying end of file
            if(token != "end_file") {
                tokens.syntaxErrors.push_back(token);
            }
        }

        // check for missing end keywords
        // either missing nested for missing at file end
        if(isTokenForStack(token) && stack.peek() == "BEGIN") {
            stack.push("END");
        } else if(token == "end_file" && stack.peek() != "END") {
            stack.push("END");
        }
    }

    // add end statements to syntax errors based on missing end statements
    if(forKeywordCount > endKeywordCount) {
        for(int i = 1; i <= forKeywordCount - endKeywordCount; ++i) {
            tokens.syntaxErrors.emplace_back("END");
        }
    }

    // add extra unbalanced parentheses to syntax errors list
    if(extraLeftParentheses != 0) {
        for(int i = 1; i <= extraLeftParentheses; ++i) {
            tokens.syntaxErrors.emplace_back("(");
        }
    }
    if(extraRightParentheses != 0) {
        for(int i = 1; i <= extraRightParentheses; ++i) {
            tokens.syntaxErrors.emplace_back(")");
        }
    }
}

// check if token is not 'end' but should be pushed to stack
bool InputParser::isTokenForStack(const std::string& token) {
    return token == "FOR" || token == "(" || token == ")";
}

void printTypes(Tokens& tokens) {
    std::cout << "Keywords: ";
    for(const std::string& token : tokens.keywords) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::cout << "Identifier: ";
    for(const std::string& token : tokens.identifiers) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::cout << "Constant: ";
    for(const std::string& token : tokens.constants) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::cout << "Operators: ";
    for(const std::string& token : tokens.operators) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::cout << "Delimiter: ";
    for(const std::string& token : tokens.delimiters) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;
}

void printSyntaxErrors(Tokens& tokens) {
    std::cout << "Syntax Error(s): ";
    if(tokens.syntaxErrors.empty()) {
        std::cout << "NA";
    }
    for(const std::string& token : tokens.syntaxErrors) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;
}

void printDepthOfNestedLoop(InputParser& parser) {
    std::cout << "The depth of nested loop(s) is " << parser.getNestedLoopDepth() << std::endl;
}

int main() {
    InputParser parser;
    Tokens tokensToPrint;

    parser.setFormattedVector(parse(getInputFromFile(defineTextFileName())));
    parser.analyzeInput(tokensToPrint);

    printDepthOfNestedLoop(parser);
    std::cout << std::endl;
    printTypes(tokensToPrint);
    std::cout << std::endl;
    printSyntaxErrors(tokensToPrint);
    return 0;
}