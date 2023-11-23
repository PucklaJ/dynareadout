#include "include_transform.hpp"
#include <cstdlib>

namespace dro {

IncludeTransform::IncludeTransform() noexcept { m_handle = {0}; }

IncludeTransform::IncludeTransform(IncludeTransform &&rhs) noexcept {
  *this = std::move(rhs);
}

IncludeTransform::IncludeTransform(const IncludeTransform &rhs) noexcept {
  *this = rhs;
}

IncludeTransform::IncludeTransform(Keyword &kw) noexcept {
  m_handle = key_parse_include_transform(kw.m_handle);
}

IncludeTransform::~IncludeTransform() noexcept {
  key_free_include_transform(&m_handle);
}

IncludeTransform &IncludeTransform::operator=(IncludeTransform &&rhs) noexcept {
  m_handle = rhs.m_handle;
  rhs.m_handle = {0};
  return *this;
}

IncludeTransform &
IncludeTransform::operator=(const IncludeTransform &rhs) noexcept {
  m_handle = rhs.m_handle;
  m_handle.file_name = strdup(rhs.m_handle.file_name);
  if (rhs.m_handle.prefix)
    m_handle.prefix = strdup(rhs.m_handle.prefix);
  if (rhs.m_handle.suffix)
    m_handle.suffix = strdup(rhs.m_handle.suffix);
  if (rhs.m_handle.fcttem)
    m_handle.fcttem = strdup(rhs.m_handle.fcttem);

  return *this;
}

void IncludeTransform::parse_include_transform_card(
    Card card, size_t card_index) noexcept {
  key_parse_include_transform_card(&m_handle, card.m_handle, card_index);
}

TransformationOption::TransformationOption(
    TransformationOption &&rhs) noexcept {
  *this = std::move(rhs);
}

TransformationOption::TransformationOption(transformation_option_t *handle)
    : m_handle(handle) {}

TransformationOption &
TransformationOption::operator=(TransformationOption &&rhs) noexcept {
  m_handle = rhs.m_handle;
  rhs.m_handle = nullptr;
  return *this;
}

DefineTransformation::DefineTransformation() noexcept { m_handle = {0}; }

DefineTransformation::DefineTransformation(
    DefineTransformation &&rhs) noexcept {
  *this = std::move(rhs);
}

DefineTransformation::DefineTransformation(
    const DefineTransformation &rhs) noexcept {
  *this = rhs;
}

DefineTransformation::DefineTransformation(Keyword &kw,
                                           bool is_title) noexcept {
  m_handle =
      key_parse_define_transformation(kw.m_handle, static_cast<int>(is_title));
}

DefineTransformation::~DefineTransformation() noexcept {
  key_free_define_transformation(&m_handle);
}

DefineTransformation &
DefineTransformation::operator=(DefineTransformation &&rhs) noexcept {
  m_handle = rhs.m_handle;
  rhs.m_handle = {0};
  return *this;
}

DefineTransformation &
DefineTransformation::operator=(const DefineTransformation &rhs) noexcept {
  m_handle.tranid = rhs.m_handle.tranid;
  if (rhs.m_handle.title) {
    m_handle.title = strdup(rhs.m_handle.title);
  }
  if (rhs.m_handle.num_options != 0 && rhs.m_handle.options) {
    m_handle.options = (transformation_option_t *)malloc(
        rhs.m_handle.num_options * sizeof(transformation_option_t));
    m_handle.num_options = rhs.m_handle.num_options;
    for (size_t i = 0; i < m_handle.num_options; i++) {
      m_handle.options[i].name = strdup(rhs.m_handle.options[i].name);
      memcpy(m_handle.options[i].parameters, rhs.m_handle.options[i].parameters,
             sizeof(transformation_option_t::parameters));
    }
  }
  return *this;
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