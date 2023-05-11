#ifndef CLASSES_H
#define CLASSES_H
enum genSettings { genPower = 20, genMaxNum = 99 };
struct ReadIterator;
class TwoThreeTree;

class Node {	//Узел дерева
private:
	void display(int, int);
	void out(int, int);
	int key;		    //Ключ
	Node* next, * down;
	void erase();
	friend class TwoThreeTree;
public:
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

using StackPairs = std::stack<std::pair<Node*, int>>;

struct ReadIterator : public std::iterator<std::forward_iterator_tag, int> {
	Node* ptr;
	StackPairs stack;
	ReadIterator(const TwoThreeTree& tree);
	ReadIterator(Node* p = nullptr) : ptr(p) { }
	ReadIterator(Node* p, const StackPairs&& St) : ptr(p), stack(std::move(St)) {}
	bool operator == (const ReadIterator& other) const { return ptr == other.ptr; }
	bool operator != (const ReadIterator& other) const { return !(*this == other); }
	ReadIterator& operator++();

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
	void display();    //Вывод на экран
	int build(int);    //Построение дерева
	void genSet();     //Генерация данных
	TwoThreeTree(char n = 'T') : root(nullptr), height(0), name(n) {};
	std::pair<ReadIterator, bool> insert(int k, ReadIterator where = ReadIterator());
	int erase(int k);
	~TwoThreeTree();
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