/**
 @file TextInput.cpp
  
 @author Morgan McGuire, graphics3d.com
 
 @cite Based on a lexer written by Aaron Orenstein. 
 
 @created 2001-11-27
 @edited  2005-01-07
 */

#include "G3D/TextInput.h"
#include "G3D/BinaryInput.h"

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


int TextInput::peekLineNumber() {
    return peek().line();
}


int TextInput::peekCharacterNumber() {
    return peek().character();
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

    return c;
}


Token TextInput::nextToken() {
    Token t;

    t._line         = lineNumber;
    t._character    = charNumber;
	t._type         = Token::END;
	t._extendedType = Token::END_TYPE;

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

            if ((c2 == '/') && options.cppComments) {
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
        } else if ((options.otherCommentCharacter != '\0') &&
                   (c == options.otherCommentCharacter)) {
            // Single line comment
            whitespaceDone = false;
            while (! isNewline(c) && (c != EOF)) {
                c = popNextChar();
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
    case '\\':
    case '(': 
    case ')':
    case ',':
    case '*':
    case '/':
    case ':':
    case ';':
    case '&':
    case '|':
    case '{':
    case '}':
    case '[':
    case ']':
    case '^':
    case '#':
    case '$':
    case '@':
    case '~':
    case '?':
    case '-':
    case '+':
    case '>':
    case '<':
    case '!':
    case '=':
        t._type = Token::SYMBOL; 
		t._extendedType = Token::SYMBOL_TYPE;
        t._string = c;

        // Complex symbols
        switch (c) {
        case '-':
            {
                char c2 = peekNextChar();

                // Could be a negative number, -, --, -=, or ->
                switch (c2) {
                case '>':
                case '-':
                case '=':
                    t._string += popNextChar();
                    return t;
                }

                // need to read ahead 1 more
                c2 = popNextChar();

                if (options.signedNumbers && 
                    (isDigit(c2) || 
                    ((c2 == '.') && isDigit(peekNextChar())))) {
                    // Negative number
                    c = c2;
                    goto numLabel;
                } else {
                    pushNextChar(c2);
                }
            }

            break;

        case '+':
            {
                char c2 = peekNextChar();

                switch (c2) {
                case '+':
                case '=':
                    t._string += popNextChar();
                    return t;
                }

                // need to read ahead 1 more
                c2 = popNextChar();

                if (options.signedNumbers &&
                    (isDigit(c2) || 
                    ((c2 == '.') && isDigit(peekNextChar())))) {
                    // Positive number
                    c = c2;
                    goto numLabel;
                } else {
                    pushNextChar(c2);
                }
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
        case '~':
        case '=':
            // ==, *=, /=, !=, ~=
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

        case '\\':
            if ((options.otherCommentCharacter != '\0') &&
                (peekNextChar() == options.otherCommentCharacter)) {
                // Return the raw comment character instead of
                // the backslash
                t._string = popNextChar();
            }
        }

        return t;

    // .  ..  ...
    case '.':
        {
            t._type = Token::SYMBOL;
			t._extendedType = Token::SYMBOL_TYPE;
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

numLabel:
    if (isDigit(c) || (c == '.')) {

        // A number.  Note-- single dots have been
        // parsed already, so a . indicates a number
        // less than 1 in floating point form.
    
        // [0-9]*(\.[0-9]) or [0-9]+ or 0x[0-9,A-F]+

        if (t._string != "-") {
            // If we picked up a leading "-" sign above, keep it,
            // otherwise drop the string parsed thus far
            t._string = "";
        }
        t._type = Token::NUMBER;
		if (c == '.') {
			t._extendedType = Token::FLOATING_POINT_TYPE;
		} else {
			t._extendedType = Token::INTEGER_TYPE;
		}

        if ((c == '0') && (peekNextChar() == 'x')) {
            // Hex number
            t._string += "0x";

            // skip the x
            popNextChar();

            c = popNextChar();
            while (isDigit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'))) {
                t._string += c;
                c = popNextChar();
            }

        } else {

            // Read the part before the decimal.
            do {
                t._string += c;
                c = popNextChar();
            } while (isDigit(c));

    
            // Read the decimal, if one exists
            if (c == '.') {
				t._extendedType = Token::FLOATING_POINT_TYPE;

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

            if ((c == 'e') || (c == 'E')) {
				t._extendedType = Token::FLOATING_POINT_TYPE;
                t._string += c;
                if ((peekNextChar() == '-') || (peekNextChar() == '+')) {
                    t._string += popNextChar();
                }

                c = popNextChar();
                while (isDigit(c)) {
                    t._string += c;
                    c = popNextChar();
                }
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

        // Double quoted string
		parseQuotedString('\"', t);
        return t;

    } else if (c == '\'') {
		if (options.singleQuotedStrings) {
			// Single quoted string
			parseQuotedString('\'', t);
		} else {
			t._string = c;
			t._type = Token::SYMBOL;
		}
        return t;

    } // end of special case tokens

    if (c == EOF) {
        t._type = Token::END;
        t._extendedType = Token::END_TYPE;
        t._string = "";
        return t;
    }

    // Some unknown token
    debugAssert(false);
    return t;
}


void TextInput::parseQuotedString(char delimiter, Token& t) {
    t._type = Token::STRING;

	if (delimiter == '\'') {
		t._extendedType = Token::SINGLE_QUOTED_TYPE;
	} else {
		t._extendedType = Token::DOUBLE_QUOTED_TYPE;
	}

    while (true) {
        char c = popNextChar();

        if (c == EOF) {
            // END inside a quoted string.
            break;
        }
            
        if (c == delimiter) {
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
            case '0':
                t._string += '\0';
                break;

            case '\\':
            case '\"':
            case '\'':
                t._string += c;
                break;

            default:
                if ((c == options.otherCommentCharacter) && 
                    (options.otherCommentCharacter != '\0')) {
                    t._string += options.otherCommentCharacter;
                } 
                // otherwise, some illegal escape sequence; skip it.
            } // switch
        } else {
            t._string += c;
        }
    }
}


double TextInput::readNumber() {
    Token t(peek());

    if ((t._type == Token::SYMBOL) && (! options.signedNumbers) && ((t._string == "-") || (t._string == "+"))) {
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
            throw WrongTokenType(sourceFile, lineNumber, charNumber, Token::NUMBER, t._type);
        }

    } else if (t._type == Token::NUMBER) {
        // Consume the token
        return read().number();
    } else {
        throw WrongTokenType(sourceFile, lineNumber, charNumber, Token::NUMBER, t._type);
        return 0;
    }
}


std::string TextInput::readString() {
    Token t(peek());
    if (t._type == Token::STRING) {
        return read()._string;
    } else {
        throw WrongTokenType(sourceFile, lineNumber, charNumber, Token::STRING, t._type);
    }
}


std::string TextInput::readSymbol() {
    Token t(peek());
    if (t._type == Token::SYMBOL) {
        return read()._string;
    } else {
        throw WrongTokenType(sourceFile, lineNumber, charNumber, Token::SYMBOL, t._type);
    }
}


void TextInput::readSymbol(const std::string& symbol) {
    Token t(peek());
    if (t._type == Token::SYMBOL) {
        if (t._string == symbol) {
            // Consume the token
            read();
        } else {
            throw WrongSymbol(sourceFile, lineNumber, charNumber, symbol, t._string);
        }
    } else {
        throw WrongTokenType(sourceFile, lineNumber, charNumber, Token::SYMBOL, t._type);
    }
}



TextInput::TextInput(const std::string& filename, const Options& opt) : options(opt) {
    init();
    BinaryInput input(filename, G3D_LITTLE_ENDIAN);
    sourceFile = filename;
    int n = input.size();
    buffer.resize(n + 1);
    System::memcpy(buffer.getCArray(), input.getCArray(), n);
    buffer.last() = EOF;
}


TextInput::TextInput(FS fs, const std::string& str, const Options& opt) : options(opt) {
    init();
    if (str.length() < 14) {
        sourceFile = std::string("\"") + str + "\"";
    } else {
        sourceFile = std::string("\"") + str.substr(0, 10) + "...\"";
    }
    buffer.resize(str.length() + 1);
    System::memcpy(buffer.getCArray(), str.c_str(), buffer.size() - 1);
    buffer.last() = EOF;
}

///////////////////////////////////////////////////////////////////////////////////

TextInput::TokenException::TokenException(
    const std::string&  src,
    int                 ln,
    int                 ch) : sourceFile(src), line(ln), character(ch) {

    message = format("%s(%d) : ", sourceFile.c_str(), line);
}

///////////////////////////////////////////////////////////////////////////////////

static const char* tokenTypeToString(Token::Type t) {
    switch (t) {
    case Token::SYMBOL:
        return "Token::SYMBOL";
    case Token::STRING:
        return "Token::STRING";
    case Token::NUMBER:
        return "Token::NUMBER";
    case Token::END:
        return "Token::END";
    default:
        debugAssertM(false, "Fell through switch");
        return "?";
    }
}

TextInput::WrongTokenType::WrongTokenType(
    const std::string&  src,
    int                 ln,
    int                 ch,
    Token::Type         e,
    Token::Type         a) :
    TokenException(src, ln, ch), expected(e), actual(a) {
         
    message += 
        format("Expected token of type %s, found type %s.",
        tokenTypeToString(e), tokenTypeToString(a));
}


TextInput::WrongSymbol::WrongSymbol(
    const std::string&  src,
    int                 ln,
    int                 ch,
    const std::string&  e,
    const std::string&  a) : 
    TokenException(src, ln, ch), expected(e), actual(a) {

    message += 
        format("Expected symbol '%s', found symbol '%s'.",
                e.c_str(), a.c_str());
}


void deserialize(bool& b, TextInput& ti) {
    b = ti.readSymbol() == "true";
}


void deserialize(int& b, TextInput& ti) {
    b = iRound(ti.readNumber());
}


void deserialize(uint8& b, TextInput& ti) {
    b = iRound(ti.readNumber());
}


void deserialize(double& b, TextInput& ti) {
    b = ti.readNumber();
}


void deserialize(float& b, TextInput& ti) {
    b = ti.readNumber();
}

} // namespace

