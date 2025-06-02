#pragma once
#include <iostream>
#include "Record.h"

template<typename TKey, typename TValue>
class Table {
protected:
    int DataCount;
    int Eff;
public:
    Table() : DataCount(0), Eff(0) {};
    virtual ~Table() {};

    int GetDataCount() { return DataCount; }
    int GetEff() { return Eff; }
    void ClearEff() { Eff = 0; }

    virtual bool IsEmpty() const { return DataCount == 0; }
    virtual bool IsFull() const = 0;
    virtual bool Find(TKey key) = 0;
    virtual void Insert(TKey key, TValue val) = 0;
    virtual void Delete(TKey key) = 0;
    virtual void Clear() = 0;
    virtual void Reset() = 0;
    virtual void GoNext() = 0;
    virtual bool IsEnd() = 0;
    virtual Record<TKey, TValue> GetCurr() = 0;
    virtual void Resize(int newSize) = 0;
};