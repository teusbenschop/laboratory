#include <map>
#include <limits>
#include <iostream>
#include <iterator>


using namespace std;


template <typename K, typename V>

// The class interval_map <K,V> is a data structure that efficiently associates intervals of keys of type K with values of type V.
class interval_map {

public:

  // The constructor associates the whole range of K with val
  // by inserting (K_min, val) into the map.
  // In case of K as an integer, it inserts (0, val) into the map.
  interval_map (V const & val) {
    m_map.insert (m_map.begin(), make_pair (numeric_limits<K>::min(), val));
  }

  // Look up the value associated with a given key.
  // Each key-value-pair (k,v) in the map means
  // that the value v is associated with the interval from k (including)
  // to the next key (excluding) in the map.
  // Example: the map (0,'A'), (3,'B'), (5,'A') represents the mapping
  // 0 -> 'A'
  // 1 -> 'A'
  // 2 -> 'A'
  // 3 -> 'B'
  // 4 -> 'B'
  // 5 -> 'A'
  // 6 -> 'A'
  // 7 -> 'A'
  // 8 -> 'A'
  // ... and so on all the way to numeric_limits<int>::max()
  V & operator [] (K const & key) {
    return (--m_map.upper_bound(key))->second;
  }
  
  // Insert K -> V into the map.
  // Do error checking.
  bool assign (K const& key_start, K const& key_end, V const& val) {

    // Keys should be in the correct order.
    if (!(key_start < key_end)) return false;
    
    // Don't insert identical values in sequence.
    V existing_value =  (--m_map.upper_bound(key_start))->second;
    if (existing_value == val) {
      cout << "Attempting to insert " << key_start << " -> " << val << " into the internal map" << endl;
      cout << "But the existing value for key " << key_start << " is " << existing_value <<  " so aborting ..." << endl;
      return false;
    }

    // Insert the key and value in the internal map.
    m_map [key_start] = val;

    // Success.
    return true;
  }
  
  // Printer the interval map contents.
  void print_interval_map (K start, K end) {
    for (K key = start; key <= end; key++) {
      V value = (--m_map.upper_bound(key))->second;
      cout << key << " -> " << value << endl;
    }
  }
  
private:
  
  // The object is implemented on top of map.
  map <K,V> m_map;

};


int main() {
  
  // Initialize the interval_map object with a value.
  interval_map <unsigned int, char> imap {'a'};
  
  // Add some more values to the internal map for debugging purposes.
  imap.assign (0, 1, 'A');
  imap.assign (3, 4, 'B');
  imap.assign (5, 6, 'A');

  imap.print_interval_map (0, 8);

  return 0;
}
