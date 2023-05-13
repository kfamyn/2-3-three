#include <iostream>
#include <locale>
#include <stdio.h>
#include <conio.h>
#include <stack>
#include "Classes.h"

//Имитация пустого узла для итератора чтения (защита от обращения к пустому узлу)
Node NULL_NODE_IMITATION;
Node* NULLNODE = &NULL_NODE_IMITATION;
//***********************************************************************
//Параметры вывода на экран
const int FIRSTROW = 0, FIRSTCOL = 40,
MAXCOL = 120, OFFSET[] = { 40, 24, 10, 4, 1 },
MAXROW = FIRSTROW + 18,
MAXOUT = FIRSTROW + 14, SHIFT = 2;
char SCREEN[MAXROW * MAXCOL];
int row = 0, col = 0;
//***********************************************************************
//Функции работы с экраном
void gotoxy(int c, int r) { row = r, col = c; }

void clrscr()
{
	for (auto& x : SCREEN) x = '.';
}

void showscr()
{
	for (int i = 0; i < MAXROW; i++)
		for (int j = 0; j < MAXCOL; j++)
			if (SCREEN[i * MAXCOL + j] && SCREEN[i * MAXCOL + j] != 'ю')
				std::cout << SCREEN[i * MAXCOL + j];
			else std::cout << ' ';
	std::cout << std::endl;
}
//***********************************************************************
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
//***********************************************************************
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
//***********************************************************************
//Постановка итератора чтения на дерево
ReadIterator::ReadIterator(const TwoThreeTree& tree) {
	if (!tree.root) {
		ptr = NULLNODE;
		return;//Дерево пусто, выход
	}
	ptr = tree.root;	//Поиск крайнего левого элемента
	stack.push(std::make_pair(ptr, ROOT));
	while (ptr->getDown()) {
		stack.push(std::make_pair(ptr, LEFT));
		ptr = ptr->getDown();
	}
};
//***********************************************************************
//Инкремент итератора чтения
ReadIterator& ReadIterator::operator++() {
	int direction;
	if (!ptr) { //Первое обращение?
		return *this; //Не работает без предварительной установки на дерево
	}
	else { //Текущий уже выдан
		if (ptr->getNext()) { //Есть лист справа, шаг вправо
			ptr = ptr->getNext();
			return (*this);
		}
		while (true) {	//Поиск очередного листа
			direction = stack.top().second;	//Шаг вверх
			ptr = stack.top().first;
			stack.pop();
			switch (direction) {
			case ROOT:
				ptr = NULLNODE;	//Вернулись к корню, конец
				return (*this);
				break;
			case LEFT:
				stack.push(std::make_pair(ptr, MIDDLE));	//Спуск по средней ветке
				ptr = ptr->getNext()->getDown();
				while (ptr->getDown()) {
					stack.push(std::make_pair(ptr, LEFT));
					ptr = ptr->getDown();
				}
				return (*this);
				break;
			case MIDDLE:
				if (ptr->getNext()->getNext()) {
					stack.push(std::make_pair(ptr, RIGHT));	//Спуск по правой (если есть)
					ptr = ptr->getNext()->getNext()->getDown();
					while (ptr->getDown()) {
						stack.push(std::make_pair(ptr, LEFT));
						ptr = ptr->getDown();
					}
					return (*this);
				}
				break;
			case RIGHT:	//Обошли всё поддерево, подъём
				break;
			}
		}
	}
	return (*this);
}
//***********************************************************************
//Вставка узла в дерево
std::pair<ReadIterator, bool> TwoThreeTree::insert(int k, ReadIterator where)
{
	Node* temporaryRootPointer, * nodePointerP, * nodePointerQ;
	int direction = ROOT, up = 0;
	std::stack<std::pair<Node*, int>> stack;
	//===== Инициализация =====
	temporaryRootPointer = root;
	if (temporaryRootPointer == nullptr) {	// Дерево пусто
		root = new Node(k);
		height = 1;
		return std::make_pair(ReadIterator(root, std::move(stack)), true);
	}
	else {		//Поиск по дереву
		stack.push(std::make_pair(root, ROOT));	// Создание и инициализация стека
										//===== Поиск места вставки =====
		while (direction) {
			if ((k == temporaryRootPointer->key) || //Проверка на совпадение значений
				(temporaryRootPointer->next) && (k == temporaryRootPointer->next->key) ||
				(temporaryRootPointer->next) && (temporaryRootPointer->next->next) && (k == temporaryRootPointer->next->next->key)) { //Элемент имеется
				return std::make_pair(ReadIterator(temporaryRootPointer, std::move(stack)), false);		//Выход "вставка не понадобилась"
			}
			if (k < temporaryRootPointer->key) {
				if (temporaryRootPointer->down) { //Идём вниз и влево
					nodePointerP = temporaryRootPointer->next->next;
					temporaryRootPointer = temporaryRootPointer->down;
					if (nodePointerP) stack.push(std::make_pair(temporaryRootPointer, 4));
					else stack.push(std::make_pair(temporaryRootPointer, 2));
				}
				else { //Новый лист слева (вставка справа и перенос данных)
					nodePointerP = new Node(temporaryRootPointer->key);
					nodePointerP->next = temporaryRootPointer->next;
					temporaryRootPointer->key = k;
					if ((temporaryRootPointer->next) && (temporaryRootPointer->next->next)) up = 1; //Вставлен четвёртый
					temporaryRootPointer->next = nodePointerP;
					direction = 0;
				}
			}
			else if (!(temporaryRootPointer->next)) { //Добавление второго элемента
				nodePointerP = new Node(k);
				temporaryRootPointer->next = nodePointerP;
				direction = 0;
			}
			else if (k < temporaryRootPointer->next->key) {
				if (temporaryRootPointer->next->down) {//Идём вниз посередине
					nodePointerP = temporaryRootPointer->next->next;
					temporaryRootPointer = temporaryRootPointer->next->down;
					if (nodePointerP) stack.push(std::make_pair(temporaryRootPointer, 5));
					else stack.push(std::make_pair(temporaryRootPointer, 3));
				}
				else { // Новый лист посередине
					nodePointerP = new Node(k);
					nodePointerP->next = temporaryRootPointer->next;
					if (temporaryRootPointer->next->next) up = 1; //Вставлен четвёртый
					temporaryRootPointer->next = nodePointerP;
					direction = 0;
				}
			}
			else if (!temporaryRootPointer->next->next) { //Третьего пути нет;
				if (temporaryRootPointer->next->down) {     //Идём вниз посередине
					temporaryRootPointer->next->key = k; 	//Меняем наибольший
					temporaryRootPointer = temporaryRootPointer->next->down;
					stack.push(std::make_pair(temporaryRootPointer, 3));
				}
				else {                   // Новый лист справа
					nodePointerP = new Node(k);
					temporaryRootPointer->next->next = nodePointerP;
					direction = 0;
				}
			}
			else {
				if (temporaryRootPointer->next->next->down) { //Идём вниз и вправо
					if (k > temporaryRootPointer->next->next->key)
						temporaryRootPointer->next->next->key = k; //Меняем наибольший
					temporaryRootPointer = temporaryRootPointer->next->next->down;
					stack.push(std::make_pair(temporaryRootPointer, 6));
				}
				else {                     //Новый лист
					nodePointerP = new Node(k);
					direction = 0;
					if (temporaryRootPointer->next->next) {    //Третий уже есть...
						up = 1;
						if (k < temporaryRootPointer->next->next->key) { // - третий из четырёх
							nodePointerP->next = temporaryRootPointer->next->next;
							temporaryRootPointer->next->next = nodePointerP;
						}
						else {                  // - четвёртый
							temporaryRootPointer->next->next->next = nodePointerP;
						}
					}
					else { //Новый лист - третий из трёх
						temporaryRootPointer->next->next = nodePointerP;
						//	           stack->Top( )->key = k;
					}
				}
			}
		}
		while (up) { //Устранение четвёртого сына (если он есть)
			nodePointerP = stack.top().first;
			direction = stack.top().second;
			stack.pop();	//nodePointerP-> узел с четырьмя сыновьями
			if (direction != ROOT) temporaryRootPointer = stack.top().first;	//temporaryRootPointer-> управляющий узел для "nodePointerP->"
			nodePointerQ = new Node(nodePointerP->next->next->next->key);
			switch (direction) {
			case ROOT:	//Корневой узел дерева: root == nodePointerP == temporaryRootPointer
				up = 0;
				height++;	//Увеличение высоты дерева
				root = temporaryRootPointer = new Node(root->next->key); //Создание двойного управляющего узла
				temporaryRootPointer->down = nodePointerP;
				temporaryRootPointer->next = nodePointerQ;
				break;
			case 2:	//Новый средний сын для двух
				up = 0;
			case 4:     //Новый средний для трёх
				nodePointerQ->next = temporaryRootPointer->next; //вставка в цепочку
				temporaryRootPointer->next = nodePointerQ;
				temporaryRootPointer->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			case 3:	//Новый правый сын для двух
				up = 0;
			case 5:     //Новый правый для трёх
				nodePointerQ->next = temporaryRootPointer->next->next;
				temporaryRootPointer->next->next = nodePointerQ;		//Присоединение к цепочке
				temporaryRootPointer->next->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			case 6:	//Новый четвёртый сын
				temporaryRootPointer->next->next->next = nodePointerQ;		//Присоединение к цепочке
				temporaryRootPointer->next->next->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			}
			nodePointerQ->down = nodePointerP->next->next;
			nodePointerP->next->next = nullptr; //Расцепление сыновей
		}
		//delete stack; 	//Уничтожение стека
		return std::make_pair(ReadIterator(temporaryRootPointer, std::move(stack)), true);
	}
}
//***********************************************************************
int TwoThreeTree::erase(int k)   //Удаление (единственного) элемента из 2-3-дерева
{
	Node* temporaryRootPointer(root), * nodePointerP(nullptr);
	int direction = ROOT, up = 0, result = 0;
	if (temporaryRootPointer) {   //Дерево не пусто
		if (temporaryRootPointer->next) {
			if (temporaryRootPointer->next->next) {
				if (k > temporaryRootPointer->next->next->key) return result;
			}
			else
				if (k > temporaryRootPointer->next->key) return result;
		}
		else
			if (k > temporaryRootPointer->key) return result;
		// //k больше максимума, выход
		std::stack<std::pair<Node*, int>> stack;
		stack.push(std::make_pair(temporaryRootPointer, ROOT));   // Создание и инициализация стека
		while (1) {
			if (temporaryRootPointer->down) { //Узел — не лист, идём вниз
				if (k < temporaryRootPointer->key) { //Идём влево
					stack.push(std::make_pair(temporaryRootPointer, 1));
					temporaryRootPointer = temporaryRootPointer->down;
				}
				else if (!(temporaryRootPointer->next->next) ||
					k < temporaryRootPointer->next->next->key) { //Идём посередине
					stack.push(std::make_pair(temporaryRootPointer, 2));
					temporaryRootPointer = temporaryRootPointer->next->down;
				}
				else { //Идём вправо
					stack.push(std::make_pair(temporaryRootPointer, 3));
					temporaryRootPointer = temporaryRootPointer->next->next->down;
				}
			}
			else {         //Дошли до листа,
				if (k == temporaryRootPointer->key) {   // проверка на совпадение значений
					if (nodePointerP = temporaryRootPointer->next) {   //Лист хотя бы двойной
						temporaryRootPointer->key = temporaryRootPointer->next->key; //Подмена первого вторым
						temporaryRootPointer->next = temporaryRootPointer->next->next;
						if (temporaryRootPointer != root) {
							result = 2; //Корректировать минимум выше по дереву
							if (!temporaryRootPointer->next) up = 1; //Остался единственный
						}
						else result = 1; //Удаляется начало в корне
						--count;
						delete nodePointerP;   //Удаление второго
					}
					else {   //Удаление единственного листа в корне
						delete temporaryRootPointer;
						height = count = 0;
						root = nullptr;
						result = 1;
					}
				}
				else if ((nodePointerP = temporaryRootPointer->next) && (k == temporaryRootPointer->next->key)) {
					temporaryRootPointer->next = temporaryRootPointer->next->next; //Исключение второго
					result = 1;
					if (!temporaryRootPointer->next && temporaryRootPointer != root) up = 1;    //Остался единственный
					--count;
					delete nodePointerP;   //Удаление второго
				}
				else if ((nodePointerP = temporaryRootPointer->next->next) && (k == temporaryRootPointer->next->next->key)) {
					temporaryRootPointer->next->next = nullptr;
					result = 1;
					--count;
					delete nodePointerP;   //Удаление третьего
				}
				break;    //Конец поиска, элемент удалён или не найден
			}
		}
		while (up) { //Устранение единственного сына (если он получился)
			up = 0;
			nodePointerP = stack.top().first; direction = stack.top().second; //сын nodePointerP — одиночный узел temporaryRootPointer
			switch (direction) {
			case 1:   //Неполный узел слева: temporaryRootPointer == nodePointerP->down
				if (nodePointerP->key != temporaryRootPointer->key) nodePointerP->key = temporaryRootPointer->key, result = 2;
				else result = 1;
				if (nodePointerP->next->down->next->next) {   //Справа — тройной узел,
					temporaryRootPointer->next = nodePointerP->next->down;             // берём сына
					nodePointerP->next->down = temporaryRootPointer->next->next;
					temporaryRootPointer->next->next = nullptr;
					nodePointerP->next->key = nodePointerP->next->down->key;
				}
				else {   //Справа — двойной, присоединяем его сыновей
					temporaryRootPointer->next = nodePointerP->next->down;
					if (nodePointerP->next->next) { //УУ — группа из трёх
						temporaryRootPointer = nodePointerP->next;
						nodePointerP->next = nodePointerP->next->next;
						delete temporaryRootPointer; result = 1;
					}
					else if (nodePointerP == root) { //Двойной — в корне: удаляем оба
						root = temporaryRootPointer;
						--height;
						delete nodePointerP->next;
						delete nodePointerP;
						result = 1;
					}
					else { //Двойной УУ —  не в корне, удаляем второй и вверх
						delete nodePointerP->next;
						nodePointerP->next = nullptr;
						temporaryRootPointer = nodePointerP;
						up = 1;
						stack.pop();
					}
				}
				break;
			case 2:   //Неполный узел посередине: nodePointerP == temporaryRootPointer->next->down
				if (nodePointerP->down->next->next) {   //Слева — тройной узел,
					nodePointerP->down->next->next->next = temporaryRootPointer; // берём сына
					nodePointerP->next->down = nodePointerP->down->next->next;
					nodePointerP->down->next->next = nullptr;
					nodePointerP->next->key = nodePointerP->next->down->key;
				}
				else {   //Слева - двойной, отдаём ему сына, удаляем УУ
					nodePointerP->down->next->next = temporaryRootPointer;
					temporaryRootPointer = nodePointerP->next;
					nodePointerP->next = nodePointerP->next->next;
					delete temporaryRootPointer;
					if (!nodePointerP->next) { //УУ был двойной
						if (nodePointerP == root) { //УУ - корневой: удаляем и второй
							root = nodePointerP->down;
							--height;
							delete nodePointerP;
							result = 1;
						}
						else { // одиночный УУ - не корень: вверх
							up = 1;
							temporaryRootPointer = nodePointerP;
							stack.pop();
						}
					}
				}
				break;
			case 3:   //Неполный узел справа: nodePointerP == temporaryRootPointer->next->next->down
				if (nodePointerP->next->down->next->next) {   //Слева - тройной узел,
					nodePointerP->next->down->next->next->next = temporaryRootPointer; // берём сына
					nodePointerP->next->next->down = nodePointerP->next->down->next->next;
					nodePointerP->next->down->next->next = nullptr;
					nodePointerP->next->next->key = nodePointerP->next->next->down->key;
				}
				else {   //Слева — двойной, отдаём ему сына, удаляем
					nodePointerP->next->down->next->next = temporaryRootPointer;
					delete nodePointerP->next->next;
					nodePointerP->next->next = nullptr;
					result = 1;
				}
			}
		}
		if (result == 2) {   //Корректировка минимума
			do {
				nodePointerP = stack.top().first;
				switch (stack.top().second) {
				case 1:
					nodePointerP->key = nodePointerP->down->key;
					break;
				case 2:
					nodePointerP->next->key = nodePointerP->next->down->key;
					result = 1;
					break;
				case 3:
					nodePointerP->next->next->key = nodePointerP->next->next->down->key;
					result = 1;
				}
				if (stack.top().first == root) break;
				stack.pop();
			} while (result == 2);
		}
	}
	return result;
}
//***********************************************************************
TwoThreeTree :: ~TwoThreeTree()
{
	if (root) root->erase();
}
//***********************************************************************
//Вывод дерева на экран
void TwoThreeTree::display()
{
	clrscr();
	gotoxy(FIRSTCOL - OFFSET[0], FIRSTROW);
	col += sprintf_s(SCREEN + row * MAXCOL + col, 11, "Дерево %c:", name);
	if (root) root->display(0, FIRSTCOL - SHIFT);
	else sprintf_s(SCREEN + row * MAXCOL + col, 9, "<Пусто!>");
	showscr();
}
//***********************************************************************
//Создание 2-3 дерева по возрастающей последовательности
int TwoThreeTree::build(int k)	// Приём возрастающей последовательности
{
	Node* nodePointerQ;				// и преобразование её в 2-3-дерево
	static Node* nodePointerP;
	static int count = 0;

	if (k) { //Добавление в очередь
		nodePointerQ = new Node(k);
		if (root == nullptr) { //Первое обращение
			root = nodePointerP = nodePointerQ;
			count = 1;
		}
		else {
			nodePointerP->next = nodePointerQ;
			nodePointerP = nodePointerQ;
			count++;
		}
	}
	else if (count) { //Ввод окончен, строим дерево
		int rest = count;
		height = 1;
		while (rest > 3) {
			nodePointerQ = root;
			height++;
			if (rest == 4) { //Два управляющих узла
				root = new Node(nodePointerQ->next->key);
				nodePointerP = new Node(nodePointerQ->next->next->next->key);
				root->next = nodePointerP;
				root->down = nodePointerQ;
				nodePointerP->down = nodePointerQ->next->next;
				nodePointerQ->next->next = nullptr;
				rest = 0;
			}
			else { //Цепочка управляющих узлов
				int last = rest - 3;
				root = nodePointerP = new Node(nodePointerQ->next->next->key);
				nodePointerP->down = nodePointerQ;
				nodePointerQ = nodePointerQ->next->next->next;
				nodePointerP->down->next->next->next = nullptr;
				rest = 1;
				while (last) {
					rest++;
					switch (last) {
					case 2:
						nodePointerP->next = new Node(nodePointerQ->next->key);
						nodePointerP->next->down = nodePointerQ;
						last = 0;
						break;
					case 3:
						nodePointerP->next = new Node(nodePointerQ->next->next->key);
						nodePointerP->next->down = nodePointerQ;
						last = 0;
						break;
					case 4:
						nodePointerP->next = new Node(nodePointerQ->next->key);
						nodePointerP->next->down = nodePointerQ;
						nodePointerP->next->next = new Node(nodePointerQ->next->next->next->key);
						nodePointerP->next->next->down = nodePointerQ->next->next;
						nodePointerQ->next->next = nullptr;
						last = 0;
						rest++;
						break;
					default:
						nodePointerP->next = new Node(nodePointerQ->next->next->key);
						nodePointerP->next->down = nodePointerQ;
						nodePointerQ = nodePointerQ->next->next->next;
						nodePointerP = nodePointerP->next;
						nodePointerP->down->next->next->next = nullptr;
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
//Генерация 2-3 дерева
void TwoThreeTree::genSet()
{
	int power = (rand() % genPower);
	while (power--)
		insert(rand() % genMaxNum + 1, ReadIterator());
}
//***********************************************************************
//Поиск по ключу в 2-3 дереве
bool TwoThreeTree::find(int num) const
{
	Node* temporaryRootPointer = root;
	while (temporaryRootPointer) {
		if (num == temporaryRootPointer->getKey())
			return true;
		if (num > temporaryRootPointer->getKey() && temporaryRootPointer->next)
			temporaryRootPointer = temporaryRootPointer->next;
		else
			temporaryRootPointer = temporaryRootPointer->down;
	}
	return false;
}
//***********************************************************************
//Перегрузка операторов 2-3 дерева
const TwoThreeTree& TwoThreeTree::operator = (const TwoThreeTree& rightTreeOperand)
{
	ReadIterator rightTreeIterator(rightTreeOperand);
	if (rightTreeOperand.root) {
		while (rightTreeIterator.ptr != NULLNODE) {
			build(*rightTreeIterator);
			rightTreeIterator++;
		}
		build(0);
	}
	return *this;
}

const TwoThreeTree& TwoThreeTree::operator &(const TwoThreeTree& rightTreeOperand) const
{
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryRootPointer = false;
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE) {
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator)
			leftTreeIterator++;
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator)
			rightTreeIterator++;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator == *leftTreeIterator) {
			temp->build(*rightTreeIterator);
			if (!temporaryRootPointer) temporaryRootPointer = true;
			leftTreeIterator++;;
			rightTreeIterator++;
		}
	}
	if (temporaryRootPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator | (const TwoThreeTree& rightTreeOperand) const
{
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryRootPointer = false;
	while (leftTreeIterator.ptr != NULLNODE || rightTreeIterator.ptr != NULLNODE) {
		if (!temporaryRootPointer) temporaryRootPointer = true;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE)
			if (*leftTreeIterator == *rightTreeIterator) {
				temp->build(*leftTreeIterator);
				leftTreeIterator++;
				rightTreeIterator++;
			}
			else {
				while (leftTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator) {
					temp->build(*leftTreeIterator);
					leftTreeIterator++;
				}
				while (rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator) {
					temp->build(*rightTreeIterator);
					rightTreeIterator++;
				}
			}
		else
			if (leftTreeIterator.ptr != NULLNODE)
				while (leftTreeIterator.ptr != NULLNODE) {
					temp->build(*leftTreeIterator);
					leftTreeIterator++;
				}
			else
				while (rightTreeIterator.ptr != NULLNODE) {
					temp->build(*rightTreeIterator);
					rightTreeIterator++;
				}
	}
	if (temporaryRootPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator ^ (const TwoThreeTree& rightTreeOperand) const
{
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryRootPointer = false;
	while (leftTreeIterator.ptr != NULLNODE || rightTreeIterator.ptr != NULLNODE) {
		if (!temporaryRootPointer) temporaryRootPointer = true;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE)
			if (*leftTreeIterator == *rightTreeIterator) {
				leftTreeIterator++;
				rightTreeIterator++;
			}
			else {
				while (leftTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator) {
					temp->build(*leftTreeIterator);
					leftTreeIterator++;
				}
				while (rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator) {
					temp->build(*rightTreeIterator);
					rightTreeIterator++;
				}
			}
		else
			if (leftTreeIterator.ptr != NULLNODE)
				while (leftTreeIterator.ptr != NULLNODE) {
					temp->build(*leftTreeIterator);
					leftTreeIterator++;
				}
			else
				while (rightTreeIterator.ptr != NULLNODE) {
					temp->build(*rightTreeIterator);
					rightTreeIterator++;
				}
	}
	if (temporaryRootPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator /(const TwoThreeTree& rightTreeOperand) const
{
	int iteration;
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryRootPointer = false;
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator) {
		temp->build(*leftTreeIterator);
		leftTreeIterator++;
		temporaryRootPointer = true;
	}
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE) {
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator)
			rightTreeIterator++;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator != *leftTreeIterator) {
			temp->build(*leftTreeIterator);
			temporaryRootPointer = true;
		}
		if (rightTreeIterator.ptr != NULLNODE)
			leftTreeIterator++;
	}
	while (leftTreeIterator.ptr != NULLNODE) {
		temp->build(*leftTreeIterator);
		temporaryRootPointer = true;
		leftTreeIterator++;
	}
	if (temporaryRootPointer)
		temp->build(0);
	return *temp;
}

int menu()
{
	int point;
	do {
		std::cin.clear();
		std::cin.sync();
		std::cout << "Выберите пункт меню" << std::endl;
		std::cout << "1 - Сгенерировать дерево" << std::endl;
		std::cout << "0 - Выход" << std::endl;
		std::cout << ">";
		std::cin >> point;
		if (std::cin.fail())
			std::cout << "Что-то пошло не так, выберите пункт меню повторно" << std::endl;
	} while (std::cin.fail());
	return point;
}

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "RUS");
	int pause;
	do
	{
		TwoThreeTree A('A'), B('B'), C('C'), D('D'), E('E'), F('F');
		switch (pause = menu())
		{
		case 1:
			A.genSet(); B.genSet(); C.genSet(); D.genSet();
			//std::cout << "Сгенерированные множества" << std::endl;
			break;
		case 0:
			std::cout << "End" << std::endl;
			break;
		default:
			std::cout << "Такого пункта не существует, повторите ввод!" << std::endl;
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
			std::cout << "Результат E = (A^B-C) U D ∩ E))" << std::endl;
			E = A / B;
			F = A & B;
			E.display();
			F.display();
			/*ReadIterator readIterator(A);
			while (readIterator.ptr != NULLNODE) {
				std::std::cout << *readIterator << " ";
				readIterator++;
			}*/
			std::cout << "Для возврата в меню введите любое число, для выхода 0: ";
			std::cin >> pause;
		}
	} while (pause);
	return 0;
}