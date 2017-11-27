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
		ofstream fout("out.txt"); // ���� ��� ������ �����������
		double t1 = 0, t2 = 0; // ���������� ��� �������� ������� ������
		int N = 0; // ������ �������

		Treap* T = NULL; // ��������� ������
		map <int, int> M; // ������������ ������ �� ������-������ ������

		Node* splay = NULL;

		vector<double> SeachTimes1; // ���������� ���������� (����� � ���������)
		vector<double> SeachTimes2; // ���������� ���������� (����� � ������-������)
		vector<double> SeachTimes3; // ���������� ���������� (�������� �����)
		vector<double> SeachTimes4; // ���������� ���������� (����� � splay)
		vector<double> RemoveTimes1; // ���������� ���������� (�������� � ����������)
		vector<double> RemoveTimes2; // ���������� ���������� (�������� � ������-������)
		vector<double> RemoveTimes3; // ���������� ���������� (�������� � splay)
		vector<double> InsertTimes1; // ���������� ���������� (������� � ���������)
		vector<double> InsertTimes2; // ���������� ���������� (������� � ������-������)
		vector<double> InsertTimes3; // ���������� ���������� (������� � splay)

		int k = 0;


		for (int N = 100000; N != 1000000; N += 100000) // �������� ������
		{
			int *x = new int[N]; // ������ ������
			int *y = new int[N]; // ������ �����������

			srand(time(NULL));
			for (int i = 0; i < N; i++) // ��������� �������
			{
				x[i] = i;
				y[i] = rand() % 1000000;
			}
			for (int i = 0; i < N; i++) {
				std::swap(x[i], x[rand() % N]);
			}
			cout << "------------------------------------------------------------\n\n";
			cout << N << " ��������� � �������\n\n";

			//------------------------------------------------------------------------
			//      ��������� �������� ������
			//------------------------------------------------------------------------

			k = rand() % N;  // ����, � ������� ��������

			Build(T, x, y, N); // ��������� ��������� ������

			for (int i = 0; i < N; i++)
				Ins(splay, new Node(x[i]));// ��������� splay ������

			for (int i = 0; i < N; i++)
				M.insert(pair<int, int>(x[i], y[i])); // ��������� ������-������ ������

			t1 = omp_get_wtime();
			int value1 = Search(T, k)->key;
			t2 = omp_get_wtime();
			cout << "����� ������ � ���������� ������: " << fixed << t2 - t1 << endl;
			cout << "��������: " << value1 << endl;
			SeachTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			int value2 = M.find(k)->first;
			t2 = omp_get_wtime();
			cout << "����� ������ � ������-������ ������ ������: " << fixed << t2 - t1 << endl;
			cout << "��������: " << value2 << endl;
			SeachTimes2.push_back(t2 - t1);

			t1 = omp_get_wtime();
			splay = Search(k, splay);
			t2 = omp_get_wtime();
			cout << "����� ������ � splay ������: " << fixed << t2 - t1 << endl;
			cout << "��������: " << value1 << endl;
			SeachTimes4.push_back(t2 - t1);


			qsort(x, 0, N - 1);
			t1 = omp_get_wtime();
			int value3 = BinarySearch(x, N, k);
			t2 = omp_get_wtime();
			cout << "����� ��������� ������: " << fixed << t2 - t1 << endl;
			cout << "��������: " << value3 << endl;
			cout << endl;
			SeachTimes3.push_back(t2 - t1);

			//------------------------------------------------------------------------
			//      ��������� �������� ��������
			//------------------------------------------------------------------------

			t1 = omp_get_wtime();
			Remove(T, k);
			t2 = omp_get_wtime();
			cout << "����� �������� � ���������� ������: " << fixed << t2 - t1 << endl;
			RemoveTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			M.erase(k);
			t2 = omp_get_wtime();
			cout << "����� �������� � ������-������ ������: " << fixed << t2 - t1 << endl;
			RemoveTimes2.push_back(t2 - t1);
			t1 = omp_get_wtime();
			Delete(splay, rand() % N);
			t2 = omp_get_wtime();
			cout << "����� �������� � splay ������: " << fixed << t2 - t1 << endl;
			cout << endl;
			RemoveTimes3.push_back(t2 - t1);

			//------------------------------------------------------------------------
			//      ��������� �������� �������
			//------------------------------------------------------------------------

			t1 = omp_get_wtime();
			Insert(T, new Treap(k, rand() % 1000000));
			t2 = omp_get_wtime();
			cout << "����� ������� � ��������� ������: " << fixed << t2 - t1 << endl;
			InsertTimes1.push_back(t2 - t1);

			t1 = omp_get_wtime();
			M.insert(pair<int, int>(k, rand() % 1000000));
			t2 = omp_get_wtime();
			cout << "����� ������� � ������-������ ������: " << fixed << t2 - t1 << endl;

			InsertTimes2.push_back(t2 - t1);
			t1 = omp_get_wtime();
			Ins(splay, new Node(rand() % N));
			t2 = omp_get_wtime();
			cout << "����� ������� � splay ������: " << fixed << t2 - t1 << endl;
			cout << endl;
			InsertTimes3.push_back(t2 - t1);
			//------------------------------------------------------------------------

			delete[] x;
			delete[] y;
			T = NULL;
			M.clear();
		}

		int i = 0;
		fout << "����� � ��������� ������: ";
		for (int i = 0; i < SeachTimes1.size(); i++)
			fout << fixed << SeachTimes1[i] << " ";
		fout << endl;
		fout << "����� � splay ������: ";
		for (int i = 0; i < SeachTimes4.size(); i++)
			fout << fixed << SeachTimes4[i] << " ";
		fout << endl;
		fout << "����� � ������-������ ������: ";
		for (int i = 0; i < SeachTimes2.size(); i++)
			fout << fixed << SeachTimes2[i] << " ";
		fout << endl;
		fout << "�������� �����: ";
		for (int i = 0; i < SeachTimes3.size(); i++)
			fout << fixed << SeachTimes3[i] << " ";
		fout << endl;
		fout << "�������� � ��������� ������: ";
		for (int i = 0; i < RemoveTimes1.size(); i++)
			fout << fixed << RemoveTimes1[i] << " ";
		fout << endl;
		fout << "�������� � splay ������: ";
		for (int i = 0; i < RemoveTimes3.size(); i++)
			fout << fixed << RemoveTimes3[i] << " ";
		fout << endl;
		fout << "�������� � ������-������ ������: ";
		for (int i = 0; i < RemoveTimes2.size(); i++)
			fout << fixed << RemoveTimes2[i] << " ";
		fout << endl;
		fout << "������� � ��������� ������: ";
		for (int i = 0; i < InsertTimes1.size(); i++)
			fout << fixed << InsertTimes1[i] << " ";
		fout << endl;
		fout << "������� � splay ������: ";
		for (int i = 0; i < InsertTimes3.size(); i++)
			fout << fixed << InsertTimes3[i] << " ";
		fout << endl;
		fout << "������� � ������-������ ������: ";
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