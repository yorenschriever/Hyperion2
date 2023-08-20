#pragma once
#include "log.hpp"
#include "server-certificate.hpp"
#include "webServer.hpp"
#include "websocketServer.hpp"
#include "webServerResponseBuilder.hpp"
#include <algorithm>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/config.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include "webServer-buffer-body-str.hpp"

template <class Derived>
class websocket_session;

class ssl_websocket_session;

class WebSocketServerSessionReceiver
{
public:
    virtual void on_connect(websocket_session<ssl_websocket_session> *session) = 0;
    virtual void on_receive(websocket_session<ssl_websocket_session> *session, std::string message) = 0;
    virtual void on_disconnect(websocket_session<ssl_websocket_session> *session) = 0;
};

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using PathMap = std::map<std::string, WebServerResponseBuilder *>;
using PathMapWs = std::map<std::string, WebSocketServerSessionReceiver *>;

// Return a reasonable mime type based on the extension of a file.
beast::string_view
mime_type(beast::string_view path) //;
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if (pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm"))
        return "text/html";
    if (iequals(ext, ".html"))
        return "text/html";
    if (iequals(ext, ".php"))
        return "text/html";
    if (iequals(ext, ".css"))
        return "text/css";
    if (iequals(ext, ".txt"))
        return "text/plain";
    if (iequals(ext, ".js"))
        return "application/javascript";
    if (iequals(ext, ".json"))
        return "application/json";
    if (iequals(ext, ".xml"))
        return "application/xml";
    if (iequals(ext, ".swf"))
        return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))
        return "video/x-flv";
    if (iequals(ext, ".png"))
        return "image/png";
    if (iequals(ext, ".jpe"))
        return "image/jpeg";
    if (iequals(ext, ".jpeg"))
        return "image/jpeg";
    if (iequals(ext, ".jpg"))
        return "image/jpeg";
    if (iequals(ext, ".gif"))
        return "image/gif";
    if (iequals(ext, ".bmp"))
        return "image/bmp";
    if (iequals(ext, ".ico"))
        return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff"))
        return "image/tiff";
    if (iequals(ext, ".tif"))
        return "image/tiff";
    if (iequals(ext, ".svg"))
        return "image/svg+xml";
    if (iequals(ext, ".svgz"))
        return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
    beast::string_view base,
    beast::string_view path) //;
{
    if (base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto &c : result)
        if (c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

// Return a response for the given request.
//
// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.
template <class Body, class Allocator>
http::message_generator
handle_request(
    beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>> &&req,
    PathMap *paths)
{
    // Returns a bad request response
    auto const bad_request =
        [&req](beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found =
        [&req](beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error =
        [&req](beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return bad_request("Unknown HTTP-method");

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    auto str_target = std::string(req.target());
    if (paths->count(str_target) > 0)
    {
        //Log::info("WebServerUnix", "path found with WebServerResponseBuilder: %s", str_target.c_str());

        http::response<http::buffer_body_str> res;

        res.result(http::status::ok);
        res.version(11);
        res.set(http::field::server, "Beast");
        res.set(http::field::transfer_encoding, "chunked");

        auto const builderWriter =
            [](const char *buffer, int size, void *userData) -> void
        {
            auto response_buffer = (std::string *)userData;
            response_buffer->append(buffer, size);
        };

        auto builder = paths->at(str_target); 
        builder->build(builderWriter, (void *)&res.body().data);
        int body_size = res.body().data.size();

        // Log::info("WEBSERVER","response = %s", res.body().data.c_str());

        res.body().size = body_size;
        res.body().more = false;

        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(req.target()));
        res.content_length(body_size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    // Handle an unknown error
    if (ec)
        return server_error(ec.message());

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // Respond to GET request
    http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const *what) //;
{
    // ssl::error::stream_truncated, also known as an SSL "short read",
    // indicates the peer closed the connection without performing the
    // required closing handshake (for example, Google does this to
    // improve performance). Generally this can be a security issue,
    // but if your communication protocol is self-terminated (as
    // it is with both HTTP and WebSocket) then you may simply
    // ignore the lack of close_notify.
    //
    // https://github.com/boostorg/beast/issues/38
    //
    // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
    //
    // When a short read would cut off the end of an HTTP message,
    // Beast returns the error beast::http::error::partial_message.
    // Therefore, if we see a short read here, it has occurred
    // after the message has been completed, so it is safe to ignore it.

    if (ec == net::ssl::error::stream_truncated)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

//------------------------------------------------------------------------------

// Echoes back all received WebSocket messages.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived>
class websocket_session
{
    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.
    Derived &
    derived()
    {
        return static_cast<Derived &>(*this);
    }

    beast::flat_buffer buffer_;

    // Start the asynchronous operation
    template <class Body, class Allocator>
    void
    do_accept(http::request<Body, http::basic_fields<Allocator>> req)
    {
        // Set suggested timeout settings for the websocket
        derived().ws().set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        derived().ws().set_option(
            websocket::stream_base::decorator(
                [](websocket::response_type &res)
                {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                                " advanced-server-flex");
                }));

        // Accept the websocket handshake
        derived().ws().async_accept(
            req,
            beast::bind_front_handler(
                &websocket_session::on_accept,
                derived().shared_from_this()));
    }

public:
    void write_txt(const char *data, int len)
    {

        beast::flat_buffer buffer(len);
        auto buf = buffer.prepare(len);
        memcpy(buf.data(), data, len);
        buffer.commit(len);
        derived().ws().text(true);
        derived().ws().write(buffer.data());
    }

    void write_bin(uint8_t *data, int len)
    {

        beast::flat_buffer buffer(len);
        auto buf = buffer.prepare(len);
        memcpy(buf.data(), data, len);
        buffer.commit(len);
        derived().ws().text(false);
        derived().ws().write(buffer.data());
    }

private:
    void
    on_accept(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "accept");

        receiver->on_connect(this);

        do_read();
    }

    void
    do_read()
    {
        // Read a message into our buffer
        derived().ws().async_read(
            buffer_,
            beast::bind_front_handler(
                &websocket_session::on_read,
                derived().shared_from_this()));
    }

    void
    on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the websocket_session was closed
        if (ec == websocket::error::closed)
            return;

        if (ec)
            return fail(ec, "read");

        if (derived().ws().got_text())
        {
            //Log::info("", "Got string: %s", buffer_.data());
            receiver->on_receive(this, boost::beast::buffers_to_string(buffer_.data()));
        }
        buffer_.consume(buffer_.size());

        do_read();
    }

    void
    on_write(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        if (ec)
            return fail(ec, "write");
    }

    WebSocketServerSessionReceiver *receiver = nullptr;

public:
    // Start the asynchronous operation
    template <class Body, class Allocator>
    void
    run(http::request<Body, http::basic_fields<Allocator>> req, PathMapWs *wsPaths)
    {
        auto str_target = std::string(req.target());
        // Log::info("", "Websocket session start for target: %s", str_target.c_str());

        if (wsPaths->count(str_target) == 0)
        {
            Log::error("WEBSOCKET", "No websocket found at this path: %s", str_target.c_str());
            return;
        }

        this->receiver = wsPaths->at(str_target);

        // Accept the WebSocket upgrade request
        do_accept(std::move(req));
    }

    ~websocket_session()
    {
        Log::info("", "Websocket session destroy");
        if (receiver)
            receiver->on_disconnect(this);
    }
};

//------------------------------------------------------------------------------

// Handles an SSL WebSocket connection
class ssl_websocket_session
    : public websocket_session<ssl_websocket_session>,
      public std::enable_shared_from_this<ssl_websocket_session>
{
    websocket::stream<
        beast::ssl_stream<beast::tcp_stream>>
        ws_;

public:
    // Create the ssl_websocket_session
    explicit ssl_websocket_session(
        beast::ssl_stream<beast::tcp_stream> &&stream)
        : ws_(std::move(stream))
    {}

    // Called by the base class
    websocket::stream<
        beast::ssl_stream<beast::tcp_stream>> &
    ws()
    {
        return ws_;
    }
};

//------------------------------------------------------------------------------

template <class Body, class Allocator>
void make_websocket_session(
    beast::ssl_stream<beast::tcp_stream> stream,
    http::request<Body, http::basic_fields<Allocator>> req,
    PathMapWs *wsPaths)
{
    std::make_shared<ssl_websocket_session>(
        std::move(stream))
        ->run(std::move(req), wsPaths);
}

//------------------------------------------------------------------------------

// Handles an HTTP server connection.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived>
class http_session
{
    std::shared_ptr<std::string const> doc_root_;
    PathMap *paths_;
    PathMapWs *wsPaths_;

    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.
    Derived &
    derived()
    {
        return static_cast<Derived &>(*this);
    }

    static constexpr std::size_t queue_limit = 8; // max responses
    std::vector<http::message_generator> response_queue_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    boost::optional<http::request_parser<http::string_body>> parser_;

protected:
    beast::flat_buffer buffer_;

public:
    // Construct the session
    http_session(
        beast::flat_buffer buffer,
        std::shared_ptr<std::string const> const &doc_root,
        PathMap *paths,
        PathMapWs *wsPaths)
        : doc_root_(doc_root), buffer_(std::move(buffer)), paths_(paths), wsPaths_(wsPaths)
    {
    }

    void
    do_read()
    {
        // Construct a new parser for each message
        parser_.emplace();

        // Apply a reasonable limit to the allowed size
        // of the body in bytes to prevent abuse.
        parser_->body_limit(10000);

        // Set the timeout.
        beast::get_lowest_layer(
            derived().stream())
            .expires_after(std::chrono::seconds(30));

        // Read a request using the parser-oriented interface
        http::async_read(
            derived().stream(),
            buffer_,
            *parser_,
            beast::bind_front_handler(
                &http_session::on_read,
                derived().shared_from_this()));
    }

    void
    on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream)
            return derived().do_eof();

        if (ec)
            return fail(ec, "read");

        // See if it is a WebSocket Upgrade
        if (websocket::is_upgrade(parser_->get()))
        {
            // Disable the timeout.
            // The websocket::stream uses its own timeout settings.
            beast::get_lowest_layer(derived().stream()).expires_never();

            // Create a websocket session, transferring ownership
            // of both the socket and the HTTP request.
            return make_websocket_session(
                derived().release_stream(),
                parser_->release(),
                wsPaths_);
        }

        // Send the response
        queue_write(handle_request(*doc_root_, parser_->release(), paths_));

        // If we aren't at the queue limit, try to pipeline another request
        if (response_queue_.size() < queue_limit)
            do_read();
    }

    void
    queue_write(http::message_generator response)
    {
        // Allocate and store the work
        response_queue_.push_back(std::move(response));

        // If there was no previous work, start the write
        // loop
        if (response_queue_.size() == 1)
            do_write();
    }

    // Called to start/continue the write-loop. Should not be called when
    // write_loop is already active.
    //
    // Returns `true` if the caller may initiate a new read
    bool
    do_write()
    {
        bool const was_full =
            response_queue_.size() == queue_limit;

        if (!response_queue_.empty())
        {
            http::message_generator msg =
                std::move(response_queue_.front());
            response_queue_.erase(response_queue_.begin());

            bool keep_alive = msg.keep_alive();

            beast::async_write(
                derived().stream(),
                std::move(msg),
                beast::bind_front_handler(
                    &http_session::on_write,
                    derived().shared_from_this(),
                    keep_alive));
        }

        return was_full;
    }

    void
    on_write(
        bool keep_alive,
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        if (!keep_alive)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return derived().do_eof();
        }

        // Inform the queue that a write completed
        if (do_write())
        {
            // Read another request
            do_read();
        }
    }
};

//------------------------------------------------------------------------------

// Handles an SSL HTTP connection
class ssl_http_session
    : public http_session<ssl_http_session>,
      public std::enable_shared_from_this<ssl_http_session>
{
    beast::ssl_stream<beast::tcp_stream> stream_;

public:
    // Create the http_session
    ssl_http_session(
        beast::tcp_stream &&stream,
        ssl::context &ctx,
        beast::flat_buffer &&buffer,
        std::shared_ptr<std::string const> const &doc_root,
        PathMap *paths,
        PathMapWs *wsPaths)
        : http_session<ssl_http_session>(
              std::move(buffer),
              doc_root,
              paths,
              wsPaths),
          stream_(std::move(stream), ctx)
    {
    }

    // Start the session
    void
    run()
    {
        // Set the timeout.
        beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

        // Perform the SSL handshake
        // Note, this is the buffered version of the handshake.
        stream_.async_handshake(
            ssl::stream_base::server,
            buffer_.data(),
            beast::bind_front_handler(
                &ssl_http_session::on_handshake,
                shared_from_this()));
    }

    // Called by the base class
    beast::ssl_stream<beast::tcp_stream> &
    stream()
    {
        return stream_;
    }

    // Called by the base class
    beast::ssl_stream<beast::tcp_stream>
    release_stream()
    {
        return std::move(stream_);
    }

    // Called by the base class
    void
    do_eof()
    {
        // Set the timeout.
        beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

        // Perform the SSL shutdown
        stream_.async_shutdown(
            beast::bind_front_handler(
                &ssl_http_session::on_shutdown,
                shared_from_this()));
    }

private:
    void
    on_handshake(
        beast::error_code ec,
        std::size_t bytes_used)
    {
        if (ec)
            return fail(ec, "handshake");

        // Consume the portion of the buffer used by the handshake
        buffer_.consume(bytes_used);

        do_read();
    }

    void
    on_shutdown(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "shutdown");

        // At this point the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

// Detects SSL handshakes
class detect_session : public std::enable_shared_from_this<detect_session>
{
    beast::tcp_stream stream_;
    ssl::context &ctx_;
    std::shared_ptr<std::string const> doc_root_;
    beast::flat_buffer buffer_;
    PathMap *paths_;
    PathMapWs *wsPaths_;

public:
    explicit detect_session(
        tcp::socket &&socket,
        ssl::context &ctx,
        std::shared_ptr<std::string const> const &doc_root,
        PathMap *paths,
        PathMapWs *wsPaths)
        : stream_(std::move(socket)), ctx_(ctx), doc_root_(doc_root), paths_(paths), wsPaths_(wsPaths)
    {
    }

    // Launch the detector
    void
    run()
    {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(
            stream_.get_executor(),
            beast::bind_front_handler(
                &detect_session::on_run,
                this->shared_from_this()));
    }

    void
    on_run()
    {
        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        beast::async_detect_ssl(
            stream_,
            buffer_,
            beast::bind_front_handler(
                &detect_session::on_detect,
                this->shared_from_this()));
    }

    void
    on_detect(beast::error_code ec, bool result)
    {
        if (ec)
            return fail(ec, "detect");

        if (result)
        {
            // Launch SSL session
            std::make_shared<ssl_http_session>(
                std::move(stream_),
                ctx_,
                std::move(buffer_),
                doc_root_,
                paths_,
                wsPaths_)
                ->run();
            return;
        }

        return fail(ec, "plain http not supported");
    }
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    net::io_context &ioc_;
    ssl::context &ctx_;
    tcp::acceptor acceptor_;
    std::shared_ptr<std::string const> doc_root_;
    PathMap *paths_;
    PathMapWs *wsPaths_;

public:
    listener(
        net::io_context &ioc,
        ssl::context &ctx,
        tcp::endpoint endpoint,
        std::shared_ptr<std::string const> const &doc_root,
        PathMap *paths,
        PathMapWs *wsPaths)
        : ioc_(ioc), ctx_(ctx), acceptor_(net::make_strand(ioc)), doc_root_(doc_root), paths_(paths), wsPaths_(wsPaths)
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void
    run()
    {
        do_accept();
    }

private:
    void
    do_accept()
    {
        // The new connection gets its own strand
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &listener::on_accept,
                shared_from_this()));
    }

    void
    on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the detector http_session and run it
            std::make_shared<detect_session>(
                std::move(socket),
                ctx_,
                doc_root_,
                paths_,
                wsPaths_)
                ->run();
        }

        // Accept another connection
        do_accept();
    }
};

//------------------------------------------------------------------------------

class WebServerUnix : public WebServer 
{
public:
    WebServerUnix(std::string root, int port)
    {
        Log::info(TAG, "Starting web server on port %d. root=%s", port, root.c_str());
        auto const doc_root = std::make_shared<std::string>(root);
        std::thread{std::bind(
                        &start_server,
                        port,
                        &paths,
                        &wsPaths,
                        doc_root)}
            .detach();
    }

    ~WebServerUnix() = default;

    void addPath(std::string path, WebServerResponseBuilder *builder) override
    {
        paths[path] = builder;
        // Log::info(TAG, "added path %s", path.c_str());
    }

    void addPathWs(std::string path, WebSocketServerSessionReceiver *websocket)
    {
        wsPaths[path] = websocket;
        // Log::info(TAG, "added ws path %s", path.c_str());
    }

private:
    static const char *TAG;

    PathMap paths;
    PathMapWs wsPaths;

    static void start_server(
        int portArg,
        PathMap *paths,
        PathMapWs *wsPaths,
        std::shared_ptr<std::string const> const &doc_root)
    {
        // Log::info(TAG, "Starting server");

        try
        {
            // Check command line arguments.
            auto const address = net::ip::make_address("0.0.0.0");
            auto const port = static_cast<unsigned short>(portArg);
            auto const threads = 1;

            // The io_context is required for all I/O
            net::io_context ioc{threads};

            // The SSL context is required, and holds certificates
            ssl::context ctx{ssl::context::tlsv12};

            // This holds the self-signed certificate used by the server
            load_server_certificate(ctx);

            // Create and launch a listening port
            std::make_shared<listener>(
                ioc,
                ctx,
                tcp::endpoint{address, port},
                doc_root,
                paths,
                wsPaths)
                ->run();

            // Capture SIGINT and SIGTERM to perform a clean shutdown
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait(
                [&](beast::error_code const &, int)
                {
                    // Stop the `io_context`. This will cause `run()`
                    // to return immediately, eventually destroying the
                    // `io_context` and all of the sockets in it.
                    ioc.stop();
                });

            // Run the I/O service on the requested number of threads
            std::vector<std::thread> v;
            v.reserve(threads - 1);
            for (auto i = threads - 1; i > 0; --i)
                v.emplace_back(
                    [&ioc]
                    {
                        ioc.run();
                    });
            ioc.run();

            // (If we get here, it means we got a SIGINT or SIGTERM)

            // Block until all the threads exit
            for (auto &t : v)
                t.join();
        }
        catch (const std::exception &e)
        {
            Log::error(TAG, "Error: %s", e.what());
        }
    }
};

const char *WebServerUnix::TAG = "WEBSERVER";

class WebsocketServerUnix : public WebsocketServer, public WebSocketServerSessionReceiver
{
public:
    using WS = websocket_session<ssl_websocket_session>;

    WebsocketServerUnix(WebServer *server, const char *path)
    {
        ((WebServerUnix *)server)->addPathWs(path, this);
    }

    ~WebsocketServerUnix() = default;

    void send(RemoteWebsocketClient *client, std::string msg) override
    {
        try
        {
            //Log::info(TAG, "sending: %s", msg.c_str());
            auto ws = (WS *)client;
            ws->write_txt(msg.c_str(), msg.size());
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

            //Log::info(TAG, "sending: %s", s_buf);
            auto ws = (WS *)client;
            ws->write_txt(s_buf, sz);
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
            // Log::info(TAG, "Sending binary");

            auto ws = (WS *)client;
            ws->write_bin(bytes, size);
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

private:
    std::set<WS *> clients;
    static const char *TAG;
};

const char *WebsocketServerUnix::TAG = "WEBSOCKET_SERVER";