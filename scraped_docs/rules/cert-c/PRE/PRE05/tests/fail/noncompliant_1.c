#define static_assert(e) \
  typedef char JOIN(assertion_failed_at_line_, __LINE__) \
    [(e) ? 1 : -1]