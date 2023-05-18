#pragma once

#include "websocketServer.hpp"
//#include "webServer-macos.hpp"

#include "log.hpp"
//#include "server-certificate.hpp"
// #include "thread.hpp"
// #include "utils.hpp"
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/ssl/stream.hpp>
// #include <boost/beast/core.hpp>
// #include <boost/beast/ssl.hpp>
// #include <boost/beast/websocket.hpp>
// #include <boost/beast/websocket/ssl.hpp>
// #include <cstdlib>
// #include <functional>
// #include <iostream>
// #include <mutex>
#include <set>
// #include <stdarg.h>
#include <string>
// #include <thread>

#include "webServer-macos-ws.hpp"
#include "websocketServerSessionReceiver.hpp"


// namespace beast = boost::beast;         // from <boost/beast.hpp>
// namespace http = beast::http;           // from <boost/beast/http.hpp>
// namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
// namespace net = boost::asio;            // from <boost/asio.hpp>
// namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
// using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebsocketServerMacOs : public WebsocketServer, public WebSocketServerSessionReceiver
{
public:
    // using WS = websocket::stream<beast::ssl_stream<tcp::socket &>>;
    using WS = websocket_session<ssl_websocket_session>;
    //using WS = void;

    WebsocketServerMacOs(WebServer *server, const char *path)
    {
        // int port = 9800;
        // std::thread{std::bind(
        //                 &start_server,
        //                 this, port)}
        //     .detach();
        ((WebServerMacOsWs *)server)->addPathWs(path, this);
    }

    ~WebsocketServerMacOs() = default;

    void send(RemoteWebsocketClient *client, std::string msg) override
    {
        try
        {
            Log::info(TAG, "sending: %s", msg.c_str());
            //auto ws = (WS *)client;
            //ws->write_txt(msg.c_str(),msg.size());
            // beast::flat_buffer buffer(msg.size());
            // auto buf = buffer.prepare(msg.size());
            // memcpy(buf.data(), msg.c_str(), msg.size());
            // buffer.commit(msg.size());
            // ws->text(true);
            // ws->write(buffer.data());
        }
        catch (const std::exception &e)
        {
            Log::error(TAG, "Error sending: %d", e.what());
        }
    };

    void sendOther(RemoteWebsocketClient *exclude, std::string msg) override
    {
        auto ws_exclude = (WS *)exclude;
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
        try
        {
            char s_buf[255];
            va_list args;
            va_start(args, message);
            int sz = vsnprintf(s_buf, sizeof(s_buf), message, args);
            va_end(args);

            Log::info(TAG, "sending: %s", s_buf);
            // auto ws = (WS *)client;
            // beast::flat_buffer buffer(sz);
            // auto buf = buffer.prepare(sz);
            // memcpy(buf.data(), s_buf, sz);
            // buffer.commit(sz);
            // ws->text(true);
            // ws->write(buffer.data());
        }
        catch (const std::exception &e)
        {
            Log::error(TAG, "Error sending: %d", e.what());
        }
    };

    void sendOther(RemoteWebsocketClient *exclude, const char *message, ...) override
    {
        char s_buf[255];
        va_list args;
        va_start(args, message);
        int sz = vsnprintf(s_buf, sizeof(s_buf), message, args);
        va_end(args);

        auto ws_exclude = (WS *)exclude;
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
        try
        {
            Log::info(TAG, "Sending binary");

            // auto ws = (WS *)client;
            // beast::flat_buffer buffer(size);
            // auto buf = buffer.prepare(size);
            // memcpy(buf.data(), bytes, size);
            // buffer.commit(size);
            // ws->text(false);
            // ws->write(buffer.data());
        }
        catch (const std::exception &e)
        {
            Log::error(TAG, "Error sending binary: %s", e.what());
        }
    };

    void sendOther(RemoteWebsocketClient *exclude, uint8_t *bytes, int size) override
    {
        auto ws_exclude = (WS *)exclude;
        for (auto ws : clients)
        {
            if (ws == ws_exclude)
                continue;
            send(ws, bytes, size);
        }
    };

    void sendAll(uint8_t *bytes, int size) override
    {
        for (auto ws : clients)
            send(ws, bytes, size);
    };

    int connectionCount() override { return clients.size(); };

    void on_connect(WS *session) override
    {
        clients.insert(session);

        if (connectionHandler != nullptr)
            connectionHandler(session, this, connectionUserData);
    }

    void on_receive(WS *session, std::string message) override
    {
        if (handler != nullptr)
            handler(session, this, message, userData);
    }

    void on_disconnect(WS *session) override
    {
        clients.erase(session);
    }






//=======
    // private:
    //     static void start_server(WebsocketServerMacOs *server, unsigned short port)
    //     {
    //         try
    //         {
    //             auto const address = net::ip::make_address("0.0.0.0");
    //             auto const port_ = static_cast<unsigned short>(port);

    //             // The io_context is required for all I/O
    //             net::io_context ioc{1};

    //             // The SSL context is required, and holds certificates
    //             ssl::context ctx{ssl::context::tlsv12};

    //             // This holds the self-signed certificate used by the server
    //             load_server_certificate(ctx);

    //             // The acceptor receives incoming connections
    //             tcp::acceptor acceptor{ioc, {address, port_}};
    //             for (;;)
    //             {
    //                 // This will receive the new connection
    //                 tcp::socket socket{ioc};

    //                 // Block until we get a connection
    //                 acceptor.accept(socket);

    //                 Thread::sleep(Utils::random(0,100));
    //                 // Launch the session, transferring ownership of the socket
    //                 std::thread(
    //                     &do_session,
    //                     std::move(socket),
    //                     std::ref(ctx),
    //                     server)
    //                     .detach();
    //             }
    //         }
    //         catch (const std::exception &e)
    //         {
    //             Log::error(TAG, "Error 3: %s", e.what());
    //         }
    //     }

    //     static void
    //     do_session(tcp::socket socket, ssl::context &ctx, WebsocketServerMacOs *server)
    //     {
    //         WS *p_ws = nullptr;
    //         try
    //         {
    //             // Construct the websocket stream around the socket
    //             WS ws{socket, ctx};
    //             p_ws = &ws;

    //             // Perform the SSL handshake
    //             ws.next_layer().handshake(ssl::stream_base::server);

    //             // Set a decorator to change the Server of the handshake
    //             ws.set_option(websocket::stream_base::decorator(
    //                 [](websocket::response_type &res)
    //                 {
    //                     res.set(http::field::server,
    //                             std::string(BOOST_BEAST_VERSION_STRING) +
    //                                 " websocket-server-sync-ssl");
    //                 }));

    //             // Accept the websocket handshake
    //             ws.accept();

    //             server->insertClient(p_ws);
    //             if (server->connectionHandler != nullptr)
    //                 server->connectionHandler(p_ws, server, server->connectionUserData);

    //             for (;;)
    //             {
    //                 // This buffer will hold the incoming message
    //                 beast::flat_buffer buffer;

    //                 // Read a message
    //                 ws.read(buffer);

    //                 auto str = beast::buffers_to_string(buffer.data());

    //                 if (server->handler != nullptr)
    //                     server->handler(p_ws, server, str, server->userData);

    //                 //Log::info(TAG, "received %s", str.c_str());
    //             }
    //         }
    //         catch (beast::system_error const &se)
    //         {
    //             // This indicates that the session was closed
    //             if (se.code() != websocket::error::closed)
    //                 Log::error(TAG, "Error 1: %s", se.code().message().c_str());
    //             server->removeClient(p_ws);
    //         }
    //         catch (std::exception const &e)
    //         {
    //             Log::error(TAG, "Error 2: %s", e.what());
    //             server->removeClient(p_ws);
    //         }
    //         server->removeClient(p_ws);
    //     }

private:
    std::set<WS *> clients;
    static const char *TAG;
    // std::mutex mtx;

    // void insertClient(WS * client){
    //     mtx.lock();
    //     clients.insert(client);
    //     //Log::info(TAG,"Websocket client connected. clients: %d",clients.size());
    //     mtx.unlock();
    // }

    // void removeClient(WS * client){
    //     if (!client)
    //         return;
    //     mtx.lock();
    //     clients.erase(client);
    //     //Log::info(TAG,"Websocket client disconnected. clients: %d",clients.size());
    //     mtx.unlock();
    // }
};

const char *WebsocketServerMacOs::TAG = "WEBSOCKET_SERVER";