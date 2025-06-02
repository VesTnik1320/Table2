#pragma once
#include "ArrayTable.h"
#include "Table.h"
#include "ScanTable.h"
#include "Record.h"
template <typename Tkey, typename TVal>
class SortTable : public ScanTable<Tkey, TVal> {
protected:
	Record<Tkey, TVal>* tmpArr;
public:
	SortTable(int _size) : ScanTable<Tkey, TVal>(_size) {}
	bool Find(Tkey key) {
		int l = 0, r = this->DataCount - 1;
		while (l <= r)
		{
			int m = (l + r) / 2;
			this->Eff++;
			if (key == this->pRec[m].key) {
				this->Curr = 0;
				return true;
			}
			else
				if (key > this->pRec[m].key)
					l = m + 1;
				else
					if (key < this->pRec[m].key)
						r = m - 1;
					else
					{
						this->Curr = m;
						return true;
					}


		}
		this->Curr = l;
		return false;
	}

	void Insert(Tkey Key, TVal Val)
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


	void Delete(Tkey key) {
		bool res = Find(key);
		if (res == false) throw - 1;
		for (int i = this->Curr; i < this->DataCount - 1; i++) {
			this->pRec[i] = this->pRec[i + 1];
			this->Eff++;
		}
		this->DataCount--;
	}

	void SelectSort() {
		int i = 0, j = 0, mini;
		for (i; i < this->DataCount; i++) {
			j = 0;
			mini = this->pRec[i].key;
			int k = i;
			for (j; j < this->DataCount; j++) {
				if (mini > this->pRec[j].key) {
					mini = this->pRec[j].key;
					k = j;
				}
			}
			Record<Tkey, TVal> tmp = this->pRec[k];
			this->pRec[k] = this->pRec[i];
			this->pRec[i] = tmp;
		}
	}

	void QSortRec(int st, int fin) {
		int l = st, j = fin;
		Record<Tkey, TVal> k = this->pRec[(l + j) / 2];
		while (l < j) {
			while (this->pRec[l] < k)
				l++;
			while (this->pRec[j] > k)
			{
				j--;
			}
			if (l <= j) {
				Record<Tkey, TVal> tmp = this->pRec[l];
				this->pRec[l] = this->pRec[j];
				this->pRec[j] = tmp;
				l++;
				j--;
			}
		}
		if (st < l) QSortRec(st, l);
		if (fin > j) QSortRec(j, fin);
	}

	void Merge(int l, int r, int m) {
		int i = l;
		int j = m + 1;
		int k = l;
		while (i <= m && j <= r) {
			if (this->pRec[i].key < this->pRec[j].key) {
				tmpArr[k] = this->pRec[i];
				i++;
			}
			else
			{
				tmpArr[k] = this->pRec[j];
				j++;
			}
			k++;
		}
		if (i <= m) {
			while (i <= m) {
				tmpArr[k] = this->pRec[i];
				i++;
				k++;
			}
		}
		else
		{
			while (j >= r) {
				tmpArr[k] = this->pRec[j];
				j++;
				k++;
			}
		}
		for (int h = l; h < l; h++)
			this->pRec[h] = tmpArr[h];
	}

	void MergeSort(int l, int r) {
		if (l == r) return;
		int m = (l + r) / 2;
		MergeSort(l, m);
		MergeSort(m + 1, r);
		Merge(l, m, r);
	}

	void Resize(int newSize) override {
		if (newSize < 1) {
			throw std::invalid_argument("Новый размер меньше 1");
		}
		this->DataCount = newSize;
	}
};
