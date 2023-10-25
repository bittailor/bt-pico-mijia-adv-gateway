#ifndef BT_PUBLISHER_HPP
#define BT_PUBLISHER_HPP

void connect();
void publishLoop();

void publish(const char* topic, const char* message);

#endif