#include"queue.h"
#include "Alloc.h"
#include<vector>
#include<iostream>
using std::endl;
using std::cout;
namespace queueSTL {

	void queue_test() {
		cout << "��ʼqueue���ԣ�" << endl;
		queue<int> qeTest;
		cout << "��Ӳ��ԣ�1��3" << endl;
		qeTest.push(1);
		qeTest.push(3);
		cout <<"��ȡ��ͷ��"<< qeTest.front()<<endl;
		cout << "��ȡ��β��" << qeTest.back()<<endl;
		cout << "��ȡ������" << qeTest.size()<<endl;
		cout << "�����п�:";
		if (qeTest.empty()) {
			cout << "�յ�"<<endl;
		}
		else {
			cout << "���ǿյ�" << endl;
		}
		cout << "���Ӳ��ԣ�1��3"<<endl;
		qeTest.pop();
		qeTest.pop();
		cout << "�����п�:";
		if (qeTest.empty()) {
			cout << "�յ�" << endl;
		}
		else {
			cout << "���ǿյ�" << endl;
		}
	}

}
