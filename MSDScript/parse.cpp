#include <iostream>
#include <sstream>
#include "parse.hpp"
#include "catch.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "value.hpp"
#include "step.hpp"

PTR(Expr) parse(std::istream &in);
static PTR(Expr) parse_expr(std::istream &in);
static PTR(Expr) parse_comparg(std::istream &in);
static PTR(Expr) parse_addend(std::istream &in);
static PTR(Expr) parse_multicand(std::istream &in);
static PTR(Expr) parse_inner(std::istream &in);
static PTR(Expr) parse_number(std::istream &in);
static PTR(Expr) parse_variable(std::istream &in);
static PTR(Expr) parse_let(std::istream &in);
static PTR(Expr) parse_if(std::istream &in);
static PTR(Expr) parse_fun(std::istream &in);
static std::string parse_keyword(std::istream &in);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static char peek_after_spaces(std::istream &in);

static char peek_after_spaces(std::istream &in) {
    char c;
    while (1) {
        c = in.peek();
        if (!isspace(c))
            break;
        c = in.get();
    }
    return c;
}

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.
PTR(Expr) parse(std::istream &in) {
    PTR(Expr) e = parse_expr(in);
    
    char c = peek_after_spaces(in);
    if (!in.eof()){
        throw std::runtime_error((std::string)"expected end of file at " + c);
    }
    return e;
}

static PTR(Expr) parse_expr(std::istream &in) {
    PTR(Expr) e = parse_comparg(in);
    char c = peek_after_spaces(in);
    if (c == '=') {
        in >> c;
        in >> c;
        if (c != '=') {
            throw std::runtime_error((std::string)"expected == at =" + c);
        }
        PTR(Expr) rhs = parse_expr(in);
        e = NEW(CompExpr)(e, rhs);
    }
    return e;
}

static PTR(Expr) parse_comparg(std::istream &in) {
    PTR(Expr) e = parse_addend(in);
    char c = peek_after_spaces(in);
    if (c == '+') {
        in >> c;
        PTR(Expr) rhs = parse_comparg(in);
        e = NEW(AddExpr)(e, rhs);
    }
    return e;
}

// Takes an input stream that starts with an addend,
// consuming the largest initial addend possible, where
// an addend is an expression that does not have `+`
// except within nested expressions (like parentheses).
static PTR(Expr) parse_addend(std::istream &in) {
    PTR(Expr) e = parse_multicand(in);
    char c = peek_after_spaces(in);
    if (c == '*') {
        c = in.get();
        PTR(Expr) rhs = parse_addend(in);
        e = NEW(MultExpr)(e, rhs);
    }
    return e;
}

static PTR(Expr) parse_multicand(std::istream &in) {
    PTR(Expr) e = parse_inner(in);
    while (peek_after_spaces(in) == '(') {
        PTR(Expr) actual_arg = parse_inner(in);
        e = NEW(CallFunExpr)(e, actual_arg);
    }
    return e;
}


// Parses something with no immediate `+` or `*` from `in`.
/* <ParseNumOrParen> = <number>
 |(<expr>)
 |<variable>
 */
static PTR(Expr) parse_inner(std::istream &in) {
    PTR(Expr) e;
    char c = peek_after_spaces(in);
    if (c == '(') {
        c = in.get();
        e = parse_expr(in);
        c = peek_after_spaces(in);
        if (c == ')'){
            c = in.get();
        }else{
            throw std::runtime_error((std::string)"expected a close parenthesis");
        }
    } else if (c == '-' || isdigit(c)) {
        e = parse_number(in);
    } else if (isalpha(c)) {
        e = parse_variable(in);
    } else if (c == '_') {
        std::string keyword = parse_keyword(in);
        if (keyword == "_true")
            return NEW(BoolExpr)(true);
        else if (keyword == "_false")
            return NEW(BoolExpr)(false);
        else if (keyword == "_let")
            return parse_let(in);
        else if (keyword == "_if")
            return parse_if(in);
        else if (keyword == "_fun")
            return parse_fun(in);
        else
            throw std::runtime_error((std::string)"unexpected keyword " + keyword);
    } else {
        throw std::runtime_error((std::string)"expected a digit or open parenthesis at " + c);
    }
    return e;
}

// Parses a number, assuming that `in` starts with a digit.
static PTR(Expr) parse_number(std::istream &in) {
    char next = in.peek();
    bool isNegative = false;
    if (next == '-') {
        isNegative = true;
        in.get();
    }
    int num = 0;
    in >> num;
    if (isNegative){
        return NEW(NumExpr)(-num);
    }else{
        return NEW(NumExpr)(num);
    }
}

// Parses an expression, assuming that `in` starts with a letter.
static PTR(Expr) parse_variable(std::istream &in) {
    return NEW(VarExpr)(parse_alphabetic(in, ""));
}

// Parses an expression, assuming that `in` starts with a letter.
static std::string parse_keyword(std::istream &in) {
    in.get();
    return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that `in` starts with a letter.
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
    std::string name = prefix;
    while (1) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        name += in.get();
    }
    return name;
}

static PTR(Expr) parse_if(std::istream &in) {
    PTR(Expr) if_part = parse_expr(in);
    std::string _then = parse_keyword(in);
    if (_then != "_then") {
        throw std::runtime_error((std::string)"expected _then, but found " + _then);
    }
    PTR(Expr) then_part = parse_expr(in);
    std::string _else = parse_keyword(in);
    if (_else != "_else") {
        throw std::runtime_error((std::string)"expected _else, but found" + _else);
    }
    PTR(Expr) else_part = parse_expr(in);
    return NEW(IfExpr)(if_part, then_part, else_part);
}

/* for tests */
static PTR(Expr) parse_str(std::string s) {
    std::istringstream in(s);
    return parse(in);
}

/* for tests */
static std::string parse_str_error(std::string s) {
    std::istringstream in(s);
    try {
        (void)parse(in);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}

static PTR(Expr) parse_let(std::istream &in) {
    peek_after_spaces(in);
    std::string varName = parse_alphabetic(in, "");
    if (varName == "") {
        throw std::runtime_error((std::string)"variable name error");
    }
    char c = peek_after_spaces(in);
    if (c != '=') {
        throw std::runtime_error((std::string)"expected =, but found " + c);
    }
    c = in.get();
    PTR(Expr) expr_rhs = parse_expr(in);
    std::string _in = parse_keyword(in);
    if (_in != "_in") {
        throw std::runtime_error((std::string)"expected _in, but found " + _in);
    }
    PTR(Expr) expr = parse_expr(in);
    return NEW(LetExpr)(varName, expr_rhs, expr);
}

static PTR(Expr) parse_fun(std::istream &in) {
    char c = peek_after_spaces(in);
    if (c != '('){
        throw std::runtime_error((std::string)"expected ( after _fun, but found" + c);
    }
    in.get();
    peek_after_spaces(in);
    std::string formal_arg = parse_alphabetic(in, "");
    if (formal_arg == ""){
        throw std::runtime_error((std::string)"formal_arg name error");
    }
    c = peek_after_spaces(in);
    if (c != ')'){
        throw std::runtime_error((std::string)"expected ) after formal_arg, but found" + c);
    }
    in.get();
    PTR(Expr) body = parse_expr(in);
    return NEW(FunExpr)(formal_arg, body);
}

TEST_CASE( "Simple expressions" ) {
    CHECK ( parse_str_error(" ( 1 ") == "expected a close parenthesis" );
    CHECK ( parse_str_error(" 1 )") == "expected end of file at )" );
    CHECK( parse_str(" 1 ")->equals(NEW(NumExpr)(1)) );
    CHECK( parse_str("1")->equals(NEW(NumExpr)(1)) );
    CHECK( parse_str("(1)")->equals(NEW(NumExpr)(1)) );
    
    // Adding two numbers
    PTR(Expr) five_plus_one = NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(1));
    CHECK( parse_str("5+1")->equals(five_plus_one) );
    CHECK( parse_str("(5+1)")->equals(five_plus_one) );
    CHECK( parse_str("(5)+1")->equals(five_plus_one) );
    CHECK( parse_str("5+(1)")->equals(five_plus_one) );
    CHECK( parse_str(" 5 ")->equals(NEW(NumExpr)(5)) );
    CHECK( parse_str(" (  5 ) ")->equals(NEW(NumExpr)(5)) );
    CHECK( parse_str(" 5  + 1")->equals(five_plus_one) );
    CHECK( parse_str(" ( 5 + 1 ) ")->equals(five_plus_one) );
    
    CHECK( parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                                   NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)),
                                                   NEW(NumExpr)(3))) );
    CHECK( parse_str("5*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(5),
                                                    NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("5+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(5),
                                                   NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("5*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(5),
                                                      NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("(2+3)*5")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)),
                                                      NEW(NumExpr)(5))) );
    CHECK( parse_str(" 11 * ( 5 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11),
                                                                five_plus_one)) );
    CHECK( parse_str(" ( 11 * 10 ) + 1 ")
          ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)),
                                NEW(NumExpr) (1))) );
    CHECK( parse_str(" 1 + 2 * 3 ")
          ->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    
    // Variable expressions
    CHECK( parse_str("abcd")->equals(NEW(VarExpr)("abcd")) );
    CHECK( parse_str("abcd+1")->equals(NEW(AddExpr)(NEW(VarExpr)("abcd"), NEW(NumExpr)(1))) );
    
    //Bool expressions
    CHECK( parse_str("_true")->equals(NEW(BoolExpr)(true)) );
    CHECK( parse_str("_false")->equals(NEW(BoolExpr)(false)) );
    
    // Invalid keywords
    CHECK( parse_str_error("_hello ") == "unexpected keyword _hello" );
    
    // Invalid chars
    CHECK ( parse_str_error("?") == "expected a digit or open parenthesis at ?" );
}

TEST_CASE( "Let Expression" ) {
    // Optimized into a single value
    CHECK(parse_str("_let y = 4 _in y + 5")->optimizer()
          ->equals(parse_str("9")));
    CHECK(parse_str("_let x = 3 _in _let y = 4 _in x + y")->optimizer()
          ->equals(parse_str("7")));
    
    // Have variables on the rhs of =
    CHECK(parse_str("_let x = z _in x + 3")->optimizer()
          ->equals(parse_str("_let x = z _in x + 3")));
    CHECK(parse_str("_let x = 4 _in _let y = z _in x + y")->optimizer()
          ->equals(parse_str("_let y = z _in 4 + y")));
    CHECK(parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")->optimizer()
          ->equals(parse_str("_let y = z + 2 _in 5 + y + 6")));
}

TEST_CASE( "If Expression" ) {
    CHECK(parse_str("_if _false _then _true _else _false" )->optimizer()->equals(NEW(BoolExpr)(false)));
    CHECK(parse_str("_if 3 + 3 == 6 _then _true _else _false" )->optimizer()->equals(NEW(BoolExpr)(true)));
    CHECK(parse_str("_if -3 + -3 == -6 _then _true _else _false" )->optimizer()->equals(NEW(BoolExpr)(true)));
    CHECK(parse_str("_if 7 == 3 + 4 _then 2 _else 4")->optimizer()->equals(NEW(NumExpr)(2)));

    CHECK(parse_str("_if (_let x = _true _in x) _then -3 _else -4")
          ->optimizer()->equals(NEW(NumExpr)(-3)));
    CHECK(parse_str("_let x = (_if _false _then _true _else _false) _in x")
          ->optimizer()->equals(NEW(BoolExpr)(false)));
}

TEST_CASE( "FunExpr & CallFunExpr " ) {
    
    CHECK( parse_str("_let f = _fun(x) x + x"
                     "_in f(2)")
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(4)) );
    CHECK( Step::interp_by_steps(parse_str("_let f = _fun(x) x + x"
                                           "_in f(2)"))
          ->equals(NEW(NumVal)(4)) );
    
    CHECK( parse_str("_let f = _fun(x)"
                     "_fun(y)"
                     "x * x + y * y"
                     "_in f(2)(3)")
          ->interp(Env::emptyenv)->equals(NEW(NumVal)(13)) );
    CHECK( Step::interp_by_steps(parse_str("_let f = _fun(x)"
                                           "_fun(y)"
                                           "x * x + y * y"
                                           "_in f(2)(3)"))
          ->equals(NEW(NumVal)(13)) );
    
    CHECK( parse_str("(_fun(x) _fun(y) x * x + y * y)(2)")
          ->interp(Env::emptyenv)->to_string() == "[FUNCTION]" );
    CHECK( Step::interp_by_steps(parse_str("(_fun(x) _fun(y) x * x + y * y)(2)"))
          ->to_string() == "[FUNCTION]" );
    
    CHECK( parse_str("((_fun(x) _fun(y) x * x + y * y)(2))(3)")
          ->interp(Env::emptyenv)->to_string() == "13" );
    CHECK( Step::interp_by_steps(parse_str("((_fun(x) _fun(y) x * x + y * y)(2))(3)"))
          ->to_string() == "13" );

    
    CHECK( parse_str("_let factrl = _fun(factrl)"
                     "                _fun(x)"
                     "                  _if x == 1"
                     "                  _then 1"
                     "                  _else x * factrl(factrl)(x + -1)"
                     "_in factrl(factrl)(5)")
          ->interp(Env::emptyenv)->to_string() == "120" );
    CHECK( Step::interp_by_steps(parse_str("_let factrl = _fun(factrl)"
                                           "                _fun(x)"
                                           "                  _if x == 1"
                                           "                  _then 1"
                                           "                  _else x * factrl(factrl)(x + -1)"
                                           "_in factrl(factrl)(5)"))
          ->to_string() == "120" );
    
    CHECK( parse_str("_let fib = _fun (fib)"
                     "              _fun (x)"
                     "                 _if x == 0"
                     "                 _then 1"
                     "                 _else _if x == 2 + -1"
                     "                 _then 1"
                     "                 _else fib(fib)(x + -1)"
                     "                       + fib(fib)(x + -2)"
                     "_in fib(fib)(10)")->interp(Env::emptyenv)->to_string() == "89");
    CHECK( Step::interp_by_steps(parse_str("_let fib = _fun (fib)"
                                           "              _fun (x)"
                                           "                 _if x == 0"
                                           "                 _then 1"
                                           "                 _else _if x == 2 + -1"
                                           "                 _then 1"
                                           "                 _else fib(fib)(x + -1)"
                                           "                       + fib(fib)(x + -2)"
                                           "_in fib(fib)(10)"))->to_string() == "89");
    
    CHECK(parse_str("_let countdown = _fun(countdown)"
                    "  _fun(n)"
                    "    _if n == 0"
                    "    _then 0"
                    "    _else countdown(countdown)(n + -1)"
                    "_in countdown(countdown)(100)")
          ->interp(Env::emptyenv)
          ->to_string() == "0");
    
    CHECK(Step::interp_by_steps(parse_str("_let countdown = _fun(countdown)"
                                          "  _fun(n)"
                                          "    _if n == 0"
                                          "    _then 0"
                                          "    _else countdown(countdown)(n + -1)"
                                          "_in countdown(countdown)(2000000)") )
          ->to_string() == "0");
}
