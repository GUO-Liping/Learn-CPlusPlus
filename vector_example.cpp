#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 使用 assign 方法将向量中的元素替换为新元素
    std::vector<int> new_elements = {10, 20, 30};
    vec.assign(3,0);
    
    // 打印替换后的向量元素
    for (const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
