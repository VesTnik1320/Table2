#pragma once
#include "Table.h"
#include "Record.h"
#include <functional>

template <typename TKey, typename TVal>
class HashTable : public Table<TKey, TVal> {
protected:
    int size;
public:
    HashTable(int sz = 10) : size(sz) {
        this->DataCount = 0;
        this->Eff = 0;
    }

    virtual ~HashTable() = default;

    virtual int HashFunc(TKey key) const {
        return std::hash<TKey>{}(key) % size;
    }
};
