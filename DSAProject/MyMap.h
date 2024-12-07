#include <iostream>
using namespace std;

template <typename Key, typename Value>
class MyMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;

        Node(const Key& k, const Value& v, Node* n = nullptr) : key(k), value(v), next(n) {}
    };

    Node** table;     // Dynamic array of pointers to linked list nodes
    size_t capacity;  // Current size of the table
    size_t size;      // Number of elements in the map


    //use a better hash function for optimization
    size_t hashFunction(const Key& key) const 
    {
        return std::hash<Key>{}(key) % capacity; // Hash and mod by capacity
    }

    // Resize the table when the load factor exceeds a threshold
    void resizeTable() {
        size_t newCapacity = capacity * 2;
        Node** newTable = new Node * [newCapacity];
        for (size_t i = 0; i < newCapacity; ++i) {
            newTable[i] = nullptr;
        }

        // Rehash all elements into the new table
        for (size_t i = 0; i < capacity; ++i) {
            Node* node = table[i];
            while (node) {
                Node* next = node->next;
                size_t newIndex = std::hash<Key>{}(node->key) % newCapacity;
                node->next = newTable[newIndex];
                newTable[newIndex] = node;
                node = next;
            }
        }

        delete[] table;
        table = newTable;
        capacity = newCapacity;
    }

    // Helper to delete all nodes in a linked list
    void clearBucket(Node* node) 
    {
        while (node) {
            Node* temp = node;
            node = node->next;
            delete temp;
        }
    }

public:
    MyMap(size_t initialCapacity = 16) : capacity(initialCapacity), size(0) 
    {
        table = new Node * [capacity];
        for (size_t i = 0; i < capacity; ++i) 
        {
            table[i] = nullptr;
        }
    }

    ~MyMap() {
        for (size_t i = 0; i < capacity; ++i) 
        {
            clearBucket(table[i]);
        }
        delete[] table;
    }

    // Check if the map is empty
    bool empty() const {
        return size == 0;
    }

    // Insert or update a key-value pair
    void insert(const Key& key, const Value& value) {
        if (size > capacity * 0.75) { // Resize if load factor exceeds 75%
            resizeTable();
        }

        size_t index = hashFunction(key);
        Node* node = table[index];

        // Search for the key in the bucket
        while (node) {
            if (node->key == key) {
                node->value = value; // Update value if key exists
                return;
            }
            node = node->next;
        }

        // Key not found, insert a new node at the head of the linked list
        table[index] = new Node(key, value, table[index]);
        size++;
    }

    // Get the value associated with a key
    bool get(const Key& key, Value& outValue) const {
        size_t index = hashFunction(key);
        Node* node = table[index];

        while (node) {
            if (node->key == key) {
                outValue = node->value;
                return true;
            }
            node = node->next;
        }

        return false; // Key not found
    }

    // Check if a key exists
    bool contains(const Key& key) const {
        size_t index = hashFunction(key);
        Node* node = table[index];

        while (node) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }

        return false;
    }

    // Remove a key-value pair
    bool remove(const Key& key) {
        size_t index = hashFunction(key);
        Node* node = table[index];
        Node* prev = nullptr;

        while (node) {
            if (node->key == key) {
                // Remove the node
                if (prev) {
                    prev->next = node->next;
                }
                else {
                    table[index] = node->next;
                }
                delete node;
                size--;
                return true;
            }
            prev = node;
            node = node->next;
        }

        return false; // Key not found
    }

    // Display all key-value pairs

    template <typename Callable>
    void display(Callable func) const {
        for (size_t i = 0; i < capacity; ++i) {
            Node* node = table[i];
            while (node) {
                func(node->key, node->value); // Apply the callable to each key-value pair
                node = node->next;
            }
        }
    }


};
