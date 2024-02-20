/**
 * 2022.03.13
 *
 */

#ifndef PORTAL_BASE_TYPES_H
#define PORTAL_BASE_TYPES_H

#include <stdint.h>
#include <string>
#include <assert.h>

namespace portal
{

using namespace std;

// C++ 模板的使用，实现类型转换
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
    return f;
}

template<typename To, typename From>
inline To down_cast(From *f)
{
    if (false)
    {
        implicit_cast<From *, To>(0);
    }

    return static_cast<To>(f);
}

};

#endif

