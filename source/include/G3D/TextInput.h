/**
 @file TextInput.h

 Simple text lexer/tokenizer.

 @maintainer Morgan McGuire, morgan@graphics3d.com

 @cite Based on a lexer written by Aaron Orenstein. 

 @created 2002-11-27
 @edited  2005-09-24

 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_TEXTINPUT_H
#define G3D_TEXTINPUT_H

#include "G3D/Array.h"
#include <string>
#include <queue>
#include <ctype.h>
#include <stdio.h>

namespace G3D {

/**
 For use with TextInput.
 */
class Token {
public:
    /**
     Strings are enclosed in quotes, symbols are not.
     */
    enum Type {STRING, SYMBOL, NUMBER, END};

	/**
	 More detailed type information than Type.
	 */
	enum ExtendedType {
		SINGLE_QUOTED_TYPE, 
		DOUBLE_QUOTED_TYPE, 
		SYMBOL_TYPE,
		INTEGER_TYPE, 
		FLOATING_POINT_TYPE, 
		END_TYPE};

private:
    friend class TextInput;

    std::string             _string;
    int                     _line;
    int                     _character;
    Type                    _type;
	ExtendedType            _extendedType;

public:

    Token() : _string(""), _line(0), _character(0), _type(END), _extendedType(END_TYPE) {}

    Token(Type t, ExtendedType e, const std::string& s, int L, int c) : _string(s), _line(L), _character(c), _type(t), _extendedType(e) {}

    Type type() const {
        return _type;
    }

    ExtendedType extendedType() const {
        return _extendedType;
    }

	/** The value of a single or double quote string, not including the quotes, or the name of a symbol. */
    std::string string() const {
        return _string;
    }

	/** Line from which this token was parsed.  Starts at 1. */
    int line() const {
        return _line;
    }
	/** Character position from which this token was parsed.  Starts at 1. */
    int character() const {
        return _character;
    }

    /** Return the numeric value for a number type. */
    double number() const {
        if (_type == NUMBER) {
            double n;
            if ((_string.length() > 2) &&
                (_string[0] == '0') &&
                (_string[1] == 'x')) {
                // Hex
                uint32 i;
                sscanf(_string.c_str(), "%x", &i);
                n = i;
            } else {
                sscanf(_string.c_str(), "%lg", &n);
            }
            return n;
        } else {
            return 0.0;
        }
    }


};


/**
 A simple tokenizer for reading text files.  TextInput handles C++ like
 text including single line comments, block comments, quoted strings with
 escape sequences, and operators.  TextInput recognizes four categories of
 tokens, which are separated by white space, quotation marks, or the end of 
 a recognized operator:

  <DT><CODE>Token::SINGLE_QUOTED_TYPE</CODE> string of characters surrounded by single quotes, e.g., 'x', '\0', 'foo'.
  <DT><CODE>Token::DOUBLE_QUOTED_TYPE</CODE> string of characters surrounded by double quotes, e.g., "x", "abc\txyz", "b o b".
  <DT><CODE>Token::SYMBOL_TYPE</CODE> legal C++ operators, keywords, and identifiers.  e.g., >=, Foo, _X, class, {
  <DT><CODE>Token::INTEGER_TYPE</CODE> numbers without decimal places or exponential notation. e.g., 10, 0x17F, 32, 0, -155
  <DT><CODE>Token::FLOATING_POINT_TYPE</CODE> numbers with decimal places or exponential nottion. e.g., 1e3, -1.2, .4, 0.5

 <P>The special ".." and "..." tokens are recognized in addition to normal C++ operators.

 Negative numbers are handled specially because of the ambiguity between unary minus and negative numbers-- 
 see the note on TextInput::read.

  TextInput does not have helper functions for types with non-obvious formatting, or 
  helpers that would be redundant.  Use the serialize methods instead for 
  parsing specific types like int, Vector3, and Color3.

  Inside quoted strings escape sequences are converted.  Thus the string
  token for ["a\nb"] is 'a', followed by a newline, followed by 'b'.  Outside
  of quoted strings, escape sequences are not converted, so the token sequence
  for [a\nb] is symbol 'a', symbol '\', symbol 'nb' (this matches what a C++ parser
  would do).  The exception is that a specified TextInput::Options::otherCommentCharacter
  preceeded by a backslash is assumed to be an escaped comment character and is
  returned as a symbol token instead of being parsed as a comment 
  (this is what a LaTex or VRML parser would do).

  <B>Examples</B>

  <PRE>
  TextInput ti(TextInput::FROM_STRING, "name = \"Max\", height = 6");

  Token t;

  t = ti.read(); 
  debugAssert(t.type == Token::SYMBOL);
  debugAssert(t.sval == "name");

  ti.read();
  debugAssert(t.type == Token::SYMBOL);
  debugAssert(t.sval == "=");

  std::string name = ti.read().sval;
  ti.read();
  </PRE>

  <PRE>
  TextInput ti(TextInput::FROM_STRING, "name = \"Max\", height = 6");
  ti.readSymbols("name", "=");
  std::string name = ti.readString();
  ti.readSymbols(",", "height", "=");
  double height = ti. readNumber();
  </PRE>

 */
class TextInput {
public:

    class Options {
    public:
        /** If true, slash-star marks a multi-line comment.  Default is true. */
        bool                cComments;

        /** If true, // begins a single line comment, the results of
            which will not appear in
            Default is true. */
        bool                cppComments;

        /** If true, \r, \n, \t, \0, \\ and other escale sequences inside strings
            are converted to the equivalent C++ escaped character.  If false,
            slashes are treated literally.  It is convenient to set to false if
            reading Windows paths, for example, like c:\foo\bar.

            Default is true.
            */
        bool                escapeSequencesInStrings;

        /** If non-null, specifies a character that begins single line 
            comments ('#' and '%' are popular choices).  This 
            is independent of the cppComments flag.  If the 
            character appears in text with a backslash in front of 
            it, it is considered escaped and is not treated as 
            a comment character.

            Default is '\0'.
         */
        char                otherCommentCharacter;

        /** Another (optional) 1-comment character.  Useful for files that support 
            multiple comment syntaxes.
         */
        char                otherCommentCharacter2;

        /** If true, "-1" parses as the number -1 instead of the symbol "-" followed
            by the number 1.  Default is true.*/
        bool                signedNumbers;

		/** If true, strings can be marked with single quotes (e.g., 'aaa'). 
		    If false, the quote character is parsed as a symbol. Default is true.
    		Backquote (`) is always parsed as a symbol. */
		bool				singleQuotedStrings;

        Options () : cComments(true), cppComments(true), escapeSequencesInStrings(true), 
            otherCommentCharacter('\0'), otherCommentCharacter2('\0'),
            signedNumbers(true), singleQuotedStrings(true) {}
    };

private:

    std::deque<Token>       stack;

    /**
     The character you'll get if you peek 1 ahead
     */
    Array<char>             peekChar;

    /**
     Characters to be parsed.
     */
    Array<char>             buffer;

    /**
     Last character index consumed.
     */
    int                     bufferLast;
    int                     lineNumber;

    /**
     Number of characters from the beginning of the line. 
     */
    int                     charNumber;

    std::string             sourceFile;
    
    Options                 options;

    void init() {
        sourceFile = "";
        charNumber = 0;
        bufferLast = -1;
        lineNumber = 1;
    }

    /**
     Returns the next character and sets filename and linenumber
     to reflect the location where the character came from.
     */
    int popNextChar();

    inline char peekNextChar() {
        return buffer[bufferLast + 1];
    }

    inline void pushNextChar(char c) {
        if (c != EOF) {
            debugAssert(c == buffer[bufferLast]);
            bufferLast--;
        }
    }

    /** Read the next token or EOF */
    Token nextToken();

	/** Helper for nextToken.  Appends characters to t._string until
	    the end delimiter is reached. */
	void parseQuotedString(char delimiter, Token& t);

public:

    class TokenException {
    public:
        std::string     sourceFile;
        int             line;
        int             character;

        /** Pre-formatted error message */
        std::string     message;

        virtual ~TokenException() {}
    protected:

        TokenException(
            const std::string&  src,
            int                 ln,
            int                 ch);

    };

    /** Thrown by the read methods. */
    class WrongTokenType : public TokenException {
    public:
        Token::Type     expected;
        Token::Type     actual;

        WrongTokenType(
            const std::string&  src,
            int                 ln,
            int                 ch,
            Token::Type         e,
            Token::Type         a);
    };

    class WrongSymbol : public TokenException {
    public:
        std::string             expected;
        std::string             actual;

        WrongSymbol(
            const std::string&  src,
            int                 ln,
            int                 ch,
            const std::string&  e,
            const std::string&  a);
    };


    class WrongString : public TokenException {
    public:
        std::string             expected;
        std::string             actual;

        WrongString(
            const std::string&  src,
            int                 ln,
            int                 ch,
            const std::string&  e,
            const std::string&  a);
    };


    TextInput(const std::string& filename, const Options& options = Options());

    enum FS {FROM_STRING};
    /** Creates input directly from a string.
        The first argument must be TextInput::FROM_STRING.*/
    TextInput(FS fs, const std::string& str, const Options& options = Options());

    /** Returns true while there are tokens remaining. */
    bool hasMore();

    /** Read the next token (which will be the END token if ! hasMore()).
    
        Signed numbers can be handled in one of two modes.  If the option 
        TextInput::Options::signedNumbers is true,
        A '+' or '-' immediately before a number is prepended onto that number and
        if there is intervening whitespace, it is read as a separate symbol.

        If TextInput::Options::signedNumbers is false,
        read() does not distinguish between a plus or minus symbol next
        to a number and a positive/negative number itself.  For example, "x - 1" and "x -1"
        will be parsed the same way by read().  
        
        In both cases, readNumber() will contract a leading "-" or "+" onto
        a number.
    */
    Token read();

    /** Throws WrongTokenType if the next token is not a number or
        a plus or minus sign followed by a number.  In the latter case,
        this will read two tokens and combine them into a single number. 
        When an exception is thrown no tokens are consumed.*/
    double readNumber();

    /** Reads a string or throws WrongTokenType.  The quotes are taken off of strings. */
    std::string readString();
    
    /** Reads a specific string or throws either WrongTokenType or WrongString. */
    void readString(const std::string& s);

    /** Reads a symbol or throws WrongTokenType */
    std::string readSymbol();

    /** Reads a specific symbol or throws either WrongTokenType or WrongSymbol*/
    void readSymbol(const std::string& symbol);

    /** Reads a series of specific symbols.  See readSymbol */
    inline void readSymbols(const std::string& s1, const std::string& s2) {
        readSymbol(s1);
        readSymbol(s2);
    }

    inline void readSymbols(
        const std::string& s1, 
        const std::string& s2, 
        const std::string& s3) {
        readSymbol(s1);
        readSymbol(s2);
        readSymbol(s3);
    }

    inline void readSymbols(
        const std::string& s1, 
        const std::string& s2, 
        const std::string& s3,     
        const std::string& s4) {
        readSymbol(s1);
        readSymbol(s2);
        readSymbol(s3);
        readSymbol(s4);
    }

    /** Look at the next token but don't extract it */
    Token peek();

    /** Returns the line number for the <B>next</B> token */
    int peekLineNumber();

    /** Returns the character number (relative to the line) for the <B>next</B> token */
    int peekCharacterNumber();

    /** Take a previously read token and push it back (used
        in the rare case where more than one token of read-ahead
        is needed so peek doesn't suffice). */
    void push(const Token& t);
};

void deserialize(bool& b, TextInput& ti);
void deserialize(int& b, TextInput& ti);
void deserialize(uint8& b, TextInput& ti);
void deserialize(double& b, TextInput& ti);
void deserialize(float& b, TextInput& ti);
void deserialize(std::string& b, TextInput& ti);

} // namespace

#endif
