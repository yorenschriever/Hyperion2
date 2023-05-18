// #pragma once
// #include <string>

// template <class Derived>
// class websocket_session;

// class ssl_websocket_session;

// class WebSocketServerSessionReceiver
// {
// public:
//     virtual void on_connect(websocket_session<ssl_websocket_session> *session)=0;
//     virtual void on_receive(websocket_session<ssl_websocket_session> *session, std::string message)=0;
//     virtual void on_disconnect(websocket_session<ssl_websocket_session> *session)=0;
// };