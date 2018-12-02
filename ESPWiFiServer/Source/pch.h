#if defined(__cplusplus)

#include <memory>
#include <assert.h>
#include <vector>
#include <map>
#include <array>

using byte = unsigned char;

#define EDITOR_ASSERT(expression) assert(expression)

#define DECLARE_POINTERS(x)                                 \
    class x;                                                \
    using x##SharedPtr = std::shared_ptr<x>;                \
    using x##ConstSharedPtr = std::shared_ptr<const x>;     \
    using x##WeakPtr = std::weak_ptr<x>;                    \
    using x##ConstWeakPtr = std::weak_ptr<const x>;         \
    using x##UniquePtr = std::unique_ptr<x>;                \
    using x##ConstUniquePtr = std::unique_ptr<const x>;     \

#include "Pointers.h"

#endif //__cplusplus