#define _CRT_SECURE_NO_WARNINGS
#define MAX_L 41
#define MAX_N 31
#define MAX_Q 100
#define MAX_K 100

#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <cstring>

using namespace std;


struct knight {
	int r, c, h, w, k;
};

int l, n, q;
int map[MAX_L][MAX_L];
int chk[MAX_L][MAX_L];			// 방문 여부 검사
int use[MAX_L][MAX_L]; // 기사 존재 여부 검사
vector<knight> vv(MAX_N);
vector<int> kk(MAX_N);

vector<pair<int, int>> qq(MAX_Q);

queue<int> candidate;
const int dr[4] = {-1, 0, 1, 0};
const int dc[4] = {0, 1, 0, -1};

bool is_Wall(int r, int c) {
	if (r < 0 || c < 0 || r >= n || c >= n)
		return false;
	return true;
}

// dir = 0, loc[vv[idx].r] 전부
// dir = 1, loc[vv[idx].r].back() ~ loc[vv[idx].r + vv[idx].w].back()
// dir = 2, loc[vv[idx].r + vv[idx].w] 전부
// dir = 3, loc[vv[idx].r].front() ~ loc[vv[idx].r + vv[idx].w].front()
bool isValid(int idx, int dir) {


	// @ 2. 해당 방향으로 이동 가능한지


	// #. 기사가 차지한 영역 좌표 저장
	vector<vector<int>> loc(l);
	queue<pair<int, int>> qp;

	for (int i = 0; i < vv[idx].h; i++) {
		for (int j = 0; j < vv[idx].w; j++) {
			if (!is_Wall(vv[idx].r + i, vv[idx].c + j))
				continue;
			switch (dir) {
			case 0:
				if (i == 0)
					qp.push({ vv[idx].r + i, vv[idx].c + j });
				break;
			case 2:
				if (i == vv[idx].h - 1)
					qp.push({ vv[idx].r + i, vv[idx].c + j });
				break;
			case 1:
				if (j == vv[idx].w - 1)
					qp.push({ vv[idx].r + i, vv[idx].c + j });
				break;
			case 3:
				if (j == 0)
					qp.push({ vv[idx].r + i, vv[idx].c + j });
				break;
			}

			loc[vv[idx].r + i].push_back(vv[idx].c + j);
		}
	}


	// # 해당 방향에 벽/밖 이 있다면 불가
	while (!qp.empty()) {
		pair<int, int> now = qp.front(); qp.pop();
		// now와 연결되는 기사들은 다 연결
		chk[now.first][now.second] = 1;
		pair<int, int> next = { now.first + dr[dir], now.second + dc[dir] };

		// next 좌표에 기사가 있는가?
		if (use[next.first][next.second] != -1)  {
			
			int next_idx = use[next.first][next.second];

			candidate.push(next_idx);

			for (int i = 0; i < vv[next_idx].h; i++) {
				for (int j = 0; j < vv[next_idx].w; j++) {
					if (chk[vv[next_idx].r + i][vv[next_idx].c + j] == 0) {

						if (!is_Wall(vv[next_idx].r + i, vv[next_idx].c + j))
							continue;
						switch (dir) {
						case 0:
							if (i == 0)
								qp.push({ vv[next_idx].r + i, vv[next_idx].c + j });
							break;
						case 2:
							if (i == vv[next_idx].h - 1)
								qp.push({ vv[next_idx].r + i, vv[next_idx].c + j });
							break;
						case 1:
							if (j == vv[next_idx].w - 1)
								qp.push({ vv[next_idx].r + i, vv[next_idx].c + j });
							break;
						case 3:
							if (j == 0)
								qp.push({ vv[next_idx].r + i, vv[next_idx].c + j });
							break;


						}
						chk[vv[next_idx].r + i][vv[next_idx].c + j] = 1;
					}

					loc[vv[idx].r + i].push_back(vv[idx].c + j);
				}
			}
			continue;
		}
		
		// next 좌표가 벽인가? : 이동 불가 
		if (map[next.first][next.second] == 2) {
			while (!candidate.empty()) candidate.pop();
			
			break;
		}
		

	}

	if (candidate.empty()) 
		return false;
	return true;
}


// idx번 기사에게 dir방향으로 가라고 명령
bool shiftKnight(int idx, int dir) {

	// 1. 기사가 존재하는지 확인 : 기사의 체력이 0보다 작거나 같으면 죽은 것
	if (vv[idx].k <= 0) 
		return false;

	if (isValid(idx, dir)) {

		// dir
		// 명령의 당사자 이동


		if (!is_Wall(vv[idx].r + dr[dir], vv[idx].c + dc[dir]))
			return false;

		vv[idx].r += dr[dir];
		vv[idx].c += dc[dir];


		return true;
	}

	return false;

}

void updateDamage(int dir) {

	// 기사 기준 해당 방향의 모든 영역 이동 및 데미지 업데이트
	// 이때 명령의 당사자는 큐에 넣지 않는다.
	while (!candidate.empty()) {
		int now = candidate.front();
		candidate.pop();
		// 영역 이동
		vv[now].r += dr[dir];
		vv[now].c += dc[dir];
		for (int i = 0; i < vv[now].h; i++) {
			for (int j = 0; j < vv[now].w; j++) {
				use[i][j] = now;
				if (map[vv[now].r + i][vv[now].c + j] == 1) {
					// 데미지 업데이트
					vv[now].k -= 1;		
					memset(use, -1, sizeof(use));
				}
			}
		}

	}




}


void solution() {
	// q번 진행
	for (int cmd = 0; cmd < q; cmd++) {
		int idx, dir;
		tie(idx, dir) = qq[cmd];

		// 1. idx번 기사에게 dir방향으로 가라고 명령
		bool is_shift = shiftKnight(idx, dir);

		memset(chk, 0, sizeof(chk));

		// 2. 만약 이동했다면 데미지 업데이트
		if (is_shift) {
			updateDamage(dir);
			// chk, use 초기화
		}

	}
}

int main() {

	// freopen("sample_input.txt", "r", stdin);

	cin >> l >> n >> q;
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < l; j++) {
			cin >> map[i][j];
		}
	}

	vv.resize(n);
	kk.resize(n);

	memset(use, -1, sizeof(use));
	for (int i = 0; i < n; i++) {
		int r, c, h, w, k;
		cin >> r >> c >> h >> w >> k;
		// 0번 기사부터 N-1번 기사
		vv[i] = { r - 1, c - 1, h, w, k };
		kk[i] = k;
		for (int a = 0; a < h; a++) {
			for (int b = 0; b < w; b++) {
				use[r - 1 + a][c - 1 + b] = i;
			}
		}

		
	}


	qq.resize(q);
	for (int i = 0; i < q; i++) {
		int idx, dir;
		cin >> idx >> dir;
		// i번 기사에게 방향 d로 한 칸 이동
		qq[i] = { idx - 1, dir };
	}
	solution();

	// @ 생존한 기사들이 총 받은 대미지의 합

	int rst = 0;
	for (int i = 0; i < n; i++) {
		if (vv[i].k > 0) {

			rst += (kk[i] - vv[i].k);
		}
	}

	cout << rst;


	return 0;

}