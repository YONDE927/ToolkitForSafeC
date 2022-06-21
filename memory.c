/*セグメンテーションフォールトを減らすためのツール群
 *メモリ違反の原因
 * 1. 割り当てしていないポインタへのアクセス
 * 2. 配列のサイズ超過アクセス
 * 3. 解放し忘れによるmallocエラー
 *
 * デバッグが大変なのは１と３
 * ある関数でヒープメモリに生成したものを別関数に渡すような場合に、解放し忘れがある。
 * 複雑になるにつれてメモリが割り当てられていることを前提として考えてしまう。
 * 毎回NULL条件をキャッチしているとコードが増えて読みにくい。
 *
 * 対策案
 * 1. 理想は高級言語のように自動的にメモリが解放されるガーベージコレクションみたいなもの
 * 2. 関数群のレイヤを超えてメモリ管理がしやすい仕組み
 * 
 * やってみること
 * メモリ管理に習熟するまで、メモリ予約のデバッグがしやすいようなラップ構造をつくる。
 * 参照カウントなどを作ってみる。
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Reserve {
	char* file;
	char* func;
	void* data;
};

#define safeMalloc(a) safemalloc(a, __FILE__, __func__);
void* safemalloc(int size, const char* filename, const char* funcname){
	struct Reserve* reserve = malloc(sizeof(struct Reserve));
	if(reserve == NULL){
		puts("fail to malloc Reserve");
		return NULL;
	}

	reserve->file = strdup(filename);
	if(reserve->file == NULL){
		puts("fail to set filename in safemalloc");
		return NULL;
	}

	reserve->func = strdup(funcname);
	if(reserve->func == NULL){
		puts("fail to set funcname in safemalloc");
		return NULL;
	}

	reserve->data = malloc(size);
	if(reserve->data == NULL){
		puts("fail to malloc data");
		return NULL;
	}
	
	return reserve->data;
}

void safeFree(void* data){
	struct Reserve* reserve;

	if(data == NULL){
		puts("data is empty in safeFree");
		return;
	}
	reserve = data - (sizeof(char*) * 2);
	free(reserve->data);
	free(reserve->file);
	free(reserve->func);
	free(reserve);
}

void printDataInfo(void* data){
	struct Reserve* reserve;

	if(data == NULL){
		puts("data is empty in safeFree");
		return;
	}
	reserve = data - (sizeof(char*) * 2);
	printf("Reserve\nfunc: %s, file: %s\n", reserve->func, reserve->file);	
}


