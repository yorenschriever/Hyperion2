#pragma once
#include "log.hpp"
#include "server-certificate.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class WebServerMacOs : public WebServer
{
public:
    WebServerMacOs(std::string root, int port)
    {
        auto const doc_root = std::make_shared<std::string>(root);
        std::thread{std::bind(
                        &start_server,
                        port,
                        &paths,
                        doc_root)}
            .detach();
    }

    ~WebServerMacOs() = default;

    void addPath(std::string path, WebServerResponseBuilder *builder) override
    {
        paths[path] = builder;
        // Log::info(TAG, "added path %s", path.c_str());
    }

private:
    static const char *TAG;

    std::map<std::string, WebServerResponseBuilder *> paths;

    static void start_server(
        int portArg,
        std::map<std::string, WebServerResponseBuilder *> *paths,
        std::shared_ptr<std::string const> const &doc_root)
    {
        try
        {
            // Check command line arguments.
            auto const address = net::ip::make_address("0.0.0.0");
            auto const port = static_cast<unsigned short>(portArg);
            // auto const doc_root = std::make_shared<std::string>("/Users/yoren/repos/Hyperion2/library/scripts/web");

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
                std::thread{std::bind(
                                &do_session,
                                std::move(socket),
                                std::ref(ctx),
                                doc_root,
                                paths)}
                    .detach();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            Log::error(TAG, e.what());
            // return EXIT_FAILURE;
        }
    }

    static void do_session(
        tcp::socket &socket,
        ssl::context &ctx,
        std::shared_ptr<std::string const> const &doc_root,
        std::map<std::string, WebServerResponseBuilder *> *paths)
    {
        beast::error_code ec;

        // Construct the stream around the socket
        beast::ssl_stream<tcp::socket &> stream{socket, ctx};

        // Perform the SSL handshake
        stream.handshake(ssl::stream_base::server, ec);
        if (ec)
        {
            Log::error(TAG, "SSL handshake failed: %s", ec.message().c_str());
            return;
        }

        // This buffer is required to persist across reads
        beast::flat_buffer buffer;

        for (;;)
        {
            // Read a request
            http::request<http::string_body> req;
            http::read(stream, buffer, req, ec);
            if (ec == http::error::end_of_stream)
                break;
            if (ec)
            {
                Log::error(TAG, "HTTP body read error %s", ec.message().c_str());
                return;
            }

            // Handle request
            http::message_generator msg =
                handle_request(
                    *doc_root,
                    paths,
                    std::move(req));

            // Determine if we should close the connection
            bool keep_alive = msg.keep_alive();

            // Send the response
            beast::write(stream, std::move(msg), ec);

            if (ec)
            {
                Log::error(TAG, "HTTP write error %s", ec.message().c_str());
                return;
            }

            if (!keep_alive)
            {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                break;
            }
        }

        // Perform the SSL shutdown
        stream.shutdown(ec);
        if (ec)
        {
            Log::error(TAG, "SSL shutdown %s", ec.message().c_str());
            return;
        }

        // At this point the connection is closed gracefully
    }

    template <class Body, class Allocator>
    static http::message_generator
    handle_request(
        beast::string_view doc_root,
        std::map<std::string, WebServerResponseBuilder *> *paths,
        http::request<Body, http::basic_fields<Allocator>> &&req)
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
            // Log::info(TAG, "path found with WebServerResponseBuilder");

            http::response<http::buffer_body> res;

            res.result(http::status::ok);
            res.version(11);
            res.set(http::field::server, "Beast");
            res.set(http::field::transfer_encoding, "chunked");

            std::string response_buffer;

            auto const builderWriter =
                [](char *buffer, int size, void *userData) -> void
            {
                auto response_buffer = (std::string *)userData;
                response_buffer->append(buffer, size);
            };

            auto builder = (*paths)[str_target];
            builder->build(builderWriter, (void *)&response_buffer);
            int body_size = response_buffer.size();

            res.body().data = (void *)response_buffer.c_str();
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

    static beast::string_view
    mime_type(beast::string_view path)
    {
        if (path.ends_with("/"))
            return "text/html";

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

    http::response<http::string_body> do_parse(std::string_view input)
    {
        beast::error_code ec;
        http::response_parser<http::string_body> p;

        // read headers
        auto buf = boost::asio::buffer(input);
        auto n = p.put(buf, ec);
        assert(p.is_header_done());

        // read body
        if (!ec)
        {
            buf += n;
            n = p.put(buf, ec);
            p.put_eof(ec);
        }
        if (ec)
            throw boost::system::system_error(ec);
        assert(p.is_done());

        return p.release();
    }

    static std::string
    path_cat(
        beast::string_view base,
        beast::string_view path)
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
};

const char *WebServerMacOs::TAG = "WEBSERVER";