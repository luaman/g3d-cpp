/**
  @file TextOutput.cpp

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2004-06-21
  @edited  2004-06-21

  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
 */

#include "G3D/TextOutput.h"

namespace G3D {

TextOutput::TextOutput(const TextOutput::Options& opt) {
    setOptions(opt);
    filename = "";
    indentLevel = 0;
    startingNewLine = true;
}


TextOutput::TextOutput(const std::string& fil, const TextOutput::Options& opt) {
    setOptions(opt);
    filename = fil;
    indentLevel = 0;
    startingNewLine = true;
}


void TextOutput::setIndentLevel(int i) {
    indentLevel = i;

    // If there were more pops than pushes, don't let that take us below 0 indent.
    // Don't ever indent more than the number of columns.
    indentSpaces = iClamp(option.spacesPerIndent * indentLevel, 0, option.numColumns - 
1);
}


void TextOutput::setOptions(const Options& _opt) {
    option = _opt;

    debugAssert(option.numColumns > 1);

    setIndentLevel(indentLevel);

    newline = (option.newlineStyle == Options::NEWLINE_WINDOWS) ? "\r\n" : "\n";
}


void TextOutput::pushIndent() {
    setIndentLevel(indentLevel + 1);
}


void TextOutput::popIndent() {
    setIndentLevel(indentLevel - 1);
}


void TextOutput::writeString(const std::string& string) {
    this->printf("\"%s\"", string.c_str());
}


void TextOutput::writeNumber(double n) {
    this->printf("%f ", n);
}


void TextOutput::writeNumber(int n) {
    this->printf("%d ", n);
}


void TextOutput::writeSymbol(const std::string& string) {
    if (string.size() > 0) {
        // TODO: check for legal symbols?
        this->printf("%s ", string.c_str());
    }
}

void TextOutput::writeSymbols(
    const std::string& a,
    const std::string& b,
    const std::string& c,
    const std::string& d,
    const std::string& e,
    const std::string& f) {

    writeSymbol(a);
    writeSymbol(b);
    writeSymbol(c);
    writeSymbol(d);
    writeSymbol(e);
    writeSymbol(f);
}


void TextOutput::printf(const std::string& formatString, ...) {
    va_list argList;
    va_start(argList, formatString);
    this->vprintf(formatString.c_str(), argList);
    va_end(argList);
}


void TextOutput::printf(const char* formatString, ...) {
    va_list argList;
    va_start(argList, formatString);
    this->vprintf(formatString, argList);
    va_end(argList);
}


void TextOutput::convertNewlines(const std::string& in, std::string& out) {
    // TODO: can be significantly optimized in cases where
    // single characters are copied in order by walking through
    // the array and copying substrings as needed.

    if (option.convertNewlines) {
        out = "";
        for (int i = 0; i < in.size(); ++i) {
            if (in[i] == '\n') {
                // Unix newline
                out += newline;
            } else if ((in[i] == '\r') && (i + 1 < in.size()) && (in[i + 1] == '\n')) {
                // Windows newline
                out += newline;
                ++i;
            } else {
                out += in[i];
            }
        }
    } else {
        out = in;
    }
}


void TextOutput::writeNewline() {
    indentAndAppend(newline);
}


void TextOutput::writeNewlines(int numLines) {
    for (int i = 0; i < numLines; ++i) {
        indentAndAppend(newline);
    }
}


void TextOutput::wordWrap(const std::string& in, std::string& out) {
    if (option.wordWrap == Options::WRAP_NONE) {
        out = in;
        return;
    }

    // Number of columns to wrap against
    int cols = option.numColumns - indentSpaces;
    // TODO: implement 
    out = in;
}


void TextOutput::indentAndAppend(const std::string& str) {

    for (int i = 0; i < str.size(); ++i) {
        
        if (startingNewLine) {
            for (int j = 0; j < indentSpaces; ++j) {
                data.push(' ');
            }
            startingNewLine = true;
        }

        data.push(str[i]);
        
        startingNewLine = (str[i] == '\n');
    }
}


void TextOutput::vprintf(const char* formatString, va_list argPtr) {
    std::string str = vformat(formatString, argPtr);

    std::string clean;
    convertNewlines(str, clean);

    std::string wrapped;
    wordWrap(clean, wrapped);

    indentAndAppend(wrapped);
}


void TextOutput::commit() {
    FILE* f = fopen(filename.c_str(), "wb");
    fwrite(data.getCArray(), 1, data.size(), f);
    fclose(f);
}


void TextOutput::commitString(std::string& out) {
    // Null terminate
    data.push('\0');
    out = data.getCArray();
    data.pop();
}


std::string TextOutput::commitString() {
    std::string str;
    commitString(str);
    return str;
}


/////////////////////////////////////////////////////////////////////

void serialize(const float& b, TextOutput& to) {
    to.writeNumber(b);
}


void serialize(const bool& b, TextOutput& to) {
    to.writeSymbol(b ? "true" : "false");
}


void serialize(const int& b, TextOutput& to) {
    to.writeNumber(b);
}


void serialize(const uint8& b, TextOutput& to) {
    to.writeNumber(b);
}


void serialize(const double& b, TextOutput& to) {
    to.writeNumber(b);
}


}
