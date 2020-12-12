#define _CRT_SECURE_NO_WARNINGS
//#include <unistd.h>

#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iomanip>
#include <fstream>
#include <climits>
using namespace std;

static int DEBUG_STATE = 0;

/* prehandle(argc,argv):
	return 0: enter help module
	return 1: enter verbose module
	reutrn 2: enter parse module
	return 3: enter stderr module */
int prehandle(int argc, char* argv[]) {
	if (argc < 2 || argc>4)
		return 3;
	if (argc == 2) {
		string val = argv[1];
		if (val == "-h" || val == "--help")
			return 0;
		return 3;
	}
	else if (argc == 3) {
		return 2;
	}
	if (strcmp(argv[1], "-v") != 0 && strcmp(argv[1], "--verbose") != 0)
		return 3;
	DEBUG_STATE = 1;
	return 1;
}
void help() {
	printf("usage: ./codesim[-v | --verbose][-h | --help] code1 code2\n");
}
void errinput() {
	printf("error: invalid input.\n");
}
void nonexistentfile() {
	printf("error: non-existent filename.\n");
}

string code_to_string(string filename) {
	ifstream infile(filename.c_str(), ios::in);
	string result = "";
	while (infile.ios::eof() == 0) {
		string  temp = "";
		infile >> temp;
		result += temp;
	}
	infile.close();
	return result;
}

vector<int> string_to_hashValue(string str, int k, int base) {
	int len = str.length();
	int hash_len = len - k + 1;
	int preValue=1;
	for(int i=0;i<k;i++) preValue = preValue * base;
	vector<int> results;
	if (hash_len < 1) return results;
	results = vector<int>(hash_len, 0);
	long long int firstValue = 0, lastValue = 0, curValue;
	for (int i = 0; i < k; i++)
		firstValue = (firstValue + (long long)str[i]) * base % INT_MAX;
	//results.push_back((int)firstValue);
	results[0] = (int)firstValue;
	lastValue = firstValue;
	for (int i = 1; i < hash_len; i++) {
		curValue = ((lastValue - (long long)str[i - 1] * preValue) + (long long)str[i + k - 1]) * base;
		curValue = curValue % INT_MAX;
		//results.push_back((int)curValue);
		results[i] = (int)curValue;
		lastValue = curValue;
	}
	return results;
}

vector<int> filterByWINNOWING(vector<int>& hashtable, int w) {
	if (w == 1)
		return hashtable;
	deque<int> queNum; // one queue in which data are increasing monotonicly.
	queNum.push_back(0);
	int minPos = 0;
	for (int i = 1; i < w; i++) {
		while (!queNum.empty() && hashtable[queNum.back()] >= hashtable[i])
			queNum.pop_back();
		queNum.push_back(i);
	}
	vector<int> results;
	results.push_back(hashtable[queNum.front()]);
	int lastStorePos = queNum.front();
	for (int i = w; i < hashtable.size(); i++) {
		if (queNum.front() == i - w)
			queNum.pop_front();
		while (!queNum.empty() && hashtable[queNum.back()] >= hashtable[i])
			queNum.pop_back();
		queNum.push_back(i);
		if (lastStorePos == queNum.front())
			continue;
		results.push_back(hashtable[queNum.front()]);
		lastStorePos = queNum.front();
	}
	return results;
}

double compSimilar2_LinearizableCompare(vector<int>& win1, vector<int>& win2) {
	sort(win1.begin(), win1.end());
	sort(win2.begin(), win2.end());
	int len1 = win1.size(), len2 = win2.size();
	int numSame = 0;
	for (int i = 0, j = 0; i < len1 && j < len2;) {
		if (win1[i] == win2[j]) {
			numSame++;
			i++; j++;
		}
		else if (win1[i] < win2[j])
			i++;
		else
			j++;
	}
	return ((double)numSame) * 2 / (len1 + len2);
}
double compSimilar1_EditDistance(vector<int>& win1, vector<int>& win2) {
	vector<vector<int>> minDist(win1.size() + 1, vector<int>(win2.size() + 1, 0));
	for (int i = 0; i <= win1.size(); i++)
		minDist[i][0] = i;
	for (int j = 0; j <= win2.size(); j++)
		minDist[0][j] = j;
	for (int i = 1; i <= win1.size(); i++) {
		for (int j = 1; j <= win2.size(); j++) {
			minDist[i][j] = min(minDist[i][j - 1], minDist[i - 1][j]) + 1;
			if (win1[i - 1] == win2[j - 1])
				minDist[i][j] = min(minDist[i - 1][j - 1], minDist[i][j]);
			else
				minDist[i][j] = min(minDist[i - 1][j - 1] + 1, minDist[i][j]);
		}
	}
	int diff = minDist[win1.size()][win2.size()];
	return (double)1 - (((double)diff) * 2 / (win1.size() + win2.size()));
}

void debug_output(string file1, string file2,vector<int> win1,vector<int> win2) {

	printf("file1:%s generates code1.s by g++\n", file1.c_str());
	printf("file2:%s generates code2.s by g++\n", file2.c_str());
	printf("---------------------------------------------------------------\n");
	printf("hash code in code1.s achieved by WINNOWING:\n");
	for (int i = 0; i < win1.size(); i++)
		printf("%d ", win1[i]);
	printf("\n");
	printf("---------------------------------------------------------------\n");
	printf("hash code in code1.s achieved by WINNOWING:\n");
	for (int i = 0; i < win2.size(); i++)
		printf("%d ", win2[i]);
	printf("\n");
}

int main(int argc, char* argv[]) {
	// step1. Pre handle
	int pre = prehandle(argc, argv);
	if (pre == 3) {
		errinput(); return 1;
	}
	else if (pre == 0) {
		help(); return 0;
	}
	string file1 = argv[1], file2 = argv[2];
	if (DEBUG_STATE) {
		file1 = argv[2]; file2 = argv[3];
	}
	if (!fopen(file1.c_str(), "r") || !fopen(file2.c_str(), "r")) {
		nonexistentfile(); return 1;
	}
	// step2. Generate compiled files with the suffix .s
	string instr1 = "g++ -S -o code1.s "+file1, instr2 = "g++ -S -o code2.s "+file2;
	system(instr1.c_str());
	system(instr2.c_str());

	// step3. Generate text without common signal e.g. \t and \n
	string text1 = code_to_string("code1.s");
	string text2 = code_to_string("code2.s");

	// step4. Generate hash code from the text
	/* The value of base,k, and t need to be modified by testing. */
	int base = 7, k = 3, t = 7;
	int w = t-k+1;
	vector<int> hash_table1 = string_to_hashValue(text1,k,base);
	vector<int> hash_table2 = string_to_hashValue(text2,k,base);
	/*for(int i=0;i<hash_table1.size();i++)
		cout<<hash_table1[i]<<" ";
	cout<<endl;*/

	// step5. Filter the hash table by the method of WINNOWING
	vector<int> winnowing1 = filterByWINNOWING(hash_table1,w);
	vector<int> winnowing2 = filterByWINNOWING(hash_table2,w);
	/*for(int i=0;i<winnowing1.size();i++)
		cout<<winnowing1[i]<<" ";*/
	
	// step6. Compare these two fingerprint-lists and analyse number of same data
	if(DEBUG_STATE) debug_output(file1,file2,winnowing1,winnowing2);

	double mark1 = compSimilar1_EditDistance(winnowing1,winnowing2);
	double mark2 = compSimilar2_LinearizableCompare(winnowing1,winnowing2);
	//cout<<"mark1:"<<mark1*100<<" mark2:"<<mark2*100<<endl;
	double avg_mark = (mark1+mark2)/2;
	printf("%.2f%%\n", 100.0*avg_mark);
	return 0;
}