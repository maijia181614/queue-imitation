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
		cout << "��ʼdeque���ԣ�" << endl;
		cout << (*deTest.begin()) << endl;
		cout << "�ռ��С���ԣ�"<<deTest.size()<<endl;
		cout << "��ʼ������";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deTest.push_back(5);
		deTest.push_back(6);
		deTest.push_front(7);
		cout << "�������ݲ��ԣ�5��6��7" << endl;
		cout << "����������";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deTest.pop_back();
		deTest.pop_front();
		cout << "�������ݲ��ԣ�7��6" << endl;
		cout << "�����������";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		deque<int>::iterator testIterator = deTest.begin() + 1;
		deTest.erase(testIterator);
		cout << "�����ڶ�λ����" << endl;
		cout << "�Ƴ��������";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		testIterator = deTest.begin() + 1;
		deTest.insert(testIterator, 2);
		cout << "����ڶ�λ����" << endl;
		cout << "����������";
		for (int i = 0; i < deTest.size(); i++) {
			cout << (deTest[i]) << ",";
		}
		cout << endl;
		cout << "�пղ��ԣ�";
		if (deTest.empty()) {
			cout << "�յ�"<<endl;
		}
		else {
			cout << "���ǿյ�" << endl;
		}
		deTest.clear();
		cout << "��ղ���" << endl;
		cout << "�пղ��ԣ�";
		if (deTest.empty()) {
			cout << "�յ�" << endl;
		}
		else {
			cout << "���ǿյ�" << endl;
		}
		cout << endl << endl << endl;
	}


	

}
