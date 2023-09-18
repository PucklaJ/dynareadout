#pragma once
#include "array.hpp"
#include "key.hpp"
#include <include_transform.h>

namespace dro {

class IncludeTransform {
public:
  IncludeTransform(Keyword &kw) noexcept;
  IncludeTransform() noexcept;
  ~IncludeTransform() noexcept;

  void parse_include_transform_card(Card card, size_t card_index) noexcept;

  inline String get_file_name() noexcept {
    return String(m_handle.file_name, false);
  }
  inline String get_prefix() noexcept { return String(m_handle.prefix, false); }
  inline String get_suffix() noexcept { return String(m_handle.suffix, false); }
  inline String get_fcttem() noexcept { return String(m_handle.fcttem, false); }

  inline int64_t get_idnoff() const noexcept { return m_handle.idnoff; }
  inline int64_t get_ideoff() const noexcept { return m_handle.ideoff; }
  inline int64_t get_idpoff() const noexcept { return m_handle.idpoff; }
  inline int64_t get_idmoff() const noexcept { return m_handle.idmoff; }
  inline int64_t get_idsoff() const noexcept { return m_handle.idsoff; }
  inline int64_t get_idfoff() const noexcept { return m_handle.idfoff; }
  inline int64_t get_iddoff() const noexcept { return m_handle.iddoff; }
  inline int64_t get_idroff() const noexcept { return m_handle.idroff; }
  inline int64_t get_incout1() const noexcept { return m_handle.incout1; }
  inline int64_t get_tranid() const noexcept { return m_handle.tranid; }

  inline double get_fctmas() const noexcept { return m_handle.fctmas; }
  inline double get_fcttim() const noexcept { return m_handle.fcttim; }
  inline double get_fctlen() const noexcept { return m_handle.fctlen; }

private:
  include_transform_t m_handle;
};

class TransformationOption {
public:
  TransformationOption(transformation_option_t *handle);

  inline String get_name() noexcept { return String(m_handle->name, false); }
  inline Array<double> get_parameters() noexcept {
    return Array<double>(m_handle->parameters, 7, false);
  }

private:
  transformation_option_t *m_handle;
};

class DefineTransformation {
public:
  DefineTransformation(Keyword &kw) noexcept;
  DefineTransformation() noexcept;
  ~DefineTransformation() noexcept;

  void parse_define_transformation_card(Card card, size_t card_index) noexcept;

  inline int64_t get_tranid() const noexcept { return m_handle.tranid; }
  Array<TransformationOption> &get_options() noexcept;
  inline Array<transformation_option_t> get_raw_options() noexcept {
    return Array<transformation_option_t>(m_handle.options,
                                          m_handle.num_options, false);
  }

private:
  define_transformation_t m_handle;
  Array<TransformationOption> m_options;
};

} // namespace dro