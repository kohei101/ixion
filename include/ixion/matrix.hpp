/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_IXION_MATRIX_HPP
#define INCLUDED_IXION_MATRIX_HPP

#include "ixion/env.hpp"

#include <memory>
#include <vector>

namespace ixion {

class numeric_matrix;

/**
 * 2-dimensional matrix consisting of elements of variable types.  Each
 * element can be numeric, string, or empty.  This class is used to
 * represent range values or in-line matrices.
 */
class IXION_DLLPUBLIC matrix
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    matrix();
    matrix(size_t rows, size_t cols);
    matrix(const matrix& other);
    matrix(matrix&& other);
    matrix(const numeric_matrix& other);
    ~matrix();

    matrix& operator= (matrix other);

    /**
     * Determine if the entire matrix consists only of numeric value elements.
     *
     * @return true if the entire matrix consits only of numeric value
     *         elements, false otherwise.
     */
    bool is_numeric() const;

    bool is_numeric(size_t row, size_t col) const;
    double get_numeric(size_t row, size_t col) const;
    void set(size_t row, size_t col, double val);
    size_t row_size() const;
    size_t col_size() const;

    void swap(matrix& r);

    numeric_matrix as_numeric() const;
};

class IXION_DLLPUBLIC numeric_matrix
{
    friend class matrix;

    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    numeric_matrix();
    numeric_matrix(size_t rows, size_t cols);
    numeric_matrix(std::vector<double> array, size_t rows, size_t cols);
    numeric_matrix(numeric_matrix&& r);
    ~numeric_matrix();

    numeric_matrix& operator= (numeric_matrix other);

    double& operator() (size_t row, size_t col);
    const double& operator() (size_t row, size_t col) const;

    void swap(numeric_matrix& r);

    size_t row_size() const;
    size_t col_size() const;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
