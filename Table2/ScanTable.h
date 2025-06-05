#pragma once
#include "Table.h"
#include "ArrayTable.h"
#include "Record.h"
template <typename TKey, typename TVal>
class ScanTable : public ArrayTable<TKey, TVal> {
public:
    ScanTable(int _size) : ArrayTable<TKey, TVal>(_size) {}

    ScanTable(const ScanTable& other) : ArrayTable<TKey, TVal>(other.size) {
        this->DataCount = other.DataCount;
        this->Curr = other.Curr;
        this->Eff = other.Eff;
        this->pRec = new Record<TKey, TVal>[this->size];
        for (int i = 0; i < this->DataCount; i++) {
            this->pRec[i] = other.pRec[i];
        }
    }

    ScanTable& operator=(const ScanTable& other) {
        if (this == &other) return *this;

        delete[] this->pRec;

        this->size = other.size;
        this->DataCount = other.DataCount;
        this->Curr = other.Curr;
        this->Eff = other.Eff;
        this->pRec = new Record<TKey, TVal>[this->size];
        for (int i = 0; i < this->DataCount; i++) {
            this->pRec[i] = other.pRec[i];
        }
        return *this;
    }

    bool Find(TKey key) {
        this->Eff = 0;
        for (int i = 0; i < this->DataCount; i++) {
            this->Eff++;
            if (key == this->pRec[i].key) {
                this->Curr = i;
                return true;
            }
        }
        this->Curr = this->DataCount;
        return false;
    }

    void Insert(TKey key, TVal val) {
        if (Find(key))
            throw - 2;

        if (this->DataCount == this->size)
            throw - 1;

        Record<TKey, TVal> r(key, val);
        this->pRec[this->DataCount] = r; 
        this->DataCount++;
        this->Eff++;
    }

    void Delete(TKey key) {
        if (!Find(key)) throw - 3;
        this->pRec[this->Curr] = this->pRec[this->DataCount - 1];
        this->DataCount--;
        this->Eff++;
    }

    void Clear() {
        this->DataCount = 0;
    }

    void Reset() {
        this->Curr = 0;
    }

    void GoNext() {
        if (this->Curr < this->DataCount)
            this->Curr++;
    }

    bool IsEnd() {
        return this->Curr >= this->DataCount;
    }

    bool IsEmpty() const {
        return this->DataCount == 0;
    }

    Record<TKey, TVal> GetCurr() {
        if (this->Curr < this->DataCount)
            return this->pRec[this->Curr];
        throw - 4;
    }

    bool IsFull() const {
        return this->DataCount == this->size;
    }
    
    void Resize(int newSize) override {
        ArrayTable<TKey, TVal>::Resize(newSize);
    }

    std::string GetTypeName() const override {
        return "ScanTable";
    }
};