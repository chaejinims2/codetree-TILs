#include <iostream>
#include <list>
#include <string>
using namespace std;

list<int> ss;
int n;
string str;
int num, nums;

int main() {

    cin >> num;
    for (int i = 0; i < num; i++) {
        cin >> str;
        switch (str.length()) {
        case 4:
            cout << ss.size() << endl;
			break;
        case 9:
            cin >> nums;
            ss.push_back(nums);
            break;

        case 8:
            ss.pop_back();
            break;
        case 3:
            cin >> nums;
            list<int>::iterator it = ss.begin();
            advance(it, nums - 1);
            cout << *it << endl;
            break;

        }
    }
    return 0;
}