#include <iostream>

#include "b_tree.h"

//test data are copied from https://www.cnblogs.com/nullzx/p/8729425.html

int main() {
	b_tree<int, 5> tree;

	tree.insert(39);
	tree.insert(22);
	tree.insert(97);
	tree.insert(41);
	tree.insert(53);

	tree.insert(13);
	tree.insert(21);
	tree.insert(40);

	tree.insert(30);
	tree.insert(27);
	tree.insert(33);
	tree.insert(36);
	tree.insert(35);
	tree.insert(34);
	tree.insert(24);
	tree.insert(29);

	tree.insert(26);

	tree.insert(17);
	tree.insert(28);
	tree.insert(29);
	tree.insert(31);
	tree.insert(32);
	tree.insert(23);

	tree.erase(21);

	tree.erase(27);

	tree.erase(32);

	tree.erase(40);

	return 0;
}