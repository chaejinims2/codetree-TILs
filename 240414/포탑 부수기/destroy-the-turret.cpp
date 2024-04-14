#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
#include <utility>
#include <tuple>

using namespace std;

#define MAX_NM 11
#define MAX_K 1001


int n, m, k;
int map[MAX_NM][MAX_NM];
pair<int, int> path[MAX_NM][MAX_NM];
bool reco[MAX_NM][MAX_NM];
int duration[MAX_NM][MAX_NM];
int cnt_k;
const int dr[9] = { 0,  0, 1, 0, -1, 1, 1, -1, -1 };
const int dc[9] = { 0,  1, 0, -1, 0, 1, -1, 1, -1 };

struct node {
	int r, c, p, t;
};

bool cmp(node n1, node n2) {
	if (n1.p != n2.p) return n1.p < n2.p;
	if (n1.t != n2.t) return n1.t > n2.t;
	if (n1.r + n1.c != n2.r + n2.c) return n1.r + n1.c > n2.r + n2.c;
	return n1.c > n2.c;
}
node str, stp;
vector<node> vv;
int use[MAX_NM][MAX_NM];

void attackBomb() {
	for (int i = 0; i < 9; i++) {

		int nr = (stp.r + dr[i] + n) % n;
		int nc = (stp.c + dc[i] + m) % m;

		// #. 자기 자신인가
		if (nr == str.r && nc == str.c) continue;

		// #. 가장 강한 포탑인가 -str.p
		if (nr == stp.r && nc == stp.c) {
			map[stp.r][stp.c] -= str.p;
			reco[stp.r][stp.c] = true;
		}
		
		// #. 그외 - str.p/2
		else {
			map[nr][nc] -= str.p/2;
			reco[nr][nc] = true;
		}
	}
}

bool attackLaser() {
	// 1. bfs : str 에서 stp까지 레이저 공격 가능한지 확인
	queue<pair<int, int>> q;
	use[str.r][str.c] = 1;
	q.push({ str.r, str.c });
	bool is_possible = false;

	while (!q.empty()) {
		int r, c;
		tie(r, c) = q.front(); q.pop();

		// #. stp 도달 가능 시 멈춤
		if (r == stp.r && c == stp.c) {
			is_possible = true;
			break;
		}

		for (int i = 1; i < 5; i++) {
			int nr = (r + dr[i] + n) % n;
			int nc = (c + dc[i] + m) % m;

			// #. 방문 체크
			if (use[nr][nc] == 1) continue;

			// #. 죽은 포탑
			if (map[nr][nc] <= 0) continue;

			use[nr][nc] = 1;
			path[nr][nc] = { r, c };
			q.push({ nr, nc });

		}

	}

	if (is_possible) {
		// 1. 가장 강한 포탑 공격
		map[stp.r][stp.c] -= str.p;
		reco[stp.r][stp.c] = true;

		// 2. 역추적하면 경로에 있는 포탑 공격
		int r, c;
		tie(r, c) = path[stp.r][stp.c];

		while (!(r == str.r && c == str.c)) {
			map[r][c] -= str.p/2;
			reco[r][c] = true;

			int nr, nc;
			tie(nr, nc) = path[r][c];
			tie(r, c) = tie(nr, nc);
		}

		return is_possible;
	}

	// 레이저 공격 불가 시 
	return false;

}

void init() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			use[i][j] = 0;
			reco[i][j] = 0;
		}
	}
	cnt_k++;
}

void solution() {
	// 공격자 선정
	sort(vv.begin(), vv.end(), cmp);
	str = vv[0];
	stp = vv[vv.size() - 1];
	// 각성
	str.p += (n + m);
	vv[0].p = str.p;
	map[str.r][str.c] = str.p;
	reco[str.r][str.c] = true;
	duration[str.r][str.c] = cnt_k;
	str.t = cnt_k;

	// 공격
	bool is_attacked = attackLaser();
	if (!is_attacked)
		attackBomb();


	// 공격력 0이하 포탑 파괴
	vector<node> tmp = vv;
	vv.clear();
	for (auto it = tmp.begin(); it != tmp.end(); it++) {
		node it_val = *it ;
		if(map[it_val.r][it_val.c] <= 0) continue;
		
		if(reco[it_val.r][it_val.c] == false)
			map[it_val.r][it_val.c]++;

		it_val.p = map[it_val.r][it_val.c];
		vv.push_back(it_val);
	}

	// @ 포탑 정비
	// #. 시간 str.t++;

}


int main() {
	// freopen("sample_input.txt", "r", stdin);

	cin >> n >> m >> k;


	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cin >> map[i][j];
			if (map[i][j] == 0) continue;
			vv.push_back({ i, j, map[i][j], 0 });
		}
	}


	while (k--) {
		init();
		solution();

		// #. 포탑이 하나 남았는가?
		if (vv.size() == 1)
			break;
		 
	}

	int ans = -1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			ans = max(ans, map[i][j]);
		}
	}
	cout << ans;


	// 남아있는 포탑 중 가장 강한 포탑의 공격력
	return 0;
}