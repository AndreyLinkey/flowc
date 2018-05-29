#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <pqxx/pqxx>
#include <string>
#include <list>

#include "field.h"

using namespace pqxx;

struct connection_info
{
    std::string dbname;
    std::string user;
    std::string password;
    std::string hostaddr;
    int port;

    connection_info()
        : dbname("postgres"), user("postgres"), password("postgres"), hostaddr("127.0.0.1"), port(5432)
    {

    }

    std::string to_string()
    {
        std::stringstream conninfo;
        conninfo << "dbname = " << dbname << " user = " << user
                 << " password = " << password << " hostaddr = " << hostaddr
                 << " port = " << port;
        return conninfo.str();
    }
};


class pgsql_transaction
{
public:
    pgsql_transaction(connection& db_connection);
    void append_data(flow_data data);
    void execute();
    std::list<flow_data> get_by_date(uint32_t from_date = 0, uint32_t to_date = UINT32_MAX);
    std::list<flow_data> get_by_id(unsigned int from_id = 0, unsigned int to_id = UINT32_MAX);
    unsigned int get_max_id();


private:
    work worker_;
    std::string statements_;
};

#endif // TRANSACTION_H
