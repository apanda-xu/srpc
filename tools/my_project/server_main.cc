#include <stdio.h>
#include <signal.h>
#include "workflow/WFHttpServer.h"
#include "workflow/WFFacilities.h"

#include "config/util.h"
#include "config/config.h"

static WFFacilities::WaitGroup wait_group(1);
static srpc::RPCConfig config;

void sig_handler(int signo)
{
    wait_group.done();
}

void init()
{
    if (config.load("./server.conf") == false)
    {
        perror("Load config failed");
        exit(1);
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
}

void process(WFHttpTask *task)
{
    // delete the example codes and fill your logic

    fprintf(stderr, "http server get request_uri: %s\n",
            task->get_req()->get_request_uri());
    print_peer_address(task);

    task->get_resp()->append_output_body("<html>Hello from server!</html>");
}

int main()
{
    init();

    WFHttpServer server(process);

    if (server.start(config.server_port()) == 0)
    {
        fprintf(stderr, "Http server started, port %u\n", config.server_port());
        wait_group.wait();
        server.stop();
    }
    else
        perror("server start");

    return 0;
}

