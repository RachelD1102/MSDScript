//
//  API.hpp
//  ArithemticParser2
//
//  Created by Rachel Di on 4/13/20.
//  Copyright © 2020 Rachel Di. All rights reserved.
//

#ifndef API_hpp
#define API_hpp

#include <string>
#include <sstream>

std::string interp(std::istream& input);

std::string step_interp(std::istream &input);

std::string optimizer(std::istream& input);

#endif /* API_hpp */
