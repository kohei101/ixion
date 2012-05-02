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

#include "ixion/function_objects.hpp"
#include "ixion/cell_listener_tracker.hpp"
#include "ixion/address.hpp"
#include "ixion/depends_tracker.hpp"
#include "ixion/formula_tokens.hpp"
#include "ixion/cell.hpp"
#include "ixion/interface/model_context.hpp"

#include <vector>
#include <boost/scoped_ptr.hpp>

#define DEBUG_FUNCTION_OBJECTS 0

#if DEBUG_FUNCTION_OBJECTS
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace ixion {

namespace {

class ref_cell_picker : public std::unary_function<const formula_token_base*, void>
{
public:
    ref_cell_picker(iface::model_context& cxt, const abs_address_t& origin, std::vector<abs_address_t>& deps) :
        m_context(cxt), m_origin(origin), m_deps(deps) {}

    void operator() (const formula_token_base* p)
    {
        switch (p->get_opcode())
        {
            case fop_single_ref:
            {
                abs_address_t addr = p->get_single_ref().to_abs(m_origin);
                if (m_context.get_celltype(addr) == celltype_formula)
                    m_deps.push_back(addr);
            }
            break;
            case fop_range_ref:
            {
                abs_range_t range = p->get_range_ref().to_abs(m_origin);
                for (sheet_t sheet = range.first.sheet; sheet <= range.last.sheet; ++sheet)
                {
                    for (col_t col = range.first.column; col <= range.last.column; ++col)
                    {
                        for (row_t row = range.first.row; row <= range.last.row; ++row)
                        {
                            abs_address_t addr(sheet, row, col);
                            if (m_context.is_empty(addr) || m_context.get_celltype(addr) != celltype_formula)
                                continue;

                            m_deps.push_back(addr);
                        }
                    }
                }
            }
            break;
            default:
                ; // ignore the rest.
        }
    }

private:
    iface::model_context& m_context;
    const abs_address_t& m_origin;
    std::vector<abs_address_t>&  m_deps;
};

class depcell_inserter : public std::unary_function<abs_address_t, void>
{
public:
    depcell_inserter(dependency_tracker& tracker, const dirty_cells_t& dirty_cells, const abs_address_t& fcell) :
        m_tracker(tracker),
        m_dirty_cells(dirty_cells),
        m_fcell(fcell) {}

    void operator() (const abs_address_t& cell)
    {
        if (m_dirty_cells.count(cell) > 0)
            m_tracker.insert_depend(m_fcell, cell);
    }
private:
    dependency_tracker& m_tracker;
    const dirty_cells_t& m_dirty_cells;
    abs_address_t m_fcell;
};

}

formula_cell_listener_handler::formula_cell_listener_handler(
    iface::model_context& cxt, const abs_address_t& addr, mode_t mode) :
    m_context(cxt),
    m_listener_tracker(cxt.get_cell_listener_tracker()),
    m_addr(addr),
    m_mode(mode)
{
#if DEBUG_FUNCTION_OBJECTS
    __IXION_DEBUG_OUT__ << "formula_cell_listener_handler: cell position=" << m_addr.get_name() << endl;
#endif
}

void formula_cell_listener_handler::operator() (const formula_token_base* p) const
{
    switch (p->get_opcode())
    {
        case fop_single_ref:
        {
            abs_address_t addr = p->get_single_ref().to_abs(m_addr);
#if DEBUG_FUNCTION_OBJECTS
            __IXION_DEBUG_OUT__ << "formula_cell_listener_handler: ref address=" << addr.get_name() << endl;
#endif
            if (m_mode == mode_add)
            {
                m_listener_tracker.add(m_addr, addr);
            }
            else
            {
                assert(m_mode == mode_remove);
                m_listener_tracker.remove(m_addr, addr);
            }
        }
        break;
        case fop_range_ref:
        {
            abs_range_t range = p->get_range_ref().to_abs(m_addr);
            if (m_mode == mode_add)
                m_context.get_cell_listener_tracker().add(m_addr, range);
            else
            {
                assert(m_mode == mode_remove);
                m_context.get_cell_listener_tracker().remove(m_addr, range);
            }
        }
        break;
        default:
            ; // ignore the rest.
    }
}

cell_dependency_handler::cell_dependency_handler(
    iface::model_context& cxt, dependency_tracker& dep_tracker, dirty_cells_t& dirty_cells) :
    m_context(cxt), m_dep_tracker(dep_tracker), m_dirty_cells(dirty_cells) {}

void cell_dependency_handler::operator() (const abs_address_t& fcell)
{
#if DEBUG_FUNCTION_OBJECTS
    __IXION_DEBUG_OUT__ << get_formula_result_output_separator() << endl;
    __IXION_DEBUG_OUT__ << "processing dependency of " << m_context.get_cell_name(fcell) << endl;
#endif
    // Register cell dependencies.
    std::vector<const formula_token_base*> ref_tokens;
    formula_cell* p = m_context.get_formula_cell(fcell);
    p->get_ref_tokens(m_context, ref_tokens);

#if DEBUG_FUNCTION_OBJECTS
    __IXION_DEBUG_OUT__ << "this cell contains " << ref_tokens.size() << " reference tokens." << endl;
#endif
    // Pick up the referenced cells from the ref tokens.  I should
    // probably combine this with the above get_ref_tokens() call above
    // for efficiency.
    std::vector<abs_address_t> deps;
    for_each(ref_tokens.begin(), ref_tokens.end(), ref_cell_picker(m_context, fcell, deps));

#if DEBUG_FUNCTION_OBJECTS
    __IXION_DEBUG_OUT__ << "number of precedent cells picked up: " << deps.size() << endl;
#endif
    // Register dependency information.  Only dirty cells should be
    // registered as precedent cells since non-dirty cells are equivalent
    // to constants.
    for_each(deps.begin(), deps.end(), depcell_inserter(m_dep_tracker, m_dirty_cells, fcell));
}

}
