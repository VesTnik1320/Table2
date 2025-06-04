#pragma once
#include "Table.h"
#include "ArrayTable.h"
#include "Record.h"
template <typename Tkey, typename TVal>
class ScanTable : public ArrayTable<Tkey, TVal> {
public:
    ScanTable(int _size) : ArrayTable<Tkey, TVal>(_size) {}

    ScanTable(const ScanTable& other) : ArrayTable<Tkey, TVal>(other.size) {
        this->DataCount = other.DataCount;
        this->Curr = other.Curr;
        this->Eff = other.Eff;
        this->pRec = new Record<Tkey, TVal>[this->size];
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
        this->pRec = new Record<Tkey, TVal>[this->size];
        for (int i = 0; i < this->DataCount; i++) {
            this->pRec[i] = other.pRec[i];
        }
        return *this;
    }

    bool Find(Tkey key) {
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

    void Insert(Tkey key, TVal val) {
        if (Find(key))
            throw - 2;  // Уже существует

        if (this->DataCount == this->size)
            throw - 1;  // Нет места

        Record<Tkey, TVal> r(key, val);
        this->pRec[this->DataCount] = r; 
        this->DataCount++;
        this->Eff++;
    }

    void Delete(Tkey key) {
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

    Record<Tkey, TVal> GetCurr() {
        if (this->Curr < this->DataCount)
            return this->pRec[this->Curr];
        throw - 4;
    }

    bool IsFull() const {
        return this->DataCount == this->size;
    }
    
    void Resize(int newSize) override {
        ArrayTable<Tkey, TVal>::Resize(newSize);
    }

    std::string GetTypeName() const override {
        return "ScanTable";
    }
};