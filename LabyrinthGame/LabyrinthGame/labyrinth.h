#pragma once
#include <iostream>
#include "windows.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <string>
#include <fstream>

class Labyrinth {

	int** lab;
	int const_x, const_y;

	std::vector<int> points_x;//êîîðäèíàòû âûðóáëåííûõ òî÷åê ëàáèðèíòà 
	std::vector<int> points_y;
	int counter;

	int end_x, end_y;//êîîðäèíàòû òî÷êè âûõîäà


public:
	Labyrinth(int y, int x) {
		//Â êîíñòðóêòîðå èíèöèàëèçàöèÿ äèíàìè÷åñêîãî ìàññèâà ëàáèðèíòà è åãî ðàçìåðîâ
		const_x = x, const_y = y;
		lab = new int* [const_y];
		for (int i = 0; i < const_y; i++) {
			lab[i] = new int[const_x];
		}
		end_x = const_x - 2, end_y = const_y - 2;
	}

	//ïîäãîòîâêà ëàáèðèíòà, åãî î÷èñòêà è çàïîëíåíèå ïîëÿ íóëÿìè
	void prepare() {
		for (int i = 0; i < const_y; i++) {
			for (int i1 = 0; i1 < const_x; i1++) { lab[i][i1] = 0; }
		}
		counter = 1;// íà÷àëüíàÿ âåðøèíà
		lab[1][1] = 1;
		points_x.push_back(1);//óñòàíîâêà íà÷àëüíîé òî÷êè
		points_y.push_back(1);
	}

	//ôóíêöèÿ ÷òîáû óçíàòü ìîæíî ëè âûðóáèòü êëåòêó x,y â ëàáèðèíòå
	//ïðîâåðêà òîãî ÷òî ýòà êëåòêà íàõîäèòñÿ âíóòðè ëàáèðèíòà, åùå íå âûðóáëåíà è ÷òî âîêðóã íåå íåò óæå îùèùåííûõ êëåòîê ëàáèðèíòà
	bool test(int y, int x) {
		if (x == end_x && y == end_y) { return true; }//÷òîáû â êîíöå âñåãäà áûëî ðàçðåøåíî ñòðîèòü, èçáåãàåì òóïèêîâ
		else if (x<1 || y<1 || x>end_x || y>end_y || lab[y][x] == 1) { return false; }
		else if (lab[y - 1][x - 1] + lab[y - 1][x] + lab[y][x - 1] == 3) { return false; }
		else if (lab[y - 1][x] + lab[y - 1][x + 1] + lab[y][x + 1] == 3) { return false; }
		else if (lab[y + 1][x - 1] + lab[y + 1][x] + lab[y][x - 1] == 3) { return false; }
		else if (lab[y + 1][x] + lab[y + 1][x + 1] + lab[y][x + 1] == 3) { return false; }
		else { return true; }
	}

	//ïðîðóáàíèå ïóòè â ëàáèðèíòå-ìàññèâå lab
	void genWay() {
		//ñíà÷àëà ÷èñòèì
		prepare();

		int rep = 0;//êîëè÷åñòâî ïîâòîðåíèé
		int point = rand() % counter;//âûáîð ñëó÷àéíîé òî÷êè èç ìàññèâîâ points_x è points_y, ÷òîáû îò íåå ïðîäîëæèòü ëàáèðèíò
		while (lab[end_y][end_x] != 1) {
			point = rand() % counter;//ñëó÷àéíî ãåíèðèðóåì êîîðäèíàòû êëåòêè, îòêóäà áóäåì ñòðîèòü ïóòü

			switch (rand() % 4) //ãåíåðèðóåì íàïðàâëåíèå ïîñòðîéêè ïðîõîäà
			{

			case 0: if (test(points_y[point] - 1, points_x[point]) && lab[points_y[point] - 2][points_x[point]] != 1) {

				lab[points_y[point] - 1][points_x[point]] = 1;

				//ïûòàåìñÿ ïîñòàâèòü åù¸ îäíó êëåòêó äëÿ ïðîõîäà
				if (test(points_y[point] - 2, points_x[point])) {

					lab[points_y[point] - 2][points_x[point]] = 1;

					//äîáàâëÿåì â ñïèñîê êëåòîê, èç êîòîðûõ ðàíäîìíî ñòðîÿòñÿ ïóòè
					points_x.push_back(points_x[point]);
					points_y.push_back(points_y[point] - 2);
					counter++;
				}
			}	break;

			case 1: if (test(points_y[point], points_x[point] + 1) && lab[points_y[point]][points_x[point] + 2] != 1) {

				lab[points_y[point]][points_x[point] + 1] = 1;

				if (test(points_y[point], points_x[point] + 2)) {

					lab[points_y[point]][points_x[point] + 2] = 1;

					points_x.push_back(points_x[point] + 2);
					points_y.push_back(points_y[point]);
					counter++;
				}
			}	break;

			case 2: if (test(points_y[point] + 1, points_x[point]) && lab[points_y[point] + 2][points_x[point]] != 1) {

				lab[points_y[point] + 1][points_x[point]] = 1;

				if (test(points_y[point] + 2, points_x[point])) {

					lab[points_y[point] + 2][points_x[point]] = 1;

					points_x.push_back(points_x[point]);
					points_y.push_back(points_y[point] + 2);
					counter++;
				}
			}	break;
			case 3: if (test(points_y[point], points_x[point] - 1) && lab[points_y[point]][points_x[point] - 2] != 1) {

				lab[points_y[point]][points_x[point] - 1] = 1;

				if (test(points_y[point], points_x[point] - 2)) {

					lab[points_y[point]][points_x[point] - 2] = 1;

					points_x.push_back(points_x[point] - 2);
					points_y.push_back(points_y[point]);
					counter++;
				}
			}	break;

			default:
				break;
			}

			//âûçîâ ôóíêöèèè äëÿ îïòèìèçàöèè
			testforDEADEND(point);
		}
	}


	//åñëè èç òåêóùåé âåðøèíû íåâîçìîæíî íè â îäíó ñòîðîíó ïðîðóáèòü ïðîõîä, òî ýòà âåðøèíà óäàëÿåòñÿ èç ñïèñêà points
	void testforDEADEND(int num) {
		if (!test(points_y[num] - 2, points_x[num]) && (!test(points_y[num] + 2, points_x[num])) && (!test(points_y[num], points_x[num] - 2)) && (!test(points_y[num], points_x[num] + 2))) {
			near_finish(num);
			if (counter > 1) {
				points_x.erase(points_x.cbegin() + num);
				points_y.erase(points_y.cbegin() + num);
				counter--;
			}
		}
	}


	void near_finish(int num) {
		if (abs(points_y[num] - end_y) <= 2 && abs(points_x[num] - end_x) <= 2) {
			for (int i = end_y - 1; i <= end_y; i++) {
				for (int i1 = end_x - 1; i1 <= end_x; i1++) {
					lab[i][i1] = 1;
				}
			}
		}
	}



	std::wstring draw() {//îòðèñîâêà ëàáèðèíòà
		std::wstring maze = L"";
		for (int i = 0; i < const_y; i++) {
			for (int j = 0; j < const_x; j++) {
				if ((i == end_y + 1 && j == end_x)) { maze += L'.'; }//âûõîä
				else if (i == 1 && j == 1) { maze += L'N'; } //âõîä
				else if (lab[i][j] == 0) { maze += L'#'; }//ñòåíû
				//else { f << '~'; }//ïîë (ïðîõîäû)
				else { maze += '.'; }//ïîêà ÷òî äëÿ ëó÷øåãî âèäåíèÿ êàðòèíêè
			}
			maze += '\n';
		}
		return maze;
	}

};

