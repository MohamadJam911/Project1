#ifndef POINTERWRAPPER_H
#define POINTERWRAPPER_H

#include <utility>
#include <stdexcept>

/**
 * PointerWrapper - an owning wrapper around a raw pointer (Phase 3).
 *
 * Semantics are similar to std::unique_ptr:
 * - The wrapper owns the pointed-to object and deletes it on destruction.
 * - Copying is disabled, so there is always exactly one owner.
 * - Ownership can be transferred with move construction / move assignment.
 */
template<typename T>
class PointerWrapper {
private:
    T* ptr;  // Raw pointer to the managed object (nullptr when empty)

public:
    // ========== CONSTRUCTION AND DESTRUCTION ==========

    /**
     * Default constructor - creates an empty wrapper
     */
    PointerWrapper() : ptr(nullptr) {}

    /**
     * Constructor from raw pointer - takes ownership of p
     */
    explicit PointerWrapper(T* p) : ptr(p) {}

    /**
     * Destructor - deletes the owned object (deleting nullptr is a no-op)
     */
    ~PointerWrapper() {
        delete ptr;
    }

    // ========== COPY OPERATIONS (DELETED) ==========

    /**
     * Copy constructor is deleted: two wrappers owning the same pointer
     * would both delete it (double free).
     */
    PointerWrapper(const PointerWrapper& other) = delete;

    /**
     * Copy assignment is deleted for the same reason.
     */
    PointerWrapper& operator=(const PointerWrapper& other) = delete;

    // ========== MOVE OPERATIONS ==========

    /**
     * Move constructor - steals other's pointer and leaves other empty
     */
    PointerWrapper(PointerWrapper&& other) noexcept
        : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    /**
     * Move assignment - frees the current object, then takes other's pointer
     * and leaves other empty. Self-assignment is a no-op.
     */
    PointerWrapper& operator=(PointerWrapper&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // ========== ACCESS OPERATIONS ==========

    /**
     * Dereference operator
     * @throws std::runtime_error if the wrapper is empty
     */
    T& operator*() const {
        if (ptr == nullptr) {
            throw std::runtime_error("PointerWrapper: attempted to dereference a null pointer");
        }
        return *ptr;
    }

    /**
     * Member access operator
     * @throws std::runtime_error if the wrapper is empty
     */
    T* operator->() const {
        if (ptr == nullptr) {
            throw std::runtime_error("PointerWrapper: attempted to access a member through a null pointer");
        }
        return ptr;
    }

    /**
     * Returns the raw pointer without transferring ownership.
     * Returns nullptr if the wrapper is empty (does not throw), so callers
     * can use it for null checks.
     */
    T* get() const {
        return ptr;
    }

    // ========== OWNERSHIP MANAGEMENT ==========

    /**
     * Gives up ownership: returns the raw pointer and leaves the wrapper empty.
     * The caller becomes responsible for deleting the returned object.
     */
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * Deletes the currently owned object (if any) and takes ownership of new_ptr.
     * Resetting to the pointer already owned does nothing.
     */
    void reset(T* new_ptr = nullptr) {
        if (ptr != new_ptr) {
            delete ptr;
            ptr = new_ptr;
        }
    }

    // ========== UTILITY FUNCTIONS ==========

    /**
     * True if the wrapper owns an object. Marked explicit so a wrapper
     * is not silently converted to bool/int in arithmetic or comparisons.
     */
    explicit operator bool() const {
        return ptr != nullptr;
    }

    /**
     * Swap the owned pointers of two wrappers
     */
    void swap(PointerWrapper& other) noexcept {
        std::swap(ptr, other.ptr);
    }
};

// ========== NON-MEMBER FUNCTIONS ==========

/**
 * Helper that allocates a T with the given constructor arguments and
 * wraps it, so a raw `new` never appears at the call site.
 */
template<typename T, typename... Args>
PointerWrapper<T> make_pointer_wrapper(Args&&... args) {
    return PointerWrapper<T>(new T(std::forward<Args>(args)...));
}

/**
 * Non-member swap, delegating to the member swap
 */
template<typename T>
void swap(PointerWrapper<T>& lhs, PointerWrapper<T>& rhs) noexcept {
    lhs.swap(rhs);
}

#endif // POINTERWRAPPER_H
