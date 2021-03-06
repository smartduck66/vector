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

#include <memory>	// For using allocator and malloc
#include <array>
#include <list>
#include <ostream>
#include <sstream>


struct invalid_range { };								// to throw as exception (on 'mime' out_of_range qui ne se déclenche que sur at() - page 693

template<typename T, typename A=allocator<T>> 
class vector {  
	// Définition du vecteur : page 680
	A alloc;
	int sz;
	T* elem;
	int space;

public:
	using size_type = unsigned long;
	using value_type = T;
	using iterator = T* ;						// Un itérateur n'est en fait qu'un pointeur...
	using const_iterator = const T*;
	
	vector():sz{0}, elem{nullptr}, space{0} {}	// constructors
	explicit vector(int s):sz{s}, elem{new T[s]}, space{s}
	{  
		for (int i = 0; i < sz; ++i) elem[i] = 0;
	}

	vector(const vector&);							// copy constructor
	vector& operator=(const vector& a);				// copy assignment (implémentation plus bas)
	
	vector(vector&&);								// move constructor
	vector& operator=(vector&&);					// move assignment

	~vector() { delete[] elem; }					// destructor

	T& operator[](int n) { return elem[n]; }		// accès
	const T& operator[](int n) const { return elem[n]; }
				
	int size() const { return sz; }					// taille
	int capacity() const { return space; }

	void resize(int newsize);
	void push_back(const T& d);
	void push_front(const T& d);
	void reserve(int newalloc);

	iterator begin() { return &elem[0]; }			// On renvoit l'adresse de la première occurence du vecteur
	const_iterator begin() const { return &elem[0]; }
	iterator end() { return &elem[sz]; }			// On renvoit l'adresse de la dernière occurence du vecteur + 1 : convention STL			
	const_iterator end() const { return &elem[sz]; }
	iterator back() { return &elem[sz - 1]; }		// renvoi du dernier élément
	iterator avancer(iterator p, const int x);		// avancer de x avec test 'out of range' (range-checked vector)
	iterator reculer(iterator p, const int x); 		// reculer de x avec test 'out of range' (range-checked vector)
	size_type size() { return sz; }

	iterator insert(iterator p, const T& val);
	iterator erase(iterator p);

	
};

template<typename T, typename A>
T* vector<T, A>::avancer(iterator p, const int x)	// On a mis T* comme type de retour et non vector<T,A>::iterator comme en page 746
{
	// Exo 18 page 755 : random-access iterator complété en permettant d'avancer de 'x' unités
	if (end() <= p+x) throw invalid_range();
	p += x;
	return p;
}

template<typename T, typename A>
T* vector<T, A>::reculer(iterator p, const int x)	// On a mis T* comme type de retour et non vector<T,A>::iterator comme en page 746
{
	// Exo 18 page 755 : random-access iterator complété en permettant de reculer de 'x' unités
	if (p-x<begin()) throw invalid_range();
	p -= x;
	return p;
}

template<typename T, typename A>
T* vector<T, A>::erase(iterator p)					// On a mis T* comme type de retour et non vector<T,A>::iterator comme en page 746
{

	if (p == end()) return p;
	for (auto pos = p + 1; pos!=end(); ++pos)
		*(pos - 1) = *pos;
	alloc.destroy(&*(end() - 1));
	--sz;
	return p;
}

template<typename T, typename A>
T* vector<T, A>::insert(iterator p, const T& val)	// On a mis T* comme type de retour et non vector<T,A>::iterator comme en page 746
{

	int index = p - begin();
	if (size() == capacity())
		reserve(size() == 0 ? 8 : 2 * size());
	alloc.construct(elem + sz, *back());
	++sz;
	iterator pp = begin() + index;
	for (auto pos = end() - 1; pos != pp; --pos)
		*pos = *(pos - 1);
	*(begin() + index) = val;
	return pp;
}

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
	alloc.construct(&elem[sz], val);					// Insertion en dernière position du vecteur de la nouvelle valeur
	++sz;

}

template<typename T, typename A>
void vector<T, A>::push_front(const T& val)
{
	// Drill page 728 : on repart de la définition de push_back excepté que l'on insère la valeur au début du vecteur et PAS A LA FIN après recopie des éléments "vers le haut" du vecteur
	if (space == 0) reserve(8);
	else if (sz == space) reserve(2 * space);
	
	++sz;												// On incrémente la taille du vecteur immédiatement sachant que le dernier élément est à ce moment-là vide

	for (int i = sz; i > 0; --i) elem[i]= elem[i-1];	// On "décale" chaque élément du vecteur en recopiant les valeurs "vers le haut" du vecteur
	
	alloc.construct(&elem[0], val);						// On insère en "première" position du vecteur la nouvelle valeur
	
}

template<typename T, typename A>
ostream& operator << (ostream& os, const vector<T>& v)
{
	os << "[";
	for (typename vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
	{
		os << " " << *ii;
	}
	os << "]";
	return os;
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

template<typename T>
void compare_drill(T c)
{
	string s = "Hello";
	
	cout << "Taille du container : "<<c.size() << endl;
	
	auto p2 = s.begin();
	
	bool identique = true;
	for (auto p1 = c.begin(); p1 != c.end(); ++p1) {
		if (*p1 != *p2) identique = false; break;
		++p2;
	}

	if (identique)
		cout << "Comparaison du container avec une chaîne 'Hello' : IDENTIQUE" << endl;
	else
		cout << "Comparaison du container avec une chaîne 'Hello' : NOT GOOD" << endl;

	auto p3 = s.begin();

	bool inferieur = true;
	for (auto p1 = c.begin(); p1 != c.end(); ++p1) {
		if (*p1 > *p3) inferieur = false; break;
		++p3;
	}


	if (inferieur)
		cout <<"Le container vient AVANT Howdy" << endl;
	else
		cout <<"Le container vient APRES Howdy" << endl;
	
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
		for (auto x : vd)
			cout << x << ',';
		cout << endl;
		
		// Test des itérateurs (page 743)
		auto p = vd.begin();
		cout << "vd.begin() =" << *p<<endl;
		++p;
		cout << "vd.begin()+1 =" << *p<<endl;
		p = vd.erase(p);
		cout << "Eléments du vecteur après erase :" << endl;
		for (auto x : vd)
			cout << x << ',';
		cout << endl;
		auto p2 = vd.begin();
		p2 = vd.insert(p2, 0.9);
		cout << "Eléments du vecteur après insert :" << endl;
		for (auto x : vd)
			cout << x << ',';
		cout << endl;

		// Copy assignment
		vd1 = vd;
		cout << "vecteur vd1 = vd" << endl;
		for (int i = 0; i < size(vd1); ++i)
			cout << vd1[i] << endl;
		cout << endl << endl;

		vi[0] = 10;
		vi[1] = 20;
		cout << "vecteur vi avec 7 éléments réservés (size = "<<size(vi)<<" - capacity = "<<vi.capacity()<<")"<< endl;
 		for (int i = 0; i < size(vi); ++i)
			cout << vi[i] << endl;

		cout << "------------------------ push_front(5)"<< endl;
		vi.push_front(5);
		for (int i = 0; i < size(vi); ++i)
			cout << vi[i] << endl;
		cout << "------------------------ Nouvelles valeurs du vecteur -> size = " << size(vi) << " - capacity = " << vi.capacity() << endl;

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

		// Drill page 742
		
		array<char, 5> b1{ 'H','e','l','l','o' };
		compare_drill(b1);
		/* le push_back sur un vector<char> provoque un problème de Linker
		vector<char> vc1{};
		vc1.push_back('H');
		vc1.push_back('e');
		vc1.push_back('l');
		vc1.push_back('l');
		vc1.push_back('o');
		compare_drill(vc1);
		*/
		list<char> lc1{ 'H','e','l','l','o' };
		compare_drill(lc1);
		
		string s1{ "Hello" };
		compare_drill(s1);
		
		// Exo 18 page 755 : complétude des possibilités de l'itérateur pour être au plus proche d'un random-access iterator "range-checked"
		vector<int> w{};
		
		w.push_back(0);
		w.push_back(1);
		w.push_back(2);
		w.push_back(3);
		w.push_back(4);
		w.push_back(5);
		w.push_back(6);
		w.push_back(7);
		w.push_back(8);
		w.push_back(9);

		auto p3 = w.begin();
		cout << "L'itérateur pointe sur la valeur " << *p3 << endl;
		p3 = w.avancer(p3, 5);
		cout << "w.avancer(p3, 5); L'itérateur pointe sur la valeur " << *p3 << endl;
		p3 = w.reculer(p3, 5);
		cout << "w.reculer(p3, 5); L'itérateur pointe sur la valeur " << *p3 << endl;

		keep_window_open();

		return 0;
	}
	
	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}

	catch (invalid_range)
	{	// this code is to produce error messages
		cout << "Bad index !" << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}

