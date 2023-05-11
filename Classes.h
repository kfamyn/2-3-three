#ifndef CLASSES_H
#define CLASSES_H


using namespace std;

enum genSettings { genPower = 20, genMaxNum = 99 };
class Stack;
struct ReadIterator;
class TwoThreeTree;

class Node {	//Узел дерева
public:
	void display(int, int);
	void out(int, int);
	int key;		    //Ключ
	Node* next, * down;
	void erase();
	friend class TwoThreeTree;

	Node() : down(nullptr), next(nullptr) {}
	Node(int k) : key(k), down(nullptr), next(nullptr) {}
	int getKey() const { return key; }
	int& getKeyReference() { return key; };
	Node* getNext() { if (next) return next; else return nullptr; }
	Node* getDown() { if (down) return down; else return nullptr; }
	~Node() { delete down; }
	//			if(next) delete next; };
};

Node sentinel;
Node* NULLNODE = &sentinel;

using StackPairs = stack<std::pair<Node*, int>>;

struct ReadIterator : public std::iterator<std::forward_iterator_tag, int> {
	Node* ptr;
	StackPairs stack;
	ReadIterator(const TwoThreeTree& tree);
	ReadIterator(Node* p = nullptr) : ptr(p) { }
	ReadIterator(Node* p, const StackPairs&& St) : ptr(p), stack(move(St)) {}
	bool operator == (const ReadIterator& other) const { return ptr == other.ptr; }
	bool operator != (const ReadIterator& other) const { return !(*this == other); }
	ReadIterator& operator++()
	{
		int a;
		if (!ptr) { //Первое обращение?
			return *this; //Не работает без предварительной установки на дерево
		}
		else { //Текущий уже выдан
			if (ptr->getNext()) { //Есть лист справа, шаг вправо
				ptr = ptr->getNext();
				return (*this);
			}
			while (true) {	//Поиск очередного листа
				a = stack.top().second;	//Шаг вверх
				ptr = stack.top().first;
				stack.pop();
				switch (a) {
				case 1:
					ptr = NULLNODE;	//Вернулись к корню, конец
					return (*this);
					break;
				case 2:
					stack.push(std::make_pair(ptr, 3));	//Спуск по средней ветке
					ptr = ptr->getNext()->getDown();
					while (ptr->getDown()) {
						stack.push(std::make_pair(ptr, 2));
						ptr = ptr->getDown();
					}
					return (*this);
					break;
				case 3:
					if (ptr->getNext()->getNext()) {
						stack.push(std::make_pair(ptr, 4));	//Спуск по правой (если есть)
						ptr = ptr->getNext()->getNext()->getDown();
						while (ptr->getDown()) {
							stack.push(std::make_pair(ptr, 2));
							ptr = ptr->getDown();
						}
						return (*this);
					}
					break;
				case 4:
					//ptr = nullptr;	//Обошли всё поддерево, подъём
					break;
				}
			}
		}
		return (*this);
	}

	ReadIterator operator++(int) { ReadIterator temp(*this); ++* this; return temp; }
	pointer operator->() const { return &ptr->getKeyReference(); }
	reference operator*() const { return ptr->getKeyReference(); }
};

class TwoThreeTree {	//Класс 2-3-дерева
	Node* root;
	int height;
	char name;
	int count;
	friend ReadIterator;
public:
	const TwoThreeTree& operator | (const TwoThreeTree&) const;
	const TwoThreeTree& operator & (const TwoThreeTree&) const;
	const TwoThreeTree& operator = (const TwoThreeTree&);

	bool find(int num) const;
	//int insert(int);   //Вставка элемента
	void display();    //Вывод на экран
	int build(int);    //Построение дерева
	int step(Node*&, Stack&) const; //Шаг обхода
	void genSet();     //Генерация данных
	TwoThreeTree(char n = 'T') : root(nullptr), height(0), name(n) {};
	pair<ReadIterator, bool> insert(int k, ReadIterator where = ReadIterator());
	int erase(int k);
	~TwoThreeTree();
};


class El {
	Node* el;
	int ctl;
	El* prev;
	friend class Stack;
public:
	El(Node* e, int c) : el(e), ctl(c), prev(nullptr) {}
};

class Stack {
	El* ptr;
public:
	Stack() : ptr(nullptr) {}
	Stack(Node* pp, int ct) { ptr = new El(pp, ct); };
	~Stack();
	Node* top() { return ptr->el; }
	void push(Node*, int);
	int pop(Node*&, int&);
};

ReadIterator::ReadIterator(const TwoThreeTree& tree) {
	if (!tree.root) {
		ptr = NULLNODE;
		return;//Дерево пусто, выход
	}
	ptr = tree.root;	//Поиск крайнего левого элемента
	stack.push(std::make_pair(ptr, 1));
	while (ptr->getDown()) {
		stack.push(std::make_pair(ptr, 2));
		ptr = ptr->getDown();
	}
};
#endif