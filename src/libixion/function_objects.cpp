/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "function_objects.hpp"
#include "depends_tracker.hpp"

#include "ixion/cell_listener_tracker.hpp"
#include "ixion/address.hpp"
#include "ixion/formula_tokens.hpp"
#include "ixion/cell.hpp"
#include "ixion/interface/formula_model_access.hpp"
#include "ixion/formula_name_resolver.hpp"

#include <cassert>
#include <vector>
#include <algorithm>

namespace ixion {

namespace {

class ref_cell_picker : public std::unary_function<const formula_token*, void>
{
public:
    ref_cell_picker(iface::formula_model_access& cxt, const abs_address_t& origin, std::vector<abs_address_t>& deps) :
        m_context(cxt), m_origin(origin), m_deps(deps) {}

    void operator() (const formula_token* p)
    {
        switch (p->get_opcode())
        {
            case fop_single_ref:
            {
                abs_address_t addr = p->get_single_ref().to_abs(m_origin);
                if (m_context.get_celltype(addr) != celltype_t::formula)
                    break;

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
                            if (m_context.get_celltype(addr) != celltype_t::formula)
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
    iface::formula_model_access& m_context;
    const abs_address_t& m_origin;
    std::vector<abs_address_t>&  m_deps;
};

class depcell_inserter : public std::unary_function<abs_address_t, void>
{
public:
    depcell_inserter(dependency_tracker& tracker, const abs_address_set_t& dirty_cells, const abs_address_t& fcell) :
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
    const abs_address_set_t& m_dirty_cells;
    abs_address_t m_fcell;
};

}

cell_dependency_handler::cell_dependency_handler(
    iface::formula_model_access& cxt, dependency_tracker& dep_tracker, abs_address_set_t& dirty_cells) :
    m_context(cxt), m_dep_tracker(dep_tracker), m_dirty_cells(dirty_cells) {}

void cell_dependency_handler::operator() (const abs_address_t& fcell)
{
    // Register cell dependencies.
    formula_cell* p = m_context.get_formula_cell(fcell);
    assert(p);
    std::vector<const formula_token*> ref_tokens = p->get_ref_tokens(m_context, fcell);

    // Pick up the referenced cells from the ref tokens.  I should
    // probably combine this with the above get_ref_tokens() call above
    // for efficiency.
    std::vector<abs_address_t> deps;
    for_each(ref_tokens.begin(), ref_tokens.end(), ref_cell_picker(m_context, fcell, deps));

    // Register dependency information.  Only dirty cells should be
    // registered as precedent cells since non-dirty cells are equivalent
    // to constants.
    for_each(deps.begin(), deps.end(), depcell_inserter(m_dep_tracker, m_dirty_cells, fcell));
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
