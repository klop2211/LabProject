#include "stdafx.h"

void print_error(const char* msg, int err_no)
{
    WCHAR* msg_buf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
    std::cout << msg;
    std::wcout << L" : 에러 : " << msg_buf;
    while (true);
    LocalFree(msg_buf);
}


void send_packet(void* packet)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(packet);
    size_t sent = 0;

    WSABUF wsabuf;
    wsabuf.buf = reinterpret_cast<CHAR*>(packet);
    wsabuf.len = sizeof(CHAR);
    ZeroMemory(&g_wsaover, sizeof(g_wsaover));

    int res = WSASend(g_server_socket, &wsabuf, 1, nullptr, 0, &g_wsaover, nullptr);
    if (0 != res)
    {
        print_error("WSASend", WSAGetLastError());
    }
    std::cout << "로그인 패킷 보냄" << std::endl;
}

//void process_data(char* net_buf, size_t io_byte)
//{
//    char* ptr = net_buf;
//    static size_t in_packet_size = 0;
//    static size_t saved_packet_size = 0;
//    static char packet_buffer[BUF_SIZE];
//
//    while (0 != io_byte) {
//        if (0 == in_packet_size) in_packet_size = ptr[0];
//        if (io_byte + saved_packet_size >= in_packet_size) {
//            memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
//            ProcessPacket(packet_buffer);
//            ptr += in_packet_size - saved_packet_size;
//            io_byte -= in_packet_size - saved_packet_size;
//            in_packet_size = 0;
//            saved_packet_size = 0;
//        }
//        else {
//            memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
//            saved_packet_size += io_byte;
//            io_byte = 0;
//        }
//    }
//}
//
//void ProcessPacket(char* ptr)
//{
//    static bool first_time = true;
//    switch (ptr[1])
//    {
//    case SC_LOGIN_INFO:
//    {
//        SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
//        g_myid = packet->id;
//        avatar.m_x = packet->x;
//        avatar.m_y = packet->y;
//        avatar.show();
//    }
//    break;
//
//    case SC_ADD_PLAYER:
//    {
//        SC_ADD_PLAYER_PACKET* my_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(ptr);
//        int id = my_packet->id;
//
//
//        if (id < MAX_USER) {
//            players[id].move(my_packet->x, my_packet->y);
//            players[id].set_name(my_packet->name);
//            players[id].show();
//        }
//        else {
//            //npc[id - NPC_START].x = my_packet->x;
//            //npc[id - NPC_START].y = my_packet->y;
//            //npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
//        }
//        break;
//    }
//    case SC_MOVE_PLAYER:
//    {
//        SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(ptr);
//        int other_id = my_packet->id;
//        if (other_id == g_myid) {
//            avatar.move(my_packet->x, my_packet->y);
//        }
//        else if (other_id < MAX_USER) {
//            players[other_id].move(my_packet->x, my_packet->y);
//        }
//        else {
//            //npc[other_id - NPC_START].x = my_packet->x;
//            //npc[other_id - NPC_START].y = my_packet->y;
//        }
//        break;
//    }
//
//    case SC_REMOVE_PLAYER:
//    {
//        SC_REMOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(ptr);
//        int other_id = my_packet->id;
//        if (other_id == g_myid) {
//            avatar.hide();
//        }
//        else if (other_id < MAX_USER) {
//            players[other_id].hide();
//        }
//        else {
//            //		npc[other_id - NPC_START].attr &= ~BOB_ATTR_VISIBLE;
//        }
//        break;
//    }
//    default:
//        printf("Unknown PACKET type [%d]\n", ptr[1]);
//    }
//}