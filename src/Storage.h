#ifndef storage_h
#define storage_h

#include <map>
#include <string>

class Storage {
protected:
  using table_name_t = std::string;
  using id_t = uint;
  using field_t = std::string;
  using table_t = std::map<id_t,field_t>;
public:
  void insert(const table_name_t&, id_t, const field_t&);
  void truncate(const table_name_t&);
  std::string intersection() const;
  std::string symmetric_difference() const;
protected:
  table_t& getTable(const std::string&);
  table_t table_A;
  table_t table_B;
  const std::string table_name_A = "A";
  const std::string table_name_B = "B";
};

#endif