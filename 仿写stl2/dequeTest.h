#include"deque.h"
#include "Alloc.h"
#include<vector>
#include<iostream>
using std::endl;
using std::cout;
namespace queueSTL {
	
	void deque_test() {
		deque<int> deTest(3,2);
		std::vector<int> ivec = { 5,6,7,8 };
		cout << "开始deque测试：" << endl;
		cout << (*deTest.begin()) << endl;
		cout << "空间大小测试："<<deTest.size()<<endl;
		cout << "初始遍历：";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deTest.push_back(5);
		deTest.push_back(6);
		deTest.push_front(7);
		cout << "插入数据测试：5、6、7" << endl;
		cout << "插入后遍历：";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deTest.pop_back();
		deTest.pop_front();
		cout << "弹出数据测试：7、6" << endl;
		cout << "弹出后遍历：";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deque<int>::iterator testIterator = deTest.begin() + 1;
		deTest.erase(testIterator);
		cout << "弹出第二位测试" << endl;
		cout << "移除后遍历：";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		testIterator = deTest.begin() + 1;
		deTest.insert(testIterator, 2);
		cout << "插入第二位测试" << endl;
		cout << "插入后遍历：";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		cout << "判空测试：";
		if (deTest.empty()) {
			cout << "空的"<<endl;
		}
		else {
			cout << "不是空的" << endl;
		}
		deTest.clear();
		cout << "清空测试" << endl;
		cout << "判空测试：";
		if (deTest.empty()) {
			cout << "空的" << endl;
		}
		else {
			cout << "不是空的" << endl;
		}
		cout << endl << endl << endl;
	}


	

}
