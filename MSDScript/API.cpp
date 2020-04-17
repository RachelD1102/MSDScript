#include "API.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "step.hpp"
#include "parse.hpp"

//interp mode
std::string interp(std::istream& input) {
    std::string output = parse(input)->interp(Env::emptyenv)->to_string();
    return output;
}

//step_interp mode
std::string step_interp(std::istream &input) {
    std::string output = Step::interp_by_steps(parse(input))->to_string();
    return output;
}

//optimizer mode
std::string optimizer(std::istream& input) {
    std::string output = parse(input)->optimizer()->to_string();
    return output;
}

