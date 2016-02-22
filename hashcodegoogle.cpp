#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define F first
#define S second

using namespace std;

typedef tuple<int, char, int, int, int> topl;

vector<topl> answers;
//int totalt[111111];

void print(topl a) {
	cout 
	<< get<0> (a) << ' ' << get<1> (a) << ' ' 
	<< get<2> (a) << ' ' << get<3> (a) << ' ' 
	<< get<4> (a) << '\n';
}

int sqr(int a) {
	return a * a;
}

int dist(int x1, int y1, int x2, int y2) {
	return (int) ceil(sqr(x1 - x2) + sqr(x2 - y2));
}

int P, W, D, C, ROWS, COLS, capacity;
int deadline;


class Drone {
	public:
	int myid;
	int x, y;
	int capacity;
	map<int, int> has;
	vector<int> orders;
	Drone() {}
	Drone(int xx, int yy, int cap, int id) {
		x = xx;
		y = yy;
		capacity = cap;
		myid = id;
	}
	void add_order(int i) {
		orders.push_back(i);
	}
	void execute();
	vector<int> find_bros(map<int, int>&);
	void doitbro(int, int);
	void trytodobro(int);
};

class Warehouse {
	public:
	int * has;
	int x, y;
	Warehouse() {}
	Warehouse(int xx, int yy, int * hhas) {
		has = new int[P];
		x = xx;
		y = yy;
		for(int i = 0; i < P; ++i) {
			has[i] = hhas[i];
		}
	} 
};

class Order {
public:
	int x, y;
	map<int, int> wants;
	Order() {}
	Order(int xx, int yy, vector<int> ww) {
		x = xx;
		y = yy;
		for(int i : ww) {
			wants[i]++;
		}
	}
};

class Item {
public:
	int weight;
	Item() {}
	Item(int w) {
		weight = w;
	}
};

#define MAXN 10000

Order o[MAXN];
Warehouse w[MAXN];
Drone d[MAXN];
Item p[MAXN];

void solve_bratan() {
	srand(time(NULL));
	//srand(0xDEADBEEF);
	int * per = new int[C];
	for(int i = 0; i < C; ++i) {
		per[i] = i;
	}
	random_shuffle(per, per + C);
	for(int _i = 0; _i < C; ++_i) {
		int i = per[_i];
		//d[rand() % D].add_order(per[i]);
		vector<int> bitches;
		for(int j = 0; j < D; ++j) {
			bitches.push_back(j);
		}
		int closes = 1e9;
		int id = -1;
		int toto = 0;
		for(int j = 0; j < D; ++j) {
			if(d[j].orders.size() > 0) {
				toto++;
				for(auto k : d[j].orders) {
					if(id == -1 || dist(o[i].x, o[i].y, o[k].x, o[k].y) < closes) {
						closes = dist(o[i].x, o[i].y, o[k].x, o[k].y);
						id = j;
					}
				}
			}
		}
		for(int i = 0; i < D / 10/*D / 10*/; ++i) {
			bitches.push_back(id);
		}
		sort(bitches.begin(), bitches.end());
		d[bitches[rand() % (int)(bitches.size())]].add_order(i);
	}

	for(int i = 0; i < D; ++i) {
		d[i].execute();
	}

}

void Drone::doitbro(int from, int to) {
	int bag = 0;
	vector<int> need;
	for(auto it = o[to].wants.begin(); it != o[to].wants.end(); ++it) {
		int x = it -> second;
		while(x--) need.push_back(it -> first);
	}

	for(int i = 0; i < need.size(); ++i) {
		int j = need[i];
		if(bag + p[j].weight > capacity) {
			for(pair<int, int> x : has) {
				w[from].has[x.F] -= x.S;
				answers.emplace_back(myid, 'L', from, x.F, x.S);
			}
			for(pair<int, int> x : has) {
				//w[from].has[x.F] -= x.S;
				answers.emplace_back(myid, 'D', to, x.F, x.S);
			}
			has.clear();
			bag = 0;
		}
		bag += p[j].weight;
		has[j] ++;
	}

	for(pair<int, int> x : has) {
		w[from].has[x.F] -= x.S;
		answers.emplace_back(myid, 'L', from, x.F, x.S);
	}
	for(pair<int, int> x : has) {
		//w[from].has[x.F] -= x.S;
		answers.emplace_back(myid, 'D', to, x.F, x.S);
	}
	has.clear();
	bag = 0;
	x = o[to].x;
	y = o[to].y;
}

vector<int> Drone::find_bros(map<int, int> & wants) {
	vector<int> v;
	for(int i = 0; i < W; ++i) {
		bool fine = 1;
		for(auto it = wants.begin(); it != wants.end(); ++it) {
			if(w[i].has[it -> first] < it -> second) {
				fine = 0;
			}
		}
		if(fine) {
			v.push_back(i);
		}
	}
	sort(v.begin(), v.end(), [=](int i, int j) -> bool {
		return sqr(w[i].x - x) + sqr(w[i].y - y) < sqr(w[j].x - x) + sqr(w[j].y - y);
	});
	return v;
}

void Drone::trytodobro(int to) {
	int * perma = new int[W];
	for(int i = 0; i < W; ++i) {
		perma[i] = i;
	}
	random_shuffle(perma, perma + W);

	int bag = 0;
	map<int, int> gotem;

	for(int _i = 0; _i < W; ++_i) {
		int i = perma[_i];
		map<int, int> takes;
		for(pair<int, int> pa : o[to].wants) {
			int cantake = min(w[i].has[pa.F], pa.S - gotem[pa.F]);
			if(!cantake) continue;
			while(cantake) {
				if(bag + p[pa.F].weight > capacity) {
					for(pair<int, int> x : takes) {
						w[i].has[x.F] -= x.S;
						answers.emplace_back(myid, 'L', i, x.F, x.S);
					}
					for(pair<int, int> x : has) {
						answers.emplace_back(myid, 'D', to, x.F, x.S);
					}
					bag = 0;
					takes.clear();
					has.clear();
				}
				has[pa.F]++;
				takes[pa.F]++;
				gotem[pa.F]++;
				bag += p[pa.F].weight;
				cantake--;
			}
		}

		for(pair<int, int> x : takes) {
			w[i].has[x.F] -= x.S;
			answers.emplace_back(myid, 'L', i, x.F, x.S);
		}
		takes.clear();
	}

	for(pair<int, int> x : has) {
		answers.emplace_back(myid, 'D', to, x.F, x.S);
	}
	has.clear();

	x = o[to].x;
	y = o[to].y;
}

bool was = 0;
void Drone::execute() {
	vector<char> did(orders.size(), 0);
	for(int i = 0; i < orders.size(); ++i) {
		int j = orders[i];
		vector<int> bros = find_bros(o[j].wants);
		if(bros.empty() && (rand() & 1)) continue;
		if(bros.empty()) {
			trytodobro(j);
			did[i] = 1;
		}
		else {
			doitbro(bros[0], j);
			did[i] = 1;
		}
	}
	for(int i = 0; i < orders.size(); ++i) {
		if(!did[i]) {
			trytodobro(orders[i]);
		}
	}
}

int main () {
	freopen ("in", "r", stdin);
	cin >> ROWS >> COLS >> D >> deadline >> capacity;

	cin >> P;
	for(int i = 0; i < P; ++i) {
		int weight;
		cin >> weight;
		p[i] = Item(weight);
	}

	cin >> W;
	for(int i = 0; i < W; ++i) {
		int * p = new int[P];
		int x, y;
		cin >> x >> y;
		for(int j = 0; j < P; ++j) {
			cin >> p[j];
		}
		w[i] = Warehouse(x, y, p);
	}

	cin >> C;
	for(int i = 0; i < C; ++i) {
		int x, y;
		int l;
		vector<int> v;
		cin >> x >> y;
		cin >> l;
		for(int j = 0; j < l; ++j) {
			int temp;
			cin >> temp;
			v.push_back(temp);
		}
		o[i] = Order(x, y, v);
	}

	for(int i = 0; i < D; ++i) {
		d[i] = Drone(w[0].x, w[0].y, capacity, i);
	}


	//if(fork() == 0) {
		solve_bratan();
		cout << answers.size() << '\n';
		for(auto huh: answers) {
			print(huh);
		}
	//}

	return 0;
}
