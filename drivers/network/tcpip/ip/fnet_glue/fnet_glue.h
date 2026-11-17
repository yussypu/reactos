/*
 * PROJECT:     ReactOS TCP/IP protocol driver
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     FNET glue, the interface the ReactOS TCP code calls
 * COPYRIGHT:   Copyright 2026 yussypu
 */

#ifndef _FNET_GLUE_H_
#define _FNET_GLUE_H_

#include <tcpip.h>
#include <fnet_ros_port.h>

/* The TCP code's result codes; the values are the ones TCPTranslateError expects */
typedef INT8 err_t;

#define ERR_OK          0
#define ERR_MEM         -1
#define ERR_BUF         -2
#define ERR_TIMEOUT     -3
#define ERR_RTE         -4
#define ERR_INPROGRESS  -5
#define ERR_VAL         -6
#define ERR_WOULDBLOCK  -7
#define ERR_USE         -8
#define ERR_ALREADY     -9
#define ERR_ISCONN      -10
#define ERR_CONN        -11
#define ERR_IF          -12
#define ERR_ABRT        -13
#define ERR_RST         -14
#define ERR_CLSD        -15
#define ERR_ARG         -16

typedef UCHAR u8_t;
typedef USHORT u16_t;
typedef ULONG u32_t;

/* IPv4 address in network order */
typedef struct ip4_addr
{
    u32_t addr;
} ip4_addr_t, ip_addr_t;

#define FNET_GLUE_PCB_TAG       'pTNF'
#define FNET_GLUE_IF_TAG        'fTNF'
#define FNET_GLUE_PACKET_TAG    'kTNF'

/* Opaque to everything outside the glue */
typedef struct tcp_pcb *PTCP_PCB;

NTSTATUS    LibTCPGetDataFromConnectionQueue(PCONNECTION_ENDPOINT Connection, PUCHAR RecvBuffer, UINT RecvLen, UINT *Received);

/* External TCP event handlers */
extern void TCPConnectEventHandler(void *arg, const err_t err);
extern void TCPAcceptEventHandler(void *arg, PTCP_PCB newpcb);
extern void TCPSendEventHandler(void *arg, const u16_t space);
extern void TCPFinEventHandler(void *arg, const err_t err);
extern void TCPRecvEventHandler(void *arg);

/* TCP functions */
PTCP_PCB    LibTCPSocket(void *arg);
VOID        LibTCPFreeSocket(PTCP_PCB pcb);
err_t       LibTCPBind(PCONNECTION_ENDPOINT Connection, ip4_addr_t *const ipaddr, const u16_t port);
PTCP_PCB    LibTCPListen(PCONNECTION_ENDPOINT Connection, const u8_t backlog);
err_t       LibTCPSend(PCONNECTION_ENDPOINT Connection, void *const dataptr, const ULONG len, ULONG *sent, const int safe);
err_t       LibTCPConnect(PCONNECTION_ENDPOINT Connection, ip4_addr_t *const ipaddr, const u16_t port);
err_t       LibTCPShutdown(PCONNECTION_ENDPOINT Connection, const int shut_rx, const int shut_tx);
err_t       LibTCPClose(PCONNECTION_ENDPOINT Connection, const int safe, const int callback);

err_t       LibTCPGetPeerName(PTCP_PCB pcb, ip4_addr_t *const ipaddr, u16_t *const port);
err_t       LibTCPGetHostName(PTCP_PCB pcb, ip4_addr_t *const ipaddr, u16_t *const port);
void        LibTCPAccept(PTCP_PCB pcb, PTCP_PCB listen_pcb, void *arg);
void        LibTCPSetNoDelay(PTCP_PCB pcb, BOOLEAN Set);
void        LibTCPSetKeepAlive(PTCP_PCB pcb, BOOLEAN Set);
void        LibTcpSetKeepAliveValues(PTCP_PCB pcb, u32_t KeepAliveTime, u32_t KeepAliveInterval);
void        LibTCPGetSocketStatus(PTCP_PCB pcb, PULONG State);

/* FNET only: a listen IRP was queued; connections may already wait in the FNET backlog */
void        LibTCPAcceptQueued(PCONNECTION_ENDPOINT Listener);

/* FNET only: a receive was queued while data may be waiting in FNET */
#define TCP_MAX_RECEIVE_NESTING 4
void        LibTCPKickReceive(PCONNECTION_ENDPOINT Connection);

/* IP functions */
void LibIPInsertPacket(void *ifarg, const void *const data, const u32_t size);
void LibIPInitialize(void);
void LibIPShutdown(void);

/* Between the two glue files */
ULONG       FnetGlueInterfaceScopeId(PIP_INTERFACE IF);
ULONG       FnetGlueInterfaceMtu(PIP_INTERFACE IF);
VOID        FnetGlueAbortAddress(ULONG Address);

#endif
