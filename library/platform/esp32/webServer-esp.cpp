#include "webServer-esp.hpp"

const char *WebServerEsp::TAG = "WEBSERVER";
struct file_server_data *WebServerEsp::server_data = nullptr;