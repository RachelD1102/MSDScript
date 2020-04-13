//
//  env.hpp
//  ArithemticParser2
//
//  Created by Rachel Di on 3/20/20.
//  Copyright © 2020 Rachel Di. All rights reserved.
//

#ifndef env_hpp
#define env_hpp

#include <string>
#include "pointer.hpp"
#include "value.hpp"


class Val;

class Env {
public:
    static PTR(Env) emptyenv;
    
    virtual PTR(Val) lookup(std::string find_name) = 0;
    
    virtual bool equals(PTR(Env) env) = 0;
};

class EmptyEnv : public Env {
public:
    PTR(Val) lookup(std::string find_name);
    bool equals(PTR(Env) env);
};

class ExtendedEnv : public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) env);
    PTR(Val) lookup(std::string find_name);
    bool equals(PTR(Env) env);
};

#endif /* env_hpp */
