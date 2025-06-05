#pragma once
#include "Table.h"
#include "Record.h"
template <typename TKey, typename TVal>
class HashTable : public Table<TKey, TVal> {
protected:
    int size;

    virtual int HashFunc(TKey key) {
        this->Eff++;
        return std::hash<TKey>{}(key) % size;
    }

public:
    HashTable(int sz = 10) : size(sz) {}
};