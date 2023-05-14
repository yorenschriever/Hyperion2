#include "websocketServer.hpp"

#include "esp_eth.h"
#include "esp_netif.h"
#include "log.hpp"
#include "lwip/sockets.h"
#include "sdkconfig.h"
#include "thread.hpp"
#include <algorithm>
#include <esp_event.h>
#include <esp_https_server.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <set>
#include <sys/param.h>

#include "hyperion.crt.h"
#include "hyperion.key.h"

class WebsocketServerEsp : public WebsocketServer
{
public:
    WebsocketServerEsp(unsigned short port)
    {
        start_wss_server(port);
    }

    ~WebsocketServerEsp() = default;

    using RemoteWebsocketClientEsp = struct
    {
        httpd_handle_t hd;
        int fd;
        WebsocketServerEsp *server;
    };

    void send(RemoteWebsocketClient *client, std::string msg) override
    {
        auto client2 = (RemoteWebsocketClientEsp *)client;

        // Log::info(TAG, "sending: %s to fd %d", msg.c_str(), client2->fd);

        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = (uint8_t *)msg.c_str();
        ws_pkt.len = msg.length();
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;

        httpd_ws_send_frame_async(client2->hd, client2->fd, &ws_pkt);
    };

    void sendOther(RemoteWebsocketClient *exclude, std::string msg) override
    {
        auto ws_exclude = (RemoteWebsocketClientEsp *)exclude;
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            send(ws, msg);
        }
    };

    void sendAll(std::string msg) override
    {
        for (auto ws : clients)
            send(ws, msg);
    };

    void send(RemoteWebsocketClient *client, const char *message, ...) override
    {
        char s_buf[255];
        va_list args;
        va_start(args, message);
        int sz = vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        auto client2 = (RemoteWebsocketClientEsp *)client;

        // Log::info(TAG, "sending: %s to fd %d", s_buf, client2->fd);

        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = (uint8_t *)s_buf;
        ws_pkt.len = sz;
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;

        httpd_ws_send_frame_async(client2->hd, client2->fd, &ws_pkt);
    };

    void sendOther(RemoteWebsocketClient *exclude, const char *message, ...) override
    {
        char s_buf[255];
        va_list args;
        va_start(args, message);
        int sz = vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        auto ws_exclude = (RemoteWebsocketClientEsp *)exclude;
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            send(ws, s_buf);
        }
    };

    void sendAll(const char *message, ...) override
    {
        char s_buf[255];
        va_list args;
        va_start(args, message);
        int sz = vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        for (auto ws : clients)
            send(ws, s_buf);
    };

    void send(RemoteWebsocketClient *client, uint8_t *bytes, int size) override
    {

        auto client2 = (RemoteWebsocketClientEsp *)client;

        // Log::info(TAG, "sending: binary to fd %d", client2->fd);

        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = bytes;
        ws_pkt.len = size;
        ws_pkt.type = HTTPD_WS_TYPE_BINARY;

        httpd_ws_send_frame_async(client2->hd, client2->fd, &ws_pkt);
    };

    void sendOther(RemoteWebsocketClient *exclude, uint8_t *bytes, int size) override
    {
        auto ws_exclude = (RemoteWebsocketClientEsp *)exclude;
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            this->send(ws, bytes, size);
        }
    };

    void sendAll(uint8_t *bytes, int size) override
    {
        for (auto ws : clients)
            send(ws, bytes, size);
    };

    int connectionCount() override { return clients.size(); };

private:
    httpd_handle_t start_wss_server(unsigned short port)
    {
        httpd_handle_t server = NULL;
        Log::info(TAG, "Starting wss server");

        httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
        conf.httpd.max_open_sockets = max_clients;
        conf.httpd.global_user_ctx = this;
        conf.httpd.open_fn = wss_open_fd;
        conf.httpd.close_fn = wss_close_fd;
        conf.port_secure = port;
        conf.port_insecure = 82;

        conf.servercert = HYPERION_CERT;
        conf.servercert_len = HYPERION_CERT_SIZE + 1;
        conf.prvtkey_pem = HYPERION_KEY;
        conf.prvtkey_len = HYPERION_KEY_SIZE + 1;
        conf.httpd.ctrl_port = port + 5000;

        Log::info(TAG, "Starting HTTP Server for websocket on port: '%d' (%d)", conf.port_secure, conf.port_insecure);
        esp_err_t ret = httpd_ssl_start(&server, &conf);
        if (ESP_OK != ret)
        {
            Log::error(TAG, "Error starting server!");
            return NULL;
        }

        // Log::info(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &ws);
        return server;
    }

    static esp_err_t ws_handler(httpd_req_t *req)
    {
        if (req->method == HTTP_GET)
        {
            Log::info(TAG, "Handshake done, the new connection was opened");
            return ESP_OK;
        }
        httpd_ws_frame_t ws_pkt;
        uint8_t *buf = NULL;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

        // First receive the full ws message
        /* Set max_len = 0 to get the frame len */
        esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
        if (ret != ESP_OK)
        {
            Log::error(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
            return ret;
        }
        // Log::info(TAG, "frame len is %d", ws_pkt.len);
        if (ws_pkt.len)
        {
            /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
            buf = (uint8_t *)calloc(1, ws_pkt.len + 1);
            if (buf == NULL)
            {
                ESP_LOGE(TAG, "Failed to calloc memory for buf");
                return ESP_ERR_NO_MEM;
            }
            ws_pkt.payload = buf;
            /* Set max_len = ws_pkt.len to get the frame payload */
            ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
                free(buf);
                return ret;
            }
        }

        if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
        {
            // Response CLOSE packet with no payload to peer
            ws_pkt.len = 0;
            ws_pkt.payload = NULL;
            ret = httpd_ws_send_frame(req, &ws_pkt);
            free(buf);
            return ret;
        }

        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT)
        {
            // Log::info(TAG, "Received packet with message: %s. hd=%d, fd=%d", ws_pkt.payload, (int)req->handle, httpd_req_to_sockfd(req));

            WebsocketServerEsp *server = (WebsocketServerEsp *)httpd_get_global_user_ctx(req->handle); // req->user_ctx;

            auto client = server->findClient(req->handle, httpd_req_to_sockfd(req));

            if (server->handler != nullptr)
                server->handler(client, server, std::string((const char *)ws_pkt.payload, ws_pkt.len), server->userData);
        }

        free(buf);
        return ESP_OK;
    }

    static esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
    {
        Log::info(TAG, "New client connected hd=%d,  fd=%d", (int)hd, sockfd);
        WebsocketServerEsp *server = (WebsocketServerEsp *)httpd_get_global_user_ctx(hd);

        auto client = new RemoteWebsocketClientEsp;
        client->hd = hd;
        client->fd = sockfd;
        client->server = server;
        server->clients.insert(client);

        if (server->connectionHandler != nullptr)
            // TODO: thread is maybe bit heavy.. but we have to wait for this function to return before sending websocket data..
            // is there another way?
            Thread::create(connectionHandlerTask, "ws_conn_hdl", Thread::Purpose::control, 30000, client, 1);
        // server->connectionHandler((RemoteWebsocketClient *)sockfd, server, server->connectionUserData);
        return ESP_OK;
    }

    static void connectionHandlerTask(void *param)
    {
        auto client = (RemoteWebsocketClientEsp *)param;
        // wait for the handshake to complete before sending init data
        // TODO actually test and wait
        Thread::sleep(500);
        client->server->connectionHandler((RemoteWebsocketClient *)client->fd, client->server, client->server->connectionUserData);
        Thread::destroy();
    }

    static void wss_close_fd(httpd_handle_t hd, int sockfd)
    {
        WebsocketServerEsp *server = (WebsocketServerEsp *)httpd_get_global_user_ctx(hd);
        Log::info(TAG, "client disconnected %d. number of clients before disconnect: %d", sockfd, server->clients.size());

        auto client = server->findClient(hd, sockfd);
        server->clients.erase(client);
        delete client;

        // Log::info(TAG, "number of clients after disconnect: %d", server->clients.size());
    }

    RemoteWebsocketClientEsp *findClient(httpd_handle_t hd, int fd)
    {
        // Log::info(TAG,"Looking for client in list of %d clients", clients.size());
        auto result = std::find_if(
            clients.begin(), clients.end(),
            [hd, fd](const RemoteWebsocketClientEsp *x)
            { return x->hd == hd && x->fd == fd; });
        if (result == clients.end())
        {
            Log::error(TAG, "findClient: WS client not known to server");
            return nullptr;
        }
        // Log::info(TAG,"client found. fd=%d", (*result)->fd);
        return *result;
    }

    static const httpd_uri_t ws;
    static const char *TAG;
    static const size_t max_clients;

    std::set<RemoteWebsocketClientEsp *> clients;
};

const httpd_uri_t WebsocketServerEsp::ws = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = WebsocketServerEsp::ws_handler,
    .user_ctx = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = true};

const char *WebsocketServerEsp::TAG = "wss_server";
const size_t WebsocketServerEsp::max_clients = 4;