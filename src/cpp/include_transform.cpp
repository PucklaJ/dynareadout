#include "include_transform.hpp"
#include <cstdlib>

namespace dro {

IncludeTransform::IncludeTransform(Keyword &kw) noexcept {
  m_handle = key_parse_include_transform(kw.m_handle);
}

IncludeTransform::IncludeTransform() noexcept {
  m_handle.file_name = NULL;
  m_handle.idnoff = IDNOFF_DEFAULT;
  m_handle.ideoff = IDEOFF_DEFAULT;
  m_handle.idpoff = IDPOFF_DEFAULT;
  m_handle.idmoff = IDMOFF_DEFAULT;
  m_handle.idsoff = IDSOFF_DEFAULT;
  m_handle.idfoff = IDFOFF_DEFAULT;
  m_handle.iddoff = IDDOFF_DEFAULT;
  m_handle.idroff = IDROFF_DEFAULT;
  m_handle.prefix = PREFIX_DEFAULT;
  m_handle.suffix = SUFFIX_DEFAULT;
  m_handle.fctmas = FCTMAS_DEFAULT;
  m_handle.fcttim = FCTTIM_DEFAULT;
  m_handle.fctlen = FCTLEN_DEFAULT;
  m_handle.fcttem = FCTTEM_DEFAULT;
  m_handle.incout1 = INCOUT1_DEFAULT;
  m_handle.tranid = TRANID_DEFAULT;
}

IncludeTransform::~IncludeTransform() noexcept {
  key_free_include_transform(&m_handle);
}

void IncludeTransform::parse_include_transform_card(
    Card card, size_t card_index) noexcept {
  key_parse_include_transform_card(&m_handle, card.m_handle, card_index);
}

TransformationOption::TransformationOption(transformation_option_t *handle)
    : m_handle(handle) {}

DefineTransformation::DefineTransformation(Keyword &kw,
                                           bool is_title) noexcept {
  m_handle =
      key_parse_define_transformation(kw.m_handle, static_cast<int>(is_title));
}

DefineTransformation::DefineTransformation() noexcept {
  m_handle.tranid = 0;
  m_handle.options = NULL;
  m_handle.num_options = 0;
}

DefineTransformation::~DefineTransformation() noexcept {
  key_free_define_transformation(&m_handle);
}

void DefineTransformation::parse_define_transformation_card(
    Card card, size_t card_index, bool is_title) noexcept {
  key_parse_define_transformation_card(&m_handle, card.m_handle, card_index,
                                       static_cast<int>(is_title));
}

Array<TransformationOption> DefineTransformation::get_options() noexcept {
  if (m_handle.num_options != 0) {
    auto options = Array<TransformationOption>::New(m_handle.num_options);
    for (size_t i = 0; i < m_handle.num_options; i++) {
      options[i] = TransformationOption(&m_handle.options[i]);
    }
    return options;
  }

  return Array<TransformationOption>();
}

} // namespace dro