#pragma once
#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_https_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_spiffs.h"
#include "esp_tls.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "hyperion.crt.h"
#include "hyperion.key.h"
#include "log.hpp"
#include "nvs_flash.h"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"
#include <dirent.h>
#include <inttypes.h>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <cstring>

/* Max length a file path can have on storage */
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)
#define SCRATCH_BUFSIZE 8192

struct file_server_data
{
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
    std::map<std::string, WebServerResponseBuilder *> *paths;
};

class WebServerEsp : public WebServer
{
public:
    WebServerEsp(int port)
    {
        Log::info(TAG, "Starting web server on port %d.", port);

        const char *base_path = "/data";
        ESP_ERROR_CHECK(mount_storage(base_path));

        ESP_ERROR_CHECK(start_file_server(base_path, port));
        //Log::info(TAG, "File server started");
    }

    ~WebServerEsp() = default;

    void addPath(std::string path, WebServerResponseBuilder *builder) override
    {
        paths[path] = builder;
        // Log::info(TAG, "added path %s", path.c_str());
    }

private:
    esp_err_t mount_storage(const char *base_path)
    {
        // Log::info(TAG, "Initializing SPIFFS");

        esp_vfs_spiffs_conf_t conf = {
            .base_path = base_path,
            .partition_label = NULL,
            .max_files = 5, // This sets the maximum number of files that can be open at the same time
            .format_if_mount_failed = true};

        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        if (ret != ESP_OK)
        {
            if (ret == ESP_FAIL)
            {
                Log::error(TAG, "Failed to mount or format filesystem");
            }
            else if (ret == ESP_ERR_NOT_FOUND)
            {
                Log::error(TAG, "Failed to find SPIFFS partition");
            }
            else
            {
                Log::error(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            }
            return ret;
        }

        size_t total = 0, used = 0;
        ret = esp_spiffs_info(NULL, &total, &used);
        if (ret != ESP_OK)
        {
            Log::error(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
            return ret;
        }

        // Log::info(TAG, "Partition size: total: %d, used: %d", total, used);
        return ESP_OK;
    }

    esp_err_t start_file_server(const char *base_path, int port)
    {
        if (server_data)
        {
            Log::error(TAG, "File server already started");
            return ESP_ERR_INVALID_STATE;
        }

        /* Allocate memory for server data */
        server_data = (struct file_server_data *)calloc(1, sizeof(struct file_server_data));
        if (!server_data)
        {
            Log::error(TAG, "Failed to allocate memory for server data");
            return ESP_ERR_NO_MEM;
        }
        strlcpy(server_data->base_path, base_path, sizeof(server_data->base_path));
        server_data->paths = &paths;

        httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
        // bin2hex does not include the null terminator in the size, but esp-idf expects it, therefore i add +1 to the size
        config.servercert = HYPERION_CERT;
        config.servercert_len = HYPERION_CERT_SIZE + 1;
        config.prvtkey_pem = HYPERION_KEY;
        config.prvtkey_len = HYPERION_KEY_SIZE + 1;

        /* Use the URI wildcard matching function in order to
         * allow the same handler to respond to multiple different
         * target URIs which match the wildcard scheme */
        config.httpd.uri_match_fn = wildcard_non_websocket;
        //config.httpd.max_open_sockets = 10;
        config.port_secure = port;

        // Log::info(TAG, "Starting HTTP Server on port: '%d' (%d)", config.port_secure, config.port_insecure);
        if (httpd_ssl_start(&server, &config) != ESP_OK)
        {
            Log::error(TAG, "Failed to start file server!");
            return ESP_FAIL;
        }

        /* URI handler for getting uploaded files */
        httpd_uri_t file_download = {
            .uri = "/*", // Match all URIs of type /path/to/file
            .method = HTTP_GET,
            .handler = download_get_handler,
            .user_ctx = server_data, // Pass server data as context

            .is_websocket = false,
            .handle_ws_control_frames = false,
            .supported_subprotocol = nullptr

        };
        httpd_register_uri_handler(server, &file_download);

        return ESP_OK;
    }

    static bool wildcard_non_websocket(const char *templ, const char *uri, size_t len)
    {
        // This matches matches /ws/ exactly, so they can be matched with the registered 
        // websockets. For other urls is does a pattern match, so we can match * 
        // for all files in the filesystem.
        if (len >= 4 && strncmp(uri,"/ws/",4)==0){
            //requested url is of a websocket. do not use template matching
            return strlen(templ) == len && strncmp(templ,uri,len)==0;
        }
        return httpd_uri_match_wildcard(templ, uri, len);
    }

    static esp_err_t download_get_handler(httpd_req_t *req)
    {
        char filepath[FILE_PATH_MAX];
        FILE *fd = NULL;
        struct stat file_stat;

        const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                                 req->uri, sizeof(filepath));

        if (!filename)
        {
            Log::error(TAG, "Filename is too long");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
            return ESP_FAIL;
        }

        std::string newFileName;
        if (filename[strlen(filename) - 1] == '/')
        {
            newFileName = filename + std::string("index.html");
            filename = newFileName.c_str();
            auto newFilePath = (filepath + std::string("index.html"));
            memcpy(filepath, (void *)newFilePath.c_str(), newFilePath.size() + 1);
        }

        // Log::info(TAG, "checking if %s is mapped", filename);
        std::map<std::string, WebServerResponseBuilder *> *paths = ((struct file_server_data *)req->user_ctx)->paths;
        if (paths->count(filename) > 0)
        {
            auto const builderWriter =
                [](const char *buffer, int size, void *userData) -> void
            {
                httpd_req_t *req = (httpd_req_t *)userData;
                httpd_resp_send_chunk(req, buffer, size);
            };

            auto builder = paths->at(filename);
            set_content_type_from_file(req, filename);
            builder->build(builderWriter, (void *)req);
            httpd_resp_send_chunk(req, NULL, 0);

            return ESP_OK;
        }

        if (stat(filepath, &file_stat) == -1)
        {
            // Log::error(TAG, "Failed to stat file : %s", filepath);
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404");
            return ESP_FAIL;
        }

        fd = fopen(filepath, "r");
        if (!fd)
        {
            // Log::error(TAG, "Failed to read existing file : %s", filepath);
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
            return ESP_FAIL;
        }

        // Log::info(TAG, "Sending file : %s (%ld bytes)...", filename, file_stat.st_size);
        set_content_type_from_file(req, filename);

        /* Retrieve the pointer to scratch buffer for temporary storage */
        char *chunk = ((struct file_server_data *)req->user_ctx)->scratch;
        size_t chunksize;
        do
        {
            /* Read file in chunks into the scratch buffer */
            chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);

            if (chunksize > 0)
            {
                /* Send the buffer contents as HTTP response chunk */
                if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK)
                {
                    fclose(fd);
                    Log::error(TAG, "File sending failed!");
                    /* Abort sending file */
                    httpd_resp_sendstr_chunk(req, NULL);
                    /* Respond with 500 Internal Server Error */
                    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                    return ESP_FAIL;
                }
            }

            /* Keep looping till the whole file is sent */
        } while (chunksize != 0);

        /* Close file after sending complete */
        fclose(fd);
        // Log::info(TAG, "File sending complete");

        /* Respond with an empty chunk to signal HTTP response completion */
        httpd_resp_send_chunk(req, NULL, 0);
        return ESP_OK;
    }

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

    /* Set HTTP response content type according to file extension */
    static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename)
    {
        if (IS_FILE_EXT(filename, ".html"))
            return httpd_resp_set_type(req, "text/html");
        if (IS_FILE_EXT(filename, ".js"))
            return httpd_resp_set_type(req, "application/javascript");
        if (IS_FILE_EXT(filename, ".json"))
            return httpd_resp_set_type(req, "application/json");

        /* This is a limited set only */
        /* For any other type always set as plain text */
        return httpd_resp_set_type(req, "text/plain");
    }

    /* Copies the full path into destination buffer and returns
     * pointer to path (skipping the preceding base path) */
    static const char *get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
    {
        const size_t base_pathlen = strlen(base_path);
        size_t pathlen = strlen(uri);

        const char *quest = strchr(uri, '?');
        if (quest)
        {
            pathlen = MIN(pathlen, quest - uri);
        }
        const char *hash = strchr(uri, '#');
        if (hash)
        {
            pathlen = MIN(pathlen, hash - uri);
        }

        if (base_pathlen + pathlen + 1 > destsize)
        {
            /* Full path string won't fit into destination buffer */
            return NULL;
        }

        /* Construct full path (base + path) */
        strcpy(dest, base_path);
        strlcpy(dest + base_pathlen, uri, pathlen + 1);

        /* Return pointer to path, skipping the base */
        return dest + base_pathlen;
    }

    // static esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd){
    //     //if (ws_open) return ws_open(hd, sockfd);
    //     return ESP_OK;
    // }
    // static void wss_close_fd(httpd_handle_t hd, int sockfd){
    //    // if (ws_close) return ws_close(hd, sockfd);
    // }

    static const char *TAG;
    static struct file_server_data *server_data;

    std::map<std::string, WebServerResponseBuilder *> paths;

public:
    httpd_handle_t server = NULL;

    // static httpd_open_func_t ws_open;
    // static httpd_close_func_t ws_close;

    //friend class WebsocketServerEsp;
};



// const char *WebServerEsp::TAG = "WEBSERVER";
// struct file_server_data *WebServerEsp::server_data = nullptr;