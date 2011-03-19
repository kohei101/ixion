/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "formula_name_resolver.hpp"
#include "address.hpp"

#include <iostream>
#include <cassert>
#include <string>

using namespace std;
using namespace ixion;

void test_name_resolver()
{
    cout << "test name resolver" << endl;
    formula_name_resolver_a1 resolver;
    string name_a1("AA23");
    cout << name_a1 << endl;
    formula_name_type res = resolver.resolve(name_a1);
    assert(res.type == formula_name_type::cell_reference);
    address_t addr;
    addr.sheet = res.address.sheet;
    addr.row = res.address.row;
    addr.column = res.address.col;
    cout << addr.get_name() << endl;
    string test_name = resolver.get_name(addr);
    cout << test_name << endl;
    assert(name_a1 == test_name);
}

int main()
{
    test_name_resolver();
    return EXIT_SUCCESS;
}
