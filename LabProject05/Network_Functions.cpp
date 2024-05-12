#include "stdafx.h"
#include "Mawang.h"

std::unordered_map<int, XMFLOAT3[2]> g_objects;
void ProcessPacket(char*);


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

void DoSend(void* packet)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(packet);
    size_t sent = 0;

    WSABUF wsabuf;
    wsabuf.buf = reinterpret_cast<CHAR*>(packet);
    wsabuf.len = p[0];
    ZeroMemory(&g_wsaover, sizeof(g_wsaover));

    int res = WSASend(g_server_socket, &wsabuf, 1, nullptr, 0, &g_wsaover, nullptr);
    if (0 != res)
    {
        print_error("WSASend", WSAGetLastError());
    }
}

// TODO: 다른 방식으로 패킷 해석 바꿀것
void ProcessData(char* net_buf, size_t io_byte)
{
    char* ptr = net_buf;
    static size_t in_packet_size = 0;
    static size_t saved_packet_size = 0;
    static char packet_buffer[BUF_SIZE];

    while (0 != io_byte) 
    {
        if (0 == in_packet_size) in_packet_size = ptr[0];
        if (io_byte + saved_packet_size >= in_packet_size) 
        {
            memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
            ProcessPacket(packet_buffer);
            ptr += in_packet_size - saved_packet_size;
            io_byte -= in_packet_size - saved_packet_size;
            in_packet_size = 0;
            saved_packet_size = 0;
        }
        else {
            memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
            saved_packet_size += io_byte;
            io_byte = 0;
        }
    }
}

void ProcessPacket(char* ptr)
{
    static bool first_time = true;
    switch (ptr[1])
    {
    case SC_LOGIN_INFO:
    {
        SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
        g_myid = packet->id;
        XMFLOAT3 coord = { static_cast<float>(packet->x), static_cast<float>(packet->y), static_cast<float>(packet->z) };
        //g_objects[g_myid].push_back(coord);
        g_objects[g_myid][V_LOCATION] = coord;

        XMFLOAT3 look = { 0, 0, 1 };
        //g_objects[g_myid].push_back(look);
        g_objects[g_myid][V_LOOK] = look;
    }
    break;

    case SC_ADD_PLAYER:
    {
        SC_ADD_PLAYER_PACKET* my_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(ptr);
        int id = my_packet->id;

        if (id == g_myid) 
        {
            // 자신일 경우 자신의 움직임
            //g_objects[id].move(my_packet->x, my_packet->y);
        }
        else 
        {
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[id][V_LOCATION] = coord;
            XMFLOAT3 look = { 0, 0, 1 };
            g_objects[id][V_LOOK] = look;
            //g_objects[id]->set_position_vector(550, 100, 550);
        }
        break;
    }
    case SC_MOVE_PLAYER:
    {
        SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) 
        {
            // 자신의 받은 움직임과 look
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[g_myid][V_LOCATION] = coord;
            XMFLOAT3 look = { my_packet->pitch, my_packet->yaw, my_packet->roll };
            g_objects[g_myid][V_LOOK] = look;
        }
        else 
        {
            // 다른 캐릭터의 받은 움직임
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[other_id][V_LOCATION] = coord;
            XMFLOAT3 look = { my_packet->pitch, my_packet->yaw, my_packet->roll };
            g_objects[other_id][V_LOOK] = look;
        }
        break;
    }

    case SC_REMOVE_PLAYER:
    {
        SC_REMOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) 
        {
            // 자신 삭제
            exit(0);
        }
        else 
        {
            // 다른 플레이어 삭제
            //g_objects.erase(other_id);
            g_objects[other_id][V_LOCATION] = XMFLOAT3(0, -999, 0);
            // 보이지 않는 곳으로 다시 이동
        }
        break;
    }
    default:
        printf("Unknown PACKET type [%d]\n", ptr[1]);
    }
}


void DoRecv() 
{
    char buffer[BUFSIZE];
    WSABUF wsabuf;
    wsabuf.buf = buffer;
    wsabuf.len = BUFSIZE;

    DWORD recv_flags = 0;
    DWORD bytes_received = 0;
    //WSAOVERLAPPED overlapped = {};

    int res = WSARecv(g_server_socket, &wsabuf, 1, &bytes_received, &recv_flags, nullptr, nullptr);
    if (res == SOCKET_ERROR) 
    {
        int err = WSAGetLastError();
        if (err == WSA_IO_PENDING || err == WSAEWOULDBLOCK) 
        {
            // NON-Blocking WOULDBLOCK
        }
        else
        {
            print_error("WSARecv", err);
            exit(0);
        }
    }
    else 
    {
        // 수신된 데이터 처리
        ProcessData(wsabuf.buf, bytes_received);
    }
}