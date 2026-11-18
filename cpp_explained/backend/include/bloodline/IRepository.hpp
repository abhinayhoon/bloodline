// EXPLAINED REFERENCE COPY. The original source is in ../backend/include/bloodline/IRepository.hpp and remains unchanged.
// This prevents this header from being included more than once in a compilation unit.
#pragma once
// This brings in <vector>, so this file can use its declarations.
#include <vector>
// This brings in <optional>, so this file can use its declarations.
#include <optional>
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <functional>, so this file can use its declarations.
#include <functional>

// This groups related C++ names together to avoid clashes with other code.
namespace bloodline {

// This is a source comment that explains nearby code.
// OOP: ABSTRACTION — IRepository<T> is a pure interface.
// This is a source comment that explains nearby code.
// Callers depend on this interface, never on the concrete file storage.
// This is a source comment that explains nearby code.
// The storage backend can be swapped without touching any other code.

// This begins a reusable type or template: a blueprint for related data and behavior.
template <typename T>
// This begins a reusable type or template: a blueprint for related data and behavior.
class IRepository {
// The following members are available to other parts of the program.
public:
// This performs one step of the current operation, then continues to the next statement.
    virtual ~IRepository() = default;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual void load() = 0;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual void save() = 0;
// This creates or updates a value that the following logic uses.
    virtual std::vector<T>& all() = 0;
// This creates or updates a value that the following logic uses.
    virtual const std::vector<T>& all() const = 0;
// This is part of the current expression or block; read it together with the nearby lines.
    virtual std::optional<std::reference_wrapper<T>> find(
// This creates or updates a value that the following logic uses.
        const std::string& key) = 0;
// This creates or updates a value that the following logic uses.
    virtual std::optional<std::reference_wrapper<const T>> find(
// This creates or updates a value that the following logic uses.
        const std::string& key) const = 0;
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
    virtual void add(T item) = 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// OOP: POLYMORPHISM (compile-time) — template filter helper works for any entity type.
// This begins a reusable type or template: a blueprint for related data and behavior.
template <typename T, typename Pred>
// This creates or updates a value that the following logic uses.
std::vector<std::reference_wrapper<const T>> filter(
// This creates or updates a value that the following logic uses.
    const std::vector<T>& items, Pred predicate) {
// This creates or updates a value that the following logic uses.
    std::vector<std::reference_wrapper<const T>> result;
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto& item : items) {
// This controls which part of the program runs next, based on the current condition or result.
        if (predicate(item)) {
// This performs one step of the current operation, then continues to the next statement.
            result.push_back(std::cref(item));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return result;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This closes the block opened above, such as a function, class, or control-flow branch.
} // namespace bloodline
