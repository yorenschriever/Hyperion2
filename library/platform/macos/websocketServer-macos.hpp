#pragma once
#include "log.hpp"
#include "platform/includes/midi-device.hpp"
#include "server-certificate.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebsocketServerMacOs : public WebsocketServer
{
public:
    // static WebsocketServer *createInstance();

    // using WebsocketMessageHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, std::string);

    WebsocketServerMacOs()
    {

        try
        {
            auto const address = net::ip::make_address("0.0.0.0");
            auto const port = static_cast<unsigned short>(std::atoi("8085"));

            // The io_context is required for all I/O
            net::io_context ioc{1};

            // The SSL context is required, and holds certificates
            ssl::context ctx{ssl::context::tlsv12};

            // This holds the self-signed certificate used by the server
            load_server_certificate(ctx);

            // The acceptor receives incoming connections
            tcp::acceptor acceptor{ioc, {address, port}};
            for (;;)
            {
                // This will receive the new connection
                tcp::socket socket{ioc};

                // Block until we get a connection
                acceptor.accept(socket);

                // Launch the session, transferring ownership of the socket
                std::thread(
                    &do_session,
                    std::move(socket),
                    std::ref(ctx))
                    .detach();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void send(RemoteWebsocketClient *client, std::string msg) override{};
    void sendOther(RemoteWebsocketClient *exclude, std::string msg) override{};
    void sendAll(std::string msg) override{};

    int connectionCount() override { return 0; };
    // void onMessage(WebsocketMessageHandler handler)
    // {
    //     this->handler = handler;
    // }

    int readMessage(uint8_t *buffer) override { return 0; };

    // void setRxBufferSize(int size)
    // {
    //     maxBufferSize = size;
    // }

private:
    static void
    do_session(tcp::socket socket, ssl::context &ctx)
    {
        try
        {
            // Construct the websocket stream around the socket
            websocket::stream<beast::ssl_stream<tcp::socket &>> ws{socket, ctx};

            // Perform the SSL handshake
            ws.next_layer().handshake(ssl::stream_base::server);

            // Set a decorator to change the Server of the handshake
            ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type &res)
                {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                                " websocket-server-sync-ssl");
                }));

            // Accept the websocket handshake
            ws.accept();

            for (;;)
            {
                // This buffer will hold the incoming message
                beast::flat_buffer buffer;

                // Read a message
                ws.read(buffer);

                // Echo the message back
                ws.text(ws.got_text());
                ws.write(buffer.data());
            }
        }
        catch (beast::system_error const &se)
        {
            // This indicates that the session was closed
            if (se.code() != websocket::error::closed)
                std::cerr << "Error: " << se.code().message() << std::endl;
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    // protected:
    //     WebsocketMessageHandler handler = NULL;
    //     std::deque<std::string *> bufferedMessages = std::deque<std::string *>();
    //     int maxBufferSize = 5;
};
