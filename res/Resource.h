#ifndef _RESOURCE_H
#define _RESOURCE_H
#include <cstddef>
class Resource
{
public:
  Resource(const char *start, const char *end)
      : mData(start), mSize(end - start)
  {
  }
  const char *const &data() const { return mData; }
  const size_t &size() const { return mSize; }
  const char *begin() const { return mData; }
  const char *end() const { return mData + mSize; }

private:
  const char *mData;
  size_t mSize;
};
#define LOAD_RESOURCE(x)                                                       \
  ([]() {                                                                      \
    extern const char _binary_##x##_start[], _binary_##x##_end[];              \
    return Resource(_binary_##x##_start, _binary_##x##_end);                   \
  })()

/**
 *#define LOAD_RESOURCE(x) \
 *  ([]() { \
 *    extern const char _binary_##x##_start, _binary_##x##_end; \
 *    return Resource(&_binary_##x##_start, &_binary_##x##_end); \
 *  })()
 */

extern Resource res_bk;
extern Resource res_tada;
extern Resource res_body;
extern Resource res_button;
extern Resource res_food;
extern Resource res_font;

#endif