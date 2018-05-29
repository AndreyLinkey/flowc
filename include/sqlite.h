#ifndef SQLITE_H
#define SQLITE_H

#include "field.h"
#include "sqlite_orm.h"

auto sqlite_storage(const std::string& db_name)
{
    using namespace sqlite_orm;
    auto storage = make_storage(db_name, make_table("flows",
        make_column("timestamp", &flow_data::timestamp),
        make_column("ip_src_addr", &flow_data::ip_src_addr),
        make_column("ip_dst_addr", &flow_data::ip_dst_addr),
        make_column("postnat_src_addr", &flow_data::postnat_src_addr)));
    storage.sync_schema();
    return storage;
}

typedef decltype(sqlite_storage("")) storage;

/*
class sqlite
{

public:
    sqlite();
    bool store_flow_data(flow_data data);

private:
    auto make_();
    typedef decltype(make_()) storage;

    storage storage_;


};
*/
#endif // SQLITE_H
