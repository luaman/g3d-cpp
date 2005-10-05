#include "../include/G3DAll.h"

void testTextInput() {
    printf("TextInput\n");

    {
        // Quoted string with escapes.  The actual expression we are parsing looks like:
        // "\\"
 		TextInput ti(TextInput::FROM_STRING, "\"\\\\\"");
		ti.readString("\\");
	}

    {
        // Quoted string without escapes: read as two backslashes
        // (the test itself has to escape the backslashes, just to write them).
        // The actual expression we are parsing is:
        // "\"
        TextInput::Options opt;
        opt.escapeSequencesInStrings = false;
 		TextInput ti(TextInput::FROM_STRING, "\"\\\"", opt);
		ti.readString("\\");
	}

	{
 		TextInput ti(TextInput::FROM_STRING, "a \'foo\' bar");

		ti.readSymbol("a");
		Token t = ti.read();
		debugAssert(t.extendedType() == Token::SINGLE_QUOTED_TYPE);
		debugAssert(t.string() == "foo");
		ti.readSymbol("bar");
	}

    {
        TextInput ti(TextInput::FROM_STRING, "2.x");

        debugAssert(ti.readNumber() == 2);
        ti.readSymbol("x");
    }
    {
        TextInput ti(TextInput::FROM_STRING, "1.E7");
        debugAssert(ti.readNumber() == 1.E7);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "\\123");
        Token t;
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "\\");
        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 123);
    }

    {
        TextInput::Options options;
        options.otherCommentCharacter = '#';

        TextInput ti(TextInput::FROM_STRING, "1#23\nA\\#2", options);
        Token t;
        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 1);

        // skip the comment
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "A");

        // Read escaped comment character
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "#");

        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 2);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "0xFEED");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        (void)n;
        debugAssert((int)n == 0xFEED);

        t = ti.read();
        debugAssert(t.type() == Token::END);
    }

    {

    TextInput::Options opt;
    opt.cppComments = false;
    TextInput ti(TextInput::FROM_STRING, 
                 "if/*comment*/(x->y==-1e6){cout<<\"hello world\"}; // foo\nbar",
                 opt);

    Token a = ti.read();
    Token b = ti.read();
    Token c = ti.read();
    Token d = ti.read();
    Token e = ti.read();
    Token f = ti.read();
    double g = ti.readNumber();
    (void)g;
    Token h = ti.read();
    Token i = ti.read();
    Token j = ti.read();
    Token k = ti.read();
    Token L = ti.read();
    Token m = ti.read();
    Token n = ti.read();
    Token p = ti.read();
    Token q = ti.read();
    Token r = ti.read();
    Token s = ti.read();
    Token t = ti.read();

    debugAssert(a.type() == Token::SYMBOL);
    debugAssert(a.string() == "if");

    debugAssert(b.type() == Token::SYMBOL);
    debugAssert(b.string() == "(");

    debugAssert(c.type() == Token::SYMBOL);
    debugAssert(c.string() == "x");

    debugAssert(d.type() == Token::SYMBOL);
    debugAssert(d.string() == "->");

    debugAssert(e.type() == Token::SYMBOL);
    debugAssert(e.string() == "y");

    debugAssert(f.type() == Token::SYMBOL);
    debugAssert(f.string() == "==");

    debugAssert(g == -1e6);

    debugAssert(h.type() == Token::SYMBOL);
    debugAssert(h.string() == ")");

    debugAssert(i.type() == Token::SYMBOL);
    debugAssert(i.string() == "{");

    debugAssert(j.type() == Token::SYMBOL);
    debugAssert(j.string() == "cout");

    debugAssert(k.type() == Token::SYMBOL);
    debugAssert(k.string() == "<<");

    debugAssert(L.type() == Token::STRING);
    debugAssert(L.string() == "hello world");

    debugAssert(m.type() == Token::SYMBOL);
    debugAssert(m.string() == "}");

    debugAssert(n.type() == Token::SYMBOL);
    debugAssert(n.string() == ";");

    debugAssert(p.type() == Token::SYMBOL);
    debugAssert(p.string() == "/");

    debugAssert(q.type() == Token::SYMBOL);
    debugAssert(q.string() == "/");

    debugAssert(r.type() == Token::SYMBOL);
    debugAssert(r.string() == "foo");

    debugAssert(s.type() == Token::SYMBOL);
    debugAssert(s.string() == "bar");

    debugAssert(t.type() == Token::END);
    }
    
    {
        TextInput ti(TextInput::FROM_STRING, "-1 +1 2.6");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        debugAssert(n == -1);

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == 1);

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == 2.6);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "- 1 ---.51");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::SYMBOL);
        ti.readSymbol("-");

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        debugAssert(n == 1);

        t = ti.peek();
        debugAssert(t.type() == Token::SYMBOL);
        ti.readSymbol("--");

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == -.51);
    }


    {
        G3D::TextInput::Options ti_opts;
        const std::string str = "'";
        ti_opts.singleQuotedStrings = false;

        G3D::TextInput ti(G3D::TextInput::FROM_STRING, str, ti_opts);

        G3D::Token t = ti.read();

        /*
        printf("t.string       = %s\n", t.string().c_str());
        printf("t.type         = %d\n", t.type());
        printf("t.extendedType = %d\n", t.extendedType());

        printf("\n");
        printf("SYMBOL         = %d\n", G3D::Token::SYMBOL);
        printf("END            = %d\n", G3D::Token::END);

        printf("\n");
        printf("SYMBOL_TYPE    = %d\n", G3D::Token::SYMBOL_TYPE);
        printf("END_TYPE       = %d\n", G3D::Token::END_TYPE);
        */

        alwaysAssertM(t.type() == G3D::Token::SYMBOL, "");
        alwaysAssertM(t.extendedType() == G3D::Token::SYMBOL_TYPE, "");
    }

}
