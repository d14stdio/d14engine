#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/TypeTraits.h"

namespace d14engine
{
    // Define ISortable as a template class, so that an object can implement
    // sorting functionality for different properties by inheriting
    // multiple ISortable<Target_T> (where T represents different properties).
    //
    // For example, suppose struct Student is inherited from
    // ISortable<Height> and ISortable<Weight>, in which case a student knows
    // their height and weight respectively and can be sorted by each of them.

    template<typename Target_T, typename Priority_T = int>
    struct ISortable
    {
        using Type = ISortable<Target_T, Priority_T>;

#define SET_OPERATOR_REQUIRE(Operator) \
static_assert \
( \
    cpp_lang_utils::has_operator::Operator<Priority_T>, \
    "TOR requires a " #Operator " operator implementation for Priority_T" \
);
        SET_OPERATOR_REQUIRE(equalTo)
        SET_OPERATOR_REQUIRE(lessThan)

#undef SET_OPERATOR_REQUIRE

    protected:
        Priority_T m_priority = {};

    public:
        const Type* id() const { return this; }

        const Priority_T& priority() const { return m_priority; }
        void setPriority(const Priority_T& value) { m_priority = value; }

        ///////////////////////
        // Raw Ptr Ascending //
        ///////////////////////

        struct RawAscending
        {
            bool operator()(const Type& lhs, const Type& rhs) const
            {
                if (lhs.id() == rhs.id())
                {
                    return false;
                }
                else // Strict unique according to ID.
                {
                    if (lhs.priority() == rhs.priority())
                    {
                        return lhs.id() < rhs.id();
                    }
                    else // Strict ascending according to priority.
                    {
                        return lhs.priority() < rhs.priority();
                    }
                }
            }
        };
        using RawPrioritySet = std::set<Target_T, RawAscending>;

        template<typename ValueType>
        using RawPriorityMap = std::map<Target_T, ValueType, RawAscending>;

        //////////////////////////
        // Shared Ptr Ascending //
        //////////////////////////

        struct SharedAscending
        {
            bool operator()(SharedPtrParam<Type> lhs, SharedPtrParam<Type> rhs) const
            {
                return RawAscending()(*lhs.get(), *rhs.get());
            }
        };
        using SharedPrioritySet = std::set<SharedPtr<Target_T>, SharedAscending>;

        template<typename ValueType>
        using SharedPriorityMap = std::map<SharedPtr<Target_T>, ValueType, SharedAscending>;

        ////////////////////////
        // Weak Ptr Ascending //
        ////////////////////////

        struct WeakAscending
        {
            bool operator()(WeakPtrParam<Type> lhs, WeakPtrParam<Type> rhs) const
            {
                // The order of comparison here is significant:
                //
                // 1. Compare "rhs" in advance of "lhs":
                //
                //    Place the expired elements in front of the set.
                //
                // 2. Return "false" in advance of "true":
                //
                //    Ensure the TOR (compatible with STL).
                //
                //-----------------------------------------------
                // TOR: Total Order Relation
                //-----------------------------------------------
                // 1. Reflexive:
                //    assert(a <= a)
                //-----------------------------------------------
                // 2. Transitive:
                //    If (a <= b && b <= c) then (a <= c)
                //-----------------------------------------------
                // 3. Antisymmetric:
                //    If (a <= b && b <= a) then (a == b)
                //-----------------------------------------------
                // 4. Strongly Connected (Total):
                //    assert(a <= b || b <= a)
                //-----------------------------------------------

                if (rhs.expired()) return false;
                if (lhs.expired()) return true;

                return SharedAscending()(lhs.lock(), rhs.lock());
            }
        };
        using WeakPrioritySet = std::set<WeakPtr<Target_T>, WeakAscending>;

        template<typename ValueType>
        using WeakPriorityMap = std::map<WeakPtr<Target_T>, ValueType, WeakAscending>;

        /////////////////////////
        // Miscellaneous Tools //
        /////////////////////////

        // Call "func" for each element in "cont".
        // The return value of "func" indicates whether to stop iterating.
        static void foreach(
            SharedPrioritySet& cont,
            FuncParam<bool(SharedPtrParam<Target_T>)> func)
        {
            for (auto& elem : cont)
            {
                if (func(elem)) break;
            }
        }
        static void foreach(
            WeakPrioritySet& cont,
            FuncParam<bool(SharedPtrParam<Target_T>)> func)
        {
            bool continueIterate = true;

            for (auto itor = cont.begin();;)
            {
                while (itor != cont.end() && itor->expired())
                {
                    itor = cont.erase(itor);
                }
                if (itor != cont.end())
                {
                    if (continueIterate)
                    {
                        continueIterate = !func(itor->lock());
                    }
                    ++itor;
                }
                else break;
            }
        }
    };
}
