#include <map>
#include <iterator>

template <typename MapType>
class MapValueIterator {
public:
    using IteratorType = typename MapType::const_iterator;

    MapValueIterator(IteratorType iter) : m_iter(iter) {}

    auto operator*() const {
        return m_iter->second;
    }

    MapValueIterator& operator++() {
        ++m_iter;
        return *this;
    }

    bool operator!=(const MapValueIterator& other) const {
        return !(m_iter == other.m_iter);
    }

    bool operator==(const MapValueIterator& other) const {
        return m_iter == other.m_iter;
    }

private:
    IteratorType m_iter;
};