﻿#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#include "Windows.h"                    // Windows API の機能定義
#include <stdlib.h>

#include "../include/lib_func.h"

// mem_size の容量でキュー用のメモリを確保する
void initialize(QUEUE* q, size_t mem_size)
{
	if (q == NULL) return;

	q->memory_begin = (int*)malloc(mem_size);
	q->memory_end = q->memory_begin + mem_size / sizeof(int);
	q->head = q->memory_begin;
	q->tail = q->memory_begin;
}

// 確保したメモリを解放する
void finalize(QUEUE* q)
{
	if (q == NULL) return;

	free(q->memory_begin);

	q->memory_begin = NULL;
	q->memory_end = NULL;
	q->head = NULL;
	q->tail = NULL;
}

// valの値をキューに入れる。実行の成否を返す
bool enqueue(QUEUE* q, int val)
{
	if (q == NULL || q->head == NULL || q->tail == NULL || q->memory_begin == NULL || q->memory_end == NULL) {
		return false;
	}

	int currentSize = countQueuedElements(q);

	if (currentSize == getMaxCount(q) - 1) {
		return false;
	}

	*(q->head) = val;
	q->head = (q->head + 1 != q->memory_end) ? q->head + 1 : q->memory_begin;

	return true;
}

// addrから始まるnum個の整数をキューに入れる。実行の成否を返す
bool enqueue_array(QUEUE* q, int* addr, int num)
{
	if (q == NULL || q->head == NULL || q->tail == NULL || q->memory_begin == NULL || q->memory_end == NULL) {
		return false;
	}

	int currentSize = countQueuedElements(q);

	if (num > countQueueableElements(q)) {
		return false;
	}

	for (int i = 0; i < num; i++) {
		if (enqueue(q, *(addr + i)) == false) {
			for (int j = 0; j < i; j++) {
				dequeue(q);
			}
			return false;
		}
	}

	return true;
}

// キューから一つの要素を取り出す(不具合時は0を返す)
int dequeue(QUEUE* q)
{
	if (q == NULL || q->head == NULL || q->tail == NULL || q->memory_begin == NULL || q->memory_end == NULL) {
		return 0;
	}

	if (isEmpty(q)) {
		return 0;
	}

	int val = *(q->tail);
	q->tail = (q->tail + 1 != q->memory_end) ? q->tail + 1 : q->memory_begin;

	return val;
}

// addrにキューからnumの要素を取り出す。取り出せた個数を返す
int dequeue_array(QUEUE* q, int* addr, int num)
{
	if (q == NULL || q->head == NULL || q->tail == NULL || q->memory_begin == NULL || q->memory_end == NULL) {
		return 0;
	}

	int count = 0;
	for (int i = 0; i < num; i++) {
		int val = dequeue(q);
		if (val != 0) {
			*(addr + i) = val;
			count++;
		}
		else {
			break;
		}
	}
	return count;
}

// キューが空かどうかを調べる
bool isEmpty(const QUEUE* q)
{
	if (q == NULL) return false;

	return q->head == q->tail;
}

static int getMaxCount(const QUEUE* q)
{
	if (q == NULL || q->memory_begin == NULL) return 0;

	return (int)(q->memory_end - q->memory_begin);
}

// 挿入されたデータ数を得る
int countQueuedElements(const QUEUE* q)
{
	if (q == NULL || q->memory_begin == NULL) return 0;

	int max_counts = getMaxCount(q);
	return (q->head + max_counts - q->tail) % max_counts;
}

// 挿入可能なデータ数を得る
int countQueueableElements(const QUEUE* q)
{
	return getMaxCount(q) - countQueuedElements(q) - 1;
}