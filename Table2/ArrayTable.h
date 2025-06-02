#pragma once
#include "Table.h"
#include "Record.h"
template<typename TKey, typename TValue>
class ArrayTable : public Table<TKey, TValue>
{
protected:
    int size, Curr;
    Record<TKey, TValue>* pRec;
public:
    ArrayTable(int size) : size(size), pRec(new Record<TKey, TValue>[size]), Curr(0), Table<TKey, TValue>() {}

    TKey GetCurrKey() { return pRec[Curr].key; }
    TValue GetCurrVal() { return pRec[Curr].val; } 
    Record<TKey, TValue> GetCurrRec() { return pRec[Curr]; } 
    ~ArrayTable() { delete[] pRec; }
    void Clear() { this->DataCount = 0; }
    bool IsFull() const { return this->DataCount == size; }
    bool IsEmpty() { return size == 0; }

    ArrayTable(const ArrayTable& arrayTable)
    {
        size = arrayTable.size;
        Curr = arrayTable.Curr;

        delete[] pRec;
        pRec = new Record<TKey, TValue>[size];
        for (int i = 0; i < size; i++)
        {
            pRec[i] = arrayTable.pRec[i];
        }
    }
    void Reset() { Curr = 0; }
    void GoNext() { Curr++; }
    bool IsEnd() { return (this->DataCount == Curr); }
    void Resize(int newSize) override {
        if (newSize < 1) {
            throw std::invalid_argument("Новый размер меньше 1");
        }
        this->DataCount = newSize;
    }
};
