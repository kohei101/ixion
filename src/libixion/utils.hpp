/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_IXION_DETAIL_UTILS_HPP
#define INCLUDED_IXION_DETAIL_UTILS_HPP

#include "ixion/types.hpp"
#include "column_store_type.hpp"

namespace ixion { namespace detail {

celltype_t to_celltype(mdds::mtv::element_t mtv_type);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
