#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

void zameni(int& x, int& y) {
	int temp = x;
	x = y;
	y = temp;
}

class Heap {
	int kapacitet, velicina, m;
	int* pniz;

	int otac(int i) const {
		return (i-1) / m;
	}
	int mladji(int i) const {
		return i * m + 1;
	}
	int stariji(int i) const {
		return (i + 1) * m;
	}

	void urediGore(int i, int &steps) {
		int ot = otac(i);
		while (i && pniz[i] < pniz[ot]) {
			steps++;
			zameni(pniz[i], pniz[ot]);
			i = ot;
			ot = otac(i);
		}
	}

	void urediDole(int i, int &steps) {
		while (i < velicina) {
			steps++;
			int manji = i;
			for (int j = mladji(i); j <= stariji(i) && j <= velicina; j++) {
				steps++;
				if (pniz[j] < pniz[manji]) manji = j;
			}
			if (manji == i) break;
			swap(pniz[i], pniz[manji]);
			i = manji;
		}
	}

public:
	Heap(int M = 2) {
		init(M);
	}

	int getKapacitet() const { return kapacitet; }
	int getVelicina() const { return velicina; }
	int getM() const { return m; }
	int get() const {
		if (velicina >= 0) return pniz[0];
		return INT_MAX;
	}

	void init(int M) {
		m = M;
		velicina = -1;
		kapacitet = 10;
		int *niz = new int[kapacitet];
		pniz = niz;
	}

	void add(int elem, int& steps) {
		if (velicina == kapacitet - 1) {
			kapacitet <<= 1;
			/*int* niz = new int[kapacitet];
			for (int i = 0; i <= velicina; i++) {
				niz[i] = pniz[i];
			}
			delete[] pniz;
			pniz = niz;*/
			pniz = (int*) realloc(pniz, kapacitet * sizeof(int));
		}
		pniz[++velicina] = elem;
		urediGore(velicina, steps);
	}

	int remove(int& steps) {
		int ans = get();
		if (velicina == -1) {
			std::cout << "Prazan heap" << endl;
		}
		else {
			zameni(pniz[0], pniz[velicina--]);
			urediDole(0, steps);
			if (velicina + 1 < kapacitet >> 1 && kapacitet >= 20) {
				kapacitet >>= 1;
				pniz = (int*) realloc(pniz, kapacitet * sizeof(int));
			}
		}
		return ans;
	}

	void convertTo(int newM) {
		m = newM;
		int steps = 0;
		for (int i = 0; i <= velicina; i++) {
			urediGore(i, steps);
		}
	}

	void unija(const Heap& h, int& steps) {
		int* niz = pniz;
		int *temp = new int[max(kapacitet, h.getKapacitet())<<1];
		pniz = temp;
		int staraVelicina = velicina;
		velicina = -1;
		int i = 0, j = 0;
		while (i <= staraVelicina || j <= h.getVelicina()) {
			if (j>h.getVelicina() || i <= staraVelicina && niz[i] <= h[j]) {
				add(niz[i], steps);
				i++;
			}
			else {
				add(h[j], steps);
				j++;
			}
		}
		kapacitet = max(kapacitet, h.getKapacitet()) << 1;
		delete[] niz;
	}

	void changeKey(int oldKey, int newKey, int& steps) {
		int i = 0;
		for (; i <= velicina; i++) {
			if (pniz[i] == oldKey) break;
		}
		if (i > velicina) return;
		pniz[i] = newKey;
		if (newKey > oldKey) urediDole(i, steps);
		else urediGore(i, steps);
	}

	void destroy() {
		delete[] pniz;
		velicina = -1;
		kapacitet = 0;
		pniz = nullptr;
	}

	~Heap() {
		destroy();
	}

	int operator[](int i) const {
		return pniz[i];
	}

	friend ostream& operator<<(ostream& os, const Heap& h);

};

ostream& operator<<(ostream& os, const Heap& h) {
	double temp = log(1 + (h.velicina + 1) * (h.m - 1)) / log(h.m);
	int height = ceil(temp) - 1;
	temp = pow(h.getM(), height);
	int razmak = temp * 2;
	int brojNaNivou = 1;
	for (int i = 0; i <= h.velicina; i++) {
		for (int j = 0; j < razmak; j++) os << ' ';
		os << setfill('0') << setw(2) << h[i];
		for (int j = 0; j < razmak; j++) os << ' ';
		if (i + 1 == brojNaNivou) {
			brojNaNivou = brojNaNivou * h.m + 1;
			razmak = (razmak + 1) / h.m - 1;
			os << endl;
		}
	}
	return os << endl;
}

class Priority_queue {
	Heap heap;

public:
	Priority_queue(int m = 2) : heap(m) {}
	bool empty() const { return heap.getVelicina() == -1; }
	int front() const { return heap.get(); }
	void remove(int &steps) { heap.remove(steps); }
	void insert(int elem, int& steps) { heap.add(elem, steps); }

	void evaluacija(string ime, int velicina, int m) {
		cout << setw(19) << to_string(velicina) + ", " + to_string(m);
		int steps = 0;
		ifstream ulfajl(ime);
		int* podaci = (int*)malloc(velicina * sizeof(int));
		for (int i = 0; i < velicina; i++) {
			ulfajl >> podaci[i];
		}
		ulfajl.close();
		auto start = chrono::high_resolution_clock::now();
		for (int i = 0; i < velicina; i++) {
			insert(podaci[i], steps);
		}
		auto end = chrono::high_resolution_clock::now();
		delete[] podaci;
		chrono::duration<double, std::milli> trajanje = end - start;
		std::cout << setw(11) << trajanje.count() << setw(16) << steps;
		steps = 0;
		start = chrono::high_resolution_clock::now();
		int prethodni = INT_MIN;
		while (!empty()) {
			if (prethodni > front()) {
				std::cout << "Greska" << endl;
				break;
			}
			prethodni = front();
			//std::cout << front() << endl;
			remove(steps);
		}
		end = chrono::high_resolution_clock::now();
		trajanje = end - start;
		std::cout << setw(11) << trajanje.count() << setw(16) << steps << endl;
	}

	friend void meni();
};

void meni() {
	ifstream ulfajl;
	Priority_queue* pq = nullptr;
	Heap* heap2 = nullptr;
	int temp1 = 0, temp2 = 0, steps = 0;
	srand(time(NULL));
	string fajlovi[] = { "test_100.txt", "test_1000.txt" , "test_10000.txt" , "test_100000.txt" };
	int movi[] = { 2,4,8 };
	int velicine[] = { 100, 1000, 10000, 100000 };
	while (1) {
		int opcija = 0;
		std::cout << "1. Stvaranje heapa" << endl;
		std::cout << "2. Umetanje elementa u heap" << endl;
		std::cout << "3. Vracanje minimalnog elementa" << endl;
		std::cout << "4. Brisanje minimalnog elementa" << endl;
		std::cout << "5. Promena reda heapa" << endl;
		std::cout << "6. Unija sa drugim heapom" << endl;
		std::cout << "7. Ispis heapa" << endl;
		std::cout << "8. Promena vrednosti odabranog kljuca" << endl;
		std::cout << "9. Brisanje heapa" << endl;
		std::cout << "10. Evaluacija performansi" << endl;
		std::cout << "11. Izlazak iz programa" << endl;
		cin >> opcija;
		switch (opcija) {
		case 1:
			std::cout << "Uneti m:" << endl;
			cin >> temp1;
			pq = new Priority_queue(temp1);
			std::cout << "1. Unos elemenata" << endl;
			std::cout << "2. Ucitavanje elemenata iz datoteke" << endl;
			std::cout << "3. Pseudoslucajno stvaranje elemenata" << endl;
			cin >> opcija;
			switch (opcija) {
			case 1:
				cin >> temp1;
				while (temp1--) {
					steps = 0;
					cin >> temp2;
					pq->insert(temp2, steps);
					//std::cout << steps << " koraka" << endl;
				}
				break;
			case 2:
				ulfajl.open("test_10.txt");
				ulfajl >> temp1;
				while (ulfajl) {
					pq->insert(temp1, steps);
					ulfajl >> temp1;
				}
				ulfajl.close();
				break;
			case 3:
				cin >> temp1;
				while (temp1--) {
					pq->insert(rand() % 100, steps);
				}
				break;
			}
			break;
		case 2:
			cin >> temp1;
			pq->insert(temp1, steps);
			break;
		case 3:
			cout << pq->front() << endl;
			break;
		case 4:
			cout << pq->front() << endl;
			pq->remove(steps);
			break;
		case 5:
			cin >> temp1;
			pq->heap.convertTo(temp1);
			break;
		case 6:
			heap2 = new Heap;
			temp1 = 0;
			do {
				cin >> temp1;
				heap2->add(temp1, steps);
			} while (temp1 != -1);
			std::cout << heap2;
			pq->heap.unija(*heap2, steps);
			std::cout << pq->heap;
			delete heap2;
			break;
		case 7:
			cout << pq->heap;
			break;
		case 8:
			cin >> temp1 >> temp2;
			pq->heap.changeKey(temp1, temp2, steps);
			break;
		case 9:
			delete pq;
			break;
		case 10:
			cout << endl;
			cout << setw(19) << "Broj elemenata, red" << setw(11) << "Vreme Um" << setw(16) << "Broj koraka Um" << setw(11) << "Vreme Iz" << setw(16) << "Broj koraka Iz" << endl;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					pq = new Priority_queue(movi[j]);
					pq->evaluacija(fajlovi[i], velicine[i], movi[j]);
					delete pq;
				}
			}
			break;
		default:
			return;
		}
	}
}

int main() {
	meni();
	return 0;
}