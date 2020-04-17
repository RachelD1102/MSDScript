#include "expr.hpp"
#include <sstream>
#include "env.hpp"
#include "value.hpp"
#include "step.hpp"
#include "cont.hpp"

//NumExpr
NumExpr::NumExpr(int rep){
    this -> rep = rep;
}

bool NumExpr::equals(PTR(Expr) e){
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL){
        return false;
    }else{
        return rep == n->rep;
    }
}

PTR(Val) NumExpr::interp(PTR(Env) env){
    return NEW(NumVal)(rep);
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
    Step::cont = Step::cont;
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(NumExpr)(rep);
}

bool NumExpr::containsVar(){
    return false;
}

PTR(Expr) NumExpr::optimizer(){
    return THIS;
}

std::string NumExpr::to_string(){
    return std::to_string(rep);
}

//AddExpr
AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e){
    PTR(AddExpr) a = CAST(AddExpr)(e);
    if (a == NULL){
        return false;
    }else{
        return (lhs->equals(a->lhs)
                && rhs->equals(a->rhs));
    }
}

PTR(Val) AddExpr::interp(PTR(Env) env){
    return lhs->interp(env)->add_to(rhs->interp(env));
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(AddExpr)(lhs->subst(var, new_val),
                        rhs->subst(var, new_val));
}

bool AddExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

PTR(Expr) AddExpr::optimizer() {
    PTR(Expr) lhs_optimized = THIS->lhs->optimizer();
    PTR(Expr) rhs_optimized = THIS->rhs->optimizer();
    
    if (!lhs_optimized->containsVar() && !rhs_optimized->containsVar()) {
        PTR(Val) new_val = lhs_optimized->interp(Env::emptyenv)->add_to(rhs_optimized
                 ->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(AddExpr)(lhs_optimized, rhs_optimized);
}

std::string AddExpr::to_string() {
    return "(" + lhs->to_string() + " + " + rhs->to_string() + ")";
}

//MultExpr
MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    THIS->lhs = lhs;
    THIS->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e){
    PTR(MultExpr) m = dynamic_cast<MultExpr*>(e);
    if (m == NULL){
        return false;
    }else{
        return (lhs->equals(m->lhs)
                && rhs->equals(m->rhs));
    }
}

PTR(Val) MultExpr::interp(PTR(Env) env){
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(MultExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

bool MultExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

PTR(Expr) MultExpr::optimizer(){
    PTR(Expr) lhs_optimized = THIS->lhs->optimizer();
    PTR(Expr) rhs_optimized = THIS->rhs->optimizer();
    
    if (!lhs_optimized->containsVar() && !rhs_optimized->containsVar()){
        PTR(Val) new_val = lhs_optimized->interp(Env::emptyenv)
        ->mult_with(rhs_optimized->interp(Env::emptyenv));
        return new_val->to_expr();
    }
    return NEW(MultExpr)(lhs_optimized, rhs_optimized);
}

std::string MultExpr::to_string(){
    return "(" + lhs->to_string() + " * " + rhs->to_string() + ")";
}

//VarExpr
VarExpr::VarExpr(std::string name){
    THIS->name = name;
}

bool VarExpr::equals(PTR(Expr) e){
    PTR(VarExpr) v = CAST(VarExpr)(e);
    if (v == NULL){
        return false;
    }else{
        return name == v->name;
    }
}

PTR(Val) VarExpr::interp(PTR(Env) env){
    return env->lookup(name);
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont;
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val){
    if (name == var){
        return new_val->to_expr();
    }else{
        return NEW(VarExpr)(name); // or `return THIS`
    }
}

bool VarExpr::containsVar(){
    return true;
}

PTR(Expr) VarExpr::optimizer(){
    return NEW(VarExpr)(name);
}

std::string VarExpr::to_string(){
    return name;
}

//BoolExpr
BoolExpr::BoolExpr(bool rep){
    THIS->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
    PTR(BoolExpr) b = CAST(BoolExpr)(e);
    if (b == NULL){
        return false;
    }else{
        return rep == b->rep;
    }
}

PTR(Val) BoolExpr::interp(PTR(Env) env){
    return NEW(BoolVal)(rep);
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
    Step::cont = Step::cont;
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(BoolExpr)(rep);
}

bool BoolExpr::containsVar(){
    return false;
}

PTR(Expr) BoolExpr::optimizer(){
    return NEW(BoolExpr)(rep);
}

std::string BoolExpr::to_string() {
    if (rep == true){
        return "_true";
    }else{
        return "_false";
    }
}

//LetExpr
LetExpr::LetExpr(std::string var_name, PTR(Expr) rhs, PTR(Expr) expr){
    THIS -> var_name = var_name;
    THIS -> rhs = rhs;
    THIS -> expr = expr;
}

bool LetExpr::equals(PTR(Expr) e) {
    PTR(LetExpr) l = dynamic_cast<LetExpr*>(e);
    
    if (l == NULL){
        return false;
    }else{
        return var_name == l->var_name && rhs->equals(l->rhs) && expr->equals(l->expr);
    }
}

PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) rhs_value = rhs -> interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(var_name, rhs_value, env);
    return expr -> interp(new_env);
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::cont = NEW(LetCont)(var_name, expr, Step::env, Step::cont);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) val){
    if (var == var_name) {
        return NEW(LetExpr)(var, rhs, expr);
    }
    return NEW(LetExpr)(var_name, rhs->subst(var, val), expr->subst(var, val));
}

bool LetExpr::containsVar(){
    PTR(Env) empty_env = NEW(EmptyEnv)();
    return rhs->containsVar() || expr->subst(var_name, rhs->interp(empty_env))->containsVar();
}

PTR(Expr) LetExpr::optimizer(){
    PTR(Expr) rhs_optimized = rhs->optimizer();
    PTR(Expr) expr_optimized = expr->optimizer();
    
    if (!rhs_optimized->containsVar()) {
        return expr_optimized->subst(var_name, rhs_optimized->interp(Env::emptyenv))->optimizer();
    }
    return NEW(LetExpr)(var_name, rhs_optimized, expr_optimized);
}

std::string LetExpr::to_string(){
    return "(_let " + var_name + " = " + rhs->to_string() + " _in " + expr->to_string() + ")";
}

//IfExpr
IfExpr::IfExpr(PTR(Expr) if_part, PTR(Expr) then_part, PTR(Expr) else_part) {
    THIS->if_part = if_part;
    THIS->then_part = then_part;
    THIS->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e){
    PTR(IfExpr) i = CAST(IfExpr)(e);
    
    if (i == NULL){
        return false;
    }else{
        return if_part->equals(i->if_part)
        && then_part->equals(i->then_part)
        && else_part->equals(i->else_part);
    }
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    PTR(Val) if_value= if_part -> interp(env);
    
    if (if_value->equals(NEW(BoolVal)(true))) {
        return then_part -> interp(env);
    } else {
        return else_part -> interp(env);
    }
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = if_part;
    Step::cont = NEW(IfCont)(then_part, else_part, Step::env, Step::cont);
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) val) {
    return NEW(IfExpr)(if_part -> subst(var, val), then_part -> subst(var, val), else_part->subst(var, val));
}

bool IfExpr::containsVar(){
    if (if_part->containsVar()) {
        return true;
    }
    PTR(Val) if_value = if_part -> interp(Env::emptyenv);
    
    if (if_value->equals(NEW(BoolVal)(true))){
        return then_part -> containsVar();
    }else{
        return else_part -> containsVar();
    }
}

PTR(Expr) IfExpr::optimizer(){
    PTR(Expr) if_part_optimized = if_part->optimizer();
    
    if (if_part_optimized->equals(NEW(BoolExpr)(true))) {
        return then_part->optimizer();
    } else if (if_part_optimized->equals(NEW(BoolExpr)(false))){
        return else_part->optimizer();
    } else {
        return NEW(IfExpr)(if_part_optimized, then_part->optimizer(), else_part->optimizer());
    }
}

std::string IfExpr::to_string() {
    return "(_if " + if_part->to_string()
    + " _then " + then_part->to_string()
    + " _else " + else_part->to_string() + ")";
}

//ComExpr
CompExpr::CompExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this ->lhs = lhs;
    this ->rhs = rhs;
}

bool CompExpr::equals(PTR(Expr) e) {
    PTR(CompExpr) ee = CAST(CompExpr)(e);
    if (ee == NULL){
        return false;
    }else{
        return lhs->equals(ee->lhs) && rhs->equals(ee->rhs);
    }
}

PTR(Val) CompExpr::interp(PTR(Env) env) {
    PTR(Val) lhs_value = lhs->interp(env);
    PTR(Val) rhs_value = rhs->interp(env);
    
    if (lhs_value->equals(rhs_value)){
        return NEW(BoolVal)(true);
    }else{
        return NEW(BoolVal)(false);
    }
}

void CompExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) CompExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CompExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool CompExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}

PTR(Expr) CompExpr::optimizer() {
    PTR(Expr) lhs_optimized = lhs->optimizer();
    PTR(Expr) rhs_optimized = rhs->optimizer();
    
    if (!lhs_optimized->containsVar() && !rhs_optimized->containsVar()){
        return NEW(BoolExpr)(lhs_optimized->equals(rhs_optimized));
    }else{
        return NEW(CompExpr)(lhs_optimized, rhs_optimized);
    }
}

std::string CompExpr::to_string() {
    return "(" + lhs->to_string() + " == " + rhs->to_string() + ")";
}

//FunExpr
FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
    this -> formal_arg = formal_arg;
    this -> body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) f = CAST(FunExpr)(e);
    
    if (f == NULL){
        return false;
    }else{
        return formal_arg == f->formal_arg && body->equals(f->body);
    }
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    if (var == formal_arg){
        return NEW(FunExpr)(formal_arg, body);
    }else{
        return NEW(FunExpr)(formal_arg, body->subst(var, val));
    }
}

// _fun(x) x + x doens't contain a variable
// _fun(x) x + y contain a variable
bool FunExpr::containsVar(){
    return body->subst(formal_arg, NEW(NumVal)(0))->containsVar();
}

PTR(Expr) FunExpr::optimizer() {
    return NEW(FunExpr)(formal_arg, body->optimizer());
}

std::string FunExpr::to_string(){
    return "(_fun(" + formal_arg + ") " + body->to_string() + ")";
}

//CallFunExpr
CallFunExpr::CallFunExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this -> to_be_called = to_be_called;
    this -> actual_arg = actual_arg;
}

bool CallFunExpr::equals(PTR(Expr) e){
    PTR(CallFunExpr) c = CAST(CallFunExpr)(e);
    
    if (c == NULL){
        return false;
    }else{
        return to_be_called->equals(c->to_be_called) && actual_arg->equals(c->actual_arg);
    }
}

PTR(Val) CallFunExpr::interp(PTR(Env) env) {
    return to_be_called->interp(env)->call(actual_arg->interp(env));
}

void CallFunExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}

PTR(Expr) CallFunExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallFunExpr)(to_be_called->subst(var, val), actual_arg->subst(var, val));
}

bool CallFunExpr::containsVar() {
    return actual_arg->containsVar() || to_be_called->containsVar();
}

PTR(Expr) CallFunExpr::optimizer() {
    return NEW(CallFunExpr)(to_be_called->optimizer(), actual_arg->optimizer());
}

std::string CallFunExpr::to_string() {
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
}

/* for tests */
static std::string evaluate_expr(PTR(Expr) expr) {
    try {
        (void)expr->interp(Env::emptyenv);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}


//TEST_CASE( "equals" ) {
//    //NumExpr
//    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
//    CHECK( (NEW(NumExpr)(100))->equals(NEW(NumExpr)(100)) );
//    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) );
//    CHECK( ! (NEW(NumExpr)(100))->equals(NEW(NumExpr)(102)) );
//    CHECK( ! (NEW(NumExpr)(0))->equals(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(0))) );
//    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))) );
//    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(VarExpr)("x")) );
//    CHECK( ! (NEW(NumExpr)(2))->equals(NEW(BoolExpr)(true)) );
//    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(BoolExpr)(false)) );
//
//    //AddExpr
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
//    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))) );
//    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5))) );
//    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(6)))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(7))) );
//    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(6)))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(7), NEW(NumExpr)(6))) );
//    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
//         ->equals(NEW(NumExpr)(8)) );
//
//    // MultExpr
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(9)))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(9))) );
//    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(7)))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(3))) );
//    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(7)))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(7))) );
//    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(6)))
//          ->equals(NEW(NumExpr)(5)) );
//    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(6)))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(6), NEW(NumExpr)(3))) );
//
//    //VarExpr
//    CHECK( (NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")) );
//    CHECK( !(NEW(VarExpr)("x"))->equals(NEW(VarExpr)("y")) );
//    CHECK( ! (NEW(VarExpr)("xyz"))->equals(NEW(VarExpr)("zyx")) );
//    CHECK( ! (NEW(VarExpr)("abcd"))->equals(NEW(VarExpr)("dabc")) );
//    CHECK( ! (NEW(VarExpr)("z"))->equals(NEW(NumExpr)(1)) );
//
//    //BoolExpr
//    CHECK( (NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(true)) );
//    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(false)) );
//    CHECK( (NEW(BoolExpr)(false))->equals(NEW(BoolExpr)(false)) );
//    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(VarExpr)("false")) );
//    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(NumExpr)(1)) );
//    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(NumExpr)(0)) );
//
//    //LetExpr
//    CHECK( !(NEW(LetExpr)("x",
//                          NEW(NumExpr)(1),
//                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->equals(NEW(NumExpr)(5)) );
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(NumExpr)(1),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->equals(NEW(LetExpr)("x",
//                                NEW(NumExpr)(1),
//                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
//
//    //IfExpr
//    CHECK( !(NEW(IfExpr)(NEW(BoolExpr)(true),
//                         NEW(NumExpr)(3),
//                         NEW(NumExpr)(4)))
//          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true),
//                               NEW(NumExpr)(3),
//                               NEW(NumExpr)(5))) );
//
//    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
//                        NEW(BoolExpr)(true),
//                        NEW(BoolExpr)(false)))
//          ->equals((NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
//                                NEW(BoolExpr)(true),
//                                NEW(BoolExpr)(false)))) );
//
//    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
//                         NEW(BoolExpr)(true),
//                         NEW(BoolExpr)(false)))
//          ->equals((NEW(IfExpr)(NEW(BoolExpr)(true),
//                                NEW(BoolExpr)(true),
//                                NEW(BoolExpr)(false)))) );
//
//    //CompExpr
//    CHECK( !(NEW(CompExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1)))
//          ->equals(NEW(CompExpr)(NEW(NumExpr)(2), NEW(BoolExpr)(true))) );
//    CHECK( !(NEW(CompExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1)))
//          ->equals(NEW(CompExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))) );
//    CHECK( (NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))
//          ->equals(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))) );
//
//    //FunExpr
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
//          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
//    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
//          ->equals(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
//
//    //CallFunExpr
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                             NEW(NumExpr)(3)))
//          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                                    NEW(NumExpr)(3))) );
//    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                              NEW(NumExpr)(3)))
//          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                                    NEW(NumExpr)(3))) );
//}
//
//TEST_CASE( "Interp") {
//    // to_value method for NumExpr
//    CHECK( (NEW(NumExpr)(3))->interp(Env::emptyenv)->equals(NEW(NumVal)(3)) );
//    CHECK( !(NEW(NumExpr)(10))->interp(Env::emptyenv)->equals(NEW(NumVal)(20)) );
//
//    CHECK( Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(10)) );
//    CHECK( !Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(20)) );
//
//    // AddExpr
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(5)))->interp(Env::emptyenv)
//          ->equals(NEW(NumVal)(8)) );
//    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->interp(Env::emptyenv)
//          ->equals(NEW(BoolVal)(true)) );
//
//    CHECK( Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(5)))
//          ->equals(NEW(NumVal)(8)) );
//    CHECK( !Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
//          ->equals(NEW(BoolVal)(true)) );
//
//    // MultExpr
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(5)))->interp(Env::emptyenv)
//          ->equals(NEW(NumVal)(15)) );
//    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->interp(Env::emptyenv)
//          ->equals(NEW(BoolVal)(false)) );
//
//    CHECK( Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(5)))
//          ->equals(NEW(NumVal)(15)) );
//    CHECK( !Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
//          ->equals(NEW(BoolVal)(false)) );
//
//    // VarExpr
//    CHECK( evaluate_expr(NEW(VarExpr)("hello")) == "free variable: hello" );
//    CHECK( evaluate_expr(NEW(VarExpr)("world")) == "free variable: world" );
//
//    // BoolExpr
//    CHECK( (NEW(BoolExpr)(true))->interp(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
//    CHECK( (NEW(BoolExpr)(false))->interp(Env::emptyenv)->equals(NEW(BoolVal)(false)) );
//    CHECK( !(NEW(BoolExpr)(false))->interp(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
//    CHECK( Step::interp_by_steps(NEW(BoolExpr)(true))->equals(NEW(BoolVal)(true)) );
//    CHECK( Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(false)) );
//    CHECK( !Step::interp_by_steps(NEW(BoolExpr)(true))->equals(NEW(BoolVal)(false)) );
//    CHECK( !Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(true)) );
//
//    //LetExpr
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(NumExpr)(1),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(5)));
//    CHECK( evaluate_expr(NEW(LetExpr)("x",
//                                      NEW(VarExpr)("y"),
//                                      NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          == "free variable: y" );
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(NumExpr)(1),
//                         NEW(LetExpr)("x",
//                                      NEW(NumExpr)(2),
//                                      NEW(VarExpr)("x"))))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(2)) );
//
//    CHECK( Step::interp_by_steps(NEW(LetExpr)("x",
//                                              NEW(NumExpr)(1),
//                                              NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->equals(NEW(NumVal)(5)));
//    CHECK( Step::interp_by_steps(NEW(LetExpr)("x",
//                                              NEW(NumExpr)(1),
//                                              NEW(LetExpr)("x",
//                                                           NEW(NumExpr)(2),
//                                                           NEW(VarExpr)("x"))))
//          ->equals(NEW(NumVal)(2)) );
//
//    // IfExpr
//    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(false),
//                        NEW(NumExpr)(3),
//                        NEW(NumExpr)(6)))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(6)) );
//    CHECK( evaluate_expr(NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
//                                     NEW(BoolExpr)(true),
//                                     NEW(BoolExpr)(false)))
//          == "free variable: x" );
//    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)),
//                         NEW(NumExpr)(3),
//                         NEW(NumExpr)(6)))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(3)) );
//
//    CHECK( Step::interp_by_steps(NEW(IfExpr)(NEW(BoolExpr)(false),
//                                             NEW(NumExpr)(3),
//                                             NEW(NumExpr)(6)))
//          ->equals(NEW(NumVal)(6)) );
//    CHECK( !Step::interp_by_steps(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(7)),
//                                              NEW(NumExpr)(3),
//                                              NEW(NumExpr)(9)))
//          ->equals(NEW(NumVal)(3)) );
//
//    // CompExpr
//    CHECK((NEW(CompExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
//          ->interp(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
//    CHECK((NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
//                         NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
//          ->interp(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
//    CHECK(evaluate_expr(NEW(CompExpr)(NEW(VarExpr)("abcde"), NEW(NumExpr)(2)))
//          == "free variable: abcde" );
//
//    CHECK( Step::interp_by_steps(NEW(CompExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
//          ->equals((NEW(BoolVal)(true))) );
//    CHECK( Step::interp_by_steps(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)),
//                                               NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
//          ->equals((NEW(BoolVal)(true))) );
//
//    // FunExpr
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->interp(Env::emptyenv)
//          ->to_string() == "[FUNCTION]" );
//    CHECK( !((NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->interp(Env::emptyenv)
//             ->to_string() == "FUNCTION]") );
//
//    CHECK( Step::interp_by_steps(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
//          ->to_string() == "[FUNCTION]" );
//    CHECK( !(Step::interp_by_steps(NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
//             ->to_string() == "FUNCTION]") );
//
//    // CallFunExpr
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                             NEW(NumExpr)(4)))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(8)));
//    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                              NEW(NumExpr)(4)))
//          ->interp(Env::emptyenv)->equals(NEW(NumVal)(15)));
//
//    CHECK( Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                                                  NEW(NumExpr)(4)))
//          ->equals(NEW(NumVal)(8)));
//    CHECK( !Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                                                   NEW(NumExpr)(4)))
//          ->equals(NEW(NumVal)(15)));
//}
//
//TEST_CASE( "subst") {
//    // subst method for NumExpr
//    CHECK( (NEW(NumExpr)(10))->subst("pig", NEW(NumVal)(3))
//          ->equals(NEW(NumExpr)(10)) );
//    CHECK( !(NEW(NumExpr)(10))->subst("pig", NEW(NumVal)(9))
//          ->equals(NEW(NumExpr)(9)) );
//
//    // AddExpr
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("pig")))->subst("pig", NEW(NumVal)(3))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
//    CHECK( !(NEW(AddExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("pig")))->subst("pig", NEW(NumVal)(3))
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
//
//    // MultExpr
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("pig")))->subst("pig", NEW(NumVal)(5))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5))) );
//    CHECK( !(NEW(MultExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("pig")))->subst("pig", NEW(NumVal)(3))
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
//
//    // VarExpr
//    CHECK( (NEW(VarExpr)("fish"))->subst("pig", NEW(NumVal)(3))
//          ->equals(NEW(VarExpr)("fish")) );
//    CHECK( (NEW(VarExpr)("pig"))->subst("pig", NEW(NumVal)(3) )
//          ->equals(NEW(NumExpr)(3)) );
//    CHECK( (NEW(VarExpr)("pig"))->subst("pig", NEW(BoolVal)(true) )
//          ->equals(NEW(BoolExpr)(true)) );
//    CHECK( !(NEW(VarExpr)("cat"))->subst("pig", NEW(NumVal)(3) )
//          ->equals(NEW(NumExpr)(3)) );
//
//    // BoolExpr
//    CHECK( (NEW(BoolExpr)(true))->subst("x", NEW(NumVal)(3))->equals(NEW(BoolExpr)(true)) );
//    CHECK( !(NEW(BoolExpr)(false))->subst("false", NEW(NumVal)(3))->equals(NEW(NumExpr)(3)) );
//
//    // subst method for LetExpr
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(VarExpr)("y"),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->subst("y", NEW(NumVal)(3))
//          ->equals(NEW(LetExpr)("x",
//                                NEW(NumExpr)(3),
//                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
//    CHECK( !(NEW(LetExpr)("x",
//                          NEW(VarExpr)("z"),
//                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->subst("y", NEW(NumVal)(3))
//          ->equals(NEW(LetExpr)("x",
//                                NEW(NumExpr)(3),
//                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
//
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(NumExpr)(3),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->subst("x", NEW(NumVal)(7))
//          ->equals(NEW(LetExpr)("x",
//                                NEW(NumExpr)(3),
//                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
//
//    // IfExpr
//    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
//                        NEW(BoolExpr)(true),
//                        NEW(BoolExpr)(false)))
//          ->subst("x", NEW(NumVal)(3))
//          ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
//                               NEW(BoolExpr)(true),
//                               NEW(BoolExpr)(false))) );
//
//    CHECK( !(NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
//                         NEW(BoolExpr)(true),
//                         NEW(BoolExpr)(false)))
//          ->subst("y", NEW(NumVal)(3))
//          ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)),
//                               NEW(BoolExpr)(true),
//                               NEW(BoolExpr)(false))) );
//
//    // CompExpr
//    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
//          ->subst("x", NEW(NumVal)(124))
//          ->equals(NEW(CompExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
//
//    CHECK( !(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
//          ->subst("yx", NEW(NumVal)(124))
//          ->equals(NEW(CompExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
//
//    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
//                          NEW(MultExpr)(NEW(NumExpr)(12), NEW(VarExpr)("x"))))
//          ->subst("x", NEW(NumVal)(124))
//          ->equals(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(124), NEW(NumExpr)(4)),
//                                 NEW(MultExpr)(NEW(NumExpr)(12), NEW(NumExpr)(124)))) );
//
//    // FunExpr
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->subst("x", NEW(NumVal)(3))
//          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("y", NEW(NumVal)(3))
//          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))) );
//    CHECK( !(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("x", NEW(NumVal)(3))
//          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("y")))) );
//
//    // CallFunExpr
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                             NEW(NumExpr)(4)))
//          ->subst("x", NEW(NumVal)(3))
//          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                                     NEW(NumExpr)(4)))) );
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))),
//                             NEW(NumExpr)(4)))
//          ->subst("y", NEW(NumVal)(3))
//          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))),
//                                     NEW(NumExpr)(4)))) );
//    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                              NEW(NumExpr)(4)))
//          ->subst("x", NEW(NumVal)(3))
//          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3))),
//                                     NEW(NumExpr)(4)))) );
//}
//
//TEST_CASE("ContainsVariable") {
//    // haveVariable method for NumExpr
//    CHECK( !(NEW(NumExpr)(5))->containsVar() );
//    CHECK( !(NEW(NumExpr)(32))->containsVar());
//
//    // AddExpr
//    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)))->containsVar() );
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->containsVar() );
//
//    // MultExpr
//    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->containsVar() );
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->containsVar() );
//
//    // VarExpr
//    CHECK( (NEW(VarExpr)("x"))->containsVar() ) ;
//    CHECK( (NEW(VarExpr)("abcd"))->containsVar() );
//
//    // BoolExpr
//    CHECK( !(NEW(BoolExpr)(true))->containsVar()) ;
//    CHECK( !(NEW(BoolExpr)(false))->containsVar() );
//
//    // LetExpr
//    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->containsVar() );
//    CHECK( !(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->containsVar() );
//
//    // IfExpr
//    CHECK( !(NEW(LetExpr)("x",
//                          NEW(NumExpr)(1),
//                          NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))
//          ->containsVar() );
//
//    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),
//                        NEW(BoolExpr)(true),
//                        NEW(BoolExpr)(false)))
//          ->containsVar() );
//
//    // CompExpr
//    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))->containsVar() );
//    CHECK( !(NEW(CompExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(2)))->containsVar() );
//
//    // FunExpr
//    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),NEW(VarExpr)("x"))))->containsVar() );
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->containsVar() );
//
//    // CallFunExpr
//    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),NEW(NumExpr)(4)))->containsVar() );
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))),
//                             NEW(NumExpr)(4)))->containsVar() );
//}
//
//TEST_CASE("optimizer") {
//    // optimizer method for NumExpr
//    CHECK( (NEW(NumExpr)(1))->optimizer()->equals(NEW(NumExpr)(1)) );
//    CHECK( !(NEW(NumExpr)(1))->optimizer()->equals(NEW(NumExpr)(10)) );
//
//    // AddExpr
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->optimizer()->equals(NEW(NumExpr)(5)) );
//    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x")))->optimizer()
//          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x"))) );
//
//    // MultExpr
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(5)))->optimizer()->equals(NEW(NumExpr)(15)) );
//    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->optimizer()
//          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))) );
//
//    // VarExpr
//    CHECK( (NEW(VarExpr)("x"))->optimizer()->equals(NEW(VarExpr)("x")) );
//    CHECK( !(NEW(VarExpr)("x"))->optimizer()->equals(NEW(VarExpr)("y")) );
//
//    // BoolExpr
//    CHECK( (NEW(BoolExpr)(true))->optimizer()->equals(NEW(BoolExpr)(true)) );
//    CHECK( !(NEW(BoolExpr)(true))->optimizer()->equals(NEW(BoolExpr)(false)) );
//
//    // optimize method for LetExpr
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(VarExpr)("y"),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
//          ->optimizer()
//          ->equals(NEW(LetExpr)("x",
//                                NEW(VarExpr)("y"),
//                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
//    CHECK( (NEW(LetExpr)("x",
//                         NEW(NumExpr)(6),
//                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5))))
//          ->optimizer()->equals(NEW(NumExpr)(11)) );
//
//    // IfExpr
//    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)),
//                                      NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))),
//                        NEW(BoolExpr)(true),
//                        NEW(BoolExpr)(false)))
//          ->optimizer()->equals(NEW(BoolExpr)(true)));
//
//    CHECK( (NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)),
//                                      NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))),
//                        NEW(BoolExpr)(true),
//                        NEW(BoolExpr)(false)))
//          ->optimizer()->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)),
//                                                         NEW(NumExpr)(8)),
//                                           NEW(BoolExpr)(true),
//                                           NEW(BoolExpr)(false))));
//
//    // CompExpr
//    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)),
//                          NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
//          ->optimizer()->equals(NEW(BoolExpr)(true)));
//
//    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
//          ->optimizer()->equals(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))));
//
//    CHECK( !(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
//          ->optimizer()->equals(NEW(BoolExpr)(true)));
//
//    // FunExpr
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->optimizer()
//          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
//    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),
//                                           NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5)))))->optimizer()
//          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
//    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),
//                                            NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5)))))->optimizer()
//          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
//
//
//    // CallFunExpr
//    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                             NEW(NumExpr)(4)))
//          ->optimizer()->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))) );
//
//    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))),
//                              NEW(NumExpr)(5)))
//          ->optimizer()->equals(NEW(NumExpr)(25)) );
//
//}
//
