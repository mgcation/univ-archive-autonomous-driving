#include "utility.h"

namespace utility{
	void vecChar2CharPtr(vector<char> vecChar, char* charPtr, int len){
		int i = 0;
		for (auto c : vecChar){
			if (i == len - 1){
				charPtr[i] = '\0';
				break;
			}
			else{
				charPtr[i++] = c;
			}
		}
	}
}
