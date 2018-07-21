/*
MyHeader.h
*/

/*
first own header

Revised July 21, 2018: initial release


*/

#include<string>
#include<iostream>

using namespace std;

inline void keep_window_open() // From "std_lib_facilities.h" of BS
{
	cin.clear();
	cout << "Please enter a character to exit\n";
	char ch;
	cin >> ch;
	return;
}

inline void keep_window_open(string s)
{
	if (s == "") return;
	cin.clear();
	cin.ignore(120, '\n');
	for (;;) {
		cout << "Please enter " << s << " to exit\n";
		string ss;
		while (cin >> ss && ss != s)
			cout << "Please enter " << s << " to exit\n";
		return;
	}
}
