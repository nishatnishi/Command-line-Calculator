
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <cctype>

using namespace std;

map<string, double> variables;

// Skip spaces in input stream
void skipSpaces(istringstream& in) {
    while (isspace(in.peek())) in.get();
}

// Parsing factor (number, variable or expression in parentheses)
double parseFactor(istringstream& in);

// Parsing exponentiation '^'
double parsePower(istringstream& in) {
    double base = parseFactor(in);
    skipSpaces(in);
    while (in.peek() == '^') {
        in.get();
        double exponent = parsePower(in);
        base = pow(base, exponent);
    }
    return base;
}

// Parsing term (Multiplication/Division/Modulo)
double parseTerm(istringstream& in) {
    double val = parsePower(in);
    skipSpaces(in);
    while (in.peek() == '*' || in.peek() == '/' || in.peek() == '%') {
        char op = in.get();
        double next = parsePower(in);
        if (op == '*') val *= next;
        else if (op == '/') {
            if (next == 0) {
                cerr << "Error: Division by Zero\n";
                exit(1);
            }
            val /= next;
        }
        else if (op == '%') {
            if (next == 0) {
                cerr << "Error: Division by Zero\n";
                exit(1);
            }
            val = fmod(val, next);
        }
        skipSpaces(in);
    }
    return val;
}

// Parsing full expression (+ and -)
double parseExpression(istringstream& in) {
    double val = parseTerm(in);
    skipSpaces(in);
    while (in.peek() == '+' || in.peek() == '-') {
        char op = in.get();
        double next = parseTerm(in);
        if (op == '+') val += next;
        else val -= next;
        skipSpaces(in);
    }
    return val;
}

// Parsing numbers, variables, parentheses
double parseFactor(istringstream& in) {
    skipSpaces(in);
    if (in.peek() == '(') {
        in.get();
        double val = parseExpression(in);
        skipSpaces(in);
        if (in.get() != ')') {
            cerr << "Error: Missing closing parenthesis\n";
            exit(1);
        }
        return val;
    }
    else if (isalpha(in.peek())) {
        string var;
        while (isalnum(in.peek()) || in.peek() == '_') var += in.get();
        if (variables.count(var)) return variables[var];
        cerr << "Error: Undefined variable '" << var << "'\n";
        exit(1);
    }
    else if (isdigit(in.peek()) || in.peek() == '.') {
        double num;
        in >> num;
        return num;
    }
    else if (in.peek() == '-') {
        in.get();
        return -parseFactor(in);
    }
    else {
        cerr << "Error: Unexpected character '" << (char)in.peek() << "'\n";
        exit(1);
    }
}

// Process Assignment (Variable = Expression)
void processAssignment(const string& line, ofstream& outFile) {
    auto pos = line.find('=');
    if (pos == string::npos) {
        cerr << "Error: Invalid assignment\n";
        exit(1);
    }
    string var = line.substr(0, pos);
    string expr = line.substr(pos + 1);

    // Trim whitespaces
    var.erase(0, var.find_first_not_of(" \t"));
    var.erase(var.find_last_not_of(" \t") + 1);

    if (var.empty() || !isalpha(var[0])) {
        cerr << "Error: Invalid variable name\n";
        exit(1);
    }

    istringstream exprStream(expr);
    double val = parseExpression(exprStream);
    variables[var] = val;

    outFile << var << " = " << val << endl;
}

// Main Function
int main() {
    ifstream inFile("input.txt");
    ofstream outFile("output.txt");

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Unable to open input/output files\n";
        return 1;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        if (line.find('=') != string::npos) {
            processAssignment(line, outFile);
        } else {
            istringstream exprStream(line);
            double result = parseExpression(exprStream);
            outFile << result << endl;
        }
    }

    inFile.close();
    outFile.close();

    return 0;
}
