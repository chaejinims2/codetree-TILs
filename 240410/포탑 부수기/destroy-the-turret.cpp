#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <utility>
#include <queue>
#include <cstring>
#include <climits>
using namespace std;

int n, m, k;
int map[11][11];
int dr[8] = { 0, 1, 0, -1, -1, -1, 1, 1 };
int dc[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };

typedef pair<int, int> myPair;
myPair initpair = { -1, -1 };
myPair path[11][11] = { initpair, };
int use[11][11] = { 0, };
int max_val = 0;
typedef struct node {
	myPair loc;
	int power;
	bool is_destroyed;
	int cnt;

	bool operator<(const node& next) const {
		if (is_destroyed != next.is_destroyed) return is_destroyed < next.is_destroyed;
		if (power != next.power) return power > next.power;
		if (cnt != next.cnt) return cnt > next.cnt;
		if (loc.first + loc.second != next.loc.first + next.loc.second) return loc.first + loc.second > next.loc.first + next.loc.second;
		return loc.second > next.loc.second;
	}

}node;


// filter 1. power, loc, 공격 받은지 흐른 시간에 따른 우선 순위
priority_queue<node> pq;



node str, stp;

void input() {

	cin >> n >> m >> k;
	for (int i = 1; i < n + 1; i++) {
		for (int j = 1; j < m + 1; j++) {
			cin >> map[i][j];
		}
	}
}

void init() {
	while (!pq.empty())
		pq.pop();
	// 살아 있는 포탑들에 map과 일치하는 정보 부여
	for (int i = 1; i < n + 1; i++) {
		for (int j = 1; j < m + 1; j++) {
			node tmp;
			if (map[i][j] == 0 || map[i][j] == -1) {
				map[i][j] = -1;
				tmp = { {i, j}, 0, true, 0 };
			}
			else {
				tmp = { {i, j}, map[i][j], false, 0 };
				pq.push(tmp);
			}
		}
	}
	//debuging
	/*while (!pq.empty()) {
		node val = pq.top();
		pq.pop();
		cout << val.power << " ";
	}*/

}



// 레이저 공격 성공 시 true
// str to stp까지 도달 가능한가?
// 벽을 만나면 반대편으로 나올 수 있음
// 부서진 포탑 (-1)이면 못감

bool dijkstra() {
	// filter 2. 최단 거리 경로 저장 
	priority_queue<pair<int, myPair>, vector<pair<int, myPair>>, greater<pair<int, myPair>>> pq2;
	vector<vector<int>> dist(n + 1, vector<int>(m + 1, INT_MAX)); // 최단 거리를 저장할 배열
	vector<vector<myPair>> prev(n + 1, vector<myPair>(m + 1, { -1, -1 }));
	dist[str.loc.first][str.loc.second] = 0; // 시작 위치의 거리는 0
	pq2.push({ 0, str.loc }); // 시작 위치를 우선순위 큐에 넣음

	while (!pq2.empty()) {
		int current_dist = pq2.top().first;
		myPair current_loc = pq2.top().second;
		pq2.pop();

		// 이미 처리된 노드는 무시
		if (dist[current_loc.first][current_loc.second] < current_dist) continue;

		// 인접한 노드를 확인
		for (int i = 0; i < 4; i++) {
			myPair next_loc = { current_loc.first + dr[i] , current_loc.second + dc[i] };

			// 벽을 만나면 반대편으로 나옴
			if (next_loc.first < 1)
				next_loc.first = n;
			else if (next_loc.second < 1)
				next_loc.second = m;
			else if (next_loc.first > n)
				next_loc.first = 1;
			else if (next_loc.second > m)
				next_loc.second = 1;

			// 부서진 포탑이면 못감
			if (map[next_loc.first][next_loc.second] < 0) continue;

			int next_dist = current_dist + 1; // 거리는 항상 1 증가



			// 더 짧은 경로를 발견하면, dist를 업데이트하고 우선순위 큐에 넣음
			if (next_dist < dist[next_loc.first][next_loc.second]) {
				dist[next_loc.first][next_loc.second] = next_dist;
				prev[next_loc.first][next_loc.second] = current_loc;
				pq2.push({ next_dist, next_loc });
			}
		}
	}

	// 목표 위치까지 도달 가능하면 true, 아니면 false 반환
	if (dist[stp.loc.first][stp.loc.second] != INT_MAX) {
		myPair current_loc = stp.loc;
		map[current_loc.first][current_loc.second] -= (str.power / 2 + str.power%2);
		while (current_loc != str.loc) {
			map[current_loc.first][current_loc.second] -= str.power/2;
			use[current_loc.first][current_loc.second] = 1;
			current_loc = prev[current_loc.first][current_loc.second];
		}

		return true;
	}

	return false;
}
void updateMap() {
	// solution 함수 내부에서 공격을 받지 않은 포탑의 power 증가
	for (int i = 1; i < n + 1; i++) {
		for (int j = 1; j < m + 1; j++) {
			if (use[i][j] == 0 && map[i][j] > 0) {
				if (str.loc.first == i && str.loc.second == j) continue;

				// 4. 포탑 재정비 공격 반지 않았던  포탑들 power + 1
				map[i][j]++;
			}
			if (map[i][j] == 0) {
				// 3. 공격받았던 포탑의 부서짐 여부 업데이트
				map[i][j] = -1;

			}
			max_val = max(max_val, map[i][j]);
		}
	}
}

bool attackLazer() {
	// 공격 전 map 저장
	int tmp[11][11];
	memmove(tmp, map, sizeof(map));

	// 레이저 공격 시작
	bool isPossible = dijkstra();

	// 레이저 공격 불가 시 : map을 복구하고 return false
	if (isPossible == false) {

		memmove(use, 0, sizeof(use));
		memmove(map, tmp, sizeof(map));

		return false;
	}

	return true;
}

void attackBomb() {
	myPair now = stp.loc;

	map[now.first][now.second] -= str.power;
	use[now.first][now.second] = 1;



	for (int i = 0; i < 8; i++) {
		myPair next = { now.first + dr[i] , now.second + dc[i] };

		// #. 벽인가? 반대편 위치로 업데이트
		if (next.first < 1)
			next.first = n;
		else if (next.second < 1)
			next.second = m;
		else if (next.first > n)
			next.first = 1;
		else if (next.second > m)
			next.second = 1;


		// #. 부서진 포탑인가?
		if (map[next.first][next.second] < 0) continue;

		// #. 자기자신 즉, 공격자인가?
		if (next == str.loc) continue;

		map[next.first][next.second] -= str.power / 2;
		use[next.first][next.second] = 1;


	}



}

void solution() {
	for (int i = 0; i < k; i++) {
		// #. pq에 남은 포탑이 1개 라면 종료
		if (pq.size() <= 1) {
			node rst = pq.top();

			cout << rst.power;
			return;

		}


		// 1 4 4 8 8 10 11 13 26
		// 1. 공격자와 대상자 선정 : pq에서 공격력이 가장 작은 포탑 & 공격력이 가장 큰 포탑
		str = pq.top();
		str.power += n + m;
		map[str.loc.first][str.loc.second] += n + m;
		while (!pq.empty()) {
			stp = pq.top();
			pq.pop();
		}



		// 2. 공격자의 공격 - 레이저 : if 출발지에서 목적지까지의 최단 거리로 도착할 수 있을 때


		// 2. 공격자의 공격 - 포탄  : else 위에 조건에 해당 안된다면 
		if (!attackLazer())
			attackBomb();

		updateMap();
		// 5. 포탑 업데이트
		init();

	}

	cout << max_val << endl;
	
}

int main(int argc, char** argv)
{
	int test_case;
	int T = 1;

	//freopen("sample_input.txt", "r", stdin);
	//cin >> T;

	for (test_case = 1; test_case <= T; ++test_case)
	{
		input();
		init();
		solution();
	}
	return 0;
}