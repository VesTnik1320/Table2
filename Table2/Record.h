#pragma once
#include <iostream>
using namespace std;
template <class TKey, class TVal>
struct Record {
	TKey key;
	TVal val;

	Record() {};
	Record(const TKey k) : key(k) {}
	Record(const TKey k, const TVal v) : key(k), val(v) {}

	bool operator>(Record<TKey, TVal> r) { return key > r.key; };
	bool operator<(Record<TKey, TVal> r) { return key < r.key; };
	bool operator==(Record<TKey, TVal> r) { return key == r.key; };
	bool operator!=(Record<TKey, TVal> r) { return (key != r.key); };
};
