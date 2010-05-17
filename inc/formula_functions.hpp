/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include "global.hpp"

#include <string>
#include <vector>

namespace ixion {

class formula_token_base;

enum formula_function_t
{
    func_unknown = 0,

    func_max,
    func_min,
    func_average,

    func_wait // dummy function used only for testing.

    // TODO: more functions to come...
};

class formula_functions
{
public:
    typedef ::std::vector<double> args_type;

    class invalid_arg : public general_error
    {
    public:
        invalid_arg(const ::std::string& msg);
    };

    static formula_function_t get_function_opcode(const formula_token_base& token);
    static formula_function_t get_function_opcode(const ::std::string& name);
    static const char* get_function_name(formula_function_t oc);

    static double interpret(formula_function_t oc, const args_type& args);

    static double max(const args_type& args);
    static double wait(const args_type& args);
private:
    formula_functions();
    ~formula_functions();
};

}
