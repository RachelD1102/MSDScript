//
//  pointer.hpp
//  ArithemticParser2
//
//  Created by Rachel Di on 3/2/20.
//  Copyright © 2020 Rachel Di. All rights reserved.
//

#ifndef pointer_hpp
#define pointer_hpp

#if 1

# define NEW(T)  new T
# define PTR(T)  T*
# define CAST(T) dynamic_cast<T*>
# define THIS    this
# define ENABLE_THIS(T) /* empty */

#else

# define NEW(T)  std::make_shared<T>
# define PTR(T)  std::shared_ptr<T>
# define CAST(T) std::dynamic_pointer_cast<T>
# define THIS    shared_from_this()
# define ENABLE_THIS(T) : std::enable_shared_from_this<T>

#endif

#endif /* pointer_hpp */
