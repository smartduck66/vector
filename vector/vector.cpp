//
// Book : Version du chapitre 19 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 19/07/2018 
// Commit en cours : Drill du chapitre - implémentation simplifiée d'un container "vecteur"
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"

#include<iostream>
#include<string>
#include<cstdlib>	// For using malloc
#include<memory>	// For using allocator

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

template<typename T, typename A=allocator<T>> class vector {  
	A alloc;
	int sz;
	T* elem;
	int space;

public:
	vector():sz{0}, elem{nullptr}, space{0} {}
	explicit vector(int s):sz{s}, elem{new T[s]}, space{s}
	{  
		for (int i = 0; i < sz; ++i) elem[i] = 0;
	}

	vector(const vector&);
	vector& operator=(const vector&);

	vector(vector&&);
	vector& operator=(vector&&);

	~vector() { delete[] elem; }

	T& operator[](int n) { return elem[n]; }
	const T& operator[](int n) const { return elem[n]; }

	int size() const { return sz; }
	int capacity() const { return space; }

	void resize(int newsize);
	void push_back(const T& d);
	void reserve(int newalloc);

};

template<typename T, typename A>
void vector<T,A>::reserve(int newalloc)
{  

	if (newalloc <= space) return;
	T* p = alloc.allocate(newalloc);
	for (int i = 0; i < sz; ++i) alloc.constructor(&p[i], elem[i]);
	for (int i = 0; i < sz; ++i) alloc.destroy(&elem[i]);
	alloc.deallocate(elem, space);
	elem = p;
	space = newalloc;
	
}

template<typename T, typename A>
void vector<T, A>::push_back(const T& val)
{

	if (space == 0) reserve(8);
	else if (sz == space) reserve(2 * space);
	alloc.construct(&elem[sz], val);
	++sz;

}

template<typename T, typename A>
void vector<T, A>::resize(int newsize)
{

	reserve(newsize);
	for (int i = sz; i < newsize; ++i) alloc.construct(&elem[i], val);
	for (int i = newsize; i < sz; ++i) alloc.destroy(&elem[i]);
	sz = newsize;

}

int main()
{
	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		// Définition multiples de vecteurs
		vector<double> vd;
		vector<int> vi;
		vector<double*> vpd;
		vector<vector<int>> vvi;

		keep_window_open();

		return 0;
	}
	/*
	catch (XXXXXXX::Invalid&)
	{
	cerr << "error: ???????????????????? ! \n";
	keep_window_open("q");	// For some Windows(tm) setup
	return 1;
	}
	*/

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}

