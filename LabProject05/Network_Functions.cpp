#include "stdafx.h"
#include "Mawang.h"

std::unordered_map<int, Clients> g_objects;
void ProcessPacket(char*);


void print_error(const char* msg, int err_no)
{
    WCHAR* msg_buf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
    std::cout << msg;
    std::wcout << L" : ���� : " << msg_buf;
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

// TODO: �ٸ� ������� ��Ŷ �ؼ� �ٲܰ�
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
        g_objects[g_myid].Location = coord;
        float yaw = 0;
        g_objects[g_myid].yaw = yaw;
    }
    break;

    case SC_ADD_PLAYER:
    {
        SC_ADD_PLAYER_PACKET* my_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(ptr);
        int id = my_packet->id;

        if (id == g_myid) 
        {
            // �ڽ��� ��� �ڽ��� ������
            //g_objects[id].move(my_packet->x, my_packet->y);
        }
        else 
        {
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[id].Location = coord;
            float yaw = 0;
            g_objects[id].yaw = yaw;
        }
        break;
    }
    case SC_MOVE_PLAYER:
    {
        SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) 
        {
            // �ڽ��� ���� �����Ӱ� look
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[g_myid].Location = coord;
            float yaw = my_packet->yaw;
            g_objects[g_myid].yaw = yaw;
        }
        else 
        {
            // �ٸ� ĳ������ ���� ������
            XMFLOAT3 coord = { static_cast<float>(my_packet->x), static_cast<float>(my_packet->y), static_cast<float>(my_packet->z) };
            g_objects[other_id].Location = coord;
            float yaw = my_packet->yaw;
            g_objects[other_id].yaw = yaw;
        }
        break;
    }
    case SC_SKILL_PLAYER:
    {
        SC_SKILL_PACKET* my_packet = reinterpret_cast<SC_SKILL_PACKET*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid)
        {

        }
        else
        {
            
        }
        break;
    }

    case SC_REMOVE_PLAYER:
    {
        SC_REMOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) 
        {
            // �ڽ� ����
            exit(0);
        }
        else 
        {
            // �ٸ� �÷��̾� ����
            g_objects[other_id].Location = XMFLOAT3(-9999, -999, -9999);
            // ������ �ʴ� ������ �ٽ� �̵�
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
        else if (err == WSAECONNRESET || err == WSAENOTCONN)
        {
            // Disconnect
            closesocket(g_server_socket);
            exit(0);
        }
        else
        {
            print_error("WSARecv", err);
            exit(0);
        }
    }
    else 
    {
        // ���ŵ� ������ ó��
        ProcessData(wsabuf.buf, bytes_received);
    }
}

