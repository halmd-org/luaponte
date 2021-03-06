// Luaponte library

// Copyright (c) 2012 Peter Colberg

// Luaponte is based on Luabind, a library, inspired by and similar to
// Boost.Python, that helps you create bindings between C++ and Lua,
// Copyright (c) 2003-2010 Daniel Wallin and Arvid Norberg.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include <luaponte/luaponte.hpp>
#include <luaponte/adopt_policy.hpp>
#include <iostream>

struct Base
{
    Base()
    {
        count++;
    }

    virtual ~Base()
    {
        count--;
    }

    static int count;
};

int Base::count = 0;

struct Base_wrap : Base, luaponte::wrap_base
{};

void destroy(Base* p)
{
    delete p;
}

Base* adopted = 0;

void take_ownership(Base* p)
{
    adopted = p;
}

void not_null(Base* p)
{
    TEST_CHECK(p);
}

void test_main(lua_State* L)
{
    using namespace luaponte;

    disable_super_deprecation();

    module(L) [
        class_<Base, Base_wrap>("Base")
            .def(constructor<>()),

        def("take_ownership", &take_ownership, adopt(_1)),
        def("not_null", &not_null)
    ];

    DOSTRING(L,
        "x = Base()\n"
    );

    TEST_CHECK(Base::count == 1);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

    DOSTRING(L,
        "class 'Derived' (Base)\n"
        "  function Derived:__init()\n"
        "      super()\n"
        "      self.x = Base()\n"
        "  end\n"
    );

    DOSTRING(L,
        "x = Derived()\n"
    );

    TEST_CHECK(Base::count == 2);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

     DOSTRING(L,
        "class 'Derived2' (Derived)\n"
        "  function Derived2:__init()\n"
        "      super()\n"
        "  end\n"
    );

    DOSTRING(L,
        "x = Derived2()\n"
    );

    TEST_CHECK(Base::count == 2);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived()\n"
    );

    TEST_CHECK(Base::count == 2);

    DOSTRING(L,
        "take_ownership(x)\n"
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 2);

    delete adopted;

    DOSTRING(L,
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived2()\n"
    );

    TEST_CHECK(Base::count == 2);

    DOSTRING(L,
        "take_ownership(x)\n"
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 2);

    delete adopted;

    DOSTRING(L,
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived()\n"
        "take_ownership(x)\n"
        "not_null(x)\n"
    );

    delete adopted;

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    TEST_CHECK(Base::count == 0);
}
