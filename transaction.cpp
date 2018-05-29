#include <iostream>

#include "include/transaction.h"

pgsql_transaction::pgsql_transaction(connection &db_connection)
    : worker_(db_connection)
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
}

void pgsql_transaction::execute()
{
    try {
        worker_.exec(statements_.c_str());
        worker_.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

std::list<flow_data> pgsql_transaction::get_by_date(uint32_t from_date, uint32_t to_date)
{
    std::stringstream sql;
    sql << "SELECT timestamp, ip_src_addr, ip_dst_addr, postnat_src_addr FROM flow_data WHERE timestamp >= "
        << static_cast<int_fast32_t>(from_date) << " AND timestamp < "
        << static_cast<int_fast32_t>(to_date) << ";";
    std::list<flow_data> flow_data_list;
    try
    {
        result res(worker_.exec(sql.str().c_str()));
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

std::list<flow_data> pgsql_transaction::get_by_id(unsigned int from_id, unsigned int to_id)
{
    std::stringstream sql;
    sql << "SELECT * FROM flow_data WHERE id >= "
        << static_cast<int_fast32_t>(from_id) << " AND id < "
        << static_cast<int_fast32_t>(to_id) << ";";
    std::list<flow_data> flow_data_list;
    try
    {
        result res(worker_.exec(sql.str().c_str()));
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


unsigned int pgsql_transaction::get_max_id()
{
    std::string sql_query("SELECT MAX(id) FROM flow_data;");

    unsigned int max_id = 0;

    try
    {
        result res(worker_.exec(sql_query.c_str()));
        max_id = static_cast<unsigned int>(res.begin()[0].as<unsigned int>());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return max_id;
}


/*
    char * sql;

       // Create SQL statement
       sql = "SELECT * from COMPANY";

       // Create a non-transactional object.
       nontransaction N(C);

       // Execute SQL query
       result R( N.exec( sql ));

       // List down all the records
       for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
          cout << "ID = " << c[0].as<int>() << endl;
          cout << "Name = " << c[1].as<string>() << endl;
          cout << "Age = " << c[2].as<int>() << endl;
          cout << "Address = " << c[3].as<string>() << endl;
          cout << "Salary = " << c[4].as<float>() << endl;
       }
       cout << "Operation done successfully" << endl;
       C.disconnect ();
    } catch (const std::exception &e) {
       cerr << e.what() << std::endl;
       return 1;
    }

    return 0;
*/
