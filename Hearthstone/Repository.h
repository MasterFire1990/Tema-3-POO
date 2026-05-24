#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <optional>


template<typename T>
concept Named = requires(const T& t) {
    { t.getName() } -> std::convertible_to<std::string>;
};

template<typename T>
class Repository {
    std::vector<std::unique_ptr<T>> items;

public:
    Repository() = default;
    Repository(const Repository&) = delete;
    Repository& operator=(const Repository&) = delete;

    Repository(Repository&&) = default;
    Repository& operator=(Repository&&) = default;

    ~Repository() = default;

    void add(std::unique_ptr<T> item) {
        items.push_back(std::move(item));
    }

    void remove(int index) {
        if (index < 0 || index >= static_cast<int>(items.size()))
            throw std::out_of_range("Repository: index invalid la remove");
        items.erase(items.begin() + index);
    }

    T* get(int index) const {
        if (index < 0 || index >= static_cast<int>(items.size()))
            throw std::out_of_range("Repository: index invalid la get");
        return items[index].get();
    }

    int size() const { return static_cast<int>(items.size()); }
    bool empty() const { return items.empty(); }

    void clear() { items.clear(); }

    
    T* findIf(std::function<bool(const T&)> predicate) const {
        auto it = std::find_if(items.begin(), items.end(),
            [&](const std::unique_ptr<T>& p) { return predicate(*p); });
        return it != items.end() ? it->get() : nullptr;
    }

    
    void sortBy(std::function<bool(const T&, const T&)> cmp) {
        std::sort(items.begin(), items.end(),
            [&](const std::unique_ptr<T>& a, const std::unique_ptr<T>& b) {
                return cmp(*a, *b);
            });
    }

    
    auto begin() { return items.begin(); }
    auto end()   { return items.end(); }
    auto begin() const { return items.cbegin(); }
    auto end()   const { return items.cend(); }

    
    template<typename U = T>
    void cloneFrom(const Repository<U>& other)
        requires requires(const U& u) { u.clone(); }
    {
        clear();
        for (const auto& item : other)
            items.push_back(std::unique_ptr<T>(static_cast<T*>(item->clone())));
    }
};




template<typename T>
void displayAll(const Repository<T>& repo, std::ostream& os) {
    if (repo.size() == 0) { os << "  (gol)\n"; return; }
    for (int i = 0; i < repo.size(); i++)
        os << "  " << (i + 1) << ". " << *repo.get(i) << "\n";
}


template<Named T>
T* findByName(const Repository<T>& repo, const std::string& name) {
    return repo.findIf([&](const T& item) { return item.getName() == name; });
}


template<typename T>
int countIf(const Repository<T>& repo, std::function<bool(const T&)> predicate) {
    int count = 0;
    for (int i = 0; i < repo.size(); i++)
        if (predicate(*repo.get(i))) count++;
    return count;
}