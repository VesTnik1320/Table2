#pragma once
#include "ArrayTable.h"
#include "Table.h"
#include "ScanTable.h"
#include "Record.h"
template <typename TKey, typename TVal>
class SortTable : public ScanTable<TKey, TVal> {
protected:
	Record<TKey, TVal>* tmpArr;
public:
	SortTable(int _size) : ScanTable<TKey, TVal>(_size), tmpArr(nullptr) {}
	~SortTable() {
		delete[] tmpArr;
	}

	SortTable& operator=(const SortTable& other) {
		if (this == &other)
			return *this;

		delete[] tmpArr;
		tmpArr = nullptr;

		if (this->size != other.size) {
			delete[] this->pRec; 
			this->size = other.size;
			this->pRec = new Record<TKey, TVal>[this->size];
		}

		this->DataCount = other.DataCount;
		this->Eff = other.Eff;
		this->Curr = other.Curr;

		for (int i = 0; i < this->DataCount; i++) {
			this->pRec[i] = other.pRec[i];
		}

		return *this;
	}

	bool Find(TKey key) {
		int l = 0, r = this->DataCount - 1;
		this->Eff = 0;
		while (l <= r) {
			int m = (l + r) / 2;
			this->Eff++;
			if (key == this->pRec[m].key) {
				this->Curr = m; 
				return true;
			}
			else if (key > this->pRec[m].key)
				l = m + 1;
			else
				r = m - 1;
		}
		this->Curr = l;
		return false;
	}

	void Insert(TKey Key, TVal Val)
	{
		if (this->DataCount == this->size) throw - 1;
		if (Find(Key)) throw - 1;
		for (size_t i = this->DataCount; i > this->Curr; i--)
		{
			this->Eff++;
			this->pRec[i] = this->pRec[i - 1];
		}
		this->pRec[this->Curr].key = Key; this->pRec[this->Curr].val = Val;
		this->DataCount++;
		this->Eff++;
	}

	void Delete(TKey key) {
		bool res = Find(key);
		if (res == false) throw - 1;
		for (int i = this->Curr; i < this->DataCount - 1; i++) {
			this->pRec[i] = this->pRec[i + 1];
			this->Eff++;
		}
		this->DataCount--;
	}

	void SelectSort() {
		this->Eff = 0; 
		for (int i = 0; i < this->DataCount - 1; i++) {
			int min_idx = i;
			for (int j = i + 1; j < this->DataCount; j++) {
				this->Eff++; 
				if (this->pRec[j].key < this->pRec[min_idx].key) {
					min_idx = j;
				}
			}
			if (min_idx != i) {
				std::swap(this->pRec[i], this->pRec[min_idx]);
				this->Eff++;
			}
		}
	}

	void QSortRec(int left, int right) {
		if (left < right) {
			int pivot = Partition(left, right);
			QSortRec(left, pivot - 1);
			QSortRec(pivot + 1, right);
		}
	}

	int Partition(int left, int right) {
		auto pivot = this->pRec[right];
		int i = left - 1;

		for (int j = left; j <= right - 1; j++) {
			this->Eff++; 
			if (this->pRec[j].key <= pivot.key) {
				i++;
				std::swap(this->pRec[i], this->pRec[j]);
				this->Eff++; 
			}
		}
		std::swap(this->pRec[i + 1], this->pRec[right]);
		this->Eff++; 
		return i + 1;
	}

	void Merge(int l, int r, int m) {
		if (tmpArr == nullptr) {
			tmpArr = new Record<TKey, TVal>[this->size];
		}

		int i = l, j = m + 1, k = l;
		while (i <= m && j <= r) {
			this->Eff++; 
			if (this->pRec[i].key < this->pRec[j].key) {
				tmpArr[k++] = this->pRec[i++];
			}
			else {
				tmpArr[k++] = this->pRec[j++];
			}
		}

		while (i <= m) {
			tmpArr[k++] = this->pRec[i++];
		}

		while (j <= r) { 
			tmpArr[k++] = this->pRec[j++];
		}

		for (int h = l; h <= r; h++) {  
			this->pRec[h] = tmpArr[h];
			this->Eff++; 
		}
	}

	void MergeSort(int l, int r) {
		if (l == r) return;
		int m = (l + r) / 2;
		MergeSort(l, m);
		MergeSort(m + 1, r);
		Merge(l, m, r);
	}

	void Resize(int newSize) override {
		ScanTable<TKey, TVal>::Resize(newSize);
	}

	std::string GetTypeName() const override {
		return "SortTable";
	}

};
