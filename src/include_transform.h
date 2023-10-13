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

#ifndef INCLUDE_TRANSFORM_H
#define INCLUDE_TRANSFORM_H

#include "key.h"
#include <stdint.h>

#define IDNOFF_DEFAULT 0
#define IDEOFF_DEFAULT 0
#define IDPOFF_DEFAULT 0
#define IDMOFF_DEFAULT 0
#define IDSOFF_DEFAULT 0
#define IDFOFF_DEFAULT 0
#define IDDOFF_DEFAULT 0
#define IDROFF_DEFAULT 0
#define PREFIX_DEFAULT NULL
#define SUFFIX_DEFAULT NULL
#define FCTMAS_DEFAULT 1.0
#define FCTTIM_DEFAULT 1.0
#define FCTLEN_DEFAULT 1.0
#define FCTTEM_DEFAULT NULL
#define INCOUT1_DEFAULT 0
#define TRANID_DEFAULT 0

/* ls-dyna_manual_volume_i_r13.pdf p. 2685*/
typedef struct {
  char *file_name;

  int64_t idnoff; /* Offset to node ID */
  int64_t ideoff; /* Offset to element ID */
  int64_t idpoff; /* Offset to part ID, nodal rigid body ID, constrained nodal
                     set ID, rigidwall ID, and cross section ID (see
                     *DATABASE_CROSS_SECTION) */
  int64_t idmoff; /* Offset to material ID and equation of state ID */
  int64_t idsoff; /* Offset to set ID */
  int64_t idfoff; /* Offset to function ID, table ID, and curve ID */
  int64_t iddoff; /* Offset to any ID defined through *DEFINE, except the
                     FUNCTION, TABLE, and CURVE options (see IDFOFF) */
  int64_t idroff; /* Used for all offsets except for those listed above */
  char *prefix; /* Preﬁx added to the beginning of the titles/heads deﬁned in
                   the keywords (like *MAT, *PART, *SECTION, *DEFINE, for
                   examples) of the included ﬁle. A dot, “.”, is automatically
                   added between the preﬁx and the existing title. */
  char *suffix;  /* Suﬃx added to the end of the titles/heads deﬁned in the
                    keywords of the included ﬁle. A dot, “.”, is automatically
                    added between the suﬃx and the existing title. */
  double fctmas; /* Mass transformation factor. For example, FCTMAS = 1000 when
                    the original mass unit is in tons and the new unit is kg. */
  double fcttim; /* Time transformation factor. For example, FCTTIM = .001 when
                    the original time unit is in milliseconds and the new time
                    unit is seconds. */
  double fctlen; /* Length transformation factor */
  char *fcttem;  /* Temperature transformation factor consisting of a four
                    character ﬂag: FtoC (Fahrenheit to Centigrade), CtoF, FtoK,
                    KtoF, KtoC, and CtoK. */
  int64_t incout1; /* Set to 1 for the creation of a ﬁle, DYNA.INC, which
                      contains the transformed data. The data in this ﬁle can be
                      used in future include ﬁles and should be checked to
                      ensure that all the data was transformed correctly.*/
  int64_t tranid; /* Transformation ID. If 0, no transformation will be applied.
                     See *DEFINE_TRANSFORMATION.*/
} include_transform_t;

/* Holds the name and parameters for every option of a DEFINE_TRANSFORMATION*/
typedef struct {
  char *name;
  double parameters[7];
} transformation_option_t;

/* Holds the components of a DEFINE_TRANSFORMATION keyword*/
typedef struct {
  int64_t tranid; /* Transform ID */
  char
      *title; /* A user defined title if DEFINE_TRANSFORMATION_TITLE is parsed*/

  transformation_option_t *options; /* The Options like TRANSL, SCALE etc.*/
  size_t num_options;
} define_transformation_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Parses an INCLUDE_TRANSFORM keyword and returns all parsed values*/
include_transform_t key_parse_include_transform(keyword_t *keyword);
/* Parse a single card from an INCLUDE_TRANSFORM keyword*/
void key_parse_include_transform_card(include_transform_t *it, card_t *card,
                                      size_t card_index);
/* Parses an DEFINE_TRANSFORMATION keyword and returns all parsed values*/
define_transformation_t key_parse_define_transformation(keyword_t *keyword,
                                                        int is_title);
/* Parses a single card form a DEFINE_TRANSFORMATION keyword*/
void key_parse_define_transformation_card(define_transformation_t *dt,
                                          card_t *card, size_t card_index,
                                          int is_title);
/* Free all allocated memory of an INCLUDE_TRANSFORM*/
void key_free_include_transform(include_transform_t *it);
/* Free all allocated memory of DEFINE_TRANSFORMATION*/
void key_free_define_transformation(define_transformation_t *dt);

/* Only change the value of the type of the current value is int*/
void _card_try_parse_int(const card_t *card, int64_t *value);
/* Only change the value of the type of the current value is float*/
void _card_try_parse_float64(const card_t *card, double *value);

#ifdef __cplusplus
}
#endif

#endif