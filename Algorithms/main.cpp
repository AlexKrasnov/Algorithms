#include <iostream>
#include <fstream>
#include <ctime>
#include <clocale>
#include <map>
#include <vector>
#include <omp.h>
#include "splay.h"
 
using namespace std;

class Treap
{
public:
	int key, priority;
	Treap * left, *right;
	Treap() { }
	Treap(int key, int priority) : key(key), priority(priority), left(NULL), right(NULL) { }
};

void Merge(Treap* & t, Treap* left, Treap* right)
{
	if (!left || !right)
		t = left ? left : right;
	else if (left->priority > right->priority)
		Merge(left->right, left->right, right), t = left;
	else
		Merge(right->left, left, right->left), t = right;
}

void Split(Treap* t, int key, Treap* & left, Treap* & right)
{
	if (!t)
		left = right = NULL;
	else if (key < t->key)
		Split(t->left, key, left, t->left), right = t;
	else
		Split(t->right, key, t->right, right), left = t;
}

Treap* Search(Treap *root, int key)
{
	if (root == NULL) return NULL;
	if (root->key == key) return root;
	if (key < root->key) return Search(root->left, key);
	else return Search(root->right, key);
}

void Insert(Treap* & t, Treap* it)
{
	if (!t)
		t = it;
	else if (it->priority > t->priority)
		Split(t, it->key, it->left, it->right), t = it;
	else
		Insert(it->key < t->key ? t->left : t->right, it);
}

void Build(Treap* & t, int x[], int y[], int length) // O(NlogN)
{
	for (int i = 0; i < length; i++)
		Insert(t, new Treap(x[i], y[i]));
}

void Remove(Treap* & t, int key)
{
	if (t->key == key)
		Merge(t, t->left, t->right);
	else
		Remove(key < t->key ? t->left : t->right, key);
}

void Print(Treap* t)
{
	if (!t)  return;
	Print(t->left);
	printf("(%d, %d) ", t->key, t->priority);
	Print(t->right);
}

int BinarySearch(int *a, int length, int key)
{
	int left = 0;
	int right = length;

	while (true)
	{
		int mid = left + (right - left) / 2;
		if (a[mid] == key)
			return a[mid];
		if (a[mid] > key)
			right = mid;
		else
			left = mid + 1;
	}
}

int qsort(int* a, int l, int r)
{
	int x = a[l + (r - l) / 2], i = l, j = r, temp;
	while (i <= j)
	{
		while (a[i] < x) i++;
		while (a[j] > x) j--;
		if (i <= j)
		{
			temp = a[i]; a[i] = a[j]; a[j] = temp;
			i++;
			j--;
		}
	}
	if (i < r) qsort(a, i, r);
	if (l < j) qsort(a, l, j);
	return a[l + (r - l) / 2];
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	try
	{
		ofstream fout("out.txt"); // файл для вывода результатов
		double t1 = 0, t2 = 0; // переменные для подсчёта времени работы
		int N = 0; // размер массива

		Treap* T = NULL; // декартово дерево
		map <int, int> M; // асоциативный массив на красно-чёрном дереве

		Node* splay = NULL;

		vector<double> SeachTimes1; // полученные результаты (поиск в декартвом)
		vector<double> SeachTimes2; // полученные результаты (поиск в красно-чёрном)
		vector<double> SeachTimes3; // полученные результаты (бинарный поиск)
		vector<double> SeachTimes4; // полученные результаты (поиск в splay)
		vector<double> RemoveTimes1; // полученные результаты (удаление в декартовом)
		vector<double> RemoveTimes2; // полученные результаты (удаление в красно-чёрном)
		vector<double> RemoveTimes3; // полученные результаты (удаление в splay)
		vector<double> InsertTimes1; // полученные результаты (вставка в декартово)
		vector<double> InsertTimes2; // полученные результаты (втсавка в красно-чёрное)
		vector<double> InsertTimes3; // полученные результаты (вставка в splay)

		int k = 0;


		for (int N = 100000; N != 1000000; N += 100000) // серийный запуск
		{
			int *x = new int[N]; // массив ключей
			int *y = new int[N]; // массив приоритетов

			srand(time(NULL));
			for (int i = 0; i < N; i++) // заполняем массивы
			{
				x[i] = i;
				y[i] = rand() % 1000000;
			}
			for (int i = 0; i < N; i++) {
				std::swap(x[i], x[rand() % N]);
			}
			cout << "------------------------------------------------------------\n\n";
			cout << N << " элементов в массиве\n\n";

			//------------------------------------------------------------------------
			//      Сравнение операции поиска
			//------------------------------------------------------------------------

			k = rand() % N;  // Ключ, с которым работаем

			Build(T, x, y, N); // заполняем декартово дерево

			for (int i = 0; i < N; i++)
				Ins(splay, new Node(x[i]));// заполняем splay дерево

			for (int i = 0; i < N; i++)
				M.insert(pair<int, int>(x[i], y[i])); // заполняем красно-чёрное дерево

			t1 = omp_get_wtime();
			int value1 = Search(T, k)->key;
			t2 = omp_get_wtime();
			cout << "Время поиска в декартовом дереве: " << fixed << t2 - t1 << endl;
			cout << "Значение: " << value1 << endl;
			SeachTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			int value2 = M.find(k)->first;
			t2 = omp_get_wtime();
			cout << "Время поиска в красно-чёрном дереве дереве: " << fixed << t2 - t1 << endl;
			cout << "Значение: " << value2 << endl;
			SeachTimes2.push_back(t2 - t1);

			t1 = omp_get_wtime();
			splay = Search(k, splay);
			t2 = omp_get_wtime();
			cout << "Время поиска в splay дереве: " << fixed << t2 - t1 << endl;
			cout << "Значение: " << value1 << endl;
			SeachTimes4.push_back(t2 - t1);


			qsort(x, 0, N - 1);
			t1 = omp_get_wtime();
			int value3 = BinarySearch(x, N, k);
			t2 = omp_get_wtime();
			cout << "Время бинарного поиска: " << fixed << t2 - t1 << endl;
			cout << "Значение: " << value3 << endl;
			cout << endl;
			SeachTimes3.push_back(t2 - t1);

			//------------------------------------------------------------------------
			//      Сравнение операции удаления
			//------------------------------------------------------------------------

			t1 = omp_get_wtime();
			Remove(T, k);
			t2 = omp_get_wtime();
			cout << "Время удаления в декартовом дереве: " << fixed << t2 - t1 << endl;
			RemoveTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			M.erase(k);
			t2 = omp_get_wtime();
			cout << "Время удаления в красно-чёрном дереве: " << fixed << t2 - t1 << endl;
			RemoveTimes2.push_back(t2 - t1);
			t1 = omp_get_wtime();
			Delete(splay, rand() % N);
			t2 = omp_get_wtime();
			cout << "Время удаления в splay дереве: " << fixed << t2 - t1 << endl;
			cout << endl;
			RemoveTimes3.push_back(t2 - t1);

			//------------------------------------------------------------------------
			//      Сравнение операции вставки
			//------------------------------------------------------------------------

			t1 = omp_get_wtime();
			Insert(T, new Treap(k, rand() % 1000000));
			t2 = omp_get_wtime();
			cout << "Время вставки в декартово дерево: " << fixed << t2 - t1 << endl;
			InsertTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			M.insert(pair<int, int>(k, rand() % 1000000));
			t2 = omp_get_wtime();
			cout << "Время вставки в красно-чёрное дерево: " << fixed << t2 - t1 << endl;

			InsertTimes2.push_back(t2 - t1);
			t1 = omp_get_wtime();
			Ins(splay, new Node(rand() % N));
			t2 = omp_get_wtime();
			cout << "Время вставки в splay дерево: " << fixed << t2 - t1 << endl;
			cout << endl;
			InsertTimes3.push_back(t2 - t1);
			//------------------------------------------------------------------------

			delete[] x;
			delete[] y;
			T = NULL;
			M.clear();
		}

		int i = 0;
		fout << "Поиск в декартвом дереве: ";
		for (int i = 0; i < SeachTimes1.size(); i++)
			fout << fixed << SeachTimes1[i] << " ";
		fout << endl;
		fout << "Поиск в splay дереве: ";
		for (int i = 0; i < SeachTimes4.size(); i++)
			fout << fixed << SeachTimes4[i] << " ";
		fout << endl;
		fout << "Поиск в красно-чёрном дереве: ";
		for (int i = 0; i < SeachTimes2.size(); i++)
			fout << fixed << SeachTimes2[i] << " ";
		fout << endl;
		fout << "Бинарный поиск: ";
		for (int i = 0; i < SeachTimes3.size(); i++)
			fout << fixed << SeachTimes3[i] << " ";
		fout << endl;
		fout << "Удаление в декартвом дереве: ";
		for (int i = 0; i < RemoveTimes1.size(); i++)
			fout << fixed << RemoveTimes1[i] << " ";
		fout << endl;
		fout << "Удаление в splay дереве: ";
		for (int i = 0; i < RemoveTimes3.size(); i++)
			fout << fixed << RemoveTimes3[i] << " ";
		fout << endl;
		fout << "Удаление в красно-чёрном дереве: ";
		for (int i = 0; i < RemoveTimes2.size(); i++)
			fout << fixed << RemoveTimes2[i] << " ";
		fout << endl;
		fout << "Вставка в декартово дерево: ";
		for (int i = 0; i < InsertTimes1.size(); i++)
			fout << fixed << InsertTimes1[i] << " ";
		fout << endl;
		fout << "Вставка в splay дерево: ";
		for (int i = 0; i < InsertTimes3.size(); i++)
			fout << fixed << InsertTimes3[i] << " ";
		fout << endl;
		fout << "Вставка в красно-чёрное дерево: ";
		for (int i = 0; i < InsertTimes2.size(); i++)
			fout << fixed << InsertTimes2[i] << " ";
		fout << endl;

		return 0;
	}
	catch (const char* error)
	{
		cout << error;
	}
}