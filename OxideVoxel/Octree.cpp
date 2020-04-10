#include "Octree.h"

Octree* GenerateRandom(int depth)
{
	int currdepth = 0;
	std::stack<Octree*> nodeStack;
	while (currdepth < depth) {
		Octree* curr = new Octree;
		currdepth++;
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(0, 1);
		for(int i = 0; i < 8;i++){
			int val = distr(eng);
			curr->data = curr->data ^ (val << i);
		}
		nodeStack.push(curr);
	}
	Octree* head = nodeStack.top();
	nodeStack.pop();
	return head;
}
