#pragma once
#include "Table.h"
#include "Record.h"
template<typename TKey, typename TVal>
class ArrayTable : public Table<TKey, TVal>
{
protected:
    int size, Curr;
    Record<TKey, TVal>* pRec;
public:
    ArrayTable(int size) : size(size), pRec(new Record<TKey, TVal>[size]), Curr(0), Table<TKey, TVal>() {}

    TKey GetCurrKey() { return pRec[Curr].key; }
    TVal GetCurrVal() { return pRec[Curr].val; } 
    Record<TKey, TVal> GetCurrRec() { return pRec[Curr]; } 
    ~ArrayTable() { delete[] pRec; }
    void Clear() { this->DataCount = 0; }
    bool IsFull() const { return this->DataCount == size; }
    bool IsEmpty() { return size == 0; }

    ArrayTable(const ArrayTable& arrayTable)
    {
        size = arrayTable.size;
        Curr = arrayTable.Curr;

        delete[] pRec;
        pRec = new Record<TKey, TVal>[size];
        for (int i = 0; i < size; i++)
        {
            pRec[i] = arrayTable.pRec[i];
        }
    }
    void Reset() { Curr = 0; }
    void GoNext() { Curr++; }
    bool IsEnd() { return (this->DataCount == Curr); }
    void Resize(int newSize) override {
        if (newSize < this->DataCount) {
            throw std::invalid_argument("Ќовый размер меньше текущего количества данных");
        }
        Record<TKey, TVal>* newArr = new Record<TKey, TVal>[newSize];
        for (int i = 0; i < this->DataCount; ++i) {
            newArr[i] = this->pRec[i];
        }
        delete[] this->pRec;
        this->pRec = newArr;
        this->size = newSize;
    }

    std::string GetTypeName() const override {
        return "ArrayTable";
    }

};
