//
// Book : Version du chapitre 19 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 19/07/2018 
// Commit en cours : Drill du chapitre - implémentation simplifiée d'un container "vecteur"
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"
#include "MyHeader.h"

#include<memory>	// For using allocator and malloc


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

