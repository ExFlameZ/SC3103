#include <stdio.h>
#include "mathlib.h"

int main(){
	int n;
	if (scanf("%d", &n) == 1){
		int result = findFibonacci(n);
		printf("%d\n", result);
	}
	return 0;
}

