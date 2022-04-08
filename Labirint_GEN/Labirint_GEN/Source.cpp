#include <iostream>
#include "windows.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <fstream>

class Labyrinth {

	int** lab;
	int const_x, const_y;
	
	std::vector<int> points_x;//���������� ����������� ����� ��������� 
	std::vector<int> points_y;
	int counter;

	int end_x, end_y;//���������� ����� ������


public:
	Labyrinth(int y, int x) {
		//� ������������ ������������� ������������� ������� ��������� � ��� ��������
		const_x = x, const_y = y;
		lab = new int*[const_y];
		for (int i = 0; i < const_y; i++) {
			lab[i] = new int[const_x];
		}
		end_x = const_x - 2, end_y = const_y - 2;
	}

	//���������� ���������, ��� ������� � ���������� ���� ������
	void prepare() {
		for (int i = 0; i < const_y; i++) {
			for (int i1 = 0; i1 < const_x; i1++) { lab[i][i1] = 0; }
		}
		counter = 1;// ��������� �������
		lab[1][1] = 1;
		points_x.push_back(1);//��������� ��������� �����
		points_y.push_back(1);
	}

	//������� ����� ������ ����� �� �������� ������ x,y � ���������
	//�������� ���� ��� ��� ������ ��������� ������ ���������, ��� �� ��������� � ��� ������ ��� ��� ��� ��������� ������ ���������
	bool test(int y, int x) {
		if (x == end_x && y == end_y) { return true; }//����� � ����� ������ ���� ��������� �������, �������� �������
		else if (x<1 || y<1 || x>end_x || y>end_y || lab[y][x] == 1) { return false; }
		else if (lab[y - 1][x - 1] + lab[y - 1][x] + lab[y][x - 1] == 3) { return false; }
		else if (lab[y - 1][x] + lab[y - 1][x + 1] + lab[y][x + 1] == 3) { return false; }
		else if (lab[y + 1][x - 1] + lab[y + 1][x] + lab[y][x - 1] == 3) { return false; }
		else if (lab[y + 1][x] + lab[y + 1][x + 1] + lab[y][x + 1] == 3) { return false; }
		else { return true; }
	}

	//���������� ���� � ���������-������� lab
	void genWay() {
		//������� ������
		prepare();

		int rep = 0;//���������� ����������
		int point = rand() % counter;//����� ��������� ����� �� �������� points_x � points_y, ����� �� ��� ���������� ��������
		while (lab[end_y][end_x] != 1) {
			point = rand() % counter;//�������� ���������� ���������� ������, ������ ����� ������� ����

			switch (rand() % 4) //���������� ����������� ��������� �������
			{

			case 0: if (test(points_y[point] - 1, points_x[point]) && lab[points_y[point] - 2][points_x[point]] != 1) {

				lab[points_y[point] - 1][points_x[point]] = 1; 

				//�������� ��������� ��� ���� ������ ��� �������
				if (test(points_y[point] - 2, points_x[point])) {

					lab[points_y[point] - 2][points_x[point]] = 1;

					//��������� � ������ ������, �� ������� �������� �������� ����
					points_x.push_back(points_x[point]);
					points_y.push_back(points_y[point]-2);
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
					points_y.push_back(points_y[point]+2);
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

			//����� �������� ��� �����������
			testforDEADEND(point);
		}
	}


	//���� �� ������� ������� ���������� �� � ���� ������� ��������� ������, �� ��� ������� ��������� �� ������ points
	void testforDEADEND(int num) {
		if (!test(points_y[num] - 2, points_x[num]) && (!test(points_y[num] + 2, points_x[num])) && (!test(points_y[num], points_x[num] - 2)) && (!test(points_y[num], points_x[num] + 2))){
			near_finish(num);
			if (counter > 1) {
				points_x.erase(points_x.cbegin() + num);
				points_y.erase(points_y.cbegin() + num);
				counter--;
			}
		}
	}


	void near_finish(int num) {
		if (abs(points_y[num]-end_y)<=2 && abs(points_x[num]-end_x)<=2) {
			for (int i = end_y - 1; i <= end_y;i++) {
				for (int i1 = end_x - 1; i1 <= end_x; i1++) {
					lab[i][i1] = 1;
				}
			}
		}
	}



	void draw() {//��������� ���������
		std::ofstream f("lab.txt");
		for (int i = 0; i < const_y; i++) {
			for (int j = 0; j < const_x; j++) {
				if ( (i == end_y+1 && j == end_x)) { f << ' '; }//�����
				else if (i == 1 && j == 1) { f << 'N'; } //����
				else if (lab[i][j] == 0) { f <<'#'; }//�����
				//else { f << '~'; }//��� (�������)
				else { f << ' '; }//���� ��� ��� ������� ������� ��������
			}
			f << std::endl;
		}
		f.close();
	}

};

int main()
{
	srand(time(0));
	setlocale(0, "");
	int x, y;
	std::cout << "������� ������� ��������� *������* � *������* (���������� 3 �� 3): ";
	std::cin >> y >> x;
	Labyrinth lab(y,x);
	lab.genWay();
	system("cls");
	lab.draw();
}