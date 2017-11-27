#include <iostream>
#include <cstdlib>

struct Node {
	int key;
	Node* lchild;
	Node* rchild;

	Node() { }
	Node(int key) : key(key), lchild(NULL), rchild(NULL) { }
};


/* RR
   k2                 k1
  /  \               /  \
 k1   Z   ==>       X    k2
 / \                    /  \
X   Y                  Y    Z
*/
inline Node* RRotate(Node* k2)
{
	Node* k1 = k2->lchild;
	k2->lchild = k1->rchild;
	k1->rchild = k2;
	return k1;
}

/* LL:
	k2                 k1
   /  \               /  \
  X    k1   ==>      k2   Z
 / \                /  \
Y    Z             X    Y
*/
inline Node* LRotate(Node* k2)
{
	Node* k1 = k2->rchild;
	k2->rchild = k1->lchild;
	k1->lchild = k2;
	return k1;
}

Node* Splay(int key, Node* root)
{
	if (!root)
		return NULL;
	Node header;
	header.lchild = header.rchild = NULL;
	Node* LeftTreeMax = &header;
	Node* RightTreeMin = &header;
	while (1)
	{
		if (key < root->key)
		{
			if (!root->lchild)
				break;
			if (key < root->lchild->key)
			{
				root = RRotate(root);
				if (!root->lchild)
					break;
			}

			RightTreeMin->lchild = root;
			RightTreeMin = RightTreeMin->lchild;
			root = root->lchild;
			RightTreeMin->lchild = NULL;
		}
		else if (key > root->key)
		{
			if (!root->rchild)
				break;
			if (key > root->rchild->key)
			{
				root = LRotate(root);
				if (!root->rchild)
					break;
			}

			LeftTreeMax->rchild = root;
			LeftTreeMax = LeftTreeMax->rchild;
			root = root->rchild;
			LeftTreeMax->rchild = NULL;
		}
		else
			break;
	}

	LeftTreeMax->rchild = root->lchild;
	RightTreeMin->lchild = root->rchild;
	root->lchild = header.rchild;
	root->rchild = header.lchild;

	return root;
}



Node* Search(int key, Node* root)
{
	return Splay(key, root);
}

void PreOrder(Node* root)
{
	if (root)
	{
		std::cout << "key: " << root->key;
		if (root->lchild)
			std::cout << " | left child: " << root->lchild->key;
		if (root->rchild)
			std::cout << " | right child: " << root->rchild->key;
		std::cout << "\n";
		PreOrder(root->lchild);
		PreOrder(root->rchild);
	}
}

void Merge(Node* & root, Node* left, Node* right)
{
	if (!left || !right)
		root = left ? left : right;
	else
		Merge(right->lchild, left, right->lchild), root = right;
}

void Split(Node* root, int key, Node* & left, Node* & right)
{
	Splay(key, root);
	if (!root)
		left = right = NULL;
	else if (key < root->key)
		Split(root->lchild, key, left, root->lchild), right = root;
	else
		Split(root->rchild, key, root->rchild, right), left = root;
}

void Delete(Node* & root, int key)
{
	root = Splay(key, root);
	Merge(root, root->lchild, root->rchild);
}

void Ins(Node* & root, Node* n)
{
	if (!root)
		root = n;
	else {
		Node *l = new Node();
		Node *r = new Node();
		Split(root, n->key, l, r);
		root = n;
		n->lchild = l;
		n->rchild = r;
	}
}
