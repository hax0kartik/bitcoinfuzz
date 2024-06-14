#ifndef TYPEWRAPPER_H
#define TYPEWRAPPER_H

#include <iostream>
#include <string>

#define PRETTY_TYPE_DEC(var, val) TypeWrapper var (val, #var);

template <typename T>
struct TypeWrapper {
    TypeWrapper(T&& t, const std::string& ref = "") {
        var = t;
        str = ref;
    }

    TypeWrapper(const T& t, const std::string& ref = "") {
        var = t;
        str = ref;
    }

#define op(x) inline bool operator x(const TypeWrapper<T>& t2) const { \
        bool ret = var x t2.var; \
        if (!ret) { \
            std::cerr << "---\n"; \
            std::cerr << "Failure:\n"; \
            std::cerr << *this << "\n"; \
            std::cerr << t2 << "\n"; \
            std::cerr << "Operation: " << #x << "\n"; \
        } \
        return ret; \
    }

    op(<) op(>) op(==) op(!=) op(<=) op(>=)
#undef op

    constexpr friend std::ostream& operator <<(std::ostream &out, const TypeWrapper<T> &t) {
        out << (t.str.empty() ? "Var" : t.str) << ": ";
        if constexpr (std::is_same_v<T, bool>)
            out << std::boolalpha << t.var;
        else if (std::is_same_v<T, std::string>)
            out << (t.var.empty() ? "(NULL)" : t.var);
        else
            out << t.var;
        return out;
    }

    constexpr T operator *() const {
        return var;
    }

    T var;
    std::string str;
};

#endif