/**
 @file TextInput.cpp
  
 @author Morgan McGuire, graphics3d.com
 
 @cite Based on a lexer written by Aaron Orenstein. 
 
 @created 2001-11-27
 @edited  2003-02-15
 */

#include "../include/G3D/TextInput.h"
#include "../include/G3D/BinaryInput.h"


// These standard C functions are renamed for clarity/naming
// conventions and to return bool, not int.
static inline bool isWhiteSpace(const char c) {
    return isspace(c) != 0;
}

static inline bool isNewline(const char c) {
    return (c == '\n') || (c == '\r');
}

static inline bool isDigit(const char c) {
    return isdigit(c) != 0;
}

static inline bool isLetter(const char c) {
    return isalpha(c) != 0;
}


namespace G3D {


Token TextInput::peek() {
    if (stack.size() == 0) {
        Token t = nextToken();
        push(t);
    }

    return stack.front();
}


Token TextInput::read() {
    if (stack.size() > 0) {
        Token t = stack.front();
        stack.pop_front();
        return t;
    } else {
        return nextToken();
    }
}


void TextInput::push(const Token& t) {
    stack.push_front(t);
}


bool TextInput::hasMore() {
    return
        (bufferLast <= buffer.length() - 1) &&
        (peek()._type != Token::END);
}


int TextInput::popNextChar() {
    // Don't go off the end
    if (bufferLast == (buffer.length() - 1)) {
        return EOF;
    }

    bufferLast++;
    unsigned char c = buffer[bufferLast];
    if (c == '\n') {
        lineNumber++;
        charNumber = 0;
    } else if (c != '\r') {
        charNumber++;
    }

    debugAssert((c >= 0) && (c <= 255));
    return c;
}


Token TextInput::nextToken() {
    Token t;

    t._line      = lineNumber;
    t._character = charNumber;

    char c = popNextChar();
    if (c == EOF) {
        return t;
    }

    bool whitespaceDone = false;
    while (! whitespaceDone) {
        whitespaceDone = true;

        // Consume whitespace
        if (isWhiteSpace(c)) {
            whitespaceDone = false;
            while (isWhiteSpace(c)) {
                c = popNextChar();
            }
        }

        // Comments 
        if (c == '/') {
            int c2 = peekNextChar();

            if (c2 == '/') {
                // Single line comment
                whitespaceDone = false;
                while (! isNewline(c) && (c != EOF)) {
                    c = popNextChar();
                }
            } else if (c2 == '*') {
                // Multi-line comment
                whitespaceDone = false;
                c2 = popNextChar();
                while (! ((c == '*') && (c2 == '/'))) {
                    c = c2;
                    c2 = popNextChar();
                }
                c = popNextChar();
            } else {
                // Just a regular division
                whitespaceDone = true;
            }
        }
    }  // While ! whitespaceDone

    t._line      = lineNumber;
    t._character = charNumber;

    // handle EOF
    if (c == EOF) {
        return t;
    }
    
    switch (c) {

    // Single character tokens are all going to be handled the same way; 
    // by creating that token.
    case '(': 
    case ')':
    case ',':
    case '*':
    case '/':
    case ':':
    case ';':
    case '{':
    case '}':
    case '[':
    case ']':
    case '^':
    case '-':
    case '+':
    case '>':
    case '<':
    case '!':
    case '=':
        t._type = Token::SYMBOL; 
        t._string = c;

        // Complex symbols
        switch (c) {
        case '-':
            // Could be a negative number, -, --, -=, or ->
            switch (peekNextChar()) {
            case '>':
            case '-':
            case '=':
                t._string += popNextChar();
                return t;
            }
            break;

        case '+':
            switch (peekNextChar()) {
            case '+':
            case '=':
                t._string += popNextChar();
                return t;
            }
            break;

        case ':':
            if (peekNextChar() == ':') {
                t._string += popNextChar();
                return t;
            }
            break;

        case '*':
        case '/':
        case '!':
        case '=':
            // ==, *=, /=, !=
            if (peekNextChar() == '=') {
                t._string += popNextChar();
                return t;
            }
            break;

        case '>':
        case '<':
        case '|':
        case '&':
            // >>, <<, <=, >=, |=, ||, &=, &&
            {
                char c2 = peekNextChar();
                if ((c2 == '=') || (c2 == c)) {
                    t._string += popNextChar();
                    return t;
                }
            }
            break;
        }

        return t;

    // .  ..  ...
    case '.':
        {
            t._type = Token::SYMBOL;
            t._string = '.';
            if (peekNextChar() == '.') {
                popNextChar();
                t._string += '.';

                if (peekNextChar() == '.') {
                    // ...
                    t._string += '.';
                    popNextChar();
                } else {
                    // ..
                }
            } else {
                // See if this is a number or a dot

                if (isDigit(peekNextChar())) {
                    // This is a number.  Abort immediately.
                    // The number code will reuse c, so don't
                    // push it back on.
                    break;
                } else {
                    // A single dot
                }
            }
        }
        return t;
    } // switch c


    if (isDigit(c) || (c == '.')) {

        // A number.  Note-- single dots have been
        // parsed already, so a . indicates a number
        // less than 1 in floating point form.
    
        // [0-9]*(\.[0-9]+) or [0-9]+

        t._string = ""; 
        t._type = Token::NUMBER;

        // Read the part before the decimal.
        do {
            t._string += c;
            c = popNextChar();
        } while (isDigit(c));

    
        // Read the decimal, if one exists
        if (c == '.') {
            if (isDigit(peekNextChar())) {
                // The '.' was a decimal point, not the start of a
                // method or range operator
                t._string += c;
                c = popNextChar();

                // Read the part after the decimal
                while (isDigit(c)) {
                    t._string += c;
                    c = popNextChar();
                }
            }
        }

        if (c == 'e') {
            t._string += "e";
            if ((peekNextChar() == '-') || (peekNextChar() == '+')) {
                t._string += popNextChar();
            }

            c = popNextChar();
            while (isDigit(c)) {
                t._string += c;
                c = popNextChar();
            }
        }

        // Push back the extra one we read.
        pushNextChar(c);

        return t;

    } else if (isLetter(c) || (c == '_')) {
        // Identifier or keyword
        // [A-Za-z_][A-Za-z_0-9]*

        t._type = Token::SYMBOL;
        t._string = "";
        do {
            t._string += c;
            c = popNextChar();
        } while (isLetter(c) || isDigit(c) || (c == '_'));


        // put back the extra character we read past the end.
        pushNextChar(c);
        return t;

    } else if (c == '\"') {

        // Quoted string
        t._type = Token::STRING;

        while (true) {
            c = popNextChar();

            if (c == EOF) {
                // END inside a quoted string.
                break;
            }
                
            if (c == '\"') {
                // End of the string
                break;
            }

            if (c == '\\') {
                // Escaped character
                c = popNextChar();
                switch (c) {
                case 'r':
                    t._string += '\r';
                    break;
                case 'n':
                    t._string += '\n';
                    break;
                case 't':
                    t._string += '\t';
                    break;
                case '\\':
                    t._string += '\\';
                    break;
                case '0':
                    t._string += '\0';
                    break;
                case '\"':
                    t._string += '\"';
                    break;
                default:
                    // Some illegal escape sequence; skip it.
                    break;
                } // switch
            } else {
                t._string += c;
            }
        }

        return t;

    } if (c == '\'') {
        t._string = c;
        t._type = Token::SYMBOL;
        return t;
    } // end of special case tokens

    if (c == EOF) {
        t._type = Token::END;
        t._string = "";
        return t;
    }

    // Some unknown token
    debugAssert(false);
    return t;
}


double TextInput::readNumber() {
    Token t(peek());

    if ((t._type == Token::SYMBOL) && ((t._string == "-") || (t._string == "+"))) {
        // Read the token
        t = read();

        // Peek one more token
        Token t2(peek());

        if (t2._type == Token::NUMBER) {
            // Read the second one.
            t2 = read();
            if (t._string == "-") {
                return -t2.number();
            } else {
                return t2.number();
            }
        } else {
            push(t);
            // Push back the first token and throw an exception
            throw WrongTokenType(Token::NUMBER, t._type);
        }

    } else if (t._type == Token::NUMBER) {
        // Consume the token
        return read().number();
    } else {
        throw WrongTokenType(Token::NUMBER, t._type);
        return 0;
    }
}


std::string TextInput::readString() {
    Token t(peek());
    if (t._type == Token::STRING) {
        return read()._string;
    } else {
        throw WrongTokenType(Token::STRING, t._type);
    }
}


std::string TextInput::readSymbol() {
    Token t(peek());
    if (t._type == Token::SYMBOL) {
        return read()._string;
    } else {
        throw WrongTokenType(Token::SYMBOL, t._type);
    }
}


void TextInput::readSymbol(const std::string& symbol) {
    Token t(peek());
    if (t._type == Token::SYMBOL) {
        if (t._string == symbol) {
            // Consume the token
            read();
        } else {
            throw WrongSymbol(symbol, t._string);
        }
    } else {
        throw WrongTokenType(Token::SYMBOL, t._type);
    }
}



TextInput::TextInput(const std::string& filename) {
    init();
    BinaryInput b(filename, G3D_LITTLE_ENDIAN);
    buffer.resize(b.getLength());
    memcpy(buffer.getCArray(), b.getCArray(), buffer.size());
}


TextInput::TextInput(FS fs, const std::string& str) {
    init();
    buffer.resize(str.length());
    memcpy(buffer.getCArray(), str.c_str(), buffer.size());
}

} // namespace

