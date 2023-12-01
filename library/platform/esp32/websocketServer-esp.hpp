#pragma once
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
#include "webServer-esp.hpp"
#include <mutex>

class WebsocketServerEsp : public WebsocketServer
{
public:
    WebsocketServerEsp(WebServerEsp *server, const char * path)
    {
        start_wss_server(server,path);
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
        clients_mutex.lock();
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            send(ws, msg);
        }
        clients_mutex.unlock();
    };

    void sendAll(std::string msg) override
    {
        clients_mutex.lock();
        for (auto ws : clients)
            send(ws, msg);
        clients_mutex.unlock();
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
        vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        auto ws_exclude = (RemoteWebsocketClientEsp *)exclude;
        clients_mutex.lock();
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            send(ws, s_buf);
        }
        clients_mutex.unlock();
    };

    void sendAll(const char *message, ...) override
    {
        char s_buf[255];
        va_list args;
        va_start(args, message);
        vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        clients_mutex.lock();
        for (auto ws : clients)
            send(ws, s_buf);
        clients_mutex.unlock();
    };

    void send(RemoteWebsocketClient *client, uint8_t *bytes, int size) override
    {

        auto client2 = (RemoteWebsocketClientEsp *)client;

        //Log::info(TAG, "sending: binary to fd %d. number of clients: %d", client2->fd, client2->server->clients.size());

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
        clients_mutex.lock();
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            this->send(ws, bytes, size);
        }
        clients_mutex.unlock();
    };

    void sendAll(uint8_t *bytes, int size) override
    {
        clients_mutex.lock();
        for (auto ws : clients)
            send(ws, bytes, size);
        clients_mutex.unlock();
    };

    int connectionCount() override { return clients.size(); };

private:
    httpd_handle_t start_wss_server(WebServerEsp *server, const char* path)
    {
        Log::info(TAG, "Starting wss server: %s", path);

        ws.uri = path;
        ws.user_ctx = this;
        httpd_register_uri_handler(server->server, &ws);
        return server;
    }

    static esp_err_t ws_handler(httpd_req_t *req)
    {
        WebsocketServerEsp *server = (WebsocketServerEsp *)req->user_ctx;
        int sockfd = httpd_req_to_sockfd(req);

        if (req->method == HTTP_GET)
        {
            //Log::info(TAG, "Handshake done, the new connection was opened");

            //Log::info(TAG, "New WS client connected hd=%d,  fd=%d, path = %s", (int)req->handle, sockfd, server->ws.uri);

            auto client = new RemoteWebsocketClientEsp;
            client->hd = req->handle;
            client->fd = sockfd;
            client->server = server;
            server->clients_mutex.lock();
            server->clients.insert(client);
            server->clients_mutex.unlock();

            if (server->connectionHandler != nullptr)
                server->connectionHandler((RemoteWebsocketClient *)sockfd, server, server->connectionUserData);

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
                Log::error(TAG, "Failed to calloc memory for buf");
                return ESP_ERR_NO_MEM;
            }
            ws_pkt.payload = buf;
            /* Set max_len = ws_pkt.len to get the frame payload */
            ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
            if (ret != ESP_OK)
            {
                Log::error(TAG, "httpd_ws_recv_frame failed with %d", ret);
                free(buf);
                return ret;
            }
        }

        if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
        {
            Log::info(TAG, "client disconnected.");// number of clients before disconnect: %d", sockfd, server->clients.size());

            if (server->disconnectionHandler != nullptr)
                server->disconnectionHandler((RemoteWebsocketClient *)sockfd, server, server->disconnectionUserData);

            server->clients_mutex.lock();
            auto client = server->findClient(req->handle, sockfd);
            server->clients.erase(client);
            delete client;
            server->clients_mutex.unlock();

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

            auto client = server->findClient(req->handle, sockfd);

            if (server->handler != nullptr)
                server->handler(client, server, std::string((const char *)ws_pkt.payload, ws_pkt.len), server->userData);
        }

        free(buf);
        return ESP_OK;
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

    httpd_uri_t ws = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = WebsocketServerEsp::ws_handler,
    .user_ctx = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = true,
    .supported_subprotocol = nullptr};
    std::set<RemoteWebsocketClientEsp *> clients;

    static const char *TAG;

    std::mutex clients_mutex;
};

const char *WebsocketServerEsp::TAG = "wss_server";