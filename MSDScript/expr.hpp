#ifndef expr_h
#define expr_h

#include <string>
#include <iostream>
#include "pointer.hpp"

class Val;
class Env;
class Expr ENABLE_THIS(Expr){
public:
    virtual bool equals(PTR(Expr) e) = 0;
    
    // To compute the number value of an expression,
    virtual PTR(Val) interp(PTR(Env) env) = 0;
    
    virtual void step_interp() = 0;
    
    // To substitute a number in place of a variable
    virtual PTR(Expr) subst(std::string var, PTR(Val) val) = 0;
    
    // To check whether a expression contains a variable
    virtual bool containsVar() = 0;
    
    // To optimize a expression
    virtual PTR(Expr) optimizer() = 0;
    
    virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
    int rep;
    NumExpr(int rep);
    
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
        
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
        
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
        
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
        
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class VarExpr : public Expr {
public:
    std::string name;
        
    VarExpr(std::string name);
    bool equals(PTR(Expr) e);
        
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class BoolExpr : public Expr {
public:
    bool rep;
    BoolExpr(bool rep);
    bool equals(PTR(Expr) e);
        
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class LetExpr : public Expr {
public:
    std::string var_name;
    PTR(Expr) rhs;
    PTR(Expr) expr;
    LetExpr(std::string var_name, PTR(Expr) rhs, PTR(Expr) expr);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class IfExpr : public Expr {
public:
    PTR(Expr) if_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
        
    IfExpr(PTR(Expr) if_part, PTR(Expr) then_part, PTR(Expr) else_part);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    std::string to_string();
};
    
class CompExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
        
    CompExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;
    FunExpr(std::string formal_arg, PTR(Expr) body);
    bool equals(PTR(Expr) e);
        
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
class CallFunExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
        
    CallFunExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    bool equals(PTR(Expr) e);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVar();
    PTR(Expr) optimizer();
    
    std::string to_string();
};
    
#endif /* expr_hpp */
