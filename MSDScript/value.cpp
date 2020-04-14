#include <stdexcept>
#include "catch.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "step.hpp"

//NumVal
NumVal::NumVal(int rep) {
    this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr){
        return false;
    }else{
        return rep == other_num_val->rep;
    }
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr){
        throw std::runtime_error("This is not a number");
    }else{
        return NEW(NumVal)(rep + other_num_val->rep);
    }
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr){
        throw std::runtime_error("This is not a number");
    }else{
        return NEW(NumVal)(rep * other_num_val->rep);
    }
}

PTR(Expr) NumVal::to_expr() {
    return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
    return std::to_string(rep);
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("Function call error occured");
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Function call error occured");
}

//BoolVal
BoolVal::BoolVal(bool rep) {
    this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
    PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
    if (other_bool_val == nullptr){
        return false;
    }else{
        return rep == other_bool_val->rep;
    }
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("Booleans could not add");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("Booleans could not multiply");
}

PTR(Expr) BoolVal::to_expr() {
    return NEW(BoolExpr)(rep);
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Function call error occured");
}

std::string BoolVal::to_string() {
    if (rep){
        return "_true";
    }else{
        return "_false";
    }
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("Function call error occured");
}

FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env) {
    this->formal_arg = formal_arg;
    this->body = body;
    this->env = env;
}

bool FunVal::equals(PTR(Val) val) {
    PTR(FunVal) f = CAST(FunVal)(val);
    
    if (f == NULL){
        return false;
    }else{
        return formal_arg == f->formal_arg && body->equals(f->body) && env->equals(f->env);
    }
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("Functions could not add.");
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("Functions could not multiply.");
}

PTR(Expr) FunVal::to_expr() {
    return NEW(FunExpr)(formal_arg, body);
}

std::string FunVal::to_string() {
    return "[FUNCTION]";
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}

TEST_CASE( "values equals" ) {
    CHECK( (new NumVal(5))->equals(new NumVal(5)) );
    CHECK( ! (new NumVal(7))->equals(new NumVal(5)) );
    
    CHECK( (new BoolVal(true))->equals(new BoolVal(true)) );
    CHECK( ! (new BoolVal(true))->equals(new BoolVal(false)) );
    CHECK( ! (new BoolVal(false))->equals(new BoolVal(true)) );
    
    CHECK( ! (new NumVal(7))->equals(new BoolVal(false)) );
    CHECK( ! (new BoolVal(false))->equals(new NumVal(8)) );
    
    CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv)) );
    CHECK( !(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->equals(NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv)) );
}

TEST_CASE( "add_to" ) {
    
    CHECK ( (new NumVal(3))->add_to(new NumVal(9))->equals(new NumVal(12)) );

    CHECK_THROWS_WITH ( (new BoolVal(false))->add_to(new BoolVal(false)),
                       "Booleans could not add" );
}

TEST_CASE( "mult_with" ) {
    
    CHECK ( (new NumVal(3))->mult_with(new NumVal(9))->equals(new NumVal(27)) );
    
    CHECK_THROWS_WITH ( (new BoolVal(false))->mult_with(new BoolVal(false)),
                       "Booleans could not multiply" );
}

TEST_CASE( "value to_expr" ) {
    CHECK( (new NumVal(6))->to_expr()->equals(new NumExpr(6)) );
    CHECK( (new BoolVal(true))->to_expr()->equals(new BoolExpr(true)) );
    CHECK( (new BoolVal(false))->to_expr()->equals(new BoolExpr(false)) );
    CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->to_expr()->equals(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
}

TEST_CASE( "value to_string" ) {
    CHECK( (new NumVal(6))->to_string() == "6" );
    CHECK( (new BoolVal(true))->to_string() == "_true" );
    CHECK( (new BoolVal(false))->to_string() == "_false" );
    CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->to_string() == "[FUNCTION]" );
}

