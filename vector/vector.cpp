//
// Book : Version des chapitres 19 & 20 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 19/07/2018 
// Commit 25/07/2018 : Drill du chapitre 19 - implémentation simplifiée d'un container "vecteur"
// Commit 10/08/2018 : Drill du chapitre 20 - sequences & iterators
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"
#include "MyHeader.h"

#include<memory>	// For using allocator and malloc
#include <array>
#include <list>

template<typename T, typename A=allocator<T>> class vector {  
	// Définition du vecteur : page 680
	A alloc;
	int sz;
	T* elem;
	int space;

public:
	vector():sz{0}, elem{nullptr}, space{0} {}	// constructors
	explicit vector(int s):sz{s}, elem{new T[s]}, space{s}
	{  
		for (int i = 0; i < sz; ++i) elem[i] = 0;
	}

	vector(const vector&);						// copy constructor
	vector& operator=(const vector& a);			// copy assignment (implémentation plus bas)
	
	vector(vector&&);							// move constructor
	vector& operator=(vector&&);				// move assignment

	~vector() { delete[] elem; }				// destructor

	T& operator[](int n) { return elem[n]; }	// accès
	const T& operator[](int n) const { return elem[n]; }

	int size() const { return sz; }				// taille
	int capacity() const { return space; }

	void resize(int newsize);
	void push_back(const T& d);
	void reserve(int newalloc);

};


template<typename T, typename A>
vector<T, A>& vector<T, A>::operator=(const vector& a)
{
	//Exo 9 page 709 : copy assignment revu avec un allocator plutôt que new/delete - version initiale "non-templatisée" : page 676
	if (this == &a) return *this;										// self-assignment : no work needed

	if (a.sz<=space) {													// enough space, no need for new allocation
		for (int i=0;i<a.sz;++i) elem[i]= a.elem[i];					// copy elements
		sz = a.sz;
		return *this;	
	
	}

	T* p = alloc.allocate(a.sz);										// allocate new space equals to "vector to copy" size
	for (int i = 0; i < a.sz; ++i) alloc.construct(&p[i], a.elem[i]);	// copy elements
	for (int i = 0; i < sz; ++i) alloc.destroy(&elem[i]);				// deallocate old space
	space = sz = a.sz;													// set new size
	elem = p;															// set new elements
	return *this;														// return a self-reference
}

template<typename T, typename A>
void vector<T,A>::reserve(int newalloc)
{  

	if (newalloc <= space) return;
	T* p = alloc.allocate(newalloc);
	for (int i = 0; i < sz; ++i) alloc.construct(&p[i], elem[i]);
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

template<typename Iter1, typename Iter2>
Iter2 copy_drill(Iter1 f1, Iter1 e1, Iter2 f2)
{
	// Drill page 753 : Copies [f1,e1) to [f2,f2+(e1-f1)) and returns f2+(e1-f1) just like the standard library copy function

	if (f1 == e1) return f2;	// Sequence is empty 

	Iter2 p2 = f2;
	for (Iter1 p1 = f1; p1 != e1; ++p1) {
		*p2 = *p1;
		++p2;
	}
	return p2;
}

int main()
{
	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		// Définition multiples de vecteurs
		vector<double> vd{};
		vector<double> vd1{};
		vector<int> vi(7);
		vector<double*> vpd{};
		vector<vector<int>> vvi{};

		vd.push_back(1.1);
		vd.push_back(1.2);
		vd.push_back(1.3);
		cout << "vecteur vd créé" << endl;
		for (int i = 0; i < size(vd); ++i)
			cout << vd[i] << endl;
		
		// Copy assignment
		vd1 = vd;
		cout << "vecteur vd1 = vd" << endl;
		for (int i = 0; i < size(vd1); ++i)
			cout << vd1[i] << endl;
		cout << endl << endl;

		vi[0] = 10;
		vi[1] = 20;
		cout << "vecteur vi avec 7 éléments réservés" << endl;
		for (int i = 0; i < size(vi); ++i)
			cout << vi[i] << endl;

		// Drill page 753 -------------------------------------------------------------------------------------------------
		array<int, 10> a1 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		vector<int> v1 {};
		for (int i = 0; i < 10; ++i)
			v1.push_back(i);
		list<int> l1 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		array<int, 10> a2 = a1;
		vector<int> v2{}; v2 = v1;
		list<int> l2 = l1;

		for (auto p = a1.begin(); p != a1.end(); ++p)
			*p +=2;

		for (int i = 0; i < 10; ++i)	// Pas d'itérateurs définis dans la version édulcorée de "vector" développée
			v1[i]+=2;

		for (auto p = l1.begin(); p != l1.end(); ++p)
			*p +=5;
		
		cout << "l1: ";
		for (auto x : l1)
			cout << x<<' ';
		cout << endl;
		

		copy_drill(l1.begin(),l1.end(), a2.begin());
		cout << "a2: ";
		for (auto x : a2)
			cout << x << ' ';
		cout << endl;
		// --------------------------------------------------------------------------------------------------------------------

		auto q = find(a2.begin(), a2.end(), 14);
		if (q == a2.end())
			cout << "Occurence inexistante !" << endl;
		else
			cout << "Occurence trouvée :-)" << endl;

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

