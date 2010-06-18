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

#ifndef __IXION_FORMULA_RESULT_HPP__
#define __IXION_FORMULA_RESULT_HPP__

#include "global.hpp"

#include <string>

namespace ixion {

/**
 * Store formula result which may be either numeric, textural, or error.  In
 * case the result is textural, it owns the instance of the string. 
 */
class formula_result
{
public:
    enum result_type { rt_value, rt_string, rt_error };

    formula_result();
    formula_result(const formula_result& r);
    formula_result(double v);
    formula_result(::std::string* p);
    formula_result(formula_error_t e);
    ~formula_result();

    void set_value(double v);
    void set_string(::std::string* p);
    void set_error(formula_error_t e);
    
    double get_value() const;
    const ::std::string& get_string() const;
    formula_error_t get_error() const;

    result_type get_type() const;

    ::std::string str() const;

    /**
     * Parse a textural representation of a formula result, and set result 
     * value of appropriate type.
     * 
     * @param str textural representation of a formula result.
     */
    void parse(const ::std::string& str);

    formula_result& operator= (const formula_result& r);
    bool operator== (const formula_result& r) const;
    bool operator!= (const formula_result& r) const;

private:
    void parse_error(const ::std::string& str);
    void parse_string(const ::std::string& str);

private:
    result_type m_type;
    union {
        ::std::string* m_string;
        double m_value;
        formula_error_t m_error;
    };
};

}

#endif