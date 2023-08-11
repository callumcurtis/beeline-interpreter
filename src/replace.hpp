#include <utility>


// RAII class that replaces a variable with another value until the end of the
// current scope.
template <typename T>
class ScopedReplace
{
public:
    ScopedReplace(T& first, T second) : first_{first}, second_{std::move(second)}
    {
        std::swap(first_, second_);
    }
    ~ScopedReplace()
    {
        std::swap(first_, second_);
    }
private:
    T& first_;
    T second_;
};
