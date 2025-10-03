#pragma once

#include <meta/src/meta/factory.hpp>
#include <meta/src/meta/meta.hpp>
#include <meta/src/meta/policy.hpp>

#define REFLECT
#define IFUNCTION
#define IPROPERTY

#define REFLECT_CLASS(_type) \
    meta::reflect<_type>(s_Hash(#_type))

#define REFLECT_BASE(_type) \
    base<_type>()

#define REFLECT_CTOR(...) \
    ctor<__VA_ARGS__>()

#define REFLECT_DTOR(_dtor) \
    dtor<&_dtor>()

#define REFLECT_FUNC(_function) \
    func<&_function>(s_Hash(#_function))

#define REFLECT_DATA(_type, _var) \
    data<&_type::_var>(s_Hash(#_var))

#define REFLECT_SGETTER(_var, _setter, _getter) \
    data<&_setter, &_getter>(s_Hash(#_var))

#define REFLECT_ELEM(_elem, _name) \
    data<_elem>(s_Hash(_name))

#define REFLECT_ENUM(_type) \
    meta::reflect<_type>(s_Hash(#_type))