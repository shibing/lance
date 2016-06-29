#include "connection.hpp"
#include "thread_group.hpp"
#include "lance.hpp"
#include "util.hpp"
#include <asio.hpp>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <getopt.h>


struct config {
    size_t connections;
    int duration;
    std::string method;
    std::string url;
};

static bool parse_arg(config &cfg, int argc, char *argv[]);
void display_usage();

int main(int argc, char *argv[])
{
    config cfg;
    if (!parse_arg(cfg, argc, argv)) {
        display_usage();
        exit(EXIT_FAILURE);
    }

    lance::lance l{cfg.connections, cfg.duration};
    l.run();
    l.output_result();
    return 0;
}

static bool parse_arg(config &cfg, int argc, char *argv[])
{
    cfg.connections = 100;
    cfg.duration = 3;
    cfg.method = "GET";
    cfg.url = "";
    static option longopts[] = {
        {"connections", required_argument, NULL, 'c'},
        {"duration", required_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};
    int opt;
    while ((opt = getopt_long(argc, argv, "c:d:h?", longopts, NULL)) != -1) {
        switch (opt) {
            case 'c':
                cfg.connections = atoi(optarg);
                break;
            case 'd':
                cfg.duration = atoi(optarg);
                break;
            case 'h':
            case '?':
            default:
                return false;
        }
    }
    if (optind >= argc) return false;
    cfg.url = argv[optind];
    return cfg.connections > 0 && cfg.duration > 0;
}

void display_usage()
{
    std::cout << "Usage: lance <options> <url>                  \n"
                "  Options:                                     \n"
                "    -c, --connections <N>  connections to open \n"
                "    -d, --duration    <D>  time to run         \n" 
                "    -h, --help             display usage       \n";
}
