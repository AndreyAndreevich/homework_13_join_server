#include "Storage.h"
#include <exception>
#include <sstream>
#include <algorithm> 

Storage::table_t& Storage::getTable(const std::string& table_name) {
  if (table_name == table_name_A) {
    return table_A;
  } else if (table_name == table_name_B) {
    return table_B;
  } 
  throw std::runtime_error("Table " + table_name + " not exist");
}


void Storage::insert(const table_name_t& table_name, id_t id, const field_t& field) {
  if (table_name.empty()) {
    throw std::runtime_error("Table name is empty: " + table_name);
  }
  decltype(auto) table = getTable(table_name);
  if (table.empty()) {
    table.emplace(id,field);
  } else {
    auto field_iter = table.lower_bound(id);
    if (field_iter != table.end() && field_iter->first == id) {
      throw std::runtime_error("duplicate " + std::to_string(id));
    }
    table.emplace_hint(field_iter,id,field);
  }
}

void Storage::truncate(const table_name_t& table_name) {
  decltype(auto) table = getTable(table_name);
  table.clear();
}

std::string Storage::intersection() const
{
  auto* t1 = &table_A;
  auto* t2 = &table_B;
  if (t1->empty() || t2->empty()) {
    return "";
  }
  std::stringstream result_stream;
  auto iter1 = t1->cbegin();
  auto iter2 = t2->lower_bound(iter1->first);
  bool is_right = true;
  while(iter1 != t1->cend() && iter2 != t2->cend()) {
    if (iter1->first == iter2->first) {
      result_stream << iter1->first << ",";
      if (is_right) {
        result_stream << iter1->second << "," << iter2->second << "\n";
      } else {
        result_stream << iter2->second << "," << iter1->second << "\n";
      }
      iter2 = std::next(iter2);
    }
    iter1 = t1->lower_bound(iter2->first);
    std::swap(iter1,iter2);
    std::swap(t1,t2);
    is_right = !is_right;
  }
  return result_stream.str();
}

std::string Storage::symmetric_difference() const
{
  if (table_A.empty() && table_B.empty())
    return "";

  std::stringstream result_stream;
  auto write_one = [&result_stream](decltype(table_A.cbegin())& table_iter, bool is_right) {
    result_stream << table_iter->first << ",";
    if (is_right) {
      result_stream << table_iter->second << ",";
    } else {
      result_stream << "," << table_iter->second;
    }
    result_stream << "\n";
  };

  auto write_stream = [&write_one](
    decltype(table_A.cbegin())& table_iter_cbegin, 
    decltype(table_A.cbegin()) table_iter_cend, 
    bool is_right) {
    for (;table_iter_cbegin != table_iter_cend; table_iter_cbegin++) {
      write_one(table_iter_cbegin,is_right);
    }
  };

  auto table_iter_1 = table_A.cbegin();
  auto table_iter_2 = table_B.cbegin();

  while(table_iter_1 != table_A.cend() && table_iter_2 != table_B.cend()) {
    if (table_iter_1->first == table_iter_2->first) {
      table_iter_1 = std::next(table_iter_1);
      table_iter_2 = std::next(table_iter_2);
    } else if (table_iter_1->first > table_iter_2->first) {
      write_stream(table_iter_2,table_B.lower_bound(table_iter_1->first),false);
    } else {
      write_stream(table_iter_1,table_A.lower_bound(table_iter_2->first),true);
    }
  }

  if (table_iter_1 == table_A.cend()) {
    write_stream(table_iter_2,table_B.cend(),false);
  } else {
    write_stream(table_iter_1,table_A.cend(),true);
  }

  return result_stream.str();
}
