#pragma once
#include <vector>

#define M_PI 3.14159265358979323846
#define DEGREE (M_PI / 180)

using namespace std;

namespace utility{
	void vecChar2CharPtr(vector<char> vecChar, char* charPtr, int len);
}