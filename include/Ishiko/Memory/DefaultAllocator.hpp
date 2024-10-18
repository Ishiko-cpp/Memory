// SPDX-FileCopyrightText: 2005-2024 Xavier Leclercq
// SPDX-License-Identifier: BSL-1.0

#ifndef GUARD_ISHIKO_CPP_MEMORY_DEFAULTALLOCATOR_HPP
#define GUARD_ISHIKO_CPP_MEMORY_DEFAULTALLOCATOR_HPP

#include "MemoryErrorCategory.hpp"
#include <Ishiko/BasePlatform.hpp>
#include <Ishiko/Errors.hpp>

namespace Ishiko
{
    template<typename T, typename... ArgTypes>
    T* NewObject(Error& error, ArgTypes&&... args) noexcept;

    template<typename T>
    void DeleteObject(T* ptr) noexcept;

    template<typename T>
    T* NewObjectArray(size_t size, Error& error) noexcept;

    template<typename T>
    void DeleteObjectArray(T* ptr) noexcept;

    template<typename T, typename... ArgTypes>
    T* NewAlignedObject(Error& error, ArgTypes&&... args) noexcept;

    template<typename T>
    void DeleteAlignedObject(T* ptr) noexcept;

    template<typename T>
    T* NewAlignedObjectArray(size_t size, Error& error) noexcept;

    template<typename T>
    void DeleteAlignedObjectArray(size_t size, T* ptr) noexcept;
}

template<typename T, typename... ArgTypes>
T* Ishiko::NewObject(Error& error, ArgTypes&&... args) noexcept
{
    static_assert(noexcept(T(std::forward<ArgTypes>(args)...)));

    T* result = new(std::nothrow) T(std::forward<ArgTypes>(args)...);
    if (result == nullptr)
    {
        Fail(MemoryErrorCategory::Value::memory_allocation_error, "Failed to allocate memory", __FILE__, __LINE__,
            error);
    }
    return result;
}

template<typename T>
void Ishiko::DeleteObject(T* ptr) noexcept
{
    delete ptr;
}

// TODO: behaviour if size is 0?
template<typename T>
T* Ishiko::NewObjectArray(size_t size, Error& error) noexcept
{
    static_assert(noexcept(T()));

    T* result = new(std::nothrow) T[size];
    if (result == nullptr)
    {
        Fail(MemoryErrorCategory::Value::memory_allocation_error, "Failed to allocate memory", __FILE__, __LINE__,
            error);
    }
    return result;
}

template<typename T>
void Ishiko::DeleteObjectArray(T* ptr) noexcept
{
    delete[] ptr;
}

template<typename T, typename... ArgTypes>
T* Ishiko::NewAlignedObject(Error& error, ArgTypes&&... args) noexcept
{
    static_assert(noexcept(T(std::forward<ArgTypes>(args)...)));

#if ISHIKO_COMPILER == ISHIKO_COMPILER_GCC
    void* allocated_memory = aligned_alloc(sizeof(T), alignof(T));
#elif ISHIKO_COMPILER == ISHIKO_COMPILER_MSVC
    void* allocated_memory = _aligned_malloc(sizeof(T), alignof(T));
#else
#error Unsupported or unrecognized compiler
#endif

    // TODO: check for allocated_memory error

    T* result = new(allocated_memory) T(std::forward<ArgTypes>(args)...);
    if (result == nullptr)
    {
        Fail(MemoryErrorCategory::Value::memory_allocation_error, "Failed to allocate memory", __FILE__, __LINE__,
            error);
    }
    return result;
}

template<typename T>
void Ishiko::DeleteAlignedObject(T* ptr) noexcept
{
    if (ptr)
    {
        ptr->~T();
#if ISHIKO_COMPILER == ISHIKO_COMPILER_GCC
        free(ptr);
#elif ISHIKO_COMPILER == ISHIKO_COMPILER_MSVC
        _aligned_free(ptr);
#else
#error Unsupported or unrecognized compiler
#endif
    }
}

// TODO: behaviour if size is 0?
template<typename T>
T* Ishiko::NewAlignedObjectArray(size_t size, Error& error) noexcept
{
    static_assert(noexcept(T()));

#if ISHIKO_COMPILER == ISHIKO_COMPILER_GCC
    T* allocated_memory = (T*)aligned_alloc(size * sizeof(T), alignof(T));
#elif ISHIKO_COMPILER == ISHIKO_COMPILER_MSVC
    T* allocated_memory = (T*)_aligned_malloc(size * sizeof(T), alignof(T));
#else
#error Unsupported or unrecognized compiler
#endif

    // TODO: check for allocated_memory error
    /*
    if (result == nullptr)
    {
        Fail(MemoryErrorCategory::Value::memory_allocation_error, "Failed to allocate memory", __FILE__, __LINE__,
            error);
    }
    */

    for (size_t i = 0; i < size; ++i)
    {
        new(allocated_memory + i) T();
    }

    return allocated_memory;
}


template<typename T>
void Ishiko::DeleteAlignedObjectArray(size_t size, T* ptr) noexcept
{
    if (ptr)
    {
        for (size_t i = 0; i < size; ++i)
        {
            ptr[i].~T();
        }
#if ISHIKO_COMPILER == ISHIKO_COMPILER_GCC
        free(ptr);
#elif ISHIKO_COMPILER == ISHIKO_COMPILER_MSVC
        _aligned_free(ptr);
#else
#error Unsupported or unrecognized compiler
#endif
    }
}

#endif
