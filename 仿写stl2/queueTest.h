#include"queue.h"
#include "Alloc.h"
#include<vector>
#include<iostream>
using std::endl;
using std::cout;
namespace queueSTL {

	void queue_test() {
		cout << "开始queue测试：" << endl;
		queue<int> qeTest;
		cout << "入队测试：1、3" << endl;
		qeTest.push(1);
		qeTest.push(3);
		cout <<"获取队头："<< qeTest.front()<<endl;
		cout << "获取队尾：" << qeTest.back()<<endl;
		cout << "获取容量：" << qeTest.size()<<endl;
		cout << "测试判空:";
		if (qeTest.empty()) {
			cout << "空的"<<endl;
		}
		else {
			cout << "不是空的" << endl;
		}
		cout << "出队测试：1、3"<<endl;
		qeTest.pop();
		qeTest.pop();
		cout << "测试判空:";
		if (qeTest.empty()) {
			cout << "空的" << endl;
		}
		else {
			cout << "不是空的" << endl;
		}
	}

}
