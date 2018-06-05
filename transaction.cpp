#include <iostream>

#include "include/transaction.h"

pgsql_transaction::pgsql_transaction()
{

}

void pgsql_transaction::append_data(flow_data data)
{
    std::stringstream sql;
    sql << "INSERT INTO flow_data (timestamp,ip_src_addr,ip_dst_addr,postnat_src_addr) VALUES ("
        << static_cast<int_least32_t>(data.timestamp) << ", "
        << static_cast<int_least32_t>(data.ip_src_addr) << ", "
        << static_cast<int_least32_t>(data.ip_dst_addr) << ", "
        << static_cast<int_least32_t>(data.postnat_src_addr) << "); ";
    statements_ += sql.str();
    rows_++;
}

void pgsql_transaction::execute(connection& db_connection)
{
    try {
        work worker(db_connection);
        worker.exec(statements_.c_str());
        worker.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    rows_ = 0;
    statements_ = "";
}

std::list<flow_data> pgsql_transaction::get_by_date(connection& db_connection, uint32_t from_date, uint32_t to_date)
{
    std::stringstream sql;
    sql << "SELECT timestamp, ip_src_addr, ip_dst_addr, postnat_src_addr FROM flow_data WHERE timestamp >= "
        << static_cast<int_fast32_t>(from_date) << " AND timestamp < "
        << static_cast<int_fast32_t>(to_date) << ";";
    std::list<flow_data> flow_data_list;
    try
    {
        work worker(db_connection);
        result res(worker.exec(sql.str().c_str()));
        for(result::const_iterator c = res.begin(); c != res.end(); ++c)
        {
            flow_data data;
            data.timestamp = static_cast<uint32_t>(c[0].as<int_fast32_t>());
            data.ip_src_addr = static_cast<uint32_t>(c[1].as<int_fast32_t>());
            data.ip_dst_addr = static_cast<uint32_t>(c[2].as<int_fast32_t>());
            data.postnat_src_addr = static_cast<uint32_t>(c[3].as<int_fast32_t>());
            flow_data_list.push_back(data);
        }
    }
        catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return flow_data_list;
}

std::list<flow_data> pgsql_transaction::get_by_id(connection& db_connection, unsigned int from_id, unsigned int to_id)
{
    std::stringstream sql;
    sql << "SELECT * FROM flow_data WHERE id >= "
        << static_cast<int_fast32_t>(from_id) << " AND id < "
        << static_cast<int_fast32_t>(to_id) << ";";
    std::list<flow_data> flow_data_list;
    try
    {
        work worker(db_connection);
        result res(worker.exec(sql.str().c_str()));
        for(result::const_iterator c = res.begin(); c != res.end(); ++c)
        {
            flow_data data;
            data.timestamp = static_cast<uint32_t>(c[1].as<int_fast32_t>());
            data.ip_src_addr = static_cast<uint32_t>(c[2].as<int_fast32_t>());
            data.ip_dst_addr = static_cast<uint32_t>(c[3].as<int_fast32_t>());
            data.postnat_src_addr = static_cast<uint32_t>(c[4].as<int_fast32_t>());
            flow_data_list.push_back(data);
        }
    }
        catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return flow_data_list;
}


unsigned int pgsql_transaction::get_max_id(connection& db_connection)
{
    std::string sql_query("SELECT MAX(id) FROM flow_data;");

    unsigned int max_id = 0;

    try
    {
        work worker(db_connection);
        result res(worker.exec(sql_query.c_str()));
        max_id = static_cast<unsigned int>(res.begin()[0].as<unsigned int>());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return max_id;
}

std::size_t pgsql_transaction::row_count()
{
    return rows_;
}

void pgsql_transaction::clear()
{
    statements_ = "";
}
