#ifndef TWOTHREETREE_H
#define TWOTHREETREE_H

enum genSettings { genPower = 20, genMaxNum = 99 };
enum directions { ROOT = 1, LEFT = 2, MIDDLE = 3, RIGHT = 4 };
struct ReadIterator;
class TwoThreeTree;
class Node;

class Node {	//Узел дерева
private:
	int key;		    //Ключ
	Node* next, * down;
	void erase();
	void display(int, int);
	void out(int, int);
	friend TwoThreeTree;
public:
	Node() : down(nullptr), next(nullptr) {}
	Node(int k) : key(k), down(nullptr), next(nullptr) {}
	int getKey() const { return key; }
	int& getKeyReference() { return key; };
	Node* getNext() { if (next) return next; else return nullptr; }
	Node* getDown() { if (down) return down; else return nullptr; }
	~Node() {}
	//			if(next) delete next; };
};

struct ReadIterator : public std::iterator<std::forward_iterator_tag, int> {
	Node* ptr;
	std::stack<std::pair<Node*, int>> stack;
	ReadIterator(const TwoThreeTree& tree);
	ReadIterator(Node* p = nullptr) : ptr(p) { }
	ReadIterator(Node* p, const std::stack<std::pair<Node*, int>>&& St) : ptr(p), stack(std::move(St)) {}
	bool operator == (const ReadIterator& other) const { return ptr == other.ptr; }
	bool operator != (const ReadIterator& other) const { return !(*this == other); }
	ReadIterator& operator++();
	ReadIterator operator++(int) { ReadIterator temp(*this); ++* this; return temp; }
	pointer operator->() const { return &ptr->getKeyReference(); }
	reference operator*() const { return ptr->getKeyReference(); }
};

template <typename Container, typename Iterator = ReadIterator>
class InsertIterator : public std::iterator<std::output_iterator_tag, typename Container::value_type> {
protected:
	Container& container;
	Iterator iterator;
	std::pair<ReadIterator, bool> temp;
public:
	InsertIterator(Container& container, Iterator iterator) : container(container), iterator(iterator) {}
	const InsertIterator<Container>& operator = (const typename Container::value_type& value) {
		temp = container.insert(value, iterator);
		if (temp.second) {
			iterator = container.insert(value, iterator).first;
		}
		else {
			std::cout << "duplicated element {" << value << '}' << std::endl;
		}
		return *this;
	}
	const InsertIterator<Container>& operator = (const InsertIterator<Container>&) { return *this; }
	InsertIterator<Container>& operator* () { return *this; }
	InsertIterator<Container>& operator++ () { return *this; }
	InsertIterator<Container>& operator++ (int) { return *this; }
};


template <typename Container, typename Iter>
inline InsertIterator<Container, Iter> inserter(Container& c, Iter it) {
	return InsertIterator<Container, Iter>(c, it);
}

class TwoThreeTree {	//Класс 2-3-дерева
	Node* root;
	int height;
	char name;
	int count;
	friend ReadIterator;
public:
	using value_type = int;
	TwoThreeTree(char n = 'T') : root(nullptr), height(0), name(n) {};
	const TwoThreeTree& operator = (const TwoThreeTree&);
	const TwoThreeTree& operator | (const TwoThreeTree&) const;
	const TwoThreeTree& operator & (const TwoThreeTree&) const;
	const TwoThreeTree& operator ^ (const TwoThreeTree&) const;
	const TwoThreeTree& operator / (const TwoThreeTree&) const; // вместо \ (разность)
	bool find(int num) const;
	void display();    //Вывод на экран
	int build(int);    //Построение дерева
	void genSet();     //Генерация данных
	int erase(int k);
	int getSize() const { return count; };
	ReadIterator begin();
	std::pair<ReadIterator, bool> insert(int k, ReadIterator where = ReadIterator());
	~TwoThreeTree();
};

class Sequence {
	TwoThreeTree keysTree;
	std::vector<int> values;
public:
	Sequence(std::initializer_list<int> valuesList);
	Sequence& merge(const Sequence& rightOperand);
	Sequence& substitute(const Sequence& rightOperand, int fromPosition);
	Sequence& erase(int fromPosition, int toPosition);
};
#endif
