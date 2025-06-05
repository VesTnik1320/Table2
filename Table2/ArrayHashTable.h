#pragma once  
#include "HashTable.h"  
#include "Record.h"
#include <functional> 
#include <stdexcept>

enum Status { Free, Deleted, Used };

template <typename TKey, typename TVal>
class ArrayHashTable : public HashTable<TKey, TVal>
{
protected:
    Record<TKey, TVal>* pRec;
    int step, curr;
    Status* status;

public:
    ArrayHashTable(int _size = 10, int _step = 1) : HashTable<TKey, TVal>(_size), step(_step), curr(0) {
        if (_size <= 0 || _step < 1)
            throw std::invalid_argument("Invalid size or step!");

        if (step == 1) {
            for (int i = 2; i < _size; ++i) {
                if (_size % i != 0) {
                    step = i;
                    break;
                }
            }
        }

        pRec = new Record<TKey, TVal>[this->size];
        status = new Status[this->size];
        for (int i = 0; i < this->size; i++) {
            status[i] = Free;
        }
    }

    ~ArrayHashTable() override {
        delete[] pRec;
        delete[] status;
    }

    ArrayHashTable(const ArrayHashTable& other) :
        HashTable<TKey, TVal>(other), step(other.step), curr(other.curr)
    {
        pRec = new Record<TKey, TVal>[this->size];
        status = new Status[this->size];

        for (int i = 0; i < this->size; i++) {
            pRec[i] = other.pRec[i];
            status[i] = other.status[i];
        }
    }

    ArrayHashTable& operator=(const ArrayHashTable& other) {
        if (this != &other) {
            delete[] pRec;
            delete[] status;

            HashTable<TKey, TVal>::operator=(other);
            step = other.step;
            curr = other.curr;

            pRec = new Record<TKey, TVal>[this->size];
            status = new Status[this->size];
            for (int i = 0; i < this->size; i++) {
                pRec[i] = other.pRec[i];
                status[i] = other.status[i];
            }
        }
        return *this;
    }

    TKey GetCurrKey() { return pRec[curr].key; }
    TVal GetCurrVal() { return pRec[curr].val; }
    Record<TKey, TVal> GetCurr() { return pRec[curr]; }
   
    Record<TKey, TVal>* GetArray() const {
        return pRec;
    }

    int GetSize() const {
        return this->size;
    }

    bool IsOccupied(int index) const {
        if (index < 0 || index >= this->size)
            throw std::out_of_range("Index out of bounds");
        return status[index] == Used;
    }

    bool Find(TKey key) override {
        curr = this->HashFunc(key);
        this->Eff = 0;
        int firstDeleted = -1;

        for (int i = 0; i < this->size; i++) {
            this->Eff++;
            if (status[curr] == Free) {
                if (firstDeleted != -1) curr = firstDeleted;
                return false;
            }
            else if (status[curr] == Deleted) {
                if (firstDeleted == -1)
                    firstDeleted = curr;
            }
            else if (pRec[curr].key == key) {
                return true;
            }

            curr = (curr + step) % this->size;
        }

        if (firstDeleted != -1) curr = firstDeleted;
        return false;
    }

    void Insert(TKey key, TVal val) override {
        if (IsFull())
            throw std::overflow_error("Table is full!");

        this->Eff = 0;
        if (Find(key))
            throw std::runtime_error("Record already exists!");

        pRec[curr] = Record<TKey, TVal>(key, val);
        status[curr] = Used;
        this->DataCount++;
        this->Eff++;
    }

    void Delete(TKey key) override {
        if (!Find(key))
            throw std::runtime_error("Record not found!");

        status[curr] = Deleted;
        this->DataCount--;
        this->Eff++;
    }

    void Reset() override {
        curr = 0;
        while (curr < this->size && (status[curr] == Free || status[curr] == Deleted)) {
            curr++;
        }
    }

    void GoNext() override {
        curr++;
        while (curr < this->size && (status[curr] == Free || status[curr] == Deleted)) {
            curr++;
        }
    }

    bool IsEnd() override {
        return curr >= this->size;
    }

    void Clear() override {
        this->DataCount = 0;
        for (int i = 0; i < this->size; i++) {
            status[i] = Free;
        }
    }

    bool IsFull() const override {
        return this->DataCount >= this->size;
    }

    void Resize(int newSize) override {
        if (newSize < this->DataCount)
            throw std::invalid_argument("New size too small");

        Record<TKey, TVal>* newRecs = new Record<TKey, TVal>[newSize];
        Status* newStatus = new Status[newSize];
        for (int i = 0; i < newSize; i++) newStatus[i] = Free;

        int oldSize = this->size;
        auto oldRecs = pRec;
        auto oldStatus = status;

        this->size = newSize; 

        for (int i = 0; i < oldSize; i++) {
            if (oldStatus[i] == Used) {
                int hash = this->HashFunc(oldRecs[i].key);
                int pos = hash;

                while (newStatus[pos] == Used) {
                    pos = (pos + step) % newSize;
                }

                newRecs[pos] = oldRecs[i];
                newStatus[pos] = Used;
            }
        }

        delete[] oldRecs;
        delete[] oldStatus;

        pRec = newRecs;
        status = newStatus;
    }

    std::string GetTypeName() const override {
        return "ArrayHashTable";
    }
};
