#pragma once
#include <vector>
#include <optional>
#include <string>
#include <functional>

namespace bloodline {

// OOP: ABSTRACTION — IRepository<T> is a pure interface.
// Callers depend on this interface, never on the concrete file storage.
// The storage backend can be swapped without touching any other code.

template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void load() = 0;
    virtual void save() = 0;
    virtual std::vector<T>& all() = 0;
    virtual const std::vector<T>& all() const = 0;
    virtual std::optional<std::reference_wrapper<T>> find(
        const std::string& key) = 0;
    virtual std::optional<std::reference_wrapper<const T>> find(
        const std::string& key) const = 0;
    virtual void add(T item) = 0;
};

// OOP: POLYMORPHISM (compile-time) — template filter helper works for any entity type.
template <typename T, typename Pred>
std::vector<std::reference_wrapper<const T>> filter(
    const std::vector<T>& items, Pred predicate) {
    std::vector<std::reference_wrapper<const T>> result;
    for (const auto& item : items) {
        if (predicate(item)) {
            result.push_back(std::cref(item));
        }
    }
    return result;
}

} // namespace bloodline
