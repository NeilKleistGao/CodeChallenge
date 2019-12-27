#pragma once
#include <deque>
#include <cmath>
#include <algorithm>

template<class type, size_t degree>
class b_tree {
private:
	struct node {
		std::deque<type> keys;
		std::deque<node*> children;
		bool is_leaf;

		node() : is_leaf(true) {
		}
	};
public:
	b_tree() : root(nullptr) {
		
	}

	~b_tree() {
		while (root != nullptr) {
			erase(root->keys.front());
		}
	}

	void insert(const type& key) {
		if (root == nullptr) {
			root = new node();
			root->keys.push_back(key);
			return;
		}

		if (!find(key)) {
			__insert(root, key);

			if (root->keys.size() > degree - 1) {
				node* temp = root;
				root = new node();
				root->is_leaf = false;
				root->children.push_back(temp);
				splitChildByIndex(root, 0);
			}
		}
	}

	bool find(const type& key) {
		if (root == nullptr) {
			return false;
		}

		return __find(root, key);
	}

	void erase(const type& key) {
		if (!find(key)) {
			return;
		}

		__erase(root, key);

		if (root->keys.empty()) {
			delete root;
			root = nullptr;
		}
		else if (root->children.empty()) {
			root->is_leaf;
		}
	}

private:
	node* root;

	void splitChildByIndex(node* parent, const size_t& index) {
		size_t pos = degree / 2;
		node* child = parent->children[index],
			* new_child = new node();

		type temp = child->keys[pos];
		new_child->is_leaf = child->is_leaf;
		parent->keys.insert(parent->keys.begin() + index, temp);
		child->keys.erase(child->keys.begin() + pos);

		for (auto i = pos; i < child->keys.size(); i++) {
			new_child->keys.push_back(child->keys[i]);
		}
		child->keys.erase(child->keys.begin() + pos, child->keys.end());

		if (!new_child->is_leaf) {
			for (auto i = pos + 1; i < child->children.size(); i++) {
				new_child->children.push_back(child->children[i]);
			}
			child->children.erase(child->children.begin() + pos + 1, child->children.end());
		}
		
		parent->children.insert(parent->children.begin() + index + 1, new_child);
	}

	void __insert(node* current, const type& key) {
		if (current->is_leaf) {
			auto pos = std::lower_bound(current->keys.begin(), current->keys.end(), key);
			current->keys.insert(pos, key);
			return;
		}
		
		auto pos = std::lower_bound(current->keys.begin(), current->keys.end(), key) - current->keys.begin() + current->children.begin();
		__insert(*pos, key);

		if ((*pos)->keys.size() > degree - 1) {
			splitChildByIndex(current, pos - current->children.begin());
		}
	}

	bool __find(node* current, const type& key) {
		if (current->is_leaf) {
			auto try_pos = std::lower_bound(current->keys.begin(), current->keys.end(), key);
			return !((try_pos == current->keys.end() || (*try_pos) != key));
		}

		auto pos = std::lower_bound(current->keys.begin(), current->keys.end(), key);
		return ((pos != current->keys.end() && (*pos) == key)) || __find(*(pos - current->keys.begin() + current->children.begin()), key);
	}

	void __erase(node*& current, const type& key) {
		if (current->is_leaf) {
			auto pos = std::lower_bound(current->keys.begin(), current->keys.end(), key);
			current->keys.erase(pos);
			return;
		}

		auto pos = std::lower_bound(current->keys.begin(), current->keys.end(), key);
		auto next = current->children.begin() + (pos - current->keys.begin()) + 1;
		if (pos != current->keys.end() && (*pos) == key) {
			(*pos) = (*next)->keys.front();
			__erase(*next, (*next)->keys.front());
		}
		else {
			next = (pos - current->keys.begin() + current->children.begin());
			__erase(*next, key);
		}

		
		if ((*next)->keys.size() < std::ceil(degree / 2.0f) - 1) {
			combineChildrenByIndex(current, next - current->children.begin());
		}
	}

	void combineChildrenByIndex(node*& parent, const size_t& index) {
		if (index + 1 < parent->children.size() && parent->children[index + 1]->keys.size() > std::ceil(degree / 2.0f) - 1) {
			auto child = parent->children[index];
			auto sibling = parent->children[index + 1];
			child->keys.push_back(parent->keys[index]);
			parent->keys[index] = sibling->keys.front();
			sibling->keys.pop_back();

			if (!child->is_leaf) {
				child->children.push_back(sibling->children.front());
				sibling->children.pop_front();
			}
		}
		else if (index > 0 && parent->children[index - 1]->keys.size() > std::ceil(degree / 2.0f) - 1){
			auto child = parent->children[index];
			auto sibling = parent->children[index - 1];
			child->keys.push_front(parent->keys[index - 1]);
			parent->keys[index - 1] = sibling->keys.back();
			sibling->keys.pop_back();

			if (!child->is_leaf) {
				child->children.push_front(sibling->children.back());
				sibling->children.pop_back();
			}
		}
		else {
			if (index == 0) {
				auto child = parent->children[index];
				auto sibling = parent->children[index + 1];
				
				child->keys.push_back(parent->keys[index]);
				parent->keys.erase(parent->keys.begin() + index);
				for (auto key : sibling->keys) {
					child->keys.push_back(key);
				}

				if (!child->is_leaf) {
					for (auto sib_child : sibling->children) {
						child->children.push_back(sib_child);
					}
				}

				parent->children.erase(parent->children.begin() + index + 1);
				delete sibling;
				sibling = nullptr;
			}
			else {
				auto child = parent->children[index];
				auto sibling = parent->children[index - 1];

				child->keys.push_front(parent->keys[index - 1]);
				parent->keys.erase(parent->keys.begin() + index - 1);
				for (auto it = sibling->keys.rbegin(); it != sibling->keys.rend(); it++) {
					child->keys.push_front(*it);
				}

				if (!child->is_leaf) {
					for (auto it = sibling->children.rbegin(); it != sibling->children.rend(); it++) {
						child->children.push_front(*it);
					}
				}

				parent->children.erase(parent->children.begin() + index - 1);
				delete sibling;
				sibling = nullptr;

				if (parent->keys.empty()) {
					parent = child;
				}
			}
		}
	}
};

