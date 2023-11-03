/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 ************************************************************************************/

#pragma once
#include <array>
#include <cstdint>
#include <ostream>

namespace dro {

typedef std::array<uint64_t, 3> uVec3;
typedef std::array<double, 3> dVec3;
typedef std::array<float, 3> fVec3;

} // namespace dro

namespace std {

static ostream &operator<<(ostream &stream, const dro::uVec3 &vec) {
  return stream << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

static ostream &operator<<(ostream &stream, const dro::dVec3 &vec) {
  return stream << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

static ostream &operator<<(ostream &stream, const dro::fVec3 &vec) {
  return stream << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

} // namespace std