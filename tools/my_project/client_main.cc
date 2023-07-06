#include <stdio.h>
#include <signal.h>
#include "workflow/HttpMessage.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/WFFacilities.h"

#include "config/config.h"

static WFFacilities::WaitGroup wait_group(1);
static srpc::RPCConfig config;

void sig_handler(int signo)
{
    wait_group.done();
}

void init()
{
    if (config.load("./client.conf") == false)
    {
        perror("Load config failed");
        exit(1);
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
}

void callback(WFHttpTask *task)
{
    int state = task->get_state();
    int error = task->get_error();
    fprintf(stderr, "Http client state = %d error = %d\n", state, error);

     if (state == WFT_STATE_SUCCESS) // print server response body
     {
        const void *body;
        size_t body_len;

        task->get_resp()->get_parsed_body(&body, &body_len);
        fwrite(body, 1, body_len, stdout);
        fflush(stdout);
     }
}

int main()
{
    init();

    std::string url = std::string("http://") + config.client_host() +
                      std::string(":") + std::to_string(config.client_port());

    WFHttpTask *task = WFTaskFactory::create_http_task(url,
                                                        config.redirect_max(),
                                                        config.retry_max(),
                                                        callback);

    task->start();

    wait_group.wait();
    return 0;
}

