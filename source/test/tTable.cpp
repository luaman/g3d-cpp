#include "../include/G3DAll.h"

class TableKey : public Hashable {
public:
    int value;
    
    inline bool operator==(const TableKey& other) const {
        return value == other.value;
    }
    
    inline bool operator!=(const TableKey& other) const {
        return value != other.value;
    }

    inline unsigned int hashCode() const {
        return value;
    }
};

class TableKeyB : public Hashable {
public:
    int value;
    
    inline bool operator==(const TableKey& other) const {
        return value == other.value;
    }
    
    inline bool operator!=(const TableKey& other) const {
        return value != other.value;
    }

    inline unsigned int hashCode() const {
        return 0;
    }
};

void testTable() {
    printf("Table\n");
    // Basic get/set
    {
        Table<int, int> table;
    
        table.set(10, 20);
        table.set(3, 1);
        table.set(1, 4);

        debugAssert(table[10] == 20);
        debugAssert(table[3] == 1);
        debugAssert(table[1] == 4);
        debugAssert(table.containsKey(10));
        debugAssert(!table.containsKey(0));

        debugAssert(table.debugGetDeepestBucketSize() == 1);
    }


    // Test overloaded pointer hashing
    {
        TableKey        x[4];
        x[0].value = 1;
        x[1].value = 2;
        x[2].value = 3;
        x[3].value = 4;
        Table<TableKey*, int> table;

        table.set(x, 10);
        table.set(x + 1, 20);
        table.set(x + 2, 30);
        table.set(x + 3, 40);
        debugAssert(table[x] == 10);
        debugAssert(table[x + 1] == 20);
        debugAssert(table[x + 2] == 30);
        debugAssert(table[x + 3] == 40);

        // Our current implementation should not have collisions in this case.
        debugAssert(table.debugGetDeepestBucketSize() == 1);
    }

    {
        TableKeyB        x[6];
        Table<TableKeyB*, int> table;
        for (int i = 0; i < 6; ++i) {
            x[i].value = i;
            table.set(x + i, i);
        }

        debugAssert(table.size() == 6);
        debugAssert(table.debugGetDeepestBucketSize() == 6);
        debugAssert(table.debugGetNumBuckets() == 10);
    }
}
