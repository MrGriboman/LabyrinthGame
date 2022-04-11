void money(int counter) {
	srand(time(0));
	while (counter > 0) {
		int pointer_1 = rand() % size(points_x) + 1;
		int pointer_2 = rand() % size(points_y) + 1;
		if (lab[pointer_1][pointer_2] != '*') {
			lab[pointer_1][pointer_2] = '*';
			counter--;
		}
	}
}

void draw() {//отрисовка лабиринта
	std::ofstream f("lab.txt");
	for (int i = 0; i < const_y; i++) {
		for (int j = 0; j < const_x; j++) {
			if ((i == end_y + 1 && j == end_x)) { f << ' '; }
			else if (i == 1 && j == 1) { f << 'N'; } 
			else if (lab[i][j] == 0) { f << '#'; }
			else if (lab[i][j] == '*') { f << '*'; }
			else { f << ' '; }
		}
		f << std::endl;
	}
	f.close();
}