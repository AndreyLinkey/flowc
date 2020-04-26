
#include <algorithm>
#include <filesystem>
#include <vector>

#include "include/crow.h"
#include "include/common.h"
#include "include/container.h"
#include "include/field.h"
#include "include/settings.h"


using directory_list = std::vector<std::filesystem::directory_entry>;

directory_list file_list(std::string directory, uint32_t start_from, uint32_t end_at)
{
    directory_list files;
    std::filesystem::directory_iterator dir(directory);
    copy_if(std::filesystem::begin(dir), std::filesystem::end(dir), std::back_inserter(files),
        [](const std::filesystem::directory_entry& entry)
        {
            return entry.status().type() == std::filesystem::file_type::regular &&
                   entry.path().extension().string() == OUTPUT_EXTENSION &&
                   std::regex_search(entry.path().stem().string(), std::regex("^\\d{8}T\\d{6}$"));
        });

    std::sort(files.begin(), files.end(),
        [](std::filesystem::directory_entry& entry1, std::filesystem::directory_entry& entry2)
        {
            return entry1.path().stem().string() < entry2.path().stem().string();
        });

    directory_list::iterator first_file = std::upper_bound(files.begin(), files.end(), timestamp_to_timestr(start_from),
        [](std::string val, const std::filesystem::directory_entry& entry) -> bool
        {
            return val < entry.path().stem().string();
        });

    if(first_file != files.begin())
      first_file--;

    directory_list::iterator last_file = std::upper_bound(files.begin(), files.end(), timestamp_to_timestr(end_at),
        [](std::string val, const std::filesystem::directory_entry& entry) -> bool
        {
            return val < entry.path().stem().string();
        });

    return directory_list(first_file, last_file);
}

int main(int argc, const char *argv[])
{
    std::string directory = OUTPUT_DIRECTORY;
    uint16_t flowc_port = FLOWC_PORT;

    try
    {
        settings cfg = settings::load_config(CONFIG_NAME);
        directory = cfg.output_directory();
        flowc_port = cfg.flowc_port();
    }
    catch (const std::exception &e)
    {
        std::cout << "error while loading config file " << e.what() << std::endl;
    }
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([]()
    {
        return crow::response(404);
    });

    CROW_ROUTE(app, "/log")
    ([&directory](const crow::request& req)
    {
        crow::json::wvalue data;
        try
        {
            uint32_t start_from = req.url_params.get("from") == nullptr ? 0 : timestr_to_timestamp(req.url_params.get("from"));
            uint32_t end_at = req.url_params.get("to") == nullptr ? static_cast<uint32_t>(time(nullptr)) : timestr_to_timestamp(req.url_params.get("to"));
            uint32_t ip_src_addr = req.url_params.get("src") == nullptr ? 0 : ipstr_to_ipnum(req.url_params.get("src"));
            uint32_t ip_dst_addr = req.url_params.get("dst") == nullptr ? 0 : ipstr_to_ipnum(req.url_params.get("dst"));
            uint32_t postnat_src_addr = req.url_params.get("postnat") == nullptr ? 0 : ipstr_to_ipnum(req.url_params.get("postnat"));

            if(end_at - start_from > 86400 || (ip_src_addr | ip_dst_addr | postnat_src_addr) == 0)
            {
                return crow::response(400);
            }

            directory_list files = file_list(directory, start_from, end_at);

//            std::cout << "required files: ";
//            for(auto f: files)
//                std::cout << f.path().string();
//            std:: cout << std::endl;

            unsigned int idx = 0;
            for(std::filesystem::directory_entry& file: files)
            {
                container cont;
                cont.open_file(file.path().string());

//                std::cout << "openning file: " << file.path().string() << std::endl;

                while(true)
                {
                    std::vector<flow_data> flows = cont.read_flows(512, start_from, end_at, ip_src_addr, ip_dst_addr, postnat_src_addr);
                    if(flows.size() == 0)
                    {
                        break;
                    }

                    for(flow_data& item: flows)
                    {
                        crow::json::wvalue& json_item = data[idx];
                        json_item["time"] = timestamp_to_timestr(item.timestamp);
                        json_item["src"] = ipnum_to_ipstr(item.ip_src_addr);
                        json_item["dst"] = ipnum_to_ipstr(item.ip_dst_addr);
                        json_item["postnat"] = ipnum_to_ipstr(item.postnat_src_addr);
                        idx++;
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "error in parameters " << e.what() << std::endl;
            return crow::response(400);
        }
        return crow::response(data);
    });

    //app.loglevel(crow::LogLevel::Warning);
    app.port(flowc_port).run();
    return 0;
}
