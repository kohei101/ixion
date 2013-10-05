/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __IXION_FORMULA_TOKENS_FWD_HPP__
#define __IXION_FORMULA_TOKENS_FWD_HPP__

#include <boost/ptr_container/ptr_vector.hpp>

namespace ixion {

class formula_token_base;
typedef ::boost::ptr_vector<formula_token_base> formula_tokens_t;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
