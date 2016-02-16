#ifndef _THE_COMMON_TYPES_
#define _THE_COMMON_TYPES_

// #define GLM_FORCE_CXX11
// #define GLM_FORCE_PURE

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <string>
#include <cstring>
#include <cmath>
#include <sstream>
#include <climits>
#include <chrono>
#include <atomic>
#include <iterator>
#include <cassert>
#include <type_traits>

#include "pugixml.hpp"
//#include "TheExternals/glm/fwd.hpp"
//#include "TheExternals/glm/glm.hpp"
#include "TheEngine/openGL.h"

using byte  	= uint8_t;
using uint  	= uint32_t;
using strref 	= const std::string &;
using callable 	= std::function<void()>;
using bytevec 	= std::vector<uint8_t>;
using timepoint = std::chrono::time_point<std::chrono::system_clock>;

template <typename T> using dictionary = std::unordered_map<std::string, T>;
template <typename T> using index_map  = std::unordered_map<size_t, T>;

#define THE_SHARED(X)               \
	public:                         \
	using ptr = std::shared_ptr<X>; \
	using cref = const ptr &;       \
	template <typename ... A>       \
    static ptr make(A && ... args) {return std::make_shared<X>(args...);}

#endif
