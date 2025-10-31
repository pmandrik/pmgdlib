  // P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef PMGDLIB_STD_HH
#define PMGDLIB_STD_HH 1

#include <map>
#include <unordered_map>
  
namespace pmgd {
  //=================================== MAP =========================================================
  template <typename K, typename V> V map_get(const std::map <K,V> & m, const K & key, const V & defval ) {
    typename std::map<K,V>::const_iterator it = m.find( key );
    if ( it == m.end() ) return defval;
    return it->second;
  }

  template <typename K, typename V> V unordered_map_get(const  std::unordered_map <K,V> & m, const K & key, const V & defval ) {
    typename std::unordered_map<K,V>::const_iterator it = m.find( key );
    if ( it == m.end() ) return defval;
    return it->second;
  }
};

#endif