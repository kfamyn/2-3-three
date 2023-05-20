#include <iostream>
#include <locale>
#include <stdio.h>
#include <conio.h>
#include <stack>
#include <vector>
#include "TwoThreeTree.h"

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
int row = 0, column = 0;
//***********************************************************************
//Функции работы с экраном
void goToPixel(int toRow, int toColumn) {
	row = toRow;
	column = toColumn;
}

void clearPixelBuffer()
{
	for (auto& x : SCREEN) x = '.';
}

void showPixelBuffer()
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
void Node::display(int lvl, int column)
{
	int row = FIRSTROW + lvl * 4;
	this->out(row, column);
	if (down) {
		down->display(lvl + 1, column - (OFFSET[lvl + 1]));
		next->down->display(lvl + 1, column);
		if ((next->next) && (next->next->down))
			next->next->down->display(lvl + 1, column + (OFFSET[lvl + 1]));
	}
}
//***********************************************************************
//Вывод узла в массив screen в точку (x, y)
void Node::out(int row, int column)
{
	if ((row > MAXROW) || (column < 1) || (column > MAXCOL)) return;
	goToPixel(column, row);
	if (row > MAXOUT) {
		sprintf_s(SCREEN + row * MAXCOL + column, 4, "..."), column += 3;
		return;
	}
	sprintf_s(SCREEN + row * MAXCOL + column, 4, "%1d ", key);
	if (next) {
		sprintf_s(SCREEN + (row + 1) * MAXCOL + column + 1, 4, "%1d ", next->key);
		if (next->next)
			sprintf_s(SCREEN + (row + 2) * MAXCOL + column + 2, 4, "%1d ", next->next->key);
		else sprintf_s(SCREEN + (row + 2) * MAXCOL + column + 2, 4, "@  ");
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
}
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
	Node* temporaryPointer, * nodePointerP, * nodePointerQ;
	int direction = ROOT, up = 0;
	std::stack<std::pair<Node*, int>> stack;
	//===== Инициализация =====
	temporaryPointer = root;
	if (temporaryPointer == nullptr) {	// Дерево пусто
		root = new Node(k);
		height = 1;
		return std::make_pair(ReadIterator(root, std::move(stack)), true);
	}
	else {		//Поиск по дереву
		stack.push(std::make_pair(root, ROOT));	// Создание и инициализация стека
										//===== Поиск места вставки =====
		while (direction) {
			if ((k == temporaryPointer->key) || //Проверка на совпадение значений
				(temporaryPointer->next) && (k == temporaryPointer->next->key) ||
				(temporaryPointer->next) && (temporaryPointer->next->next) && (k == temporaryPointer->next->next->key)) { //Элемент имеется
				return std::make_pair(ReadIterator(temporaryPointer, std::move(stack)), false);		//Выход "вставка не понадобилась"
			}
			if (k < temporaryPointer->key) {
				if (temporaryPointer->down) { //Идём вниз и влево
					nodePointerP = temporaryPointer->next->next;
					temporaryPointer = temporaryPointer->down;
					if (nodePointerP) stack.push(std::make_pair(temporaryPointer, 4));
					else stack.push(std::make_pair(temporaryPointer, 2));
				}
				else { //Новый лист слева (вставка справа и перенос данных)
					nodePointerP = new Node(temporaryPointer->key);
					nodePointerP->next = temporaryPointer->next;
					temporaryPointer->key = k;
					if ((temporaryPointer->next) && (temporaryPointer->next->next)) up = 1; //Вставлен четвёртый
					temporaryPointer->next = nodePointerP;
					direction = 0;
				}
			}
			else if (!(temporaryPointer->next)) { //Добавление второго элемента
				nodePointerP = new Node(k);
				temporaryPointer->next = nodePointerP;
				direction = 0;
			}
			else if (k < temporaryPointer->next->key) {
				if (temporaryPointer->next->down) {//Идём вниз посередине
					nodePointerP = temporaryPointer->next->next;
					temporaryPointer = temporaryPointer->next->down;
					if (nodePointerP) stack.push(std::make_pair(temporaryPointer, 5));
					else stack.push(std::make_pair(temporaryPointer, 3));
				}
				else { // Новый лист посередине
					nodePointerP = new Node(k);
					nodePointerP->next = temporaryPointer->next;
					if (temporaryPointer->next->next) up = 1; //Вставлен четвёртый
					temporaryPointer->next = nodePointerP;
					direction = 0;
				}
			}
			else if (!temporaryPointer->next->next) { //Третьего пути нет;
				if (temporaryPointer->next->down) {     //Идём вниз посередине
					temporaryPointer->next->key = k; 	//Меняем наибольший
					temporaryPointer = temporaryPointer->next->down;
					stack.push(std::make_pair(temporaryPointer, 3));
				}
				else {                   // Новый лист справа
					nodePointerP = new Node(k);
					temporaryPointer->next->next = nodePointerP;
					direction = 0;
				}
			}
			else {
				if (temporaryPointer->next->next->down) { //Идём вниз и вправо
					if (k > temporaryPointer->next->next->key)
						temporaryPointer->next->next->key = k; //Меняем наибольший
					temporaryPointer = temporaryPointer->next->next->down;
					stack.push(std::make_pair(temporaryPointer, 6));
				}
				else {                     //Новый лист
					nodePointerP = new Node(k);
					direction = 0;
					if (temporaryPointer->next->next) {    //Третий уже есть...
						up = 1;
						if (k < temporaryPointer->next->next->key) { // - третий из четырёх
							nodePointerP->next = temporaryPointer->next->next;
							temporaryPointer->next->next = nodePointerP;
						}
						else {                  // - четвёртый
							temporaryPointer->next->next->next = nodePointerP;
						}
					}
					else { //Новый лист - третий из трёх
						temporaryPointer->next->next = nodePointerP;
						//	           stack->Top( )->key = k;
					}
				}
			}
		}
		while (up) { //Устранение четвёртого сына (если он есть)
			nodePointerP = stack.top().first;
			direction = stack.top().second;
			stack.pop();	//nodePointerP-> узел с четырьмя сыновьями
			if (direction != ROOT) temporaryPointer = stack.top().first;	//temporaryPointer-> управляющий узел для "nodePointerP->"
			nodePointerQ = new Node(nodePointerP->next->next->next->key);
			switch (direction) {
			case ROOT:	//Корневой узел дерева: root == nodePointerP == temporaryPointer
				up = 0;
				height++;	//Увеличение высоты дерева
				root = temporaryPointer = new Node(root->next->key); //Создание двойного управляющего узла
				temporaryPointer->down = nodePointerP;
				temporaryPointer->next = nodePointerQ;
				break;
			case 2:	//Новый средний сын для двух
				up = 0;
			case 4:     //Новый средний для трёх
				nodePointerQ->next = temporaryPointer->next; //вставка в цепочку
				temporaryPointer->next = nodePointerQ;
				temporaryPointer->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			case 3:	//Новый правый сын для двух
				up = 0;
			case 5:     //Новый правый для трёх
				nodePointerQ->next = temporaryPointer->next->next;
				temporaryPointer->next->next = nodePointerQ;		//Присоединение к цепочке
				temporaryPointer->next->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			case 6:	//Новый четвёртый сын
				temporaryPointer->next->next->next = nodePointerQ;		//Присоединение к цепочке
				temporaryPointer->next->next->key = nodePointerP->next->key;	//Коррекция ссылочного значения
				break;
			}
			nodePointerQ->down = nodePointerP->next->next;
			nodePointerP->next->next = nullptr; //Расцепление сыновей
		}
		return std::make_pair(ReadIterator(temporaryPointer, std::move(stack)), true);
	}
}
//***********************************************************************
int TwoThreeTree::erase(int k)   //Удаление (единственного) элемента из 2-3-дерева
{
	Node* temporaryPointer(root), * nodePointerP(nullptr);
	int direction = ROOT, up = 0, result = 0;
	if (temporaryPointer) {   //Дерево не пусто
		if (temporaryPointer->next) {
			if (temporaryPointer->next->next) {
				if (k > temporaryPointer->next->next->key) return result;
			}
			else
				if (k > temporaryPointer->next->key) return result;
		}
		else
			if (k > temporaryPointer->key) return result;
		// //k больше максимума, выход
		std::stack<std::pair<Node*, int>> stack;
		stack.push(std::make_pair(temporaryPointer, ROOT));   // Создание и инициализация стека
		while (1) {
			if (temporaryPointer->down) { //Узел — не лист, идём вниз
				if (k < temporaryPointer->key) { //Идём влево
					stack.push(std::make_pair(temporaryPointer, 1));
					temporaryPointer = temporaryPointer->down;
				}
				else if (!(temporaryPointer->next->next) ||
					k < temporaryPointer->next->next->key) { //Идём посередине
					stack.push(std::make_pair(temporaryPointer, 2));
					temporaryPointer = temporaryPointer->next->down;
				}
				else { //Идём вправо
					stack.push(std::make_pair(temporaryPointer, 3));
					temporaryPointer = temporaryPointer->next->next->down;
				}
			}
			else {         //Дошли до листа,
				if (k == temporaryPointer->key) {   // проверка на совпадение значений
					if (nodePointerP = temporaryPointer->next) {   //Лист хотя бы двойной
						temporaryPointer->key = temporaryPointer->next->key; //Подмена первого вторым
						temporaryPointer->next = temporaryPointer->next->next;
						if (temporaryPointer != root) {
							result = 1; //Корректировать минимум выше по дереву
							if (!temporaryPointer->next) up = 1; //Остался единственный
						}
						else result = 2; //Удаляется начало в корне
						--count;
						delete nodePointerP;   //Удаление второго
					}
					else {   //Удаление единственного листа в корне
						delete temporaryPointer;
						height = count = 0;
						root = nullptr;
						result = 2;
					}
				}
				else if ((nodePointerP = temporaryPointer->next) && (k == temporaryPointer->next->key)) {
					temporaryPointer->next = temporaryPointer->next->next; //Исключение второго
					result = 1;
					if (!temporaryPointer->next && temporaryPointer != root) up = 1;    //Остался единственный
					--count;
					delete nodePointerP;   //Удаление второго
				}
				else if ((nodePointerP = temporaryPointer->next->next) && (k == temporaryPointer->next->next->key)) {
					temporaryPointer->next->next = nullptr;
					result = 2;
					--count;
					delete nodePointerP;   //Удаление третьего
				}
				break;    //Конец поиска, элемент удалён или не найден
			}
		}
		while (up) { //Устранение единственного сына (если он получился)
			up = 0;
			nodePointerP = stack.top().first;
			direction = stack.top().second; //сын nodePointerP — одиночный узел temporaryPointer
			switch (direction) {
			case 1:   //Неполный узел слева: temporaryPointer == nodePointerP->down
				if (nodePointerP->key != temporaryPointer->key) nodePointerP->key = temporaryPointer->key, result = 2;
				else result = 2;
				if (nodePointerP->next->down->next->next) {   //Справа — тройной узел,
					temporaryPointer->next = nodePointerP->next->down;             // берём сына
					nodePointerP->next->down = temporaryPointer->next->next;
					temporaryPointer->next->next = nullptr;
					nodePointerP->next->key = nodePointerP->next->down->key;
				}
				else {   //Справа — двойной, присоединяем его сыновей
					temporaryPointer->next = nodePointerP->next->down;
					if (nodePointerP->next->next) { //УУ — группа из трёх
						temporaryPointer = nodePointerP->next;
						nodePointerP->next = nodePointerP->next->next;
						delete temporaryPointer; result = 2;
					}
					else if (nodePointerP == root) { //Двойной — в корне: удаляем оба
						root = temporaryPointer;
						--height;
						delete nodePointerP->next;
						delete nodePointerP;
						result = 2;
					}
					else { //Двойной УУ —  не в корне, удаляем второй и вверх
						delete nodePointerP->next;
						nodePointerP->next = nullptr;
						temporaryPointer = nodePointerP;
						up = 1;
						stack.pop();
					}
				}
				break;
			case 2:   //Неполный узел посередине: nodePointerP == temporaryPointer->next->down
				if (nodePointerP->down->next->next) {   //Слева — тройной узел,
					nodePointerP->down->next->next->next = temporaryPointer; // берём сына
					nodePointerP->next->down = nodePointerP->down->next->next;
					nodePointerP->down->next->next = nullptr;
					nodePointerP->next->key = nodePointerP->next->down->key;
				}
				else {   //Слева - двойной, отдаём ему сына, удаляем УУ
					nodePointerP->down->next->next = temporaryPointer;
					temporaryPointer = nodePointerP->next;
					nodePointerP->next = nodePointerP->next->next;
					delete temporaryPointer;
					if (!nodePointerP->next) { //УУ был двойной
						if (nodePointerP == root) { //УУ - корневой: удаляем и второй
							root = nodePointerP->down;
							--height;
							delete nodePointerP;
							result = 2;
						}
						else { // одиночный УУ - не корень: вверх
							up = 1;
							temporaryPointer = nodePointerP;
							stack.pop();
						}
					}
				}
				break;
			case 3:   //Неполный узел справа: nodePointerP == temporaryPointer->next->next->down
				if (nodePointerP->next->down->next->next) {   //Слева - тройной узел,
					nodePointerP->next->down->next->next->next = temporaryPointer; // берём сына
					nodePointerP->next->next->down = nodePointerP->next->down->next->next;
					nodePointerP->next->down->next->next = nullptr;
					nodePointerP->next->next->key = nodePointerP->next->next->down->key;
				}
				else {   //Слева — двойной, отдаём ему сына, удаляем
					nodePointerP->next->down->next->next = temporaryPointer;
					delete nodePointerP->next->next;
					nodePointerP->next->next = nullptr;
					result = 1;
				}
			}
		}
		if (result == 2) {   //Корректировка максимума
			do {
				nodePointerP = stack.top().first;
				switch (stack.top().second) {
				case 1:
					if (nodePointerP->down->next) {
						if (nodePointerP->down->next->next) {
							nodePointerP->key = nodePointerP->down->next->next->key;
						}
						else
							nodePointerP->key = nodePointerP->down->next->key;
					}
					else
						nodePointerP->key = nodePointerP->down->key;
					break;
				case 2:
					if (nodePointerP->next->down->next) {
						if (nodePointerP->next->down->next->next) {
							nodePointerP->next->key = nodePointerP->next->down->next->next->key;
						}
						else
							nodePointerP->next->key = nodePointerP->next->down->next->key;
					}
					else
						nodePointerP->next->key = nodePointerP->next->down->key;
					result = 1;
					break;
				case 3:
					if (nodePointerP->next->next->down->next) {
						if (nodePointerP->next->next->down->next->next) {
							nodePointerP->next->next->key = nodePointerP->next->next->down->next->next->key;
						}
						else
							nodePointerP->next->next->key = nodePointerP->next->next->down->next->key;
					}
					else
						nodePointerP->next->next->next->key = nodePointerP->next->next->next->down->key;
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
void TwoThreeTree::display() const
{
	clearPixelBuffer();
	goToPixel(FIRSTCOL - OFFSET[0], FIRSTROW);
	column += sprintf_s(SCREEN + row * MAXCOL + column, 11, "Tree %c:", name);
	if (root) root->display(0, FIRSTCOL - SHIFT);
	else sprintf_s(SCREEN + row * MAXCOL + column, 9, "<Empty!>");
	showPixelBuffer();
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
	Node* temporaryPointer = root;
	while (temporaryPointer) {
		if (num == temporaryPointer->getKey())
			return true;
		if (num > temporaryPointer->getKey() && temporaryPointer->next)
			temporaryPointer = temporaryPointer->next;
		else
			temporaryPointer = temporaryPointer->down;
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
	bool temporaryPointer = false;
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE) {
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator)
			leftTreeIterator++;
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator)
			rightTreeIterator++;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator == *leftTreeIterator) {
			temp->build(*rightTreeIterator);
			if (!temporaryPointer) temporaryPointer = true;
			leftTreeIterator++;;
			rightTreeIterator++;
		}
	}
	if (temporaryPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator | (const TwoThreeTree& rightTreeOperand) const
{
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryPointer = false;
	while (leftTreeIterator.ptr != NULLNODE || rightTreeIterator.ptr != NULLNODE) {
		if (!temporaryPointer) temporaryPointer = true;
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
	if (temporaryPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator ^ (const TwoThreeTree& rightTreeOperand) const
{
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryPointer = false;
	while (leftTreeIterator.ptr != NULLNODE || rightTreeIterator.ptr != NULLNODE) {
		if (!temporaryPointer) temporaryPointer = true;
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
	if (temporaryPointer)
		temp->build(0);
	return *temp;
}

const TwoThreeTree& TwoThreeTree::operator /(const TwoThreeTree& rightTreeOperand) const
{
	int iteration;
	TwoThreeTree* temp = new TwoThreeTree;
	ReadIterator leftTreeIterator(*this);
	ReadIterator rightTreeIterator(rightTreeOperand);
	bool temporaryPointer = false;
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *leftTreeIterator < *rightTreeIterator) {
		temp->build(*leftTreeIterator);
		leftTreeIterator++;
		temporaryPointer = true;
	}
	while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE) {
		while (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator < *leftTreeIterator)
			rightTreeIterator++;
		if (leftTreeIterator.ptr != NULLNODE && rightTreeIterator.ptr != NULLNODE && *rightTreeIterator != *leftTreeIterator) {
			temp->build(*leftTreeIterator);
			temporaryPointer = true;
		}
		if (rightTreeIterator.ptr != NULLNODE)
			leftTreeIterator++;
	}
	while (leftTreeIterator.ptr != NULLNODE) {
		temp->build(*leftTreeIterator);
		temporaryPointer = true;
		leftTreeIterator++;
	}
	if (temporaryPointer)
		temp->build(0);
	return *temp;
}

ReadIterator TwoThreeTree::begin() {
	return ReadIterator(*this);
}

Sequence::Sequence(std::initializer_list<int> valuesList) {
	ReadIterator readIterator = keysTree.begin();
	InsertIterator<TwoThreeTree> insertIterator = inserter(keysTree, readIterator);
	for (int value : valuesList) {
		insertIterator = value;
		values.push_back(value);
	}
}

Sequence& Sequence::merge(const Sequence& rightOperand) {
	ReadIterator readIterator = keysTree.begin();
	InsertIterator<TwoThreeTree> insertIterator = inserter(keysTree, readIterator);
	for (int rightSequenceValues : rightOperand.values) {
		insertIterator = rightSequenceValues;
		values.push_back(rightSequenceValues);
	}
	return *this;
}

Sequence& Sequence::substitute(const Sequence& rightOperand, int fromPosition) {
	ReadIterator readIterator = keysTree.begin();
	InsertIterator<TwoThreeTree> insertIterator = inserter(keysTree, readIterator);
	for (size_t i = fromPosition; i < rightOperand.values.size(); ++i) {
		insertIterator = rightOperand.values[i];
		values.push_back(rightOperand.values[i]);
	}
	return *this;
}

Sequence& Sequence::erase(int fromPosition, int toPosition) {
	for (size_t index = fromPosition; index < toPosition; index++)
	{
		keysTree.erase(values[fromPosition]);
		values.erase(values.begin() + fromPosition);
	}
	return *this;
}

void Sequence::display() {
	keysTree.display();
	std::cout << "Initial sequence: { ";
	for (int value : values) {
		std::cout << value << " ";
	}
	std::cout << "}" << std::endl;
}

int menu()
{
	int point;
	do {
		std::cin.clear();
		std::cin.sync();
		std::cout << "Menu: " << std::endl;
		std::cout << "1 - Generate trees and perform operations" << std::endl;
		std::cout << "0 - Exit" << std::endl;
		std::cout << ">";
		std::cin >> point;
		if (std::cin.fail())
			std::cout << "Something went wrong, please, try again: " << std::endl;
	} while (std::cin.fail());
	return point;
}

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "RUS");
	int pause = 0;
	do
	{
		TwoThreeTree A('A'), B('B'), C('C'), D('D'), E('E'), F('F');
		switch (pause = menu())
		{
		case 1:
			A.genSet(); B.genSet(); C.genSet(); D.genSet(); E.genSet();
			//std::cout << "Generated sets: " << std::endl;
			break;
		case 0:
			std::cout << "End" << std::endl;
			break;
		default:
			std::cout << "Please, enter correct number: " << std::endl;
		}
		if (pause == 1)
		{
			system("cls");
			/*Sequence T = {2, 4, 5, 4, 7, 8, 10};
			T.display();
			T.erase(0, 2);
			std::cout << "Sequence T after erase method: \n";
			T.display();
			Sequence D = { 1, 15, 6 };
			T.merge(D);
			std::cout << "Sequence T after merging with D: \n";
			T.display();*/
			A.display();
			B.display();
			C.display();
			D.display();
			E.display();
			std::cout << "(A ^ B)\n";
			(A ^ B).display();
			std::cout << "((A ^ B) / C)\n";
			((A ^ B) / C).display();
			std::cout << "(((A ^ B) / C) & D)\n";
			(((A ^ B) / C) & D).display();
			std::cout << "((((A ^ B) / C) & D) | E)\n";
			((((A ^ B) / C) & D) | E).display();
			std::cout << "Enter 0 to exit, any number to continue: ";
			std::cin >> pause;
		}
	} while (pause);
	return 0;
}
