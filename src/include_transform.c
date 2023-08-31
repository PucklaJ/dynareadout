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

#include "include_transform.h"
#include <stdlib.h>

include_transform_t key_parse_include_transform(keyword_t *keyword) {
  include_transform_t it;
  it.file_name = NULL;
  it.idnoff = IDNOFF_DEFAULT;
  it.ideoff = IDEOFF_DEFAULT;
  it.idpoff = IDPOFF_DEFAULT;
  it.idmoff = IDMOFF_DEFAULT;
  it.idsoff = IDSOFF_DEFAULT;
  it.idfoff = IDFOFF_DEFAULT;
  it.iddoff = IDDOFF_DEFAULT;
  it.idroff = IDROFF_DEFAULT;
  it.prefix = PREFIX_DEFAULT;
  it.suffix = SUFFIX_DEFAULT;
  it.fctmas = FCTMAS_DEFAULT;
  it.fcttim = FCTTIM_DEFAULT;
  it.fctlen = FCTLEN_DEFAULT;
  it.fcttem = FCTTEM_DEFAULT;
  it.incout1 = INCOUT1_DEFAULT;
  it.tranid = TRANID_DEFAULT;

  /* Parse five cards*/
  size_t i = 0;
  while (i < keyword->num_cards && i < 5) {
    key_parse_include_transform_card(&it, &keyword->cards[i], i);

    i++;
  }

  return it;
}

void key_parse_include_transform_card(include_transform_t *it, card_t *card,
                                      size_t card_index) {
  switch (card_index) {
  case 0:
    it->file_name = card_parse_whole(card);
    break;
  case 1:
    card_parse_begin(card, DEFAULT_VALUE_WIDTH);

    int64_t *parse_value = &it->idnoff;
    const int64_t *parse_end = &it->iddoff;
    while (!card_parse_done(card) && parse_value <= parse_end) {
      _card_try_parse_int(card, parse_value);
      card_parse_next(card);

      parse_value++;
    }
    break;
  case 2:
    card_parse_begin(card, DEFAULT_VALUE_WIDTH);

    if (card_parse_done(card))
      break;
    _card_try_parse_int(card, &it->idroff);
    card_parse_next(card);
    if (card_parse_done(card))
      break;
    card_parse_next(card);
    if (card_parse_done(card))
      break;
    it->prefix = card_parse_string(card);
    card_parse_next(card);
    if (card_parse_done(card))
      break;
    it->suffix = card_parse_string(card);
    break;
  case 3:
    card_parse_begin(card, DEFAULT_VALUE_WIDTH);

    double *parse_value_float = &it->fctmas;
    while (!card_parse_done(card) && parse_value_float <= &it->fctlen) {
      _card_try_parse_float64(card, parse_value_float);
      card_parse_next(card);

      parse_value_float++;
    }

    if (card_parse_done(card))
      break;
    it->fcttem = card_parse_string(card);
    card_parse_next(card);
    if (card_parse_done(card))
      break;
    _card_try_parse_int(card, &it->incout1);
    break;
  case 4:
    card_parse_begin(card, DEFAULT_VALUE_WIDTH);
    if (card_parse_done(card))
      break;

    _card_try_parse_int(card, &it->tranid);
    break;
  }
}

void key_free_include_transform(include_transform_t *it) {
  if (it->file_name) {
    free(it->file_name);
  }

  if (it->prefix) {
    free(it->prefix);
  }

  if (it->suffix) {
    free(it->suffix);
  }
}

void _card_try_parse_int(const card_t *card, int64_t *value) {
  const card_parse_type card_type = card_parse_get_type(card);
  switch (card_type) {
  case CARD_PARSE_INT:
    *value = card_parse_int(card);
    break;
  case CARD_PARSE_FLOAT:
    const double float_value = card_parse_float64(card);
    *value = (int64_t)float_value;
    break;
  default:
    break;
  }
}

void _card_try_parse_float64(const card_t *card, double *value) {
  const card_parse_type card_type = card_parse_get_type(card);
  switch (card_type) {
  case CARD_PARSE_INT:
  case CARD_PARSE_FLOAT:
    *value = card_parse_float64(card);
    break;
  default:
    break;
  }
}