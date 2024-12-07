#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>

template <typename T>
struct Node {
    T data;
    std::unique_ptr<Node> next;

    template <typename... Args>
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}

    // Equality comparison
    bool operator==(const Node& other) const { return data == other.data; }

    // Three-way comparison (C++20 spaceship operator)
    std::strong_ordering operator<=>(const Node& other) const { return data <=> other.data; }
};

// Specialize std::hash for Node<T>
namespace std {
template <typename T>
struct hash<Node<T>> {
    size_t operator()(const Node<T>& node) const { return std::hash<T>{}(node.data); }
};
}  // namespace std

template <typename T>
class LinkedList {
private:
    std::unique_ptr<Node<T>> head;
    Node<T>* tail;
    size_t _size = 0;

public:
    // Iterator traits for standard compliance
    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;

    // Constructor and Rule of Five
    LinkedList() noexcept = default;

    // Initializer list constructor
    LinkedList(std::initializer_list<T> init) {
        for (const auto& item : init) {
            push_back(item);
        }
    }

    // Copy constructor
    LinkedList(const LinkedList& other) {
        for (const auto& item : other) {
            push_back(item);
        }
    }

    // Move constructor
    LinkedList(LinkedList&& other) noexcept = default;

    // Copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            // Copy-and-swap idiom
            LinkedList temp(other);
            std::swap(head, temp.head);
            std::swap(tail, temp.tail);
            std::swap(_size, temp._size);
        }
        return *this;
    }

    // Move assignment
    LinkedList& operator=(LinkedList&& other) noexcept = default;

    // Destructor is defaulted (unique_ptr handles cleanup)
    ~LinkedList() = default;

    [[nodiscard]] bool empty() const noexcept { return _size == 0; }
    [[nodiscard]] size_t size() const noexcept { return _size; }

    void clear() noexcept {
        head.reset();
        tail = nullptr;
        _size = 0;
    }

    // Safe front and back access with optional
    [[nodiscard]] std::optional<T> safe_front() const noexcept {
        return empty() ? std::nullopt : std::optional(head->data);
    }

    [[nodiscard]] std::optional<T> safe_back() const noexcept {
        return empty() ? std::nullopt : std::optional(tail->data);
    }

    [[nodiscard]] T& front() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    [[nodiscard]] const T& front() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    [[nodiscard]] T& back() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }

    [[nodiscard]] const T& back() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }

    template <typename... Args>
    void emplace_front(Args&&... args) {
        auto new_node = std::make_unique<Node<T>>(std::forward<Args>(args)...);
        if (!head) {
            tail = new_node.get();
        }
        new_node->next = std::move(head);
        head = std::move(new_node);
        ++_size;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        auto new_node = std::make_unique<Node<T>>(std::forward<Args>(args)...);
        if (!head) {
            head = std::move(new_node);
            tail = head.get();
        } else {
            tail->next = std::move(new_node);
            tail = tail->next.get();
        }
        ++_size;
    }

    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(std::move(value)); }
    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    std::optional<T> pop_front() {
        if (empty()) return std::nullopt;

        T value = std::move(head->data);
        head = std::move(head->next);
        --_size;

        if (!head) {
            tail = nullptr;
        }

        return value;
    }

    std::optional<T> pop_back() {
        if (empty()) return std::nullopt;

        if (_size == 1) {
            T value = std::move(head->data);
            head.reset();
            tail = nullptr;
            _size = 0;
            return value;
        }

        auto* curr = head.get();
        while (curr->next.get() != tail) {
            curr = curr->next.get();
        }

        T value = std::move(tail->data);
        curr->next.reset();
        tail = curr;
        --_size;

        return value;
    }

    bool remove(const T& value) {
        if (empty()) {
            return false;
        }

        if (head->data == value) {
            pop_front();
            return true;
        }

        auto* curr = head.get();
        while (curr->next && curr->next->data != value) {
            curr = curr->next.get();
        }

        if (curr->next) {
            curr->next = std::move(curr->next->next);
            --_size;
            if (!curr->next) {
                tail = curr;
            }
            return true;
        }

        return false;
    }

    void print() const {
        for (auto* curr = head.get(); curr != nullptr; curr = curr->next.get()) {
            std::cout << curr->data << (curr->next ? " -> " : "\n");
        }
    }

    class iterator {
    private:
        Node<T>* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit iterator(Node<T>* node) : current(node) {}

        // Dereference operator
        reference operator*() {
            if (current == nullptr) {
                throw std::out_of_range("Cannot dereference end iterator");
            }
            return current->data;
        }

        // Pointer-like access
        pointer operator->() {
            if (current == nullptr) {
                throw std::out_of_range("Cannot access through end iterator");
            }
            return &(current->data);
        }

        // Pre-increment
        iterator& operator++() {
            if (current) {
                current = current->next.get();
            }
            return *this;
        }

        // Post-increment
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // Comparison operators
        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    class const_iterator {
    private:
        const Node<T>* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        explicit const_iterator(const Node<T>* node) : current(node) {}

        // Allow conversion from non-const iterator
        const_iterator(const iterator& other) : current(other.current) {}

        // Dereference operator
        reference operator*() const {
            if (current == nullptr) {
                throw std::out_of_range("Cannot dereference end iterator");
            }
            return current->data;
        }

        // Pointer-like access
        pointer operator->() const {
            if (current == nullptr) {
                throw std::out_of_range("Cannot access through end iterator");
            }
            return &(current->data);
        }

        // Pre-increment
        const_iterator& operator++() {
            if (current) {
                current = current->next.get();
            }
            return *this;
        }

        // Post-increment
        const_iterator operator++(int) {
            const_iterator temp = *this;
            if (current) {
                current = current->next.get();
            }
            return temp;
        }

        // Comparison operators
        bool operator==(const const_iterator& other) const { return current == other.current; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };

    [[nodiscard]] iterator begin() { return iterator(head.get()); }
    [[nodiscard]] iterator end() { return iterator(nullptr); }

    [[nodiscard]] const_iterator begin() const { return const_iterator(head.get()); }
    [[nodiscard]] const_iterator end() const { return const_iterator(nullptr); }
    [[nodiscard]] const_iterator cbegin() const { return const_iterator(head.get()); }
    [[nodiscard]] const_iterator cend() const { return const_iterator(nullptr); }

    iterator find(const T& value) { return std::find(begin(), end(), value); }
    [[nodiscard]] const_iterator find(const T& value) const { return std::find(begin(), end(), value); }
    bool contains(const T& value) const { return find(value) != end(); }
};

int main() {
    LinkedList<std::string> list;

    list.push_back("Apple");
    list.push_back("Banana");
    list.push_back("Cherry");
    list.push_front("Grape");

    std::cout << "Original list: ";
    list.print();

    std::cout << "Iterating over the list: ";
    for (const auto& value : list) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    std::cout << "Front element: " << list.front() << "\n";

    std::cout << "Find 'Banana': " << (list.contains("Banana") ? "Found" : "Not Found") << "\n";

    std::cout << "Remove 'Banana': " << (list.remove("Banana") ? "Removed" : "Not Found") << "\n";

    list.print();

    std::cout << "Size of the list: " << list.size() << "\n";

    const LinkedList<std::string> list_copy(list);
    std::cout << "Copied list size: " << list_copy.size() << "\n";

    std::unordered_set<std::shared_ptr<Node<int>>> node_set;

    auto node1 = std::make_shared<Node<int>>(5);
    auto node2 = std::make_shared<Node<int>>(10);

    node_set.insert(node1);
    node_set.insert(node2);

    std::cout << "Hash set contains " << node_set.size() << " nodes.\n";

    return 0;
}
