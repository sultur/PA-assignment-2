// Copyright (C) 2022
// Klaus Kraßnitzer

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <utility>

using namespace std;

// pair formatter
template<typename T, typename U> ostream& operator<<(ostream &os, const pair<T,U> &p) {
   os << "(" << p.first << ", " << p.second << ")";
   return os;
}

// vector formatter
template<typename T> ostream& operator<<(ostream &os, const vector<T> &v) {
    os << "[";
    for (int i=0; i<v.size(); i++) {
        os << v[i];
        if (i < v.size()-1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

#endif /* UTIL_H */