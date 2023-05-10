#include <iostream>
#include <locale>
#include <stdio.h>
#include <conio.h>
#include "Classes.h"

using std::cin;
using std::cout;
using std::endl;

pair<ReadIterator, bool> TREE23::insert(int k, ReadIterator where)
{
	Node* t, * p, * q;
	int a = 1, up = 0;
	stack<pair<Node*, int>> St;
	//===== Инициализация =====
	t = root;
	if (t == nullptr) {	// Дерево пусто
		root = new Node(k);
		height = 1;
		return make_pair(ReadIterator(root, move(St)), true);
	}
	else {		//Поиск по дереву
		St.push(make_pair(root, 1));	// Создание и инициализация стека
										//===== Поиск места вставки =====
		while (a) {
			if ((k == t->key) || //Проверка на совпадение значений
				(t->next) && (k == t->next->key) ||
				(t->next) && (t->next->next) && (k == t->next->next->key)) { //Элемент имеется
				return make_pair(ReadIterator(t, move(St)), false);		//Выход "вставка не понадобилась"
			}
			if (k < t->key) {
				if (t->down) { //Идём вниз и влево
					p = t->next->next;
					t = t->down;
					if (p) St.push(make_pair(t, 4));
					else St.push(make_pair(t, 2));
				}
				else { //Новый лист слева (вставка справа и перенос данных)
					p = new Node(t->key);
					p->next = t->next;
					t->key = k;
					if ((t->next) && (t->next->next)) up = 1; //Вставлен четвёртый
					t->next = p;
					a = 0;
				}
			}
			else if (!(t->next)) { //Добавление второго элемента
				p = new Node(k);
				t->next = p;
				a = 0;
			}
			else if (k < t->next->key) {
				if (t->next->down) {//Идём вниз посередине
					p = t->next->next;
					t = t->next->down;
					if (p) St.push(make_pair(t, 5));
					else St.push(make_pair(t, 3));
				}
				else { // Новый лист посередине
					p = new Node(k);
					p->next = t->next;
					if (t->next->next) up = 1; //Вставлен четвёртый
					t->next = p;
					a = 0;
				}
			}
			else if (!t->next->next) { //Третьего пути нет;
				if (t->next->down) {     //Идём вниз посередине
					t->next->key = k; 	//Меняем наибольший
					t = t->next->down;
					St.push(make_pair(t, 3));
				}
				else {                   // Новый лист справа
					p = new Node(k);
					t->next->next = p;
					a = 0;
				}
			}
			else {
				if (t->next->next->down) { //Идём вниз и вправо
					if (k > t->next->next->key)
						t->next->next->key = k; //Меняем наибольший
					t = t->next->next->down;
					St.push(make_pair(t, 6));
				}
				else {                     //Новый лист
					p = new Node(k);
					a = 0;
					if (t->next->next) {    //Третий уже есть...
						up = 1;
						if (k < t->next->next->key) { // - третий из четырёх
							p->next = t->next->next;
							t->next->next = p;
						}
						else {                  // - четвёртый
							t->next->next->next = p;
						}
					}
					else { //Новый лист - третий из трёх
						t->next->next = p;
						//	           St->Top( )->key = k;
					}
				}
			}
		}
		while (up) { //Устранение четвёртого сына (если он есть)
			p = St.top().first;
			a = St.top().second;
			St.pop();	//p-> узел с четырьмя сыновьями
			if (a != 1) t = St.top().first;	//t-> управляющий узел для "p->"
			q = new Node(p->next->next->next->key);
			switch (a) {
			case 1:	//Корневой узел дерева: root == p == t
				up = 0;
				height++;	//Увеличение высоты дерева
				root = t = new Node(root->next->key); //Создание двойного управляющего узла
				t->down = p;
				t->next = q;
				break;
			case 2:	//Новый средний сын для двух
				up = 0;
			case 4:     //Новый средний для трёх
				q->next = t->next; //вставка в цепочку
				t->next = q;
				t->key = p->next->key;	//Коррекция ссылочного значения
				break;
			case 3:	//Новый правый сын для двух
				up = 0;
			case 5:     //Новый правый для трёх
				q->next = t->next->next;
				t->next->next = q;		//Присоединение к цепочке
				t->next->key = p->next->key;	//Коррекция ссылочного значения
				break;
			case 6:	//Новый четвёртый сын
				t->next->next->next = q;		//Присоединение к цепочке
				t->next->next->key = p->next->key;	//Коррекция ссылочного значения
				break;
			}
			q->down = p->next->next;
			p->next->next = nullptr; //Расцепление сыновей
		}
		//delete St; 	//Уничтожение стека
		return make_pair(ReadIterator(t, move(St)), true);
	}
}

int TREE23::erase(int k)   //Удаление (единственного) элемента из 2-3-дерева
{
	Node* t(root), * p(nullptr);
	int a = 1, up = 0, result = 0;
	if (t) {   //Дерево не пусто
		if (t->next) {
			if (t->next->next) {
				if (k > t->next->next->key) return result;
			}
			else
				if (k > t->next->key) return result;
		}
		else
			if (k > t->key) return result;
		// //k больше максимума, выход
		stack<pair<Node*, int>> St;
		St.push(make_pair(t, 1));   // Создание и инициализация стека
		while (1) {
			if (t->down) { //Узел — не лист, идём вниз
				if (k < t->key) { //Идём влево
					St.push(make_pair(t, 1));
					t = t->down;
				}
				else if (!(t->next->next) ||
					k < t->next->next->key) { //Идём посередине
					St.push(make_pair(t, 2));
					t = t->next->down;
				}
				else { //Идём вправо
					St.push(make_pair(t, 3));
					t = t->next->next->down;
				}
			}
			else {         //Дошли до листа,
				if (k == t->key) {   // проверка на совпадение значений
					if (p = t->next) {   //Лист хотя бы двойной
						t->key = t->next->key; //Подмена первого вторым
						t->next = t->next->next;
						if (t != root) {
							result = 2; //Корректировать минимум выше по дереву
							if (!t->next) up = 1; //Остался единственный
						}
						else result = 1; //Удаляется начало в корне
						--count;
						delete p;   //Удаление второго
					}
					else {   //Удаление единственного листа в корне
						delete t;
						height = count = 0;
						root = nullptr;
						result = 1;
					}
				}
				else if ((p = t->next) && (k == t->next->key)) {
					t->next = t->next->next; //Исключение второго
					result = 1;
					if (!t->next && t != root) up = 1;    //Остался единственный
					--count;
					delete p;   //Удаление второго
				}
				else if ((p = t->next->next) && (k == t->next->next->key)) {
					t->next->next = nullptr;
					result = 1;
					--count;
					delete p;   //Удаление третьего
				}
				break;    //Конец поиска, элемент удалён или не найден
			}
		}
		while (up) { //Устранение единственного сына (если он получился)
			up = 0;
			p = St.top().first; a = St.top().second; //сын p — одиночный узел t
			switch (a) {
			case 1:   //Неполный узел слева: t == p->down
				if (p->key != t->key) p->key = t->key, result = 2;
				else result = 1;
				if (p->next->down->next->next) {   //Справа — тройной узел,
					t->next = p->next->down;             // берём сына
					p->next->down = t->next->next;
					t->next->next = nullptr;
					p->next->key = p->next->down->key;
				}
				else {   //Справа — двойной, присоединяем его сыновей
					t->next = p->next->down;
					if (p->next->next) { //УУ — группа из трёх
						t = p->next;
						p->next = p->next->next;
						delete t; result = 1;
					}
					else if (p == root) { //Двойной — в корне: удаляем оба
						root = t;
						--height;
						delete p->next;
						delete p;
						result = 1;
					}
					else { //Двойной УУ —  не в корне, удаляем второй и вверх
						delete p->next;
						p->next = nullptr;
						t = p;
						up = 1;
						St.pop();
					}
				}
				break;
			case 2:   //Неполный узел посередине: p == t->next->down
				if (p->down->next->next) {   //Слева — тройной узел,
					p->down->next->next->next = t; // берём сына
					p->next->down = p->down->next->next;
					p->down->next->next = nullptr;
					p->next->key = p->next->down->key;
				}
				else {   //Слева - двойной, отдаём ему сына, удаляем УУ
					p->down->next->next = t;
					t = p->next;
					p->next = p->next->next;
					delete t;
					if (!p->next) { //УУ был двойной
						if (p == root) { //УУ - корневой: удаляем и второй
							root = p->down;
							--height;
							delete p;
							result = 1;
						}
						else { // одиночный УУ - не корень: вверх
							up = 1;
							t = p;
							St.pop();
						}
					}
				}
				break;
			case 3:   //Неполный узел справа: p == t->next->next->down
				if (p->next->down->next->next) {   //Слева - тройной узел,
					p->next->down->next->next->next = t; // берём сына
					p->next->next->down = p->next->down->next->next;
					p->next->down->next->next = nullptr;
					p->next->next->key = p->next->next->down->key;
				}
				else {   //Слева — двойной, отдаём ему сына, удаляем
					p->next->down->next->next = t;
					delete p->next->next;
					p->next->next = nullptr;
					result = 1;
				}
			}
		}
		if (result == 2) {   //Корректировка минимума
			do {
				p = St.top().first;
				switch (St.top().second) {
				case 1:
					p->key = p->down->key;
					break;
				case 2:
					p->next->key = p->next->down->key;
					result = 1;
					break;
				case 3:
					p->next->next->key = p->next->next->down->key;
					result = 1;
				}
				if (St.top().first == root) break;
				St.pop();
			} while (result == 2);
		}
	}
	return result;
}

void Node::erase() //удаление узла
{
	if (down) {
		if (next->next) next->next->down->erase();
		next->down->erase();
		down->erase();
	}
	else {
		if (next) {
			if (next->next) delete next->next;
			delete next;
		}
		delete this;
	}
}
//-----------------------------------------------------------------------
TREE23 :: ~TREE23()
{
	if (root) root->erase();
}
//***********************************************************************
//Параметры вывода на экран
const int FIRSTROW = 0, FIRSTCOL = 40,
MAXCOL = 80, OFFSET[] = { 40, 24, 10, 4, 1 },
MAXROW = FIRSTROW + 18,
MAXOUT = FIRSTROW + 14, SHIFT = 2;
char SCREEN[MAXROW * MAXCOL];
int row = 0, col = 0;
//***********************************************************************
//Функции работы с экраном
void gotoxy(int c, int r) { row = r, col = c; }
//-----------------------------------------------------------------------
void clrscr()
{
	for (auto& x : SCREEN) x = '.';
}
//-----------------------------------------------------------------------
void showscr()
{
	for (int i = 0; i < MAXROW; i++)
		for (int j = 0; j < MAXCOL; j++)
			if (SCREEN[i * MAXCOL + j] && SCREEN[i * MAXCOL + j] != 'ю')
				cout << SCREEN[i * MAXCOL + j];
			else cout << ' ';
	cout << endl;
}
//***********************************************************************
//Вывод узла в массив screen в точку (x, y)
void Node::out(int row, int col)
{
	if ((row > MAXROW) || (col < 1) || (col > MAXCOL)) return;
	gotoxy(col, row);
	if (row > MAXOUT) {
		sprintf_s(SCREEN + row * MAXCOL + col, 4, "..."), col += 3;
		return;
	}
	sprintf_s(SCREEN + row * MAXCOL + col, 4, "%1d ", key);
	if (next) {
		sprintf_s(SCREEN + (row + 1) * MAXCOL + col + 1, 4, "%1d ", next->key);
		if (next->next)
			sprintf_s(SCREEN + (row + 2) * MAXCOL + col + 2, 4, "%1d ", next->next->key);
		else sprintf_s(SCREEN + (row + 2) * MAXCOL + col + 2, 4, "@  ");
	}
}
//-----------------------------------------------------------------------
//Вывод дерева на экран
void TREE23::display()
{
	clrscr();
	gotoxy(FIRSTCOL - OFFSET[0], FIRSTROW);
	col += sprintf_s(SCREEN + row * MAXCOL + col, 11, "Дерево %c:", name);
	if (root) root->display(0, FIRSTCOL - SHIFT);
	else sprintf_s(SCREEN + row * MAXCOL + col, 9, "<Пусто!>");
	showscr();
}
//-----------------------------------------------------------------------
//Вывод узла на экран и обход
void Node::display(int lvl, int col)
{
	int row = FIRSTROW + lvl * 4;
	this->out(row, col);
	if (down) {
		down->display(lvl + 1, col - (OFFSET[lvl + 1]));
		next->down->display(lvl + 1, col);
		if ((next->next) && (next->next->down))
			next->next->down->display(lvl + 1, col + (OFFSET[lvl + 1]));
	}
}
//***********************************************************************
//Вставка в 2-3 дерево
//***********************************************************************
//Создание 2-3 дерева по возрастающей последовательности
int TREE23::build(int k)	// Приём возрастающей последовательности
{
	Node* q;				// и преобразование её в 2-3-дерево
	static Node* p;
	static int count = 0;

	if (k) { //Добавление в очередь
		q = new Node(k);
		if (root == nullptr) { //Первое обращение
			root = p = q;
			count = 1;
		}
		else {
			p->next = q;
			p = q;
			count++;
		}
	}
	else if (count) { //Ввод окончен, строим дерево
		int rest = count;
		height = 1;
		while (rest > 3) {
			q = root;
			height++;
			if (rest == 4) { //Два управляющих узла
				root = new Node(q->next->key);
				p = new Node(q->next->next->next->key);
				root->next = p;
				root->down = q;
				p->down = q->next->next;
				q->next->next = nullptr;
				rest = 0;
			}
			else { //Цепочка управляющих узлов
				int last = rest - 3;
				root = p = new Node(q->next->next->key);
				p->down = q;
				q = q->next->next->next;
				p->down->next->next->next = nullptr;
				rest = 1;
				while (last) {
					rest++;
					switch (last) {
					case 2:
						p->next = new Node(q->next->key);
						p->next->down = q;
						last = 0;
						break;
					case 3:
						p->next = new Node(q->next->next->key);
						p->next->down = q;
						last = 0;
						break;
					case 4:
						p->next = new Node(q->next->key);
						p->next->down = q;
						p->next->next = new Node(q->next->next->next->key);
						p->next->next->down = q->next->next;
						q->next->next = nullptr;
						last = 0;
						rest++;
						break;
					default:
						p->next = new Node(q->next->next->key);
						p->next->down = q;
						q = q->next->next->next;
						p = p->next;
						p->down->next->next->next = nullptr;
						last -= 3;
						break;
					}
				}
			}
		}
	}
	return count;
}
//***********************************************************************
//Обход 2-3 дерева
Stack globalStack;	//Внешний стек для функции step
int TREE23::step(Node*& p, Stack& stack = globalStack) const
{
	int a;
	if (!p) { //Первое обращение
		if (!root) return 0; //Дерево пусто, выход
		p = root;	//Поиск крайнего левого элемента
		stack.push(p, 1);
		while (p->down) {
			stack.push(p, 2);
			p = p->down;
		}
		return 1;
	}
	else { //Текущий уже выдан
		if (p->next) { //Есть лист справа, шаг вправо
			p = p->next;
			return 1;
		}
		while (true) {	//Поиск очередного листа
			stack.pop(p, a);	//Шаг вверх
			switch (a) {
			case 1:
				return 0;	//Вернулись к корню, конец
			case 2:
				stack.push(p, 3);	//Спуск по средней ветке
				p = p->next->down;
				while (p->down) {
					stack.push(p, 2);
					p = p->down;
				}
				return 1;
			case 3:
				if (p->next->next) {
					stack.push(p, 4);	//Спуск по правой (если есть)
					p = p->next->next->down;
					while (p->down) {
						stack.push(p, 2);
						p = p->down;
					}
					return 1;
				}
			case 4:;	//Обошли всё поддерево, подъём
			}
		}
	}
}
//***********************************************************************
//Генерация 2-3 дерева
void TREE23::genSet()
{
	int power = (rand() % genPower);
	while (power--)
		insert(rand() % genMaxNum + 1, ReadIterator());
}
//***********************************************************************
//Поиск по ключу в 2-3 дереве
bool TREE23::find(int num) const
{
	Node* t = root;
	while (t) {
		if (num == t->getKey())
			return true;
		if (num > t->getKey() && t->next)
			t = t->next;
		else
			t = t->down;
	}
	return false;
}
//***********************************************************************
//Перегрузка операторов 2-3 дерева
const TREE23& TREE23::operator &(const TREE23& rightExp) const
{
	TREE23* temp = new TREE23;
	ReadIterator leftTree(*this);
	ReadIterator rightTree(rightExp);
	//Stack stackL, stackR;
	int checkL, checkR;
	bool t = false;
	Node* curL = nullptr, * curR = nullptr;
	while ((leftTree.ptr != nullptr) && (rightTree.ptr != nullptr)) {
		while (((leftTree.ptr != nullptr) && (rightTree.ptr != nullptr)) && *leftTree < *rightTree)
			leftTree++;
		while (((rightTree.ptr != nullptr) && (rightTree.ptr != nullptr)) && *rightTree < *leftTree)
			rightTree++;
		if (*rightTree == *leftTree) {
			temp->build(*rightTree);
			if (!t) t = true;
			leftTree++;;
			rightTree++;
		}
	}
	if (t)
		temp->build(0);
	return *temp;
}

const TREE23& TREE23::operator | (const TREE23& rightExp) const
{
	TREE23* temp = new TREE23;
	Stack stackL, stackR;
	int checkL, checkR;
	bool t = false;
	Node* curL = nullptr, * curR = nullptr;
	checkR = rightExp.step(curR, stackR);
	checkL = step(curL, stackL);
	while (checkL || checkR) {
		if (!t) t = true;
		if (checkL && checkR)
			if (curR->getKey() == curL->getKey()) {
				temp->build(curR->getKey());
				checkL = step(curL, stackL);
				checkR = rightExp.step(curR, stackR);
			}
			else {
				while (curL->getKey() < curR->getKey() && checkL) {
					temp->build(curL->getKey());
					checkL = step(curL, stackL);
				}
				while (curR->getKey() < curL->getKey() && checkR) {
					temp->build(curR->getKey());
					checkR = rightExp.step(curR, stackR);
				}
			}
		else
			if (checkL)
				while (checkL) {
					temp->build(curL->getKey());
					checkL = step(curL, stackL);
				}
			else
				while (checkR) {
					temp->build(curR->getKey());
					checkR = step(curR, stackR);
				}
	}
	if (t)
		temp->build(0);
	return *temp;
}

const TREE23& TREE23::operator = (const TREE23& rightExp)
{
	Node* cur = nullptr;
	if (rightExp.root) {
		while (rightExp.step(cur))
			build(cur->getKey());
		build(0);
	}
	return *this;
}

int menu()
{
	int point;
	do {
		cin.clear();
		cin.sync();
		cout << "Выберите пункт меню" << endl;
		cout << "1 - Сгенерировать дерево" << endl;
		cout << "0 - Выход" << endl;
		cout << ">";
		cin >> point;
		if (cin.fail())
			cout << "Что-то пошло не так, выберите пункт меню повторно" << endl;
	} while (cin.fail());
	return point;
}

Stack::~Stack()
{
	El* s;
	while (ptr) {
		s = ptr;
		ptr = ptr->prev;
		delete s;
	}
}

void Stack::push(Node* pp, int ct) //Вставить
{
	El* s = new El(pp, ct);
	s->prev = ptr;
	ptr = s;
}

int Stack::pop(Node*& pp, int& ct) //Вытолкнуть
{
	El* s = ptr;
	pp = ptr->el;
	ct = ptr->ctl;
	ptr = ptr->prev;
	delete s;
	return (ptr != nullptr);
}

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "RUS");
	int pause;
	do
	{
		TREE23 A('A'), B('B'), C('C'), D('D'), E('E');
		switch (pause = menu())
		{
		case 1:
			A.genSet(); B.genSet(); C.genSet(); D.genSet();
			//cout << "Сгенерированные множества" << endl;
			break;
		case 0:
			cout << "End" << endl;
			break;
		default:
			cout << "Такого пункта не существует, повторите ввод!" << endl;
		}
		if (pause == 1)
		{
			system("cls");
			A.display();
			B.display();
			/*A.insert(34, ReadIterator());
			A.insert(35, ReadIterator());
			B.insert(54, ReadIterator());
			A.display();
			B.display();
			A.erase(34);
			B.erase(54);
			A.display();
			B.display();*/
			cout << "Результат E = (A^B-C) U D ∩ E))" << endl;
			E = A & B;
			E.display();
			cout << "Для возврата в меню введите любое число, для выхода 0: ";
			cin >> pause;
		}
	} while (pause);
	return 0;
}